#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Custom implementation of malloc and free using a free list to manage memory.
 * This file includes functions to allocate (`malloc`) and free (`free`) memory,
 * while maintaining a list of available memory chunks for future allocations.
 *
 * @author: Tianyun Song
 * @version: December 5, 2024
 */


/**
 * Structure representing a memory chunk.
 * Each chunk keeps track of its size, the amount of memory currently in use,
 * and a pointer to the next chunk in the free list.
 */
struct chunk {
  int size;
  int used;
  struct chunk *next;
};


// Pointer to the head of the free list.
struct chunk *flist = NULL;

/**
 * Custom implementation of malloc that searches the free list for an available chunk.
 * If no suitable chunk is found, more memory is requested from the system.
 *
 * @param size The size of memory requested.
 * @return Pointer to the allocated memory or NULL if allocation fails.
 */
void *malloc (size_t size) {
  // TODO: Implement malloc with a free list (See reading for details)
  // return NULL;
  if (size == 0) {
    return NULL;
  }

  struct chunk *prev = NULL;
  struct chunk *current = flist;

  // Search for a suitable chunk in the free list (first-fit strategy).
  while (current != NULL) {
    if (current->size >= size) {
      // Found a suitable chunk.
      current->used = size;

      // Remove the chunk from the free list.
      if (prev == NULL) {
        flist = current->next;
      } else {
        prev->next = current->next;
      }

      return (void *)(current + 1);
    }
    prev = current;
    current = current->next;
  }

  // No suitable chunk found, request more memory from sbrk.
  struct chunk *new_chunk = sbrk(size + sizeof(struct chunk));
  if (new_chunk == (void *)-1) {
    return NULL; // sbrk failed.
  }
  new_chunk->size = size;
  new_chunk->used = size;
  new_chunk->next = NULL;

  return (void *)(new_chunk + 1);
}

/**
 * Custom implementation of free that adds the given memory block back to the free list.
 *
 * @param memory Pointer to the memory block to be freed.
 */
void free(void *memory) {
  // TODO: Implement malloc with a free list (See reading for details)
  // return;
  if (memory == NULL) {
    return;
  }

  // Get the chunk header.
  struct chunk *chunk_to_free = (struct chunk *)memory - 1;
  chunk_to_free->used = 0;

  // Add the chunk back to the free list (at the beginning).
  chunk_to_free->next = flist;
  flist = chunk_to_free;
}
