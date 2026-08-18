#include "codexion.h"

// Returns 1 if the string contains only digits, otherwise returns 0.
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

// Validates the 8 arguments required by the assignment:
// the first 7 must be valid numbers (no sign, no letters), and the
// 8th must be exactly "fifo" or "edf".
// Also rejects number_of_coders == 0, which would break the mallocs later.
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