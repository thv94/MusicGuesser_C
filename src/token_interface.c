#include "token_interface.h"

char* token_interface_get_access_token(void)
{
    return access_token;
}

char* token_interface_get_device_id(void)
{
    return device_id;
}

size_t token_interface_token_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total_size;

    printf("TOKEN CALLBACK CALLED\n");

     total_size = size * nmemb;
    strncat((char *)userdata, (char *)ptr, total_size);
    return total_size;
}

int token_interface_is_token_expired(void) {
    time_t current_time;
    long expiration_time;

    current_time = time(NULL);

    if (current_time == (time_t)-1) 
    {
        perror("Error getting current time");
        return -1;
    }

    expiration_time = atol(token_time);

    if (expiration_time == 0 && strcmp(token_time, "0") != 0) {
        fprintf(stderr, "Error: Invalid expiration time\n");
        return -1;
    }

    printf("CURRENT TIME: %ld\n", current_time);
    printf("TOKEN_TIME: %ld\n", expiration_time);

    if (current_time >= expiration_time) {
        printf("TOKEN EXPIRED \n");
        return 1; /* Token has expired */
    } else {
        printf("TOKEN NOT EXPIRED \n");
        return 0; /* Token is still valid */
    }
}

void token_interface_save_token_to_file(const char* token, const char* filename)
{
    FILE *file = fopen(filename, "w");

    if (!file)
    {
        perror("Error opening token file for write");
        return;
    }

    fprintf(file, "%s", token);
    fclose(file);
}

void token_interface_save_expiration_time(int expires_in) 
{
    time_t expiration_time;
    FILE *file = fopen(TOKEN_TIMESTAMP_FILE_NAME, "w");

    if (!file) 
    {
        perror("Error opening expiration file");
        return;
    }

    expiration_time = time(NULL) + expires_in;
    fprintf(file, "%ld\n", expiration_time);
    fclose(file);
}

void token_interface_extract_and_store_access_token(const char *response) 
{
    cJSON *json = cJSON_Parse(response);
    cJSON *access_token_json;
    cJSON *refresh_token_json;
    cJSON *expires_in_json;

    if (!json) 
    {
        fprintf(stderr, "Error: Failed to parse JSON response\n");
        return;
    }

    access_token_json  = cJSON_GetObjectItem(json, "access_token");
    refresh_token_json = cJSON_GetObjectItem(json, "refresh_token");
    expires_in_json    = cJSON_GetObjectItem(json, "expires_in");

    if (cJSON_IsString(access_token_json) && access_token_json->valuestring && cJSON_IsNumber(expires_in_json)) 
    {
        printf("WRITING ACCESS TOKEN FILE\n");
        strncpy(access_token, access_token_json->valuestring, ACCESS_TOKEN_BUFFER_SIZE - 1);
        access_token[ACCESS_TOKEN_BUFFER_SIZE - 1] = '\0';
        token_interface_save_token_to_file(access_token_json->valuestring, ACCESS_TOKEN_FILE_NAME);
        
        strncpy(refresh_token, refresh_token_json->valuestring, REFRESH_TOKEN_BUFFER_SIZE - 1);
        refresh_token[REFRESH_TOKEN_BUFFER_SIZE - 1] = '\0';
        token_interface_save_token_to_file(refresh_token_json->valuestring, REFRESH_TOKEN_FILE_NAME);

        token_interface_save_expiration_time(expires_in_json->valueint);
    } 
    else 
    {
        fprintf(stderr, "Error: Invalid JSON response\n");
    }

    cJSON_Delete(json);
}

void token_interface_populate_access_token(void)
{
    if (!utils_file_exists(ACCESS_TOKEN_FILE_NAME))
    {
        printf("ACCESS TOKEN FILE DOESN'T EXIST! Generating...\n");
        token_interface_get_initial_access_token();
    }
    else 
    {
        printf("ACCESS TOKEN FILE EXISTS\n");
        if (token_interface_is_token_expired())
        {
            token_interface_refresh_access_token();
        }
    }
}

void token_interface_refresh_access_token(void) 
{
    char response_buffer[RESPONSE_BUFFER_SIZE] = {0};
    char post_data[POST_DATA_BUFFER_SIZE] = {0};
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    CURLcode res;
    cJSON *json;
    cJSON *access_token_json;
    cJSON *expires_in_json;

    printf("REFRESHING TOKEN...\n");

    if (!curl) 
    {
        fprintf(stderr, "Error initializing CURL\n");
        return;
    }

    snprintf(post_data, sizeof(post_data), "grant_type=refresh_token&refresh_token=%s&client_id=%s&client_secret=%s", refresh_token, client_id, client_secret);

    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    curl_easy_setopt(curl, CURLOPT_URL, "https://accounts.spotify.com/api/token");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, token_interface_token_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);
    
    printf("REFRESH TOKEN POST DATA: %s\n", post_data);

    res = curl_easy_perform(curl);
    
    printf("REFRESH TOKEN RESPONSE BUFFER: %s\n", response_buffer);

    if (res != CURLE_OK) 
    {
        fprintf(stderr, "CURL error: %s\n", curl_easy_strerror(res));
    } else 
    {
        json = cJSON_Parse(response_buffer);
        if (json) 
        {
            access_token_json = cJSON_GetObjectItem(json, "access_token");
            expires_in_json   = cJSON_GetObjectItem(json, "expires_in");

            printf("REFRESH ACCESS_TOKEN: %s\n", access_token_json->valuestring);
            printf("REFRESH EXPIRES_IN: %d\n", expires_in_json->valueint);

            if (cJSON_IsString(access_token_json) && access_token_json->valuestring && cJSON_IsNumber(expires_in_json)) 
            {
                printf("WRITING REFRESH ACCESS TOKEN FILE\n");

                /* Write Access Token */
                strncpy(access_token, access_token_json->valuestring, ACCESS_TOKEN_BUFFER_SIZE - 1);
                access_token[ACCESS_TOKEN_BUFFER_SIZE - 1] = '\0';
                token_interface_save_token_to_file(access_token_json->valuestring, ACCESS_TOKEN_FILE_NAME);

                /* Write Token Time */
                /* NOTE: For whatever reason when calling refresh expires_in does not have a valuestring value, only valueint */
                snprintf(token_time, (TOKEN_TIME_BUFFER_SIZE - 1), "%d", expires_in_json->valueint);
                token_time[TOKEN_TIME_BUFFER_SIZE - 1] = '\0';
                token_interface_save_expiration_time(expires_in_json->valueint);

                printf("REFRESHED ACCESS TOKEN\n");
            } 
            else 
            {
                fprintf(stderr, "Error: Invalid JSON response\n");
            }

            cJSON_Delete(json);
        } 
        else 
        {
            fprintf(stderr, "Error: Failed to parse JSON response\n");
        }
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

void token_interface_get_initial_access_token(void) 
{
    char response_buffer[RESPONSE_BUFFER_SIZE] = {0};
    char post_data[POST_DATA_BUFFER_SIZE] = {0};
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    CURLcode res;

    if (curl) 
    {
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

        snprintf(post_data, sizeof(post_data), "grant_type=authorization_code&code=%s&redirect_uri=%s&client_id=%s&client_secret=%s", auth_code, REDIRECT_URI, client_id, client_secret);

        curl_easy_setopt(curl, CURLOPT_URL, "https://accounts.spotify.com/api/token");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, token_interface_token_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) 
        {
            fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
        } 
        else 
        {
            token_interface_extract_and_store_access_token(response_buffer);
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

void token_interface_read_all_value_files(void)
{
    token_interface_read_value_file(auth_code, sizeof(auth_code), AUTH_CODE_FILE_NAME);
    token_interface_read_value_file(access_token, sizeof(access_token), ACCESS_TOKEN_FILE_NAME);
    token_interface_read_value_file(refresh_token, sizeof(refresh_token), REFRESH_TOKEN_FILE_NAME);
    token_interface_read_value_file(client_id, sizeof(client_id), CLIENT_ID_FILE_NAME);
    token_interface_read_value_file(client_secret, sizeof(client_secret), CLIENT_SECRET_FILE_NAME);
    token_interface_read_value_file(device_id, sizeof(device_id), DEVICE_ID_FILE_NAME);
    token_interface_read_value_file(token_time, sizeof(token_time), TOKEN_TIMESTAMP_FILE_NAME);
}

int token_interface_read_value_file(char* buffer, int buffer_size, const char* filename)
{
    FILE* value_file;

    value_file = fopen(filename, "r");

    if (!value_file)
    {
        fprintf(stderr, "Error opening %s file:", filename);
        perror(NULL); /* append perror output */
        return 1;
    }

    if (!fgets(buffer, buffer_size, value_file))
    {
        perror("Error reading value from file:");
        fclose(value_file);
        return 1;
    }

    /* Strip newline */
    buffer[strcspn(buffer, "\n")] = 0;

    fclose(value_file);
    return 0;
}
