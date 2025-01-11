#ifndef CLIENT_H
#define CLIENT_H

#include <curl/curl.h>
#include <sr_keychain.h>

#ifndef NDEBUG
#define TARGET_URL "http://judge_ui:8888"
#else
#define TARGET_URL "https://judge.eluminatis-of-lu.com"
#endif

#define target_url(x) TARGET_URL x

struct response
{
    char *memory;
    size_t size;
};
extern struct response response_body;
extern CURL *curl;

void init_curl(void);
void cleanup_curl(void);
void print_cookies(void);
void save_cookies(void);
size_t noop_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

#endif // CLIENT_H