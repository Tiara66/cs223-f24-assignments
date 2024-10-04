/*----------------------------------------------
 * Author: Tianyun Song
 * Date: 10.1.2024
 * Description: This program demonstrates reading a PPM file, writing its
 * content to a new file, then reading back and displaying the new file's
 * content. It handles file operations and memory management with error checking.
 ---------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_ppm.h"
#include "write_ppm.h"

/**
 * Main program to test PPM file reading and writing.
 * @param argc Argument count.
 * @param argv Argument vector (not used).
 * @return Returns 0 on success, or 1 if an error occurs.
 */
int main(int argc, char** argv) {
  int w, h;
  struct ppm_pixel** pixels = read_ppm_2d("feep-raw.ppm", &w, &h);

  // test writing the file to test.ppm, reload it, and print the contents
  if (pixels == NULL) {
    fprintf(stderr, "Error reading PPM file\n");
    return 1;
  }

  // Write the read pixels to a new file
  write_ppm_2d("test.ppm", pixels, w, h);

  // Free the original pixel data
  for (int i = 0; i < h; i++) {
    free(pixels[i]);
  }
  free(pixels);

  // Read back the newly written file to verify content
  struct ppm_pixel** test_pixels = read_ppm_2d("test.ppm", &w, &h);
  if (test_pixels == NULL) {
    fprintf(stderr, "Error reading back the written PPM file\n");
    return 1;
  }

  // Print the pixels to verify correctness
  printf("Testing file test.ppm: %d %d\n", w, h);
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      printf("(%d,%d,%d) ", test_pixels[i][j].red, test_pixels[i][j].green, test_pixels[i][j].blue);
    }
    printf("\n");
  }

  // Clean up
  for (int i = 0; i < h; i++) {
    free(test_pixels[i]);
  }
  free(test_pixels);

  return 0;
}
