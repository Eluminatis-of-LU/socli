#include <client.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <logger.h>

struct curl_slist *headers = NULL;
CURL *curl = NULL;
struct response response_body = {0};

static size_t
mem_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct response *mem = (struct response *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

#ifndef NDEBUG
    printf("Response: %s\n", mem->memory);
#endif

    return realsize;
}

void init_curl(void)
{
    LOG_TRACE("Initializing curl");
    curl = curl_easy_init();
    if (curl == NULL)
    {
        LOG_ERROR("Failed to initialize curl");
        exit(EXIT_FAILURE);
    }
    atexit(cleanup_curl);

    response_body.memory = malloc(0);
    response_body.size = 0;

    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, mem_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response_body);

    char *cookie = NULL;
    LOG_TRACE("Getting cookie from keyring");
    if (!sr_keychain_get_password(TARGET_URL, "socli", &cookie))
    {
        LOG_DEBUG("Cookie found in keyring.");
        LOG_TRACE("Cookie: %s", cookie);
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, cookie);
        free(cookie);
    }

    LOG_TRACE("Setting headers");
    LOG_TRACE("Accept: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    LOG_TRACE("Headers set");
    LOG_TRACE("Curl initialized");
}

void cleanup_curl(void)
{
    if (curl)
        curl_easy_cleanup(curl);
    if (headers)
        curl_slist_free_all(headers);
    if (response_body.memory)
        free(response_body.memory);
}

void clear_cookies(void)
{
    curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
}

void print_cookies(void)
{
    struct curl_slist *cookies;
    CURLcode rev = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
    if (cookies)
    {
        for (struct curl_slist *cookie = cookies; cookie; cookie = cookie->next)
        {
            printf("%s\n", cookie->data);
        }
        curl_slist_free_all(cookies);
    }
}

void save_cookies(void)
{
    struct curl_slist *cookies;
    curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
    if (cookies)
    {
        for (struct curl_slist *cookie = cookies; cookie; cookie = cookie->next)
        {
            if (sr_keychain_set_password(TARGET_URL, "socli", cookie->data))
            {
                LOG_WARN("Failed to save cookie to keyring.");
            }
        }
        curl_slist_free_all(cookies);
    }
}

size_t noop_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    return size * nmemb;
}