#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "ketopt.h"
#ifndef NDEBUG
#define STB_LEAKCHECK_IMPLEMENTATION
#define STB_LEAKCHECK_SHOWALL
#include "stb_leakcheck.h"
#define TARGET_URL "http://judge_ui:8888"
#else
#define TARGET_URL "https://judge.eluminatis-of-lu.com"
#endif
#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define target_url(x) TARGET_URL "/" x

CURL *curl;
char *username = NULL, *password = NULL;

struct command
{
    const char *name;
    const char *desc;
    const char *help;
    struct command *sub;
    int (*func)(struct command *cur, int argc, char **argv);
};

struct command root_command = {
    .name = "socli",
    .desc = "socli is a command line tool for managing SeriousOJ.",
    .help = "\nUsage: socli [command] [options]\n\nCommands:\n",
    .sub = NULL,
    .func = NULL};

void socli_exit(int exitcode)
{
    curl_easy_cleanup(curl);
    exit(exitcode);
}

void print_help(struct command *cmd)
{
    printf("%s\n", cmd->name);
    printf("%s\n", cmd->desc);
    printf("%s\n", cmd->help);
    for (int i = 0; i < arrlen(cmd->sub); ++i)
    {
        printf("  %s\n", cmd->sub[i].name);
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
        if (strcmp(argv[0], cur->sub[i].name) == 0)
        {
            return cur->sub[i].func(&cur->sub[i], argc - 1, argv + 1);
        }
    }
    return 0;
}

int login_command_func(struct command *cur, int argc, char **argv)
{
    static ko_longopt_t login_longopts[] = {
        {"username", 1, 0, 'u'},
        {"password", 1, 0, 'p'},
        {"help", 0, 0, 'h'},
        {0, 0, 0, 0}};
    ketopt_t opt = KETOPT_INIT;
    int i, c;
    bool help = false;
    while ((c = ketopt(&opt, argc + 1, argv - 1, 1, "u:p:h", login_longopts)) != -1)
    {
        switch (c)
        {
        case 'u':
            username = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
            break;
        case 'p':
            password = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
            break;
        case 'h':
            help = true;
            break;
        default:
            break;
        }
        switch (opt.longidx)
        {
        case 0:
            username = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
            break;
        case 1:
            password = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
            break;
        case 2:
            help = true;
            break;
        default:
            break;
        }
    }
    help = help || !username || !password;
    if (help)
    {
        print_help(cur);
    }
    if (!help)
    {
        char postdata[1024];
        snprintf(postdata, 1023, "uname=%s&password=%s", username, password);
        curl_easy_setopt(curl, CURLOPT_URL, target_url("login"));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            socli_exit(EXIT_FAILURE);
        }
        CURLcode http_code;
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (!(http_code == 302 && res != CURLE_ABORTED_BY_CALLBACK))
        {
            fprintf(stderr, "login failed\n");
            socli_exit(EXIT_FAILURE);
        }
        printf("login success\n");
    }
    return 0;
}

int make_announcement_command_func(struct command *cur, int argc, char **argv)
{
    static ko_longopt_t make_announcement_longopts[] = {
        {"message", 1, 0, 'm'},
        {"help", 0, 0, 'h'},
        {0, 0, 0, 0}};
    ketopt_t opt = KETOPT_INIT;
    int i, c;
    bool help = false;
    char *message = NULL;
    while ((c = ketopt(&opt, argc + 1, argv - 1, 1, "m:h", make_announcement_longopts)) != -1)
    {
        switch (c)
        {
        case 'm':
            message = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
            break;
        case 'h':
            help = true;
            break;
        default:
            break;
        }
        switch (opt.longidx)
        {
        case 0:
            message = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
            break;
        case 1:
            help = true;
            break;
        default:
            break;
        }
    }
    help = help || !message;
    if (help)
    {
        print_help(cur);
    }
    if (!help)
    {
        char postdata[1024];
        snprintf(postdata, 1023, "message=%s", message);
        curl_easy_setopt(curl, CURLOPT_URL, target_url("api/v1/announcement"));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            socli_exit(EXIT_FAILURE);
        }
        CURLcode http_code;
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (!(http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK))
        {
            fprintf(stderr, "make announcement failed\n");
            socli_exit(EXIT_FAILURE);
        }
        printf("make announcement success\n");
    }
    return 0;
}

size_t noop_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    return size * nmemb;
}

const char* get_home_directory()
{
#ifdef _WIN32
    static char home_dir[MAX_PATH];

    if (GetEnvironmentVariable("USERPROFILE", home_dir, MAX_PATH))
    {
        return home_dir;
    }
    else
    {
        return NULL;
    }
#else
    return getenv("HOME");
#endif
}

void cleanup_commands(struct command *cmd)
{
    for (int i = 0; i < arrlen(cmd->sub); ++i)
    {
        cleanup_commands(&cmd->sub[i]);
    }
    arrfree(cmd->sub);
}

void cleanup_memory()
{
    if (username != NULL)
    {
        free(username);
        username = NULL;
    }
    if (password != NULL)
    {
        free(password);
        password = NULL;
    }

    cleanup_commands(&root_command);
}

int main(int argc, char **argv)
{
    atexit(cleanup_memory);

    curl = curl_easy_init();
    if (curl == NULL)
    {
        fprintf(stderr, "Failed to initialize curl\n");
        return 1;
    }

    char COOKIEJAR[1024];

    snprintf(COOKIEJAR, 1023, "%s/.socli_cookie", get_home_directory());

    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, COOKIEJAR);
    curl_easy_perform(curl);
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, COOKIEJAR);

#ifdef NDEBUG
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, noop_write_callback);
#else
#endif

    struct command login_command = {
        .name = "login",
        .desc = "login to SeriousOJ",
        .help = "login --username [username] --password [password]",
        .sub = NULL,
        .func = login_command_func,
    };

    struct command make_announcement_command = {
        .name = "make-announcement",
        .desc = "make an announcement",
        .help = "make-announcement --message [message]",
        .sub = NULL,
        .func = make_announcement_command_func,
    };

    arrpush(root_command.sub, login_command);
    arrpush(root_command.sub, make_announcement_command);

    root_command.func = print_help_and_traverse;
    root_command.func(&root_command, argc - 1, argv + 1);
    
    socli_exit(EXIT_SUCCESS);
    return 0;
}
