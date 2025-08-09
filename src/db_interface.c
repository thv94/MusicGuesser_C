#include "db_interface.h"

#define MUSIC_DB_FILE "music.db"

int db_interface_open_db(sqlite3 **db)
{
    int rc;
    
    rc = sqlite3_open(MUSIC_DB_FILE, db);

    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        return 1;
    }

    return rc;
}

void db_interface_get_random_song_id(sqlite3* db, int year, char random_song_id[SONG_ID_BUFFER_SIZE])
{
    int rc;
    QueryResult query_result;
    char sql[RANDOM_SONG_SQL_BUFFER_SIZE];
    char *errMsg = 0;
    memset(query_result.song_id, 0, sizeof(query_result.song_id));

    sprintf(sql, "SELECT * FROM General WHERE Year=%d ORDER BY RANDOM() LIMIT 1", year);

    rc = sqlite3_exec(db, sql, db_interface_get_random_song_id_callback, &query_result, &errMsg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    }

    strncpy(random_song_id, query_result.song_id, SONG_ID_BUFFER_SIZE);
}

int db_interface_get_random_song_id_callback(void *data, int argc, char **argv, char **colName) 
{
    QueryResult *query_result = (QueryResult *)data;

    /* Unused */
    (void)argc;
    (void)colName;

    strncpy(query_result->song_id, argv[SONG_ID_IDX], SONG_ID_BUFFER_SIZE - 1);
    query_result->song_id[SONG_ID_BUFFER_SIZE - 1] = '\0';
    return 0;
}
