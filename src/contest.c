#include <contest.h>
#include <stdlib.h>
#include <stb_ds.h>
#include <stdio.h>
#include <client.h>
#include <ketopt.h>
#include <stdbool.h>
#include <kson.h>
#include <time.h>

int contest_list_command_func(struct command *cur, int argc, char **argv)
{
    static ko_longopt_t contest_list_longopts[] = {
        {"page", 1, 0},
        {"help", 0, 0},
        {0, 0, 0}};
    ketopt_t opt = KETOPT_INIT;
    int c;
    bool help = false;
    int page = 1;
    while ((c = ketopt(&opt, argc + 1, argv - 1, 1, "p:h", contest_list_longopts)) != -1)
    {
        switch (c)
        {
        case 'p':
            page = atoi(opt.arg);
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
            page = atoi(opt.arg);
            break;
        case 1:
            help = true;
            break;
        default:
            break;
        }
    }
    if (help)
    {
        print_help(cur);
        return 0;
    }
    static char url[1024];
    snprintf(url, 1023, target_url("/api/v1/contest?page=%d"), page);
    curl_easy_setopt(curl, CURLOPT_URL, url);
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
        fprintf(stderr, "Failed to list contests: %s\n", curl_easy_strerror(res));
        exit(EXIT_FAILURE);
    }
    kson_t *kson = kson_parse(response_body.memory);

    if (kson == NULL)
    {
        fprintf(stderr, "Failed to parse JSON: %s\n", response_body.memory);
        exit(EXIT_FAILURE);
    }

    const kson_node_t *contests;

    contests = kson_by_key(kson->root, "contests");

    for (long i = 0; i < contests->n; ++i)
    {
        const kson_node_t *contest = kson_by_index(contests, i);
        const kson_node_t *contest_id = kson_by_key(contest, "_id");
        const kson_node_t *contest_name = kson_by_key(contest, "title");
        const kson_node_t *contest_start_time = kson_by_key(contest, "begin_at");
        const kson_node_t *contest_end_time = kson_by_key(contest, "end_at");
        const kson_node_t *contest_attendance = kson_by_key(contest, "attend");
        const kson_node_t *contest_hidden = kson_by_key(contest, "hidden");
        printf("ID: %s\n", contest_id->v.str);
        printf("Name: %s ", contest_name->v.str);
        if (contest_hidden && strcmp(contest_hidden->v.str, "true") == 0)
            printf("(Hidden)\n");
        else
            printf("\n");
        time_t start_time = ((time_t)atoll(contest_start_time->v.str)) / 1000;
        time_t end_time = ((time_t)atoll(contest_end_time->v.str)) / 1000;
        printf("Start Time: %s", ctime(&start_time));
        printf("End Time: %s", ctime(&end_time));
        printf("Attendance: %s\n", contest_attendance->v.str);
        printf("\n");
    }

    kson_destroy(kson);

    return 0;
}

struct command contest_list_command = {
    .name = "list",
    .desc = "list all contests",
    .help = "\nUsage: socli contest list [--page]\n\n",
    .sub = NULL,
    .func = contest_list_command_func};

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
