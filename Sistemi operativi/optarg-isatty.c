int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "cl")) != -1) {
        switch (c)
        {
        case 'c':
            if (char_count) send_error(DUP_ARG, c);
            char_count = 1;
            before_char = (char_count == 1 && row_count == 0);
            break;
            
        case 'l':
            if (row_count) send_error(DUP_ARG, c);
            row_count = 1;
            before_char = !(row_count == 1 && char_count == 0);
            break;

        default:
            send_error(WRONG_ARG, optopt);
        }
    }

    if (optind < argc) {
        file_name = argv[optind];

        int fd = open(file_name, O_RDONLY);
        if (fd < 0) {
            send_error(WRONG_FILE, file_name);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    if (isatty(STDIN_FILENO)) {
        // È true se il programma ha come STDIN la bash
        send_error(MISSING_INPUT);
    }
