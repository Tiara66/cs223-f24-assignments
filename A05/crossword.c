/*----------------------------------------------
 * Author: Tianyun Song
 * Date: 10.1.2024
 * Description: This program creates a crossword grid by placing two words
 * vertically and horizontally on the grid if they share a common letter.
 * Usage involves passing two words as command line arguments.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Creates a dynamically allocated 2D array initialized with dots.
 * @param width The number of columns in the grid.
 * @param height The number of rows in the grid.
 * @return Pointer to the created grid.
 */
char** create_grid(int width, int height) {
    char** grid = malloc(height * sizeof(char*));
    for (int i = 0; i < height; i++) {
        grid[i] = malloc(width * sizeof(char));
        for (int j = 0; j < width; j++) {
            grid[i][j] = '.';  // Initialize the grid with dots
        }
    }
    return grid;
}

/**
 * Frees a dynamically allocated 2D array.
 * @param grid The grid to free.
 * @param height The number of rows in the grid.
 */
void free_grid(char** grid, int height) {
    for (int i = 0; i < height; i++) {
        free(grid[i]);
    }
    free(grid);
}

/**
 * Prints the crossword grid.
 * @param grid The grid to print.
 * @param width The width of the grid.
 * @param height The height of the grid.
 */
void print_grid(char** grid, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

/**
 * Main function that handles the crossword creation.
 * @param argc The argument count.
 * @param argv The argument vector, containing words for the crossword.
 * @return Execution status code.
 */
int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <word1> <word2>\n", argv[0]);
        exit(0);
    }

    char* word1 = argv[1];
    char* word2 = argv[2];
    int found = 0;
    int word1_len = strlen(word1);
    int word2_len = strlen(word2);

    // Find the first common letter
    for (int i = 0; i < word1_len && !found; i++) {
        for (int j = 0; j < word2_len && !found; j++) {
            if (word1[i] == word2[j]) {
                // Create grid
                int width = word2_len;
                int height = word1_len;
                char** grid = create_grid(width, height);

                // Place word1 vertically
                for (int k = 0; k < word1_len; k++) {
                    grid[k][j] = word1[k];
                }

                // Place word2 horizontally
                for (int k = 0; k < word2_len; k++) {
                    grid[i][k] = word2[k];
                }

                print_grid(grid, width, height);
                free_grid(grid, height);
                found = 1;
            }
        }
    }

    if (!found) {
        printf("No common letter!\n");
    }

    return 0;
}