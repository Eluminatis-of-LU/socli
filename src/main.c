#include <stdio.h>
#include <stdlib.h>
#include <client.h>
#include <ketopt.h>
#include <sr_keychain.h>
#include <command.h>
#include <login.h>
#include <announcement.h>
#include <contest.h>
#include <judge.h>
#include <logger.h>
#include <version.h>

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
    .func = print_help_and_traverse};

int version_command_func(struct command *cur, int argc, char **argv)
{
    printf("socli version %s\n", PROJECT_VERSION_STR);
    printf("Targert URL: %s\n", TARGET_URL);
    printf("Built with %s\n", curl_version());
    printf("Built with sr_keychain\n");
    printf("Built with stb_ds\n");
    printf("Built with stb_leakcheck\n");
    printf("Built with ketopt\n");
    printf("Built with kson\n");
    printf("Built with c-logger\n");
    printf("Built on %s %s\n", __DATE__, __TIME__);
    return 0;
}

struct command root_version_command = {
    .name = "version",
    .desc = "show version",
    .help = "\nUsage: socli version\n\n",
    .sub = NULL,
    .func = version_command_func};

void cleanup_memory(void)
{
    cleanup_commands(&root_command);
}

LogLevel get_log_level(void)
{
    LogLevel verbose = LogLevel_INFO;
    char *verbose_env = getenv("SOC_LOG_LEVEL");
    if (verbose_env)
    {
        verbose = (LogLevel)atoi(verbose_env);
    }
    free(verbose_env);
    return verbose;
}

void set_logger(void)
{
    logger_initConsoleLogger(stderr);
    LogLevel log_level = get_log_level();
    printf("Log level: %d\n", log_level);
    logger_setLevel(log_level);
}

int main(int argc, char **argv)
{
    set_logger();

    atexit(cleanup_memory);

    init_curl();

    arrpush(root_command.sub, init_announcement_command());
    arrpush(root_command.sub, init_contest_command());
    arrpush(root_command.sub, init_judge_command());
    arrpush(root_command.sub, init_login_command());
    arrpush(root_command.sub, &root_version_command);

    return root_command.func(&root_command, argc - 1, argv + 1);
}
