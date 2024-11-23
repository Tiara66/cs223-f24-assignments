/**
 * Transforms lowercase letters in a user-inputted word to random symbols.
 *
 * This program prompts the user to enter a word, then iterates over each character.
 * If a character is a lowercase letter, it replaces it with a randomly chosen symbol
 * from the set {'@', '!', '#', '*'} and prints the transformed word.
 *
 * @author: Tianyun Song
 * @version: 9/10/2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
  char buff[32];
  printf("Enter a word: ");
  fgets(buff, sizeof(buff), stdin);  // Read input with a maximum size of 31 characters + null terminator
  
  // Initialize random number generator
  srand(time(NULL));

  printf("OMG! ");
  for (int i = 0; buff[i] != '\0'; i++) {
    if (buff[i] >= 'a' && buff[i] <= 'z') {  // Check if the character is lowercase
      // Randomly choose a character from the set {@, !, #, *}
      char symbols[] = {'@', '!', '#', '*'};
      buff[i] = symbols[rand() % 4];  // Replace with a random symbol
    }
    putchar(buff[i]);  // Output the character
  }
  return 0;
}