# MusicGuesser_C
A music guessing game in ANSI C using the Spotify API. The program will choose a random year, and then play 3 songs from that year. The player will try to guess what year the songs are from.

# Prerequisites
* libcurl
* sqlite3
* cJSON

In addition, you will also need a Spotify developer account and a web application with a Client ID and Client Secret.

# Setup
1. Configure an SQLITE 3 database with the following schema:
   
   | Name          | Type          | Attributes      |
   | ------------- | ------------- | --------------- |
   | Title         | TEXT          | NOT NULL        |
   | Artist        | TEXT          | NOT NULL        |
   | Album         | TEXT          | NOT NULL        |
   | Year          | TEXT          | NOT NULL        |
   | ID            | TEXT          | NOT NULL        |
   | Index         | TEXT          | NOT NULL UNIQUE |

   * The metadata (title, artist, album) is currently unused, but could be used for display in the game.
   * The year is used for song lookup based on the random year chosen.
   * The ID is the Spotify song ID that will be used to play the song in Spotify.
       * Ex: ```spotify:track:4wM7V31TsZzzDBEH02Hltb```
   
3. Create a token_data directory and provide the following files
    * client_id.txt - your Spotify App Client ID
    * client_secret.txt - your Spotify App Client Secret
    * device_id.txt - your Spotify Device ID on which you want to play the music
    * auth_code.txt - your Spotify API Auth Code (refer to "Authorization" in Spotify API docs)

Once the token data above is provided, the application will automatically retrieve the Access Key and Refresh Key. It will also automatically refresh the Access Key once it expires.

# Running
Run the ```music_guesser``` executable. The game will pick a random year and then play three songs from that year. Good luck!
