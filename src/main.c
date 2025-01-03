#include <stdio.h>
#include <curl/curl.h>

int main(int argc, char **argv)
{
    printf("Hello, World!\n");
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        fprintf(stderr, "Failed to initialize curl\n");
        return 1;
    }

    CURLcode res = curl_easy_setopt(curl, CURLOPT_URL, "https://www.example.com");

    if (res != CURLE_OK) {
        fprintf(stderr, "Failed to set URL: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return 1;
    }

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Failed to perform request: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return 1;
    }

    printf("Request successful\n");

    curl_easy_cleanup(curl);
    return 0;
}
