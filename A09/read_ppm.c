#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_ppm.h"

/**
 * Reads a PPM (P6) image file and loads its pixel data into a dynamically
 * allocated array. This function provides an interface for reading PPM
 * images and handles basic error checking for unsupported formats and 
 * memory allocation issues.
 *
 * @file read_ppm.c
 * @author Tianyun Song
 * @date November 6, 2024
 */

/**
 * Reads a PPM image file in binary format (P6) and loads the pixel data.
 * 
 * This function reads the image's width, height, and color depth from
 * the file header, then loads each pixel's RGB values into an array.
 * 
 * @param filename The path to the PPM file to read.
 * @param w Pointer to an integer where the image width will be stored.
 * @param h Pointer to an integer where the image height will be stored.
 * @return A pointer to an array of ppm_pixel structs containing the image data,
 *         or NULL if an error occurs (e.g., file not found or invalid format).
 *         The caller is responsible for freeing the returned array.
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
