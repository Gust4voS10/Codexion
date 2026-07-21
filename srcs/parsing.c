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