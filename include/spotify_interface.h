#ifndef SPOTIFY_INTERFACE_H
#define SPOTIFY_INTERFACE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "utils.h"
#include "token_interface.h"

#define SONG_ID_BUFFER_SIZE         1024
#define PLAY_SONG_JSON_BUFFER_SIZE  2048
#define AUTH_HEADER_BUFFER_SIZE     1024

#define CURL_URL_BUFFER_SIZE      1024

size_t discard_callback(void *ptr, size_t size, size_t nmemb, void *userdata);

void spotify_interface_play_music(const char* song_id, int position_ms);
void spotify_interface_pause_music(void);
void spotify_interface_next_track(void);
void spotify_interface_previous_track(void);
void spotify_interface_seek_to_position(int position_ms);
void spotify_interface_add_to_queue(const char* song_id);
void spotify_interface_call_curl(const char* curl_url, const char* json_data);

#endif /* SPOTIFY_INTERFACE_H */
