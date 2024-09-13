/**
 * Song Management Program
 *
 * This program manages a list of songs, allowing the user to view details
 * and edit attributes such as artist, title, duration, and danceability.
 * The user can interact with the program through a simple console interface.
 *
 * @author: Tianyun Song
 * @version: 9/10/2024
 */

#include <stdio.h>
#include <string.h>

// Define a struct to hold song information
typedef struct Song {
  char title[32];
  char artist[32];
  int minutes;
  int seconds;
  float danceability;
} Song;

// Function prototypes
void printSongs(Song songs[], int size);
void editSong(Song *song);

int main() {
  // Initialize songs
  Song songs[3] = {
      {"Shout", "Tears for Fears", 4, 11, 0.50},
      {"As it was", "Harry Styles", 2, 47, 0.70},
      {"Wish you were here", "Pink Floyd", 5, 34, 0.30}
  };
    
  int size = sizeof(songs) / sizeof(songs[0]);
  int songId;

  printf("Welcome to Steven Struct's Song List.\n\n");
  printSongs(songs, size);

  // Main interaction loop
  while (1) {
    printf("\nEnter a song id to edit [0,1,2] (or -1 to quit): ");
    scanf("%d%*c", &songId); // Read integer and consume newline

    if (songId == -1) {
      break;
    }

    if (songId < 0 || songId >= size) {
      printf("Invalid choice!\n");
      continue;
    }

    editSong(&songs[songId]);
    printSongs(songs, size);
  }

  return 0;
}

/**
 * Prints all songs in a formatted table.
 * @param songs Array of Song structs
 * @param size Number of songs in the array
 */
void printSongs(Song songs[], int size) {
  printf("\n");
  for (int i = 0; i < size; i++) {
    printf("%d) %-20s artist: %-20s duration: %d:%02d  danceability: %.2f\n",
      i, songs[i].title, songs[i].artist, 
      songs[i].minutes, songs[i].seconds, 
      songs[i].danceability);
    }
  printf("\n=======================\n");
}

/**
 * Edits attributes of a song based on user input.
 * @param song Pointer to the Song struct to edit
 */
void editSong(Song *song) {
  char attribute[20];
  // Temporary input buffer to safely store strings before copying
  char tempInput[32];
  
  printf("Which attribute do you wish to edit? "
        "[artist, title, duration, danceability]: ");
  scanf("%s%*c", attribute); // Read string and consume newline

  if (strcmp(attribute, "artist") == 0) {
    printf("Enter a artist: ");
    scanf(" %[^\n]%*c", tempInput); // Read line and ignore newline
    strcpy(song->artist, tempInput); // Safely copy the new artist into the struct
  } else if (strcmp(attribute, "title") == 0) {
    printf("Enter a title: ");
    scanf(" %[^\n]%*c", tempInput); // Read line and ignore newline
    strcpy(song->title, tempInput); // Safely copy the new title into the struct
  } else if (strcmp(attribute, "duration") == 0) {
    printf("Enter a duration (minutes): ");
    int tempMinutes;
    scanf("%d%*c", &tempMinutes); // Read one integer and consume newline
    
    printf("Enter a duration (seconds): ");
    int tempSeconds;
    scanf("%d%*c", &tempSeconds); // Read another integer and consume newline
    song->minutes = tempMinutes; // Update the minutes directly
    song->seconds = tempSeconds; // Update the seconds directly
  } else if (strcmp(attribute, "danceability") == 0) {
    printf("Enter danceability: ");
    float tempDanceability;
    scanf("%f%*c", &tempDanceability); // Read a float and consume newline
    song->danceability = tempDanceability; // Update the danceability directly
  } else {
    printf("Invalid attribute.\n");
  }
}