#ifndef DB_INTERFACE_H
#define DB_INTERFACE_H

#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

#define SONG_ID_BUFFER_SIZE         1024
#define RANDOM_SONG_SQL_BUFFER_SIZE 1024

#define SONG_ID_IDX 5

typedef struct 
{
    char song_id[SONG_ID_BUFFER_SIZE];
} QueryResult;

int db_interface_open_db(sqlite3 **db);
void db_interface_get_random_song_id(sqlite3* db, int year, char random_song_id[SONG_ID_BUFFER_SIZE]);
int db_interface_get_random_song_id_callback(void *data, int argc, char **argv, char **colName);

#endif /* DB_INTERFACE_H */
