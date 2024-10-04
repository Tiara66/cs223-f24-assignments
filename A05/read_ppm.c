/*----------------------------------------------
 * Author: Tianyun Song
 * Date: 10.3.2024
 * Description: This program reads a PPM file and stores its pixel data in
 * a dynamically allocated 2D array of ppm_pixel structures. The program
 * handles PPM format validation and memory allocation errors gracefully.
 * It is designed to be robust against common file and format errors.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_ppm.h"

// Choose *one* to implement (do not remove the other one!)

struct ppm_pixel* read_ppm(const char* filename, int* w, int* h) {
  return NULL;
}

/**
 * Reads PPM image data from a file into a 2D array of ppm_pixel structures.
 * @param filename The path to the PPM file.
 * @param w Pointer to store the width of the image.
 * @param h Pointer to store the height of the image.
 * @return A pointer to the 2D array of pixels, or NULL if an error occurs.
 */
struct ppm_pixel** read_ppm_2d(const char* filename, int* w, int* h) {
   FILE* fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Failed to open file %s\n", filename);
        return NULL;
    }

    char buffer[256];
    // Read magic number, skip if it's a comment
    if (!fgets(buffer, sizeof(buffer), fp) || buffer[0] == '#') {
        fclose(fp);
        return NULL;
    }

    // Read header lines, skipping any comment lines
    do {
        if (!fgets(buffer, sizeof(buffer), fp)) {
            fclose(fp);
            return NULL;
        }
    } while (buffer[0] == '#');

    // Read image dimensions
    if (sscanf(buffer, "%d %d", w, h) != 2) {
        fclose(fp);
        return NULL;
    }

    // Read max color value (must be 255 for this format)
    if (!fgets(buffer, sizeof(buffer), fp) || sscanf(buffer, "%*d") != 0) {
        fclose(fp);
        return NULL;
    }

    // Allocate memory for pixels
    struct ppm_pixel** pixels = malloc(*h * sizeof(struct ppm_pixel*));
    if (pixels == NULL) {
        fclose(fp);
        return NULL;
    }
    for (int i = 0; i < *h; i++) {
        pixels[i] = malloc(*w * sizeof(struct ppm_pixel));
        if (pixels[i] == NULL) {  // Check each row allocation
            // Free previously allocated rows
            for (int k = 0; k < i; k++) {
                free(pixels[k]);
            }
            free(pixels);
            fclose(fp);
            return NULL;
        }
        fread(pixels[i], sizeof(struct ppm_pixel), *w, fp);
    }

    fclose(fp);
    return pixels;
}
