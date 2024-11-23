/*----------------------------------------------
 * Author: Tianyun Song
 * Date: 10.12.2024
 * Description: This program reads a PPM file specified by the filename, handles various
 * formats, skips comments, and reads pixel data into a dynamically allocated
 * array. It supports the P6 PPM format.
 * This file contains functions to read PPM files into a 1D array of ppm_pixel
 * structures and to handle errors associated with file I/O and format issues.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "read_ppm.h"

// Choose *one* to implement (do not remove the other one!)

/**
 * Reads a PPM file in binary format (P6) and returns a 1D array of ppm_pixel.
 * Skips comment lines and verifies the format and dimensions.
 *
 * @param filename The path to the PPM file to read.
 * @param w Pointer to an integer where the width of the image will be stored.
 * @param h Pointer to an integer where the height of the image will be stored.
 * @return A dynamically allocated array of ppm_pixel representing the image,
 *         or NULL if an error occurs during file reading or memory allocation.
 */
struct ppm_pixel* read_ppm(const char* filename, int* w, int* h) {
  FILE* fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Error: Unable to open file %s\n", filename);
    return NULL;
  }

  char magic[3];
  if (fscanf(fp, "%2s", magic) != 1 || strcmp(magic, "P6") != 0) {
    fprintf(stderr, "Unsupported file format\n");
    fclose(fp);
    return NULL;
  }

  //debug
  //printf("Magic Number: %s\n", magic);

  int c;
  do {
    c = fgetc(fp);
    if (c == '#') { // Skip comment lines
      while ((c = fgetc(fp)) != '\n' && c != EOF); // Move to end of line
    }
  } while (isspace(c)); // Skip whitespace
  ungetc(c, fp); // Return the last read character back to the stream

  int width, height, maxval;
  if (fscanf(fp, "%d %d %d", &width, &height, &maxval) != 3) {
    fprintf(stderr, "Invalid image size or max value\n");
    fclose(fp);
    return NULL;
  }

  //debug
  //printf("Width: %d, Height: %d, MaxVal: %d\n", width, height, maxval);

  // Read the single whitespace after the max value before the pixel data
  if (fgetc(fp) != '\n') {
    fprintf(stderr, "Expected newline after max value\n");
    fclose(fp);
    return NULL;
  }

  int totalPixels = width * height;
  struct ppm_pixel* pixels = malloc(totalPixels * sizeof(struct ppm_pixel));
  if (pixels == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    fclose(fp);
    return NULL;
  }

  if (fread(pixels, sizeof(struct ppm_pixel), totalPixels, fp) != totalPixels) {
    fprintf(stderr, "File read error\n");
    free(pixels);
    fclose(fp);
    return NULL;
  }

  fclose(fp);
  *w = width;
  *h = height;
  return pixels;
}

struct ppm_pixel** read_ppm_2d(const char* filename, int* w, int* h) {
  return NULL;
}
