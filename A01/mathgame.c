/***************************************************
 * mathgame.c
 * Author: Tianyun Song
 * Implements a math game
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
  int rounds, i, num1, num2, userAnswer, correctAnswers = 0;

  srand((unsigned int)time(NULL));

  // Display the welcome message
  printf("Welcome to Math Game!\n");
  printf("How many rounds do you want to play? ");
  scanf("%d", &rounds);

  for (i = 0; i < rounds; i++) {
    // Generate two random numbers between 1 and 9
    num1 = rand() % 9 + 1;
    num2 = rand() % 9 + 1;

    // Prompt the user with an addition problem
    printf("\n%d + %d = ? ", num1, num2);
    scanf("%d", &userAnswer); 

    // Check the user's answer and respond accordingly
    if (userAnswer == num1 + num2) {
      printf("Correct!\n");
      correctAnswers++;
    } else {
      printf("Incorrect :(\n");
    }
  }

  printf("You answered %d/%d correctly.\n", correctAnswers, rounds);

  return 0;
}