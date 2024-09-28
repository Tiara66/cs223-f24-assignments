/*----------------------------------------------
 * Author: Tianyun Song
 * Date: 09/26/2024
 * Description: This program manages a dynamically linked list of songs loaded from a CSV file. 
 *              It allows users to interactively display and delete the most danceable song 
 *              based on a 'danceability' score. Songs are displayed with their details such as
 *              title, artist, duration (formatted in minutes and seconds), danceability, energy,
 *              tempo, and valence. The user can repeatedly display and remove the top danceable 
 *              song until all songs are deleted or the user decides to quit the program.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct for storing song details.
typedef struct song {
    char title[100];
    char artist[100];
    int duration;
    float danceability;
    float energy;
    float tempo;
    float valence;
    struct song *next;
} Song;

// Struct for managing the list of songs.
typedef struct {
    Song *head;
    int size;
} SongList;

// Declaration of functions used in the program.
void load_songs_from_csv(const char *filename, SongList *list);
Song *create_song(char *title, char *artist, int duration, float danceability, float energy, float tempo, float valence);
void insert_song(SongList *list, Song *song);
void display_songs(const SongList *list);
void show_and_remove_most_danceable(SongList *list);
void clear_song_list(SongList *list);

/**
 * Loads songs from a CSV file into a linked list.
 * 
 * @param filename The path to the CSV file containing song data.
 * @param list The linked list to populate with song data.
 */
void load_songs_from_csv(const char *filename, SongList *list) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file.\n");
        exit(1);
    }

    char line[256];
    // Skip header line
    fgets(line, sizeof(line), file);

    // Read each line from the CSV and parse it
    while (fgets(line, sizeof(line), file)) {
        char title[100], artist[100];
        int duration;
        float danceability, energy, tempo, valence;
        // Parse the song data from the line
        if (sscanf(line, "%99[^,],%99[^,],%d,%f,%f,%f,%f",
                   title, artist, &duration, &danceability, &energy, 
                   &tempo, &valence) == 7) {
            Song *new_song = create_song(title, artist, duration, 
                                        danceability, energy, 
                                        tempo, valence);
            insert_song(list, new_song);
        }
    }

    fclose(file);
}

/**
 * Creates a new song struct and returns a pointer to it.
 * 
 * @param title The title of the song.
 * @param artist The artist of the song.
 * @param duration The duration of the song in milliseconds.
 * @param danceability The danceability score of the song.
 * @param energy The energy level of the song.
 * @param tempo The tempo of the song.
 * @param valence The valence of the song.
 * @return A pointer to the newly created song struct.
 */
Song *create_song(char *title, char *artist, int duration, 
                  float danceability, float energy, 
                  float tempo, float valence) {
    // Allocate memory for the new song
    Song *new_song = malloc(sizeof(Song));

    if (new_song == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        exit(1);
    }

    // Initialize the song's data fields
    strcpy(new_song->title, title);
    strcpy(new_song->artist, artist);
    new_song->duration = duration;
    new_song->danceability = danceability;
    new_song->energy = energy;
    new_song->tempo = tempo;
    new_song->valence = valence;
    new_song->next = NULL;

    // Return the pointer to the new song
    return new_song;
}

/**
 * Inserts a song into the linked list.
 * 
 * @param list The linked list to which the song will be added.
 * @param song The song to add to the list.
 */
void insert_song(SongList *list, Song *song) {
    if (list->head == NULL) {
        list->head = song;
    } else {
        Song *current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = song;
    }
    list->size++;
}

/**
 * Displays all songs in the list.
 * 
 * @param list The linked list of songs to display.
 */
void display_songs(const SongList *list) {
    int index = 0;
    Song *current = list->head;
    while (current != NULL) {
        printf("%d) %s\t\t%s\t(%d:%02ds) D: %.3f E: %.3f T: %.3f V: %.3f\n",
               index++, current->title, current->artist, 
               current->duration / 60000, (current->duration % 60000) / 1000,
               current->danceability, current->energy, current->tempo, current->valence);
        current = current->next;
    }
    printf("\nDataset contains %d songs\n\n", list->size);
}

/**
 * Shows and removes the most danceable song from the list.
 * 
 * @param list The linked list from which the song will be removed.
 */
void show_and_remove_most_danceable(SongList *list) {
    if (list->head == NULL) {
        printf("No songs in the dataset.\n");
        return;
    }
    Song *current = list->head;
    Song *max_dance = current;
    Song *prev = NULL, *prev_max = NULL;

    while (current != NULL) {
        if (current->danceability > max_dance->danceability) {
            max_dance = current;
            prev_max = prev;
        }
        prev = current;
        current = current->next;
    }

    printf("----------------------------------------"
            "Most danceable"
            "------------------------------------\n");
    printf("%s\t\t%s\t(%d:%02ds) D: %.3f E: %.3f T: %.3f V: %.3f\n",
           max_dance->title, max_dance->artist, max_dance->duration / 60000, 
           (max_dance->duration % 60000) / 1000,
           max_dance->danceability, max_dance->energy, 
           max_dance->tempo, max_dance->valence);
    printf("-----------------------------------------"
            "----------------------------------------"
            "-----------\n\n");

    if (prev_max == NULL) {
        list->head = max_dance->next;
    } else {
        prev_max->next = max_dance->next;
    }
    
    free(max_dance);
    list->size--;
}

/**
 * Clears all songs from the linked list, freeing memory.
 * 
 * @param list The linked list to clear.
 */
void clear_song_list(SongList *list) {
    Song *current = list->head;
    while (current != NULL) {
        Song *next = current->next;
        free(current);
        current = next;
    }

    list->head = NULL;
}

/**
 * The main function to execute the song management program.
 */
int main() {
    SongList list = {NULL, 0};
    load_songs_from_csv("songlist.csv", &list);

    // Display all songs initially
    display_songs(&list);

    char command;
    do {
      printf("=======================\n");
      printf("Press 'd' to show the most danceable"
              "song (any other key to quit): ");
      scanf(" %c", &command);
      if (command == 'd') {
        if (list.size > 0) {
            show_and_remove_most_danceable(&list);
            display_songs(&list);
        } else {
            printf("Dataset contains 0 songs\n\n");
        }
      }
    } while (command == 'd' || (command != 'd' && list.size > 0));
    
    clear_song_list(&list); // Free all remaining nodes
    return 0;
}