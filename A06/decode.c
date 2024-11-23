/*----------------------------------------------
 * Author: Tianyun Song
 * Date: 10.12.2024
 * Description: Program to decode a message hidden in the least significant bits (LSBs)
 * of the pixel data of a PPM image.
 * This program reads a PPM file in binary format, extracts the message 
 * encoded in the LSBs of the image's color channels, and prints the 
 * message to the console. It stops decoding once a null terminator 
 * ('\0') is encountered.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "read_ppm.h"

/**
 * Decodes a message hidden in the least significant bits (LSBs) of the pixel data 
 * of a PPM image file in binary format.
 *
 * The program reads the image file, extracts the LSBs of each pixel color component,
 * assembles them into characters, and prints the decoded message.
 *
 * @param argc The number of command-line arguments (must be 2).
 * @param argv The command-line arguments, where argv[1] is the input PPM file.
 * @return Returns 0 if successful, or 1 if an error occurs (such as file read failure).
 */
int main(int argc, char** argv) {
    if (argc != 2) {
      printf("usage: decode <file.ppm>\n");
      return 0;
    }

    int width, height;
    struct ppm_pixel* pixels = read_ppm(argv[1], &width, &height);
    if (!pixels) {
        return 1; // Error already reported by read_ppm
    }

    int maxChars = (width * height * 3) / 8;
    printf("Reading %s with width %d and height %d\n", argv[1], width, height);
    printf("Max number of characters in the image: %d\n", maxChars);

    char* message = malloc(maxChars + 1); // +1 for null terminator
    if (!message) {
        fprintf(stderr, "Memory allocation failed\n");
        free(pixels);
        return 1;
    }

    unsigned char* bytePtr = (unsigned char*) pixels;
    int bitPos = 0;
    unsigned char currentChar = 0;
    int msgIndex = 0;

    for (int i = 0; i < width * height * 3; ++i) {
        unsigned char lsb = bytePtr[i] & 1;  // Extract LSB from each color component
        currentChar |= (lsb << (7 - bitPos)); // Place the lsb in the correct position of currentChar
        
        // Debugging output to trace bit addition and character formation
        //printf("Bit: %d, Current Char: %02X\n", lsb, currentChar);
        
        bitPos++;

        if (bitPos == 8) {  // Once we have assembled a full byte
            if (currentChar == '\0') {
                break;  // Stop if we encounter the string terminator
            }
            message[msgIndex++] = currentChar;  // Store the character
            currentChar = 0;  // Reset for next character
            bitPos = 0;  // Reset bit position
        }
    }

    message[msgIndex] = '\0'; // Null-terminate the string
    printf("%s\n", message);

    free(pixels);
    free(message);
    return 0;
}
