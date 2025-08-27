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
    char curl_url[CURL_URL_BUFFER_SIZE];
    char json_data[PLAY_SONG_JSON_BUFFER_SIZE];
        
    sprintf(json_data, "{ \"uris\": [\"%s\"] , \"position_ms\": %d}", song_id, position_ms);

    sprintf(curl_url, "https://api.spotify.com/v1/me/player/play?device_id=%s", token_interface_get_device_id());

    spotify_interface_call_curl(curl_url, json_data);
}

void spotify_interface_pause_music(void)
{
    spotify_interface_call_curl("https://api.spotify.com/v1/me/player/pause", NULL);
}

void spotify_interface_next_track(void)
{
    spotify_interface_call_curl("https://api.spotify.com/v1/me/player/next", NULL);
}

void spotify_interface_previous_track(void)
{
    spotify_interface_call_curl("https://api.spotify.com/v1/me/player/previous", NULL);
}

void spotify_interface_seek_to_position(int position_ms)
{
    char json_data[PLAY_SONG_JSON_BUFFER_SIZE];

    sprintf(json_data, "{ \"position_ms\": %d}", position_ms);

    spotify_interface_call_curl("https://api.spotify.com/v1/me/player/seek", json_data);
}

void spotify_interface_add_to_queue(const char* song_id)
{
    /* TODO: song_id is coming in as spotify:track:______, I need to strip off the colons and use %3 */
    spotify_interface_call_curl("https://api.spotify.com/v1/me/queue?uri=spotify%%3Atrack%%3%s", song_id);
}

void spotify_interface_call_curl(const char* curl_url, const char* json_data)
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

        curl_easy_setopt(curl, CURLOPT_URL, curl_url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

        if (json_data != NULL)
        {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        }

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
