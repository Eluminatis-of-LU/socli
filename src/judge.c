#include <judge.h>
#include <stdlib.h>
#include <stb_ds.h>
#include <stdio.h>
#include <client.h>
#include <kson.h>

int judge_list_command_func(struct command *self, int argc, char **argv)
{
    static char url[1024];
    snprintf(url, 1023, target_url("/judge"));
    curl_easy_setopt(curl, CURLOPT_URL, url);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "Failed to list judges: %s\n", curl_easy_strerror(res));
        exit(EXIT_FAILURE);
    }
    CURLcode http_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (!(http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK))
    {
        fprintf(stderr, "Failed to list judges: %s\n", curl_easy_strerror(res));
        exit(EXIT_FAILURE);
    }
    kson_t *kson = kson_parse(response_body.memory);
    if (kson == NULL)
    {
        fprintf(stderr, "Failed to parse response.\n");
        exit(EXIT_FAILURE);
    }
    const kson_node_t *judges = kson_by_key(kson->root, "judges");
    if (judges == NULL || judges->n == 0)
    {
        printf("No judges found.\n");
        kson_destroy(kson);
        exit(EXIT_SUCCESS);
    }    
    for (int i = 0; i < judges->n; i++)
    {
        const kson_node_t *judge = judges->v.child[i];
        const kson_node_t *id = kson_by_key(judge, "_id");
        const kson_node_t *concurrency = kson_by_key(judge, "concurrency");
        const kson_node_t *version = kson_by_key(judge, "version");
        const kson_node_t *expire_at = kson_by_key(judge, "expire_at");        
        time_t expire_time = ((time_t)atoll(expire_at->v.str)) / 1000;
        printf("ID: %s\n", id->v.str);
        printf("Concurrency: %s\n", concurrency->v.str);
        printf("Version: %s\n", version->v.str);
        printf("Expire At: %s", ctime(&expire_time));
        printf("\n");
    }

    kson_destroy(kson);
    return 0;
}

struct command judge_command = {
    .name = "judge",
    .desc = "manage judges",
    .help = "\nUsage: socli judge [command] [options]\n\nCommands:\n",
    .sub = NULL,
    .func = print_help_and_traverse
};

struct command judge_list_command = {
    .name = "list",
    .desc = "list judges",
    .help = "\nUsage: socli judge list\n\nList all judges.\n",
    .sub = NULL,
    .func = judge_list_command_func
};

struct command *init_judge_command(void)
{
    arrpush(judge_command.sub, &judge_list_command);
    return &judge_command;
}
