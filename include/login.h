#ifndef LOGIN_H
#define LOGIN_H
#include <command.h>

int login_command_func(struct command *cur, int argc, char **argv);

struct command login_command;

#endif // LOGIN_H