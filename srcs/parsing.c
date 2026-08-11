#include "codexion.h"

int is_valid_number(char *str)
{
    int i;

    i = 0;
    if (str[0] == '\0')
        return (0);
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return (0);
        i++;
    }
    return (1);
}

// Valida os 8 argumentos exigidos pelo enunciado:
// os 7 primeiros devem ser números válidos (sem sinal, sem letras),
// e o 8o deve ser exatamente "fifo" ou "edf".
// Também rejeita number_of_coders == 0, que quebraria os malloc depois.
int parse_args(int argc, char **argv)
{
    int i;

    (void)argc;
    i = 1;
    while (i <= 7)
    {
        if (!is_valid_number(argv[i]))
            return (0);
        i++;
    }
    if (atoi(argv[1]) < 1)
        return (0);
    if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
        return (0);
    return (1);
}