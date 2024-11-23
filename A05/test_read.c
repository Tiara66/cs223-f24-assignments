/*----------------------------------------------
 * Author: Tianyun Song
 * Date: 10.1.2024
 * Description: This program reads PPM image data from "feep-raw.ppm" and
 * displays each pixel's RGB values. It demonstrates proper memory management
 * by dynamically allocating memory for pixel data and freeing it appropriately.
 * It also handles errors in file reading gracefully.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "read_ppm.h"

/**
 * The main function reads a PPM file and prints out its pixel grid.
 * It demonstrates error handling and memory management.
 *
 * @return Returns 0 on success, 1 on error.
 */
int main() {
  int w, h;
  struct ppm_pixel** pixels = read_ppm_2d("feep-raw.ppm", &w, &h);

  // todo: print out the grid of pixels
  if (pixels == NULL) {
    fprintf(stderr, "Error reading PPM file\n");
    return 1;
  }

  printf("Testing file feep-raw.ppm: %d %d\n", w, h);
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      printf("(%d,%d,%d) ", pixels[i][j].red, pixels[i][j].green, pixels[i][j].blue);
    }
    printf("\n");
  }

  // Free memory
  for (int i = 0; i < h; i++) {
    free(pixels[i]);
  }

  free(pixels);
  return 0;
}