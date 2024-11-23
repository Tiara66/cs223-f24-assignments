/*----------------------------------------------
 * Author: Tianyun Song
 * Date: 10.1.2024
 * Description: This program writes a 2D array of ppm_pixel structures to a file
 * in PPM format. It includes error handling for file operations and ensures that
 * all operations conform to the PPM file format specifications.
 ---------------------------------------------*/
#include "write_ppm.h"
#include <stdio.h>
#include <string.h>

// implement *one* (but do not remove the other one)!

void write_ppm(const char* filename, struct ppm_pixel* pixels, int w, int h) {

}

/**
 * Writes a 2D array of ppm_pixel structures to a PPM file.
 * @param filename The name of the file to write the pixels to.
 * @param pixels A 2D array of ppm_pixel structures containing the image data.
 * @param w The width of the image.
 * @param h The height of the image.
 */
void write_ppm_2d(const char* filename, struct ppm_pixel** pixels, int w, int h) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Failed to open file %s for writing\n", filename);
        return;
    }

    fprintf(fp, "P6\n%d %d\n255\n", w, h);
    for (int i = 0; i < h; i++) {
        fwrite(pixels[i], sizeof(struct ppm_pixel), w, fp);
    }

    fclose(fp);
}
