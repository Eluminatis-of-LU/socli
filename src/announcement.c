#include <announcement.h>
#include <client.h>
#include <stdbool.h>
#include <ketopt.h>

struct command make_announcement_command = {
    .name = "make-announcement",
    .desc = "make an announcement",
    .help = "make-announcement --message [message]",
    .sub = NULL,
    .func = make_announcement_command_func,
};

int make_announcement_command_func(struct command *cur, int argc, char **argv)
{
    static ko_longopt_t make_announcement_longopts[] = {
        {"message", 1, 0},
        {"help", 0, 0},
        {0, 0, 0}};
    ketopt_t opt = KETOPT_INIT;
    int c;
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
            exit(EXIT_FAILURE);
        }
        CURLcode http_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (!(http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK))
        {
            fprintf(stderr, "make announcement failed\n");
            exit(EXIT_FAILURE);
        }
        printf("make announcement success\n");
    }
    return 0;
}