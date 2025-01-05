#include <stdio.h>
#include <stdlib.h>
#include <client.h>
#include <ketopt.h>
#include <sr_keychain.h>
#include <command.h>
#include <login.h>
#include <announcement.h>

#ifndef NDEBUG
#define STB_LEAKCHECK_IMPLEMENTATION
#define STB_LEAKCHECK_SHOWALL
#include <stb_leakcheck.h>
#endif

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

struct command root_command = {
    .name = "socli",
    .desc = "socli is a command line tool for managing SeriousOJ.",
    .help = "\nUsage: socli [command] [options]\n\nCommands:\n",
    .sub = NULL,
    .func = NULL};

void cleanup_memory(void)
{
    cleanup_commands(&root_command);
}

int main(int argc, char **argv)
{
    printf("URL: %s\n", TARGET_URL);
    atexit(cleanup_memory);

    init_curl();

    arrpush(root_command.sub, login_command);
    arrpush(root_command.sub, make_announcement_command);

    root_command.func = print_help_and_traverse;
    return root_command.func(&root_command, argc - 1, argv + 1);
}
