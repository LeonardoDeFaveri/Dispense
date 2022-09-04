#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>

#define MIN_ARGC 4
#define MAX_ARGC 5
/* Actions name*/
#define A_NEW "new"
#define A_PUT "put"
#define A_GET "get"
#define A_DEL "del"
#define A_EMP "emp"
#define A_MOV "mov"
/* Actions value*/
#define NEW 1
#define PUT 2
#define GET 3
#define DEL 4
#define EMP 5
#define MOV 6

#define QUEUE_ID 1
#define M_TYPE 1

int parse_action(char *action);
#define CREATE 1
#define NO_CREATE 0
int get_queue(char *queue, int pid, short create);

#define BUFFER_MAX 32
struct msg{
    long mtype;
    char mtext[BUFFER_MAX];
};


#define WRONG_ARGC 1
#define INVALID_PID 2
#define INVALID_ACTION 3
#define FILE_CREATION_ERROR 4
#define KEY_CREATION_ERROR 5
#define QUEUE_CREATION_ERROR 6
#define QUEUE_OPENING_ERROR 7
#define QUEUE_SEND_ERROR 8
#define QUEUE_RECEIVE_ERROR 9
#define QUEUE_DELITION_ERROR 10
#define SIGNAL_ERROR 11

void quit(int error, int pid);

int main(int argc, char **argv) {
    if (argc < MIN_ARGC || argc > MAX_ARGC) exit(WRONG_ARGC);

    char *queue = argv[1];
    char *action = argv[2];
    char *value;
    int pid = atoi(argv[argc - 1]);
    if (pid <= 0) exit(INVALID_PID);

    int queue_id;
    switch (parse_action(action))
    {
    case NEW:
        if (3 < argc - 1) quit(WRONG_ARGC, pid);
        
        queue_id = get_queue(queue, pid, CREATE);
        printf("%d\n", queue_id);
        fflush(stdout);
        break;
    case PUT:
        if (3 == argc - 1) quit(WRONG_ARGC, pid);

        queue_id = get_queue(queue, pid, CREATE);
        {
            struct msg msg;
            msg.mtype = M_TYPE;
            strcpy(msg.mtext, argv[3]);
            if (msgsnd(queue_id, &msg, sizeof(msg.mtext), 0) < 0)
                quit(QUEUE_SEND_ERROR, pid);
        }
        break;
    case GET:
        if (3 < argc - 1) quit(WRONG_ARGC, pid);

        queue_id = get_queue(queue, pid, CREATE);
        {
            struct msg msg;
            int read = msgrcv(queue_id, &msg, sizeof(msg.mtext), M_TYPE, IPC_NOWAIT);
            if (read < 0 && errno != ENOMSG) quit(QUEUE_RECEIVE_ERROR, pid);
            if (read > 0) {
                printf("%s\n", msg.mtext);
                fflush(stdout);
            }                
        }
        break;
    case DEL:
        if (3 < argc - 1) quit(WRONG_ARGC, pid);

        queue_id = get_queue(queue, pid, NO_CREATE);
        if (msgctl(queue_id, IPC_RMID, NULL) < 0)
            quit(QUEUE_DELITION_ERROR, pid);
        break;
    case EMP:
        if (3 < argc - 1) quit(WRONG_ARGC, pid);

        queue_id = get_queue(queue, pid, CREATE);
        {
            struct msg msg;
            int read;
            while ((read = msgrcv(queue_id, &msg, sizeof(msg.mtext), M_TYPE, IPC_NOWAIT)) >= 0) {
                printf("%s\n", msg.mtext);
                fflush(stdout);
            }
            if (errno != ENOMSG) quit(QUEUE_RECEIVE_ERROR, pid);
        }
        break;
    case MOV:
        if (3 == argc - 1) quit(WRONG_ARGC, pid);

        queue_id = get_queue(queue, pid, NO_CREATE);
        {
            char *dest_queue = argv[3];
            int dest_queue_id = get_queue(dest_queue, pid, CREATE);
            struct msg msg;
            int read;
            int msg_count = 0;
            while ((read = msgrcv(queue_id, &msg, sizeof(msg.mtext), M_TYPE, IPC_NOWAIT)) >= 0) {
                printf("%s\n", msg.mtext);
                fflush(stdout);
                if (msgsnd(dest_queue_id, &msg, sizeof(msg.mtext), 0) < 0)
                    quit(QUEUE_SEND_ERROR, pid);
                msg_count++;
            }
            if (errno != ENOMSG) quit(QUEUE_RECEIVE_ERROR, pid);
            printf("%d\n", msg_count);
            fflush(stdout);
            if (msgctl(queue_id, IPC_RMID, NULL) < 0)
            quit(QUEUE_DELITION_ERROR, pid);
        }
        break;
    default:
        quit(INVALID_ACTION, pid);
        break;
    }

    if (kill(pid, SIGUSR1) < 0) {} //quit(SIGNAL_ERROR, pid);
    return 0;
}

void quit(int error, int pid) {
    if (kill(pid, SIGUSR2) < 0) {}//exit(SIGNAL_ERROR);
    exit(error);
}

int parse_action(char *action) {
    if (strcmp(action, A_NEW) == 0) return NEW;
    if (strcmp(action, A_PUT) == 0) return PUT;
    if (strcmp(action, A_GET) == 0) return GET;
    if (strcmp(action, A_DEL) == 0) return DEL;
    if (strcmp(action, A_EMP) == 0) return EMP;
    if (strcmp(action, A_MOV) == 0) return MOV;

    return -1;
}

int get_queue(char *queue, int pid, short create) {
    if (access(queue, F_OK) != 0) {
        int fd = creat(queue, 0777);
        if (fd < 0) quit(FILE_CREATION_ERROR, pid);
        close(fd);
    }
    key_t key = ftok(queue, QUEUE_ID);
    if (key < 0) quit(KEY_CREATION_ERROR, pid);
    int id;
    if (create)
        id = msgget(key, 0777 | IPC_CREAT);
    else
        id = msgget(key, 0777);
    if (id < 0) quit(QUEUE_OPENING_ERROR, pid);
    return id;
}