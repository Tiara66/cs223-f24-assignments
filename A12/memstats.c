/**
 * Memory management test program for simulating allocations and frees.
 * This program simulates the allocation and freeing of memory chunks,
 * gathering memory statistics at each round.
 *
 * @author: Tianyun Song
 * @version: December 5, 2024
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include "rand.h"

#define ROUNDS 3
#define BUFFER 5
#define LOOP 10

/**
 * Structure representing a memory chunk.
 * Each chunk tracks its size, the amount of used memory, and the next chunk.
 */
struct chunk {
  int size;
  int used;
  struct chunk *next;
};

// Pointer to the head of the free list
extern struct chunk* flist;

/**
 * Prints memory statistics for the current state of the free list and buffer.
 *
 * @param freelist Pointer to the head of the free list
 * @param buffer Array of pointers representing allocated memory chunks
 * @param len Length of the buffer array
 */
void memstats(struct chunk* freelist, void* buffer[], int len) {
    int free_blocks = 0;
    int free_memory = 0;
    
    // Traverse the freelist to gather statistics about free memory
    struct chunk* node = freelist;
    while (node != NULL) {
        free_blocks++;
        free_memory += node->size;
        node = node->next;
    }

    int used_blocks = 0;
    int used_memory = 0;
    int unapplied_memory = 0;

    // Traverse the buffer to gather statistics about used memory
    for (int i = 0; i < len; i++) {
        if (buffer[i] != NULL) {
            used_blocks++;
            // Jump back to the header to read the memory
            struct chunk* header = (struct chunk*)((struct chunk*)buffer[i] - 1);
            used_memory += header->size;
            // If there is unused memory
            if (header->size > header->used) {
                unapplied_memory += (header->size - header->used);
            }
        }
    }

    int total_memory = used_memory + free_memory;
    float underutilized_memory = (float)unapplied_memory / (float)used_memory;
    int total_blocks = free_blocks + used_blocks;

    // Print out the statistics
    printf("Total blocks: %d Free blocks: %d Used blocks: %d\n", total_blocks, free_blocks, used_blocks);
    printf("Total memory allocated: %d Free memory: %d Used memory: %d\n", total_memory, free_memory, used_memory);
    // debug : printf("Unaaplied_memory: %d\n", unapplied_memory);
    printf("Underutilized memory: %.2f\n", underutilized_memory);
}

/**
 * Main driver function to run the memory allocation and freeing simulation.
 *
 * @param argc Argument count
 * @param argv Argument values
 * @return Returns 0 on success, 1 on memory allocation failure
 */
int main ( int argc, char* argv[]) {

  printf("Starting test..\n");

  srand(100);

  double timer;
  struct timeval tstart, tend;

  gettimeofday(&tstart, NULL);

  void *buffer[BUFFER];
  for (int i = 0; i < BUFFER; i++) {
    buffer[i] = NULL;
  }

  void *init = sbrk(0);
  void *current;
  printf("The initial top of the heap is %p.\n", init);
  for (int j = 0 ; j < ROUNDS; j++) {
    printf("---------------\n%d\n" , j);

    for (int i= 0 ; i < LOOP ; i++) {
      int index = rand() % BUFFER;
      if (buffer[index] != NULL) {
        free(buffer[index]);
        buffer[index] = NULL;
        printf("Freeing index %d\n", index);
      } 
      else {
        size_t size = (size_t) randExp(8, 4000); 
        int *memory = NULL;
        memory = malloc(size);

        if (memory == NULL) {
          fprintf(stderr, "malloc failed\n");
          return(1);
        } 
        *memory = 123;
        buffer[index] = memory;
        printf("Allocating %d bytes at index %d\n", (int) size, index);
      }
    }
    extern struct chunk* flist;
    current = sbrk(0);
    int allocated = current - init;
    init = current;

    printf("The new top of the heap is %p.\n", current);
    printf("Increased by %d (0x%x) bytes\n", allocated, allocated);
    memstats(flist, buffer, BUFFER);
  }

  for (int i = 0; i < BUFFER; i++) {
    free(buffer[i]); 
  }
  gettimeofday(&tend, NULL);
  timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;
  printf("Time is %g\n", timer);

  return 0 ;
}
