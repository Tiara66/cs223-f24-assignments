/**
 * Acronym Generator
 * 
 * This program prompts the user to enter a phrase and then extracts all uppercase
 * letters to form an acronym, which it prints out. This is particularly useful for
 * generating acronyms from phrases where only the initial letters of each word are
 * capitalized.
 *
 * @author: Tianyun Song
 * @date: 9/10/2024
 */

#include <stdio.h>

int main() {
  char phrase[1024]; // Buffer to store the input phrase
  char acronym[1024]; // Buffer to store the acronym
  int j = 0; // Index for filling the acronym array

  printf("Enter a phrase: ");
  // Read a line of text until a newline is encountered
  scanf("%[^\n]%*c", phrase);

  // Loop through the characters in the string
  for(int i = 0; phrase[i] != '\0'; i++ ) {
    // Check if the character is an uppercase letter
    if(phrase[i] >= 'A' && phrase[i] <= 'Z') {
      acronym[j++] = phrase[i]; // Add to acronym if it's an uppercase letter
    }
  }
  
  // Terminate the acronym string with a null character
  acronym[j] = '\0';
  
  printf("Your acronym is %s\n", acronym);
  return 0;
}