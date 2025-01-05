#include <contest.h>
#include <stdlib.h>
#include <stb_ds.h>
#include <stdio.h>
#include <client.h>

int contest_list_command_func(struct command *cur, int argc, char **argv)
{
    printf("list\n");
    return 0;
}

struct command contest_list_command = {
    .name = "list",
    .desc = "list all contests",
    .help = "\nUsage: socli contest list [--page]\n\n",
    .sub = NULL,
    .func = print_help_and_traverse};

struct command contest_command = {
    .name = "contest",
    .desc = "manage contests",
    .help = "\nUsage: socli contest [command] [options]\n\nCommands:\n",
    .sub = NULL,
    .func = print_help_and_traverse};

struct command *init_contest_command(void)
{
    arrpush(contest_command.sub, &contest_list_command);
    return &contest_command;
}
