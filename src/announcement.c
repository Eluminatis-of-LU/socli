#include <announcement.h>
#include <client.h>
#include <stdbool.h>
#include <ketopt.h>
#include <stb_ds.h>
#include <confirm.h>
#include <logger.h>

int announcement_create_command_func(struct command *cur, int argc, char **argv)
{
    static ko_longopt_t announcement_create_longopts[] = {
        {"message", 1, 0},
        {"help", 0, 0},
        {0, 0, 0}};
    ketopt_t opt = KETOPT_INIT;
    int c;
    bool help = false;
    char *message = NULL;
    while ((c = ketopt(&opt, argc + 1, argv - 1, 1, "m:h", announcement_create_longopts)) != -1)
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
        if (message)
            curl_free(message);
        if (!confirm_command(cur->name))
        {
            return 0;
        }
        curl_easy_setopt(curl, CURLOPT_URL, target_url("/api/v1/announcement"));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            LOG_ERROR("Failed to make announcement, curl_easy_perform() failed: %s", curl_easy_strerror(res));
            exit(EXIT_FAILURE);
        }
        CURLcode http_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (!(http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK))
        {
            LOG_ERROR("Failed to make announcement: http_code: %d", http_code);
            exit(EXIT_FAILURE);
        }
        printf("make announcement success\n");
    }
    return 0;
}

struct command announcement_command = {
    .name = "announcement",
    .desc = "manage announcements",
    .help = "\nUsage: socli announcement [command] [options]\n\nCommands:\n",
    .sub = NULL,
    .func = print_help_and_traverse,
};

struct command announcement_create_command = {
    .name = "create",
    .desc = "create an announcement",
    .help = "create --message",
    .sub = NULL,
    .func = announcement_create_command_func,
};

struct command *init_announcement_command(void)
{
    arrpush(announcement_command.sub, &announcement_create_command);
    return &announcement_command;
}