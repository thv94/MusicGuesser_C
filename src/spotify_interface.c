#include "spotify_interface.h"

size_t discard_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total_size;

    /* Unused */
    (void)ptr;
    (void)userdata;

    total_size = size * nmemb;
    return total_size;  /* Tells CURL we successfully "processed" the data */
}

void spotify_interface_play_music(const char* song_id, int position_ms) 
{
    CURL *curl = curl_easy_init();
    char curl_url[CURL_URL_BUFFER_SIZE];
    char json_data[PLAY_SONG_JSON_BUFFER_SIZE];
    char auth_header[AUTH_HEADER_BUFFER_SIZE];
    CURLcode res;
    struct curl_slist *headers = NULL;
    
    if (curl) 
    {
        headers = curl_slist_append(headers, "Content-Type: application/json");

        sprintf(auth_header, "Authorization: Bearer %s", token_interface_get_access_token());

        headers = curl_slist_append(headers, auth_header);

        sprintf(json_data, "{ \"uris\": [\"%s\"] , \"position_ms\": %d}", song_id, position_ms);

        sprintf(curl_url, "https://api.spotify.com/v1/me/player/play?device_id=%s", token_interface_get_device_id());

        printf("PLAY AUTH HEADER: %s\n", auth_header);
        printf("PLAY JSON DATA: %s\n", json_data);
        printf("PLAY CURL URL: %s\n", curl_url);

        curl_easy_setopt(curl, CURLOPT_URL, curl_url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discard_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL); /* Don't store response data */

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) 
        {
            fprintf(stderr, "Playback failed: %s\n", curl_easy_strerror(res));
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

void spotify_interface_pause_music(void)
{
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    char auth_header[AUTH_HEADER_BUFFER_SIZE];
    CURLcode res;
    
    if (curl) 
    {
        headers = curl_slist_append(headers, "Content-Type: application/json");

        sprintf(auth_header, "Authorization: Bearer %s", token_interface_get_access_token());

        headers = curl_slist_append(headers, auth_header);

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/me/player/pause");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discard_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL); /* Don't store response data */

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) 
        {
            fprintf(stderr, "Pause failed: %s\n", curl_easy_strerror(res));
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}
