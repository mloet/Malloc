#include <stdlib.h>
#include <stdio.h>
#include "heaplib.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "spinlock.h"

/* 
 * Global lock object.  You should use this global lock for any locking you need to do.
 */
#ifdef __riscv
volatile lock_t malloc_lock = {.riscv_lock = 0};
#else
volatile lock_t malloc_lock = {.pthread_lock = PTHREAD_MUTEX_INITIALIZER};
#endif

/* Useful shorthand: casts a pointer to a (char *) before adding */
#define ADD_BYTES(base_addr, num_bytes) (((char *)(base_addr)) + (num_bytes))

typedef struct _block_header_t
{
    unsigned int block_size_t;
    unsigned int in_use;
} block_header_t;

typedef struct _heap_header_t
{
    unsigned int size;
} heap_header_t;

/* (HELPER FUNCTION:) Given a pointer to the heap, returns a pointer to the 
first BLOCK header in the heap. (Takes into account heap alignment issues
so blocks will always be 8 byte aligned, assumes the heap header is 8 bytes.) */
void *get_first_block_head(void *heap)
{
    unsigned int heap_unaligned = 0;
    if ((unsigned long)heap % 8 != 0)
    {
        heap_unaligned = 8 - (unsigned long)heap % 8;
    }
    heap_header_t *header = (heap_header_t *)heap;
    return ADD_BYTES(header, heap_unaligned + 8);
}

/* (HELPER FUNCTION:) Given a pointer to the heap, returns a pointer to the 
first end of the heap. (Takes into account heap alignment issues
so blocks will always be 8 byte aligned.) */
void *end_of_heap(void *heap)
{
    heap_header_t *header = (heap_header_t *)heap;
    return ADD_BYTES(header, header->size);
}

/* (HELPER FUNCTION:) Given a pointer to the current block header, returns a 
pointer to the next block header in the heap. (Assumes there is a next block 
header, may need to fix.) */
void *get_next_block_head(void *block_head)
{
    block_header_t *current = (block_header_t *)block_head;
    return ADD_BYTES(current, current->block_size_t);
}

/* (HELPER FUNCTION:) Given a pointer to a block and the heap, returns a 
pointer to the desired block header. (Assumes the block header
is 8 bytes and that the block is already in the heap.)*/
void *find_block_head(void *heap, void *block)
{
    void *current = get_first_block_head(heap);
    block_header_t *current_block_head = (block_header_t *)(current);
    while (current < end_of_heap(heap))
    {
        current_block_head = (block_header_t *)current;
        if (ADD_BYTES(current_block_head, 8) == block)
        {
            return current_block_head;
        }
        current = get_next_block_head(current);
    }
    return NULL;
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * Set up heap header to store overall data about the heap (size).
 * Allocate one large block equal to 8-byte aligned heap size and point
 * to the front of the block header. The block is set to be free (in_use = 0), 
 * because no memory has been allocated yet. 
 * 
 * 
 */
int hl_init(void *heap, unsigned int heap_size)
{
    unsigned int heap_unaligned = 0;
    if ((unsigned long)heap % 8 != 0)
    {
        heap_unaligned = 8 - (unsigned long)heap % 8;
    }
    if (heap_size < MIN_HEAP_SIZE)
    {
        return FAILURE;
    }
    heap_header_t *header = (heap_header_t *)heap;
    header->size = heap_size;
    block_header_t *block = (block_header_t *)(get_first_block_head(heap));
    block->block_size_t = heap_size - heap_unaligned - 8;
    block->in_use = 0;
    return SUCCESS;
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * Start searching for a free block at the start of the heap. Stop searching
 * (1) If a large enough free block is found 
 *  or (2) If we get to the end of the heap.
 * 
 *  After finding a free block of large enough size, add in the padding and
 *  aligned size of the header to adjust the block size. If there is left over 
 *  space after allocating the block, then create a new free block next to 
 *  the allocated one. 
 * 
 *  (If there is no free block of a valid size found, then return FAILURE)
 */
void *hl_alloc(void *heap, unsigned int block_size)
{
    unsigned int padding = (block_size % 8 == 0) ? 0 : 8 - (block_size % 8);
    void *current = get_first_block_head(heap);
    block_header_t *current_block = (block_header_t *)(current);
    while (current < end_of_heap(heap))
    {
        current_block = (block_header_t *)current;
        if (current_block->in_use == 0 && (current_block->block_size_t - 8 - padding >= block_size))
        {
            current_block->in_use = 1;
            unsigned int old_block_size = current_block->block_size_t;
            current_block->block_size_t = block_size + padding + 8;
            if (old_block_size >= current_block->block_size_t + 8)
            {
                block_header_t *new_free_block = (block_header_t *)(get_next_block_head(current));
                new_free_block->in_use = 0;
                new_free_block->block_size_t = old_block_size - current_block->block_size_t;
            }
            return (ADD_BYTES(current_block, 8));
        }
        current = get_next_block_head(current);
    }
    return FAILURE;
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * Set in_use = 0 for the block given.
 * 
 * Then consider adding coalescing to prevent internal fragmentation.
 */
void hl_release(void *heap, void *block)
{
    if (block == 0 || block == NULL)
    {
        return;
    }
    block_header_t *block_head = (block_header_t *)(find_block_head(heap, block));
    block_head->in_use = 0;
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * allocate, memmove, resize
 */
void *hl_resize(void *heap, void *block, unsigned int new_size)
{
    if (block == 0 || block == NULL)
    {
        return hl_alloc(heap, new_size);
    }
    block_header_t *old_block = (block_header_t *)(ADD_BYTES(block, -8));
    unsigned int old_size = old_block->block_size_t;
    unsigned int padding = (new_size % 8 == 0) ? 0 : 8 - (new_size % 8);
    if (new_size + padding + 8 == old_size)
    {
        return block;
    }
    else if (new_size + padding + 8 <= old_size - 8)
    {
        old_block->block_size_t = new_size + padding + 8;
        block_header_t *new_free_block = (block_header_t *)(get_next_block_head(block));
        new_free_block->in_use = 0;
        new_free_block->block_size_t = old_size - old_block->block_size_t;
        return block;
    }
    void *dest = hl_alloc(heap, new_size);
    if (dest != NULL && dest != 0)
    {
        memmove(dest, block, old_size - 8);
        hl_release(heap, block);
        return dest;
    }
    else
    {
        return FAILURE;
    }
}
