#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "read_ppm.h"
#include "write_ppm.h"

/**
 * Generates a Mandelbrot set image based on given input parameters
 * and writes the generated image to a PPM file.
 *
 * This program accepts command-line options for image size and coordinate
 * boundaries. It calculates each pixel’s color based on the Mandelbrot
 * set equation and saves the output in a PPM format with a filename that
 * includes a timestamp.
 *
 * @param argc The number of command-line arguments
 * @param argv The array of command-line arguments
 * @return 0 if the program completes successfully, 1 if memory allocation fails
 * 
 * @author: Tianyun Song
 * @date: 11/6/2024
 */

int main(int argc, char* argv[]) {
    // Default values for image generation
    int size = 480;
    float xmin = -2.0;
    float xmax = 0.47;
    float ymin = -1.12;
    float ymax = 1.12;
    int maxIterations = 1000;

    int opt;
    while ((opt = getopt(argc, argv, ":s:l:r:t:b:")) != -1) {
      switch (opt) {
        case 's': size = atoi(optarg); break;
        case 'l': xmin = atof(optarg); break;
        case 'r': xmax = atof(optarg); break;
        case 't': ymax = atof(optarg); break;
        case 'b': ymin = atof(optarg); break;
        case '?': 
            printf("usage: %s -s <size> -l <xmin> -r <xmax> -b <ymin> -t <ymax>\n", argv[0]); 
            break;
      }
    }
    printf("Generating mandelbrot with size %dx%d\n", size, size);
    printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
    printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);

    // Seed the random number generator for palette generation
    srand(time(0));

    // Allocate memory for the color palette
    struct ppm_pixel *palette = malloc(maxIterations * sizeof(struct ppm_pixel));
    if (!palette) {
        fprintf(stderr, "Failed to allocate memory for color palette\n");
        return 1;
    }

    // Populate palette with random colors for each iteration level
    for (int i = 0; i < maxIterations; i++) {
        palette[i].red = rand() % 256;
        palette[i].green = rand() % 256;
        palette[i].blue = rand() % 256;
    }

    // Allocate space for the image
    struct ppm_pixel *image = malloc(size * size * sizeof(struct ppm_pixel));
    if (!image) {
        fprintf(stderr, "Failed to allocate memory for image\n");
        free(palette);
        return 1;
    }

    // Record start time to measure the computation time
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Calculate Mandelbrot set membership for each pixel
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            // Map pixel position to the corresponding complex plane coordinates
            float x0 = xmin + (float)col / size * (xmax - xmin);
            float y0 = ymin + (float)row / size * (ymax - ymin);
            float x = 0, y = 0;
            int iter = 0;

            // Mandelbrot iteration loop
            while (iter < maxIterations && x * x + y * y < 4) {
                float xtemp = x * x - y * y + x0;
                y = 2 * x * y + y0;
                x = xtemp;
                iter++;
            }

            // Assign color based on escape iteration count or default to black
            if (iter < maxIterations) {
                // Color based on escape time
                image[row * size + col] = palette[iter];
            } else {
                // Black for points inside the set
                image[row * size + col].red = 0;
                image[row * size + col].green = 0;
                image[row * size + col].blue = 0;
            }
        }
    }

    // Record end time and compute the total elapsed time in seconds
    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + 
        (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Computed mandelbrot set (%dx%d) in %f seconds\n", size, size, elapsed);

    // Generate a unique filename using the image size and current timestamp
    char filename[64];
    snprintf(filename, sizeof(filename), "mandelbrot-%d-%ld.ppm", size, time(0));
    write_ppm(filename, image, size, size);
    printf("Writing file: %s\n", filename);

    // Free allocated memory for palette and image data
    free(palette);
    free(image);
    return 0;
}
