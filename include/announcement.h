#ifndef ANNOUNCEMENT_H
#define ANNOUNCEMENT_H
#include <command.h>

int make_announcement_command_func(struct command *cur, int argc, char **argv);

struct command make_announcement_command;

#endif // ANNOUNCEMENT_H