#include <client.h>
#include <stdio.h>

void init_curl(void)
{
    curl = curl_easy_init();
    atexit(cleanup_curl);
    if (curl == NULL)
    {
        fprintf(stderr, "Failed to initialize curl\n");
        exit(EXIT_FAILURE);
    }

    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");

    char *cookie = NULL;
    if (!sr_keychain_get_password(TARGET_URL, "socli", &cookie))
    {
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, cookie);
        free(cookie);
    }

#ifdef NDEBUG
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, noop_write_callback);
#else
#endif
}

void cleanup_curl(void)
{
    if (curl)
        curl_easy_cleanup(curl);
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
                fprintf(stderr, "Failed to save cookie to keyring.\n");
            }
        }
        curl_slist_free_all(cookies);
    }
}

size_t noop_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    return size * nmemb;
}