#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "ketopt.h"

struct command
{
    const char *name;
    const char *desc;
    const char *help;
    struct command **sub;
    int sub_count;
    int (*func)(int argc, char **argv);
};

struct command root_command = {
    .name = "socli",
    .desc = "socli is a command line tool for managing SeriousOJ.",
    .help = "\nUsage: socli [command] [options]\n\nCommands:\n",
    .sub = NULL,
    .sub_count = 0,
    .func = NULL};

void make_request()
{
    CURL *curl = curl_easy_init();
    if (curl == NULL)
    {
        fprintf(stderr, "Failed to initialize curl\n");
        return 1;
    }

    CURLcode res = curl_easy_setopt(curl, CURLOPT_URL, "https://www.example.com");

    if (res != CURLE_OK)
    {
        fprintf(stderr, "Failed to set URL: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return 1;
    }

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        fprintf(stderr, "Failed to perform request: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return 1;
    }

    printf("Request successful\n");

    curl_easy_cleanup(curl);
}

void print_help(struct command *cmd)
{
    printf("%s\n", cmd->name);
    printf("%s\n", cmd->desc);
    printf("%s\n", cmd->help);
    for (int i = 0; i < cmd->sub_count; ++i)
    {
        printf("  %s\n", cmd->sub[i]->name);
    }
}

int root_command_func(int argc, char **argv)
{
    if (argc == 0)
    {
        print_help(&root_command);
        return 0;
    }
    return 0;
}

int main(int argc, char **argv)
{
    root_command.sub = malloc(sizeof(struct command *) * 2);
    root_command.sub[0] = malloc(sizeof(struct command));
    root_command.sub[0]->name = "test";
    root_command.sub[0]->desc = "test command";
    root_command.sub[0]->help = "test help";
    root_command.sub[0]->sub = NULL;
    root_command.sub[0]->sub_count = 0;
    root_command.sub[0]->func = NULL;
    root_command.sub[1] = NULL;
    root_command.sub_count = 1;
    root_command.func = root_command_func;
    root_command.func(argc - 1, argv + 1);
    free(root_command.sub[0]);
    free(root_command.sub);
    return 0;
}
