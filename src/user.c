#include <user.h>
#include <client.h>
#include <stdbool.h>
#include <ketopt.h>
#include <stb_ds.h>
#include <confirm.h>
#include <logger.h>
#include <kson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int user_get_command_func(struct command *cur, int argc, char **argv)
{
    static ko_longopt_t user_get_longopts[] = {
        {"help", 0, 0},
        {0, 0, 0}};
    ketopt_t opt = KETOPT_INIT;
    int c;
    bool help = false;
    while ((c = ketopt(&opt, argc + 1, argv - 1, 1, "h", user_get_longopts)) != -1)
    {
        switch (c)
        {
        case 'h':
            help = true;
            break;
        default:
            break;
        }
        switch (opt.longidx)
        {
        case 0:
            help = true;
            break;
        default:
            break;
        }
    }
    int remaining_args = (argc + 1) - opt.ind;
    char *uid = NULL;
    if (remaining_args > 0)
        uid = argv[opt.ind - 1];
    help = help || !uid;
    if (help)
    {
        print_help(cur);
        return 0;
    }
    char url[1024];
    snprintf(url, sizeof(url), "%s/api/v1/user/%s", g_target_url, uid);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        LOG_ERROR("Failed to get user, curl_easy_perform() failed: %s", curl_easy_strerror(res));
        exit(EXIT_FAILURE);
    }
    CURLcode http_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (!(http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK))
    {
        LOG_ERROR("Failed to get user, http_code: %d", http_code);
        exit(EXIT_FAILURE);
    }
    kson_t *kson = kson_parse(response_body.memory);
    if (kson == NULL)
    {
        LOG_ERROR("Failed to parse response: %s", response_body.memory);
        exit(EXIT_FAILURE);
    }
    const kson_node_t *user_id = kson_by_key(kson->root, "_id");
    const kson_node_t *user_uname = kson_by_key(kson->root, "uname");
    const kson_node_t *user_mail = kson_by_key(kson->root, "mail");
    if (user_id)
        printf("ID: %s\n", user_id->v.str);
    if (user_uname)
        printf("Username: %s\n", user_uname->v.str);
    if (user_mail)
        printf("Email: %s\n", user_mail->v.str);
    kson_destroy(kson);
    return 0;
}

int user_create_command_func(struct command *cur, int argc, char **argv)
{
    static ko_longopt_t user_create_longopts[] = {
        {"uname", 1, 0},
        {"password", 1, 0},
        {"email", 1, 0},
        {"help", 0, 0},
        {0, 0, 0}};
    ketopt_t opt = KETOPT_INIT;
    int c;
    bool help = false;
    char *uname = NULL, *password = NULL, *email = NULL;
    while ((c = ketopt(&opt, argc + 1, argv - 1, 1, "u:p:e:h", user_create_longopts)) != -1)
    {
        switch (c)
        {
        case 'u':
            uname = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
            break;
        case 'p':
            password = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
            break;
        case 'e':
            email = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
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
            uname = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
            break;
        case 1:
            password = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
            break;
        case 2:
            email = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
            break;
        case 3:
            help = true;
            break;
        default:
            break;
        }
    }
    help = help || !uname || !password || !email;
    if (help)
    {
        print_help(cur);
        if (uname)
            curl_free(uname);
        if (password)
            curl_free(password);
        if (email)
            curl_free(email);
        return 0;
    }
    if (!confirm_command(cur->name))
    {
        curl_free(uname);
        curl_free(password);
        curl_free(email);
        return 0;
    }
    char postdata[1024];
    snprintf(postdata, 1023, "uname=%s&password=%s&mail=%s", uname, password, email);
    curl_free(uname);
    curl_free(password);
    curl_free(email);
    char url[1024];
    snprintf(url, sizeof(url), "%s/api/v1/user", g_target_url);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        LOG_ERROR("Failed to create user, curl_easy_perform() failed: %s", curl_easy_strerror(res));
        exit(EXIT_FAILURE);
    }
    CURLcode http_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (!(http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK))
    {
        LOG_ERROR("Failed to create user, http_code: %d", http_code);
        exit(EXIT_FAILURE);
    }
    printf("create user success\n");
    return 0;
}

int user_update_command_func(struct command *cur, int argc, char **argv)
{
    static ko_longopt_t user_update_longopts[] = {
        {"email", 1, 0},
        {"help", 0, 0},
        {0, 0, 0}};
    ketopt_t opt = KETOPT_INIT;
    int c;
    bool help = false;
    char *email = NULL;
    while ((c = ketopt(&opt, argc + 1, argv - 1, 1, "e:h", user_update_longopts)) != -1)
    {
        switch (c)
        {
        case 'e':
            email = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
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
            email = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
            break;
        case 1:
            help = true;
            break;
        default:
            break;
        }
    }
    int remaining_args = (argc + 1) - opt.ind;
    char *uid = NULL;
    if (remaining_args > 0)
        uid = argv[opt.ind - 1];
    help = help || !uid || !email;
    if (help)
    {
        print_help(cur);
        if (email)
            curl_free(email);
        return 0;
    }
    if (!confirm_command(cur->name))
    {
        curl_free(email);
        return 0;
    }
    char postdata[1024];
    snprintf(postdata, 1023, "mail=%s", email);
    curl_free(email);
    char url[1024];
    snprintf(url, sizeof(url), "%s/api/v1/user/%s", g_target_url, uid);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        LOG_ERROR("Failed to update user, curl_easy_perform() failed: %s", curl_easy_strerror(res));
        exit(EXIT_FAILURE);
    }
    CURLcode http_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (!(http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK))
    {
        LOG_ERROR("Failed to update user, http_code: %d", http_code);
        exit(EXIT_FAILURE);
    }
    printf("update user success\n");
    return 0;
}

int user_password_command_func(struct command *cur, int argc, char **argv)
{
    static ko_longopt_t user_password_longopts[] = {
        {"password", 1, 0},
        {"help", 0, 0},
        {0, 0, 0}};
    ketopt_t opt = KETOPT_INIT;
    int c;
    bool help = false;
    char *password = NULL;
    while ((c = ketopt(&opt, argc + 1, argv - 1, 1, "p:h", user_password_longopts)) != -1)
    {
        switch (c)
        {
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
            password = curl_easy_escape(curl, opt.arg, strlen(opt.arg));
            break;
        case 1:
            help = true;
            break;
        default:
            break;
        }
    }
    int remaining_args = (argc + 1) - opt.ind;
    char *uid = NULL;
    if (remaining_args > 0)
        uid = argv[opt.ind - 1];
    help = help || !uid || !password;
    if (help)
    {
        print_help(cur);
        if (password)
            curl_free(password);
        return 0;
    }
    if (!confirm_command(cur->name))
    {
        curl_free(password);
        return 0;
    }
    char postdata[1024];
    snprintf(postdata, 1023, "password=%s", password);
    curl_free(password);
    char url[1024];
    snprintf(url, sizeof(url), "%s/api/v1/user/%s/password", g_target_url, uid);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        LOG_ERROR("Failed to change password, curl_easy_perform() failed: %s", curl_easy_strerror(res));
        exit(EXIT_FAILURE);
    }
    CURLcode http_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (!(http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK))
    {
        LOG_ERROR("Failed to change password, http_code: %d", http_code);
        exit(EXIT_FAILURE);
    }
    printf("change password success\n");
    return 0;
}

struct command user_get_command = {
    .name = "get",
    .desc = "get user public info by UID",
    .help = "\nUsage: socli user get <uid>\n\n",
    .sub = NULL,
    .func = user_get_command_func,
};

struct command user_create_command = {
    .name = "create",
    .desc = "create a new user",
    .help = "\nUsage: socli user create --uname <uname> --password <password> --email <email>\n\n",
    .sub = NULL,
    .func = user_create_command_func,
};

struct command user_update_command = {
    .name = "update",
    .desc = "update user email",
    .help = "\nUsage: socli user update <uid> --email <email>\n\n",
    .sub = NULL,
    .func = user_update_command_func,
};

struct command user_password_command = {
    .name = "password",
    .desc = "change a user's password",
    .help = "\nUsage: socli user password <uid> --password <password>\n\n",
    .sub = NULL,
    .func = user_password_command_func,
};

struct command user_command = {
    .name = "user",
    .desc = "manage users",
    .help = "\nUsage: socli user [command] [options]\n\nCommands:\n",
    .sub = NULL,
    .func = print_help_and_traverse,
};

struct command *init_user_command(void)
{
    arrpush(user_command.sub, &user_get_command);
    arrpush(user_command.sub, &user_create_command);
    arrpush(user_command.sub, &user_update_command);
    arrpush(user_command.sub, &user_password_command);
    return &user_command;
}
