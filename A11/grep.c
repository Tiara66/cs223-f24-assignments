#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
/**
 * A multi-threaded program that searches for a keyword in multiple files.
 * 
 * This program divides the files among threads, performs the search concurrently, 
 * and aggregates the results. Each match is printed alongside its thread ID 
 * and the line where it occurred.
 * 
 * @author: Tianyun Song
 * @version: 12.1.2024
 */

// Shared variables
pthread_mutex_t mutex;
int total_matches = 0;

// Thread data structure
typedef struct {
    int thread_id;
    char **files;
    int start;
    int end;
    char *keyword;
} thread_data_t;

/**
 * Searches the specified files for a keyword.
 * 
 * @param arg Pointer to a thread_data_t struct 
 * containing the thread's work details
 * @return NULL
 */
void *search_files(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    int matches = 0;

    for (int i = data->start; i < data->end; i++) {
        FILE *file = fopen(data->files[i], "r");
        if (!file) {
            pthread_mutex_lock(&mutex);
            fprintf(stderr, "Thread %d: Error opening file %s\n", data->thread_id, data->files[i]);
            pthread_mutex_unlock(&mutex);
            continue;
        }

        char line[1024];
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, data->keyword)) {
                pthread_mutex_lock(&mutex);
                printf("(%d) %s: %s", data->thread_id, data->files[i], line);
                pthread_mutex_unlock(&mutex);
                matches++;
            }
        }
        fclose(file);
    }

    pthread_mutex_lock(&mutex);
    total_matches += matches;
    printf("Thread %d: Found %d matches\n", data->thread_id, matches);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

/**
 * Main driver function for the multi-threaded file search program.
 * 
 * @param argc Number of command-line arguments
 * @param argv Command-line arguments: <NumThreads> <Keyword> <Files>
 * @return 0 on success, 1 on failure
 */
int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <NumThreads> <Keyword> <Files>\n", argv[0]);
        return 1;
    }

    int thread_count = atoi(argv[1]);
    if (thread_count <= 0) {
        fprintf(stderr, "Error: Number of threads must be positive.\n");
        return 1;
    }

    char *keyword = argv[2];
    char **files = &argv[3];
    int file_count = argc - 3;

    printf("Searching %d files with %d threads for keyword: '%s'\n", file_count, thread_count, keyword);

    pthread_mutex_init(&mutex, NULL);

    pthread_t *threads = malloc(thread_count * sizeof(pthread_t));
    thread_data_t *thread_data = malloc(thread_count * sizeof(thread_data_t));

    int files_per_thread = file_count / thread_count;
    int extra_files = file_count % thread_count;

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    int start = 0;
    for (int i = 0; i < thread_count; i++) {
        int end = start + files_per_thread + (i < extra_files ? 1 : 0);
        thread_data[i] = (thread_data_t){
            .thread_id = i,
            .files = files,
            .start = start,
            .end = end,
            .keyword = keyword
        };
        printf("Thread %d: Files %d to %d\n", i, start, end - 1);
        pthread_create(&threads[i], NULL, search_files, &thread_data[i]);
        start = end;
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&end_time, NULL);
    double elapsed = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1e6;

    printf("Total matches: %d\n", total_matches);
    printf("Elapsed time: %.6f seconds\n", elapsed);

    free(threads);
    free(thread_data);

    pthread_mutex_destroy(&mutex);
    return 0;
}