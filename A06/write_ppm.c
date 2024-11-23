/*----------------------------------------------
 * Author: Tianyun Song
 * Date: 10.12.2024
 * Description: This file contains the implementation of functions to write PPM images in 
 * binary (P6) format. The function `write_ppm` writes a 1D array of pixels to 
 * a file, and `write_ppm_2d` is intended for 2D arrays (not yet implemented).
 ---------------------------------------------*/
#include "write_ppm.h"
#include <stdio.h>
#include <string.h>

// implement *one* (but do not remove the other one)!

/**
 * Writes a PPM image to a file in binary format (P6). The pixel data is
 * provided as a 1D array.
 *
 * @param filename The name of the output PPM file.
 * @param pixels A 1D array of ppm_pixel structs representing the image.
 * @param w The width of the image.
 * @param h The height of the image.
 */
void write_ppm(const char* filename, struct ppm_pixel* pixels, int w, int h) {
    FILE* fp = fopen(filename, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Error: Cannot open file %s for writing\n", filename);
        return;
    }

    // Write the header
    fprintf(fp, "P6\n%d %d\n255\n", w, h);

    // Write pixel data
    fwrite(pixels, sizeof(struct ppm_pixel), w * h, fp);
    fclose(fp);
}

void write_ppm_2d(const char* filename, struct ppm_pixel** pixels, int w, int h) {

}