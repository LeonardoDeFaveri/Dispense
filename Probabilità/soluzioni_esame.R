# Esercizio 1
media <- 12.1
ds <- 2.57
n1 <- 24
risp_1 <- pnorm(q = 306.54, mean = media * n1, sd = ds * sqrt(n1)) -
  pnorm(q = 295.25, mean = media * n1, sd = ds * sqrt(n1))

# Esercizio 2
lambda <- 1.7
risp_2 <- dpois(3, lambda) / dpois(1, lambda)
# risp_2 <- dpois(3, lambda) / ppois(0, lambda, lower.tail = FALSE) Giusta

indici <- 0:1000
prob_non_1_quartiere_per_i_nubifragi <- function(i) {
  dpois(i, lambda) * (1 - dunif(1, 1, 8))^i
}
risp_3 <- sum(prob_non_1_quartiere_per_i_nubifragi(indici))

# Esercizio 3
x <- c(5.63, 7.03, 4.44, 9.40, 7.20, 4.76, 3.60, 5.13, 8.03, 7.48, 2.60)
n3 = length(x)
alpha <- 1 - 0.95
media3 <- 4.49
risp_4 <- sum((x - media3)^2) / n3
risp_5 <- risp_4 * n3 / qchisq(p = alpha / 2, df = n3)

# Esercizio 4
p <- 2 / 7
val1 <- phyper(0, k = 4, m = 2, n = 5, lower.tail = FALSE)
risp_6 <- 2 * val1 - val1 ^ 2
risp_7 <- phyper(0, k = 8, m = 4, n = 10, lower.tail = FALSE)
p2 <- 4 / 14
risp_8 <- pbinom(0, 8, p2, lower.tail = FALSE)
