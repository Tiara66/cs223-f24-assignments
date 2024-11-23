/*----------------------------------------------
 * Author: Tianyun Song
 * Date: 09.17.2024
 * Description: This program reads song metadata from a CSV file, "songlist.csv",
 * and stores it in a dynamically allocated array of Song structures. Each song
 * includes information such as title, artist, duration in milliseconds, danceability,
 * energy, tempo, and valence—attributes provided by Spotify's audio analysis.
 * The program reads the number of songs from the first line of the CSV, skips the header,
 * and processes each subsequent line into the Song array. It then prints a formatted
 * list of songs and their attributes, converting the duration from milliseconds to
 * minutes and seconds for display. It handles file I/O errors and ensures proper memory
 * management by freeing allocated memory before exiting, preventing leaks.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char title[128]; // Title of the song
    char artist[128]; // Artist of the song
    int duration;    // Duration in milliseconds
    float danceability; // Danceability score
    float energy; // Energy score
    float tempo; // Beats per minute
    float valence; // Musical positiveness conveyed
} Song;

int main() {
    FILE *file = fopen("songlist.csv", "r");
    if (!file) {
        fprintf(stderr, "Error opening file.\n");
        return 1;
    }

    // Buffer for each line in the file
    char line[1024];
    // Read the first line to get number of songs
    fgets(line, sizeof(line), file);
    int num_songs;
    // Parse the number of songs from the first line.
    sscanf(line, "%d", &num_songs);

    // Allocate memory for storing songs.
    Song *songs = (Song *)malloc(num_songs * sizeof(Song));
    if (!songs) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    // Read the header line and discard it
    fgets(line, sizeof(line), file);

    int index = 0;
    while (fgets(line, sizeof(line), file) && index < num_songs) {
        char *token = strtok(line, ",");
        strcpy(songs[index].title, token);

        token = strtok(NULL, ",");
        strcpy(songs[index].artist, token);

        token = strtok(NULL, ",");
        songs[index].duration = atoi(token);

        token = strtok(NULL, ",");
        songs[index].danceability = atof(token);

        token = strtok(NULL, ",");
        songs[index].energy = atof(token);

        token = strtok(NULL, ",");
        songs[index].tempo = atof(token);

        token = strtok(NULL, ",");
        songs[index].valence = atof(token);

        index++;
    }

    // Print the songs in a formatted table
    printf("Welcome to Dynamic Donna's Danceability Directory.\n\n");
    // Print the header row for the table
    printf("%-3s %-30s %-30s %-10s %-10s %-10s %-10s %-10s\n", 
           "ID", "Title", "Artist", "Duration", "Danceabl.", 
           "Energy", "Tempo", "Valence");

    // Print separator line
    printf("------------------------------");
    printf("------------------------------");
    printf("------------------------------");
    printf("------------------------------\n");

    for (int i = 0; i < num_songs; i++) {
        int minutes = songs[i].duration / 60000;
        int seconds = (songs[i].duration / 1000) % 60;

        // Format each song's data into the table
        printf("%-3d %-30s %-30s %02d:%02d     %-10.3f %-10.3f %-10.3f %-10.3f\n", 
               i, songs[i].title, songs[i].artist, minutes, seconds, 
               songs[i].danceability, songs[i].energy, 
               songs[i].tempo, songs[i].valence);
    }

    // Clean up: Free allocated memory and close the file.
    free(songs);
    fclose(file);
    return 0;
}