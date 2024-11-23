/*----------------------------------------------
 * Author: Tianyun Song
 * Date: 10.12.2024
 * Description: This program encodes a user-provided message into the least significant 
 * bits (LSBs) of a PPM image file in binary (P6) format. The program reads 
 * the image, encodes the message, and writes the modified image to a new 
 * file with "-encoded" appended to the filename.
 ---------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_ppm.h"
#include "write_ppm.h"

/**
 * The main function that encodes a message into the least significant bits
 * (LSBs) of the pixel data of a PPM image file. It reads the image, encodes
 * the user-provided message, and writes the modified image to a new file with
 * "-encoded" appended to the filename.
 *
 * @param argc The number of command-line arguments (must be 2).
 * @param argv The command-line arguments, where argv[1] is the input PPM file.
 * @return Returns 0 on successful completion, or 1 if an error occurs (e.g., 
 *         file cannot be read, message too long, or memory allocation failure).
 */
int main(int argc, char** argv) {
    if (argc != 2) {
      printf("usage: encode <file.ppm>\n");
      return 0;
    }

    int width, height;
    struct ppm_pixel* pixels = read_ppm(argv[1], &width, &height);
    if (pixels == NULL) {
        fprintf(stderr, "Error: Cannot read file %s\n", argv[1]);  // Error if file cannot be read
        return 1;
    }

    // Calculate the maximum characters, subtracting 1 for the null terminator space
    int maxChars = (width * height * 3) / 8 - 1;
    printf("Reading %s with width %d and height %d\n", argv[1], width, height);
    printf("Max number of characters in the image: %d\n", maxChars);

    char message[256];
    printf("Enter a phrase: ");
    fgets(message, 256, stdin);
    message[strcspn(message, "\n")] = 0; // Remove newline character

    if (strlen(message) > maxChars) {
        fprintf(stderr, "Error: Message too long for the image\n");
        free(pixels);
        return 1;
    }

    // Encode the message into the image
    unsigned char* bytePtr = (unsigned char*) pixels;
    for (int i = 0, bitIndex = 0; message[i] != '\0'; i++) {
        for (int bit = 0; bit < 8; bit++, bitIndex++) {
            //Debug: print original value in hex
            //printf("Before: %02X ", bytePtr[bitIndex]);
            int bitValue = (message[i] >> bit) & 1;
            bytePtr[bitIndex] = (bytePtr[bitIndex] & ~1) | bitValue;
            // Debug: print modified value in hex
            //printf("After: %02X\n", bytePtr[bitIndex]);
        }
    }

    // Mark the end of the message with a null terminator
    for (int bit = 0; bit < 8; bit++) {
        int bitIndex = 8 * strlen(message) + bit;
        // Debug: print original value in hex
        // printf("Before End: %02X ", bytePtr[bitIndex]);
        bytePtr[bitIndex] = (bytePtr[bitIndex] & ~1); // Clear the LSB
        // Debug: print modified value in hex
        //printf("After End: %02X\n", bytePtr[bitIndex]);
    }

    // Format the output filename to append "-encoded" before the file extension
    char outputFilename[260];
    snprintf(outputFilename, 260, "%.*s-encoded%s", 
            (int)(strrchr(argv[1], '.') - argv[1]), 
            argv[1], strrchr(argv[1], '.'));
    write_ppm(outputFilename, pixels, width, height);

    printf("Writing file %s\n", outputFilename);

    free(pixels);
    return 0;
}
