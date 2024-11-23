#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_ppm.h"

/**
 * PPM Reader Implementation
 *
 * This program provides functions to read PPM image files (P6 format)
 * and return the pixel data as a flat array or a 2D array.
 *
 * @author: Tianyun Song
 * @version: November 15, 2024
 */

/**
 * Reads a PPM image file (P6 format) and returns a flat array of pixels.
 * 
 * @param filename The name of the PPM file to read
 * @param w Pointer to store the width of the image
 * @param h Pointer to store the height of the image
 * @return A flat array of ppm_pixel structures containing the image data,
 *         or NULL if the file could not be read
 */

struct ppm_pixel* read_ppm(const char* filename, int* w, int* h) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Unable to open file %s\n", filename);
        return NULL;
    }

    char format[3];
    fscanf(file, "%2s", format);
    if (strcmp(format, "P6") != 0) {
        fprintf(stderr, "Unsupported PPM format (only P6 is supported)\n");
        fclose(file);
        return NULL;
    }

    fscanf(file, "%d %d", w, h);
    int maxval;
    fscanf(file, "%d", &maxval);
    fgetc(file);  // Consume the newline character after maxval

    struct ppm_pixel* pixels = malloc((*w) * (*h) * sizeof(struct ppm_pixel));
    if (!pixels) {
        fprintf(stderr, "Failed to allocate memory for pixels\n");
        fclose(file);
        return NULL;
    }

    fread(pixels, sizeof(struct ppm_pixel), (*w) * (*h), file);
    fclose(file);
    return pixels;
}

struct ppm_pixel** read_ppm_2d(const char* filename, int* w, int* h) {
    
  return NULL;
}