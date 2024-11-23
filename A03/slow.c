/*----------------------------------------------
 * Author: Tianyun Song
 * Date: 09.18.2024
 * Description: This program transforms a given text into "ent-speech,"
 * where each character of the input text is followed by a specified
 * number of periods, simulating the slow and deliberate speech of Ents
 * from "The Lord of the Rings." The program prompts the user to enter
 * both the text and the number of pauses (periods) after each character.
 * It handles input through standard input functions and uses dynamic
 * memory allocation to construct the output string with the added periods.
 * The transformed text is then displayed to the standard output.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Buffer to hold the input text.
    char buff[32];
    // Pause length between characters.
    int pause_length;

    // Prompt for the pause length.
    printf("Pause length: ");
    scanf("%d", &pause_length);

    // Clear the input buffer and prompt for the text.
    // Clear the input buffer to handle trailing newline.
    while (getchar() != '\n'); 
    printf("Text: ");
    fgets(buff, sizeof(buff), stdin);

    // Remove newline character from fgets input.
    size_t len = strlen(buff);
    if (buff[len - 1] == '\n') {
        buff[len - 1] = '\0';
    }

    // Calculate the length of the final string.
    // Recalculate the length of the modified input.
    len = strlen(buff);
    size_t out_len = len + len * pause_length;

    // Allocate memory for the resulting string.
    // +1 for the null terminator.
    char *output = (char *)malloc(out_len + 1);
    if (output == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    // Construct the slow speech output.
    char *ptr = output;
    for (size_t i = 0; i < len; i++) {
        *ptr++ = buff[i];
        for (int j = 0; j < pause_length; j++) {
          *ptr++ = '.';
        }
    }
    // Null-terminate the string.
    *ptr = '\0';

    // Output the slow speech.
    printf("%s\n", output);

    // Free the allocated memory.
    free(output);
    return 0;
}