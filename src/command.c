#include <stdio.h>
#include <stdlib.h>
#include <stb_ds.h>
#include <command.h>
#ifndef NDEBUG
#include <stb_leakcheck.h>
#endif

void print_help(struct command *cmd)
{
    printf("%s\n", cmd->name);
    printf("%s\n", cmd->desc);
    printf("%s\n", cmd->help);
    for (int i = 0; i < arrlen(cmd->sub); ++i)
    {
        printf("  %s\n", cmd->sub[i]->name);
    }
}

int print_help_and_traverse(struct command *cur, int argc, char **argv)
{
    if (argc == 0)
    {
        print_help(cur);
        return 0;
    }
    for (int i = 0; i < arrlen(cur->sub); ++i)
    {
        if (strcmp(argv[0], cur->sub[i]->name) == 0)
        {
            return cur->sub[i]->func(cur->sub[i], argc - 1, argv + 1);
        }
    }
    print_help(cur);
    return -1;
}

void cleanup_commands(struct command *cmd)
{
    for (int i = 0; i < arrlen(cmd->sub); ++i)
    {
        cleanup_commands(cmd->sub[i]);
    }
    arrfree(cmd->sub);
}