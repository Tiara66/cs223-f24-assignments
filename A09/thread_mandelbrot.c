#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h> 
#include <time.h>
#include <pthread.h>
#include "read_ppm.h"
#include "write_ppm.h"

/**
 * Multi-threaded Mandelbrot set generator for creating a PPM image.
 * Each thread processes a quadrant of the image, calculating color
 * values based on Mandelbrot set membership and storing them in a
 * shared image array.
 *
 * @author: Tianyun Song
 * @date: 11/6/2024
 */

// Define a struct to hold data for each thread
typedef struct {
    int start_row;
    int end_row;
    int start_col;
    int end_col;
    int size;
    float xmin, xmax, ymin, ymax;
    int maxIterations;
    struct ppm_pixel* image;
    struct ppm_pixel* palette;
    pthread_t thread_id;
} ThreadData;

/**
 * Computes a quadrant of the Mandelbrot set for this thread’s assigned block.
 * Maps pixel coordinates to the complex plane, iterates the Mandelbrot
 * function, and assigns a color based on the iteration count.
 *
 * @param arg Pointer to ThreadData struct with the thread’s configuration
 */
void* compute_mandelbrot(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int size = data->size;
    int maxIterations = data->maxIterations;
    float xmin = data->xmin;
    float xmax = data->xmax;
    float ymin = data->ymin;
    float ymax = data->ymax;
    
    printf("Thread %ld) sub-image block: cols (%d, %d) to rows (%d, %d)\n",
           data->thread_id, data->start_col, data->end_col, data->start_row, data->end_row);

    for (int row = data->start_row; row < data->end_row; row++) {
        for (int col = data->start_col; col < data->end_col; col++) {
            float x0 = xmin + (float)col / size * (xmax - xmin);
            float y0 = ymin + (float)row / size * (ymax - ymin);
            float x = 0, y = 0;
            int iter = 0;

            // Mandelbrot iteration
            while (iter < maxIterations && x * x + y * y < 4) {
                float xtemp = x * x - y * y + x0;
                y = 2 * x * y + y0;
                x = xtemp;
                iter++;
            }

            // Assign color based on escape iteration
            if (iter < maxIterations) {
                data->image[row * size + col] = data->palette[iter];
            } else {
                data->image[row * size + col].red = 0;
                data->image[row * size + col].green = 0;
                data->image[row * size + col].blue = 0;
            }
        }
    }
    
    printf("Thread %ld) finished\n", data->thread_id);
    pthread_exit(NULL);
}

/**
 * Main function to initialize and manage multi-threaded Mandelbrot set generation.
 * Parses command-line options for image size and coordinates, sets up color palette,
 * spawns threads to compute each quadrant, measures execution time, and writes output
 * to a PPM file.
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line arguments
 * @return 0 on success, 1 if an error occurs
 */
int main(int argc, char* argv[]) {
    int size = 480;
    float xmin = -2.0;
    float xmax = 0.47;
    float ymin = -1.12;
    float ymax = 1.12;
    int maxIterations = 1000;
    int numProcesses = 4;

    int opt;
    while ((opt = getopt(argc, argv, ":s:l:r:t:b:p:")) != -1) {
      switch (opt) {
        case 's': size = atoi(optarg); break;
        case 'l': xmin = atof(optarg); break;
        case 'r': xmax = atof(optarg); break;
        case 't': ymax = atof(optarg); break;
        case 'b': ymin = atof(optarg); break;
        case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> "
          "-b <ymin> -t <ymax> -p <numProcesses>\n", argv[0]); break;
      }
    }
    printf("Generating mandelbrot with size %dx%d\n", size, size);
    printf("  Num processes = %d\n", numProcesses);
    printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
    printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);

    srand(time(0));

    // Allocate and populate the color palette
    struct ppm_pixel *palette = malloc(maxIterations * sizeof(struct ppm_pixel));
    if (!palette) {
        fprintf(stderr, "Failed to allocate memory for color palette\n");
        return 1;
    }

    for (int i = 0; i < maxIterations; i++) {
        palette[i].red = rand() % 256;
        palette[i].green = rand() % 256;
        palette[i].blue = rand() % 256;
    }

    // Allocate memory for the image
    struct ppm_pixel *image = malloc(size * size * sizeof(struct ppm_pixel));
    if (!image) {
        fprintf(stderr, "Failed to allocate memory for image\n");
        free(palette);
        return 1;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Set up threads and assign quadrants
    pthread_t threads[4];
    ThreadData thread_data[4];
    int half_size = size / 2;

    for (int i = 0; i < 4; i++) {
        thread_data[i].size = size;
        thread_data[i].xmin = xmin;
        thread_data[i].xmax = xmax;
        thread_data[i].ymin = ymin;
        thread_data[i].ymax = ymax;
        thread_data[i].maxIterations = maxIterations;
        thread_data[i].image = image;
        thread_data[i].palette = palette;

        // Define each quadrant
        if (i == 0) { // Top-left
            thread_data[i].start_row = 0;
            thread_data[i].end_row = half_size;
            thread_data[i].start_col = 0;
            thread_data[i].end_col = half_size;
        } else if (i == 1) { // Top-right
            thread_data[i].start_row = 0;
            thread_data[i].end_row = half_size;
            thread_data[i].start_col = half_size;
            thread_data[i].end_col = size;
        } else if (i == 2) { // Bottom-left
            thread_data[i].start_row = half_size;
            thread_data[i].end_row = size;
            thread_data[i].start_col = 0;
            thread_data[i].end_col = half_size;
        } else { // Bottom-right
            thread_data[i].start_row = half_size;
            thread_data[i].end_row = size;
            thread_data[i].start_col = half_size;
            thread_data[i].end_col = size;
        }

        // Create each thread and check for errors
        if (pthread_create(&threads[i], NULL, compute_mandelbrot, 
            (void*)&thread_data[i]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            free(palette);
            free(image);
            return 1;
        }
        thread_data[i].thread_id = threads[i];
    }

    // Join threads and handle errors if any
    for (int i = 0; i < 4; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error joining thread %d\n", i);
            free(palette);
            free(image);
            return 1;
        }
    }

    // End timing and calculate elapsed time
    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Computed mandelbrot set (%dx%d) in %f seconds\n", size, size, elapsed);

    // Create output file
    char filename[64];
    snprintf(filename, sizeof(filename), "mandelbrot-%d-%ld.ppm", size, time(0));
    write_ppm(filename, image, size, size);
    printf("Writing file: %s\n", filename);

    // Free allocated memory
    free(palette);
    free(image);
    return 0;
}
