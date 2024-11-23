#include "write_ppm.h"
#include <stdio.h>
#include <string.h>

/**
 * PPM Image Writer
 *
 * This file contains functions to write pixel data to a PPM (Portable Pixmap)
 * image file in binary (P6) format.
 *
 * @file write_ppm.c
 * @author Tianyun Song
 * @date November 6, 2024
 */

/**
 * Writes pixel data to a PPM file in binary format (P6).
 *
 * This function opens a file for binary writing and writes the PPM header
 * with image dimensions and color depth. It then writes each pixel's RGB
 * values from the given array of ppm_pixel structs.
 *
 * @param filename The name of the file to write to.
 * @param pixels   A flat array of ppm_pixel structs representing the image data.
 *                 Must contain at least `w * h` elements.
 * @param w        The width of the image in pixels.
 * @param h        The height of the image in pixels.
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