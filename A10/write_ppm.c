#include "write_ppm.h"
#include <stdio.h>
#include <string.h>

/**
 * PPM Writer Implementation
 *
 * This program provides functions to write PPM image files (P6 format)
 * from a flat array or a 2D array of pixel data.
 *
 * @author: Tianyun Song
 * @version: November 15, 2024
 */

/**
 * Writes a PPM image file (P6 format) from a flat array of pixels.
 * 
 * @param filename The name of the PPM file to write
 * @param pixels A flat array of ppm_pixel structures containing the image data
 * @param w The width of the image
 * @param h The height of the image
 */
void write_ppm(const char* filename, struct ppm_pixel* pixels, int w, int h) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Unable to open file %s for writing\n", filename);
        return;
    }

    fprintf(file, "P6\n%d %d\n255\n", w, h);
    fwrite(pixels, sizeof(struct ppm_pixel), w * h, file);
    fclose(file);
}

void write_ppm_2d(const char* filename, struct ppm_pixel** pxs, int w, int h) {
}