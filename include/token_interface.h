#ifndef TOKEN_INTERFACE_H
#define TOKEN_INTERFACE_H

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "utils.h"

#define AUTH_CODE_BUFFER_SIZE     1024
#define CLIENT_ID_BUFFER_SIZE     1024
#define CLIENT_SECRET_BUFFER_SIZE 1024
#define DEVICE_ID_BUFFER_SIZE     1024
#define ACCESS_TOKEN_BUFFER_SIZE  1024
#define REFRESH_TOKEN_BUFFER_SIZE 1024
#define RESPONSE_BUFFER_SIZE      1024
#define TOKEN_TIME_BUFFER_SIZE    1024

#define CURL_URL_BUFFER_SIZE      1024
#define POST_DATA_BUFFER_SIZE     4096

#define TOKEN_DATA_DIR "token_data/"

#define ACCESS_TOKEN_FILE_NAME    TOKEN_DATA_DIR "access_token.txt"
#define REFRESH_TOKEN_FILE_NAME   TOKEN_DATA_DIR "refresh_token.txt"
#define AUTH_CODE_FILE_NAME       TOKEN_DATA_DIR "auth_code.txt"
#define CLIENT_ID_FILE_NAME       TOKEN_DATA_DIR "client_id.txt"
#define CLIENT_SECRET_FILE_NAME   TOKEN_DATA_DIR "client_secret.txt"
#define DEVICE_ID_FILE_NAME       TOKEN_DATA_DIR "device_id.txt"
#define TOKEN_TIMESTAMP_FILE_NAME TOKEN_DATA_DIR "token_time.txt"

#define REDIRECT_URI "http://localhost:8888/callback"

char* token_interface_get_access_token(void);
char* token_interface_get_device_id(void);
size_t token_interface_token_callback(void *ptr, size_t size, size_t nmemb, void *userdata);
void token_interface_refresh_access_token(void);
void token_interface_save_token_to_file(const char* token, const char* filename);
void token_interface_populate_access_token(void);
void token_interface_save_expiration_time(int expires_in);
int token_interface_is_token_expired(void);
void token_interface_extract_and_store_access_token(const char *response);
void token_interface_get_initial_access_token(void);
void token_interface_read_all_value_files(void);
int token_interface_read_value_file(char* buffer, int buffer_size, const char* filename);

#endif /* TOKEN_INTERFACE_H */
