#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include "read_ppm.h"
#include "write_ppm.h"

#define MAX_ITER 1000
#define GAMMA 0.681

/**
 * Buddhabrot Generator
 *
 * This program generates a Buddhabrot visualization using multithreading.
 * It divides the work among threads, each processing a quadrant of the
 * image. The program computes Mandelbrot set membership, escaping point
 * counts, and gamma-corrected coloring for the visualization.
 *
 * Usage: ./buddhabrot -s <size> -l <xmin> -r <xmax> -b <ymin> -t <ymax>
 *                     -p <numProcesses>
 *
 * Output: The image is written to a PPM file with the format
 *         buddhabrot-<size>-<timestamp>.ppm.
 *
 * @author: Tianyun Song
 * @version: November 15, 2024
 */

/**
 * Rounds a double to the nearest integer.
 * @param x The value to round
 * @return The rounded integer
 */
int custom_round(double x) {
    return (int)(x + (x >= 0 ? 0.5 : -0.5));
}

/**
 * Computes the base-10 logarithm of an integer.
 * @param x The input value
 * @return The logarithm of x
 */
double custom_log(int x) {
    double result = 0.0;
    int base = 2;
    while (x > 1) {
        x /= base;
        result += 0.30102999566;  // log2 to log10
    }
    return result;
}

/**
 * Computes base raised to the power of exponent.
 * @param base The base value
 * @param exponent The exponent value
 * @return base raised to the power of exponent
 */
double custom_pow(double base, double exponent) {
    if (exponent == 0) return 1.0;
    double result = 1.0;
    int i;
    for (i = 0; i < (int)exponent; i++) {
        result *= base;
    }
    return result;
}

// Structure to hold thread arguments and pixel data
typedef struct {
    int startRow, endRow;
    int startCol, endCol;
    int size;
    float xmin, xmax, ymin, ymax;
    int *membership;
    int *counts;
    struct ppm_pixel *image;
} ThreadData;

// Global variables for synchronization
pthread_barrier_t barrier;
pthread_mutex_t countMutex;
int maxCount = 0;

/**
 * Determines Mandelbrot set membership for a quadrant.
 * @param data The thread data containing the quadrant range and other info
 */
void check_mandelbrot(ThreadData *data) {
    float xScale = (data->xmax - data->xmin) / data->size;
    float yScale = (data->ymax - data->ymin) / data->size;

    for (int row = data->startRow; row < data->endRow; row++) {
        for (int col = data->startCol; col < data->endCol; col++) {
            float x0 = data->xmin + col * xScale;
            float y0 = data->ymin + row * yScale;
            float x = 0, y = 0;
            int iter = 0;

            // Check if the point escapes within MAX_ITER iterations
            while (iter < MAX_ITER && x * x + y * y < 4) {
                float xtmp = x * x - y * y + x0;
                y = 2 * x * y + y0;
                x = xtmp;
                iter++;
            }
            data->membership[row * data->size + col] = (iter >= MAX_ITER);
        }
    }
}

/**
 * Computes the visited counts for escaping points.
 * @param data The thread data containing the quadrant range and other info
 */
void compute_counts(ThreadData *data) {
    int *localCounts = calloc(data->size * data->size, sizeof(int));
    for (int i = 0; i < data->size * data->size; i++) {            
        localCounts[i] = 0;
    }

    float xScale = (data->xmax - data->xmin) / data->size;
    float yScale = (data->ymax - data->ymin) / data->size;

    for (int row = data->startRow; row < data->endRow; row++) {
        for (int col = data->startCol; col < data->endCol; col++) {
            int idx = row * data->size + col;

            if (!data->membership[idx]) {
                float x0 = data->xmin + col * xScale;
                float y0 = data->ymin + row * yScale;
                float x = 0, y = 0;

                // Iterate through the escaping trajectory
                while (x * x + y * y < 4) {
                    float xtmp = x * x - y * y + x0;
                    y = 2 * x * y + y0;
                    x = xtmp;

                    int yrow = custom_round(data->size * (y - data->ymin) / (data->ymax - data->ymin));
                    int xcol = custom_round(data->size * (x - data->xmin) / (data->xmax - data->xmin));

                    if (yrow >= 0 && yrow < data->size && xcol >= 0 && xcol < data->size) {
                        localCounts[yrow * data->size + xcol]++;
                    }
                }
            }
        }
    }

    // Merge local counts into global counts
    int localMax = 0;
    for (int i = 0; i < data->size * data->size; i++) {
        data->counts[i] += localCounts[i];
        if (data->counts[i] > localMax) {
            localMax = data->counts[i];
        }
    }

    // Update maxCount with the local maximum
    pthread_mutex_lock(&countMutex);
    if (localMax > maxCount) {
        maxCount = localMax;
    }
    pthread_mutex_unlock(&countMutex);
    free(localCounts);
}

/**
 * Computes pixel colors based on counts and gamma correction.
 * @param data The thread data containing the quadrant range and other info
 * @param image The image array to store pixel colors
 */
void compute_colors(ThreadData *data, struct ppm_pixel *image) {
    for (int row = data->startRow; row < data->endRow; row++) {
        for (int col = data->startCol; col < data->endCol; col++) {
            float value = 0;
            int idx = row * data->size + col;

            if (data->counts[idx] > 0) {
                value = custom_log(data->counts[idx]) / custom_log(maxCount);
                value = custom_pow(value, 1.0 / GAMMA);
            }
            image[idx].red = (unsigned char)(value * 255);
            image[idx].green = (unsigned char)(value * 255);
            image[idx].blue = (unsigned char)(value * 255);
        }
    }
}

/**
 * Thread function to execute all Buddhabrot steps.
 * @param arg Pointer to thread data
 * @return NULL
 */
void *start(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    printf("Thread %lu) sub-image block: cols (%d,%d) to rows (%d,%d)\n",
           pthread_self(), data->startCol, data->endCol, data->startRow, data->endRow);

    // Determine Mandelbrot set membership
    check_mandelbrot(data);
    // Compute visited counts
    compute_counts(data);
    // Synchronize threads
    pthread_barrier_wait(&barrier);
    // Compute colors
    compute_colors(data, data->image);

    printf("Thread %lu) finished\n", pthread_self());
    pthread_exit(NULL);
}

/**
 * Main driver function for the Buddhabrot Generator.
 *
 * Parses command-line arguments, initializes data structures, and
 * manages multithreaded computation of the Buddhabrot visualization.
 * Outputs the generated image in PPM format with the correct filename.
 *
 * @param argc The number of command-line arguments
 * @param argv The array of command-line arguments
 * @return Exit status of the program
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
    printf("Generating buddhabrot with size %dx%d\n", size, size);
    printf("  Num processes = %d\n", numProcesses);
    printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
    printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);


    clock_t startTime = clock(); // Start timing

    int *membership = malloc(size * size * sizeof(int));
    int *counts = malloc(size * size * sizeof(int));
    struct ppm_pixel *image = malloc(size * size * sizeof(struct ppm_pixel));

    for (int i = 0; i < size * size; i++) {
        membership[i] = 0;
        counts[i] = 0;
        image[i].red = image[i].green = image[i].blue = 0;
    }

    // Allocate memory for thread data and thread handles
    ThreadData *data = malloc(numProcesses * sizeof(ThreadData));
    pthread_t *threads = malloc(numProcesses * sizeof(pthread_t));

    // Initialize synchronization primitives
    pthread_barrier_init(&barrier, NULL, numProcesses);
    pthread_mutex_init(&countMutex, NULL);

    // Determine the boundaries for each thread
    int rowMid = size / 2;
    int colMid = size / 2;

    // Create threads to handle each quadrant of the image
    for (int i = 0; i < numProcesses; i++) {
        data[i].size = size;
        data[i].xmin = xmin;
        data[i].xmax = xmax;
        data[i].ymin = ymin;
        data[i].ymax = ymax;
        data[i].membership = membership;
        data[i].counts = counts;
        data[i].image = image;

        if (i == 0) { 
            data[i].startRow = 0; 
            data[i].endRow = rowMid; 
            data[i].startCol = 0;
            data[i].endCol = colMid; 
        }
        if (i == 1) { 
            data[i].startRow = 0; 
            data[i].endRow = rowMid; 
            data[i].startCol = colMid; 
            data[i].endCol = size; 
        }
        if (i == 2) { 
            data[i].startRow = rowMid; 
            data[i].endRow = size; 
            data[i].startCol = 0; 
            data[i].endCol = colMid; 
        }
        if (i == 3) { 
            data[i].startRow = rowMid; 
            data[i].endRow = size; 
            data[i].startCol = colMid; 
            data[i].endCol = size; 
        }

        pthread_create(&threads[i], NULL, start, &data[i]);
    }

    // Wait for all threads to complete their work
    for (int i = 0; i < numProcesses; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t endTime = clock(); // End timing
    double elapsed = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;
    printf("Computed buddhabrot set (%dx%d) in %.6f seconds\n", size, size, elapsed);

    // Generate the output filename with a timestamp
    time_t currentTime = time(0);
    char filename[256];
    snprintf(filename, sizeof(filename), "buddhabrot-%d-%ld.ppm", size, currentTime);
    write_ppm(filename, image, size, size);
    printf("Writing file: %s\n", filename);

    // Clean up synchronization primitives
    pthread_barrier_destroy(&barrier); 
    pthread_mutex_destroy(&countMutex);

    // Free dynamically allocated memory
    free(membership);
    free(counts);
    free(image);
    free(data);
    free(threads);

    return 0;
}