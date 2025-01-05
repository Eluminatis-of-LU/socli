#include <login.h>
#include <ketopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <client.h>

char *username = NULL, *password = NULL;

void free_login_memory(void)
{
    if (username)
        curl_free(username);
    if (password)
        curl_free(password);
}

int login_command_func(struct command *cur, int argc, char **argv)
{
    atexit(free_login_memory);
    static ko_longopt_t login_longopts[] = {
        {"username", 1, 0},
        {"password", 1, 0},
        {"help", 0, 0},
        {0, 0, 0}};
    ketopt_t opt = KETOPT_INIT;
    int c;
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
            exit(EXIT_FAILURE);
        }
        CURLcode http_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (!(http_code == 302 && res != CURLE_ABORTED_BY_CALLBACK))
        {
            fprintf(stderr, "login failed\n");
            exit(EXIT_FAILURE);
        }
        printf("login success\n");
        save_cookies();
    }
    return 0;
}

struct command login_command = {
    .name = "login",
    .desc = "login to SeriousOJ",
    .help = "login --username --password",
    .sub = NULL,
    .func = login_command_func,
};


struct command* init_login_command(void)
{
    return &login_command;
}