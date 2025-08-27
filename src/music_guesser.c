#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "spotify_interface.h"
#include "db_interface.h"
#include "token_interface.h"

#define SEC_TO_MS 1000

#define DEFAULT_MIN_YEAR                       1960
#define DEFAULT_MAX_YEAR                       2022
#define DEFAULT_POSITION_MS                    (30 * SEC_TO_MS)
#define DEFAULT_SONG_PLAY_DURATION_SEC         10
#define DEFAULT_SONG_WAIT_DURATION_SEC         2
#define DEFAULT_FINAL_ANSWER_WAIT_DURATION_SEC 3
#define DEFAULT_NUM_ROUNDS                     3

int generate_random_year(int min_year, int max_year);
void show_final_answer(int year, int final_answer_wait_duration_sec);

int main(int argc, char** argv) {
    int min_year;
    int max_year;
    int year;
    int num_rounds;
    int play_position_ms;
    int song_play_duration_sec;
    int song_wait_duration_sec;
    int final_answer_wait_duration_sec;
    sqlite3 *db;
    char random_song_id[SONG_ID_BUFFER_SIZE];
    int round_idx;

    /* Unused */
    (void) argv;

    srand((unsigned int)time(NULL));

    if (argc != 1)
    {
        printf("Usage: music_guesser");
        return 1;
    }

    min_year                       = DEFAULT_MIN_YEAR;
    max_year                       = DEFAULT_MAX_YEAR;
    num_rounds                     = DEFAULT_NUM_ROUNDS;
    play_position_ms               = DEFAULT_POSITION_MS;
    song_play_duration_sec         = DEFAULT_SONG_PLAY_DURATION_SEC;
    song_wait_duration_sec         = DEFAULT_SONG_WAIT_DURATION_SEC;
    final_answer_wait_duration_sec = DEFAULT_FINAL_ANSWER_WAIT_DURATION_SEC;
    
    year = generate_random_year(min_year, max_year);

    db_interface_open_db(&db);
    token_interface_read_all_value_files();
    token_interface_populate_access_token();

    for (round_idx = 0; round_idx < num_rounds; round_idx++)
    {
        printf("Round #%d\n", (round_idx + 1));

        db_interface_get_random_song_id(db, year, random_song_id);

        spotify_interface_add_to_queue(random_song_id);

        spotify_interface_next_track();

        spotify_interface_play_music(random_song_id, play_position_ms);

        sleep((unsigned int)song_play_duration_sec);

        spotify_interface_pause_music();

        sleep((unsigned int)song_wait_duration_sec);
    }

    sqlite3_close(db);

    show_final_answer(year, final_answer_wait_duration_sec);

    return 0;
}

void show_final_answer(int year, int final_answer_wait_duration_sec)
{
    sleep((unsigned int)final_answer_wait_duration_sec);
    
    printf("The year was: %d\n", year);
}

int generate_random_year(int min_year, int max_year)
{
    return min_year + rand() % (max_year - min_year + 1);
}
