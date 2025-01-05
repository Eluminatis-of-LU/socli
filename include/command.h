#ifndef COMMAND_H
#define COMMAND_H

struct command
{
    const char *name;
    const char *desc;
    const char *help;
    struct command *sub;
    int (*func)(struct command *cur, int argc, char **argv);
};

void print_help(struct command *cmd);

int print_help_and_traverse(struct command *cur, int argc, char **argv);

void cleanup_commands(struct command *cmd);

#endif // COMMAND_H