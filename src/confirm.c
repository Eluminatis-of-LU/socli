#include <confirm.h>
#include <stdio.h>
#include <stdlib.h>

const char *random_5_characters()
{
    static char str[6];
    srand(time(NULL));
    for (int i = 0; i < 5; i++)
    {
        str[i] = 'A' + rand() % 26;
    }
    str[5] = '\0';
    return str;
}

bool confirm_command(const char *command_name)
{
    char *validate = random_5_characters();
    printf("%s is a write operation. Please type \'%s\' to confirm.\n", command_name, validate);
    char input[6];
    scanf("%5s", input);
    return (strcmp(input, validate) == 0);
}