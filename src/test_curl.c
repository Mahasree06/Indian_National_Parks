#include <stdio.h>
#include <curl/curl.h>

int main()
{
    CURL *curl;
    CURLcode res;
    FILE *fp;

    curl = curl_easy_init();

    if (curl == NULL)
    {
        printf("Failed to initialize curl\n");
        return 1;
    }

    fp = fopen("download.html", "w");

    if (fp == NULL)
    {
        printf("Failed to create download.html\n");
        curl_easy_cleanup(curl);
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL,
        "https://en.wikipedia.org/wiki/List_of_national_parks_of_India");

    /* Identify our program to Wikipedia */
    curl_easy_setopt(curl, CURLOPT_USERAGENT,
        "IndianNationalParksDataAnalyzer/1.0");

    /* Follow redirects */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    /* Write downloaded data into the file */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        printf("Download failed: %s\n",
               curl_easy_strerror(res));
    }
    else
    {
        printf("Wikipedia page downloaded successfully\n");
    }

    fclose(fp);
    curl_easy_cleanup(curl);

    return 0;
}
