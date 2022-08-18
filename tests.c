#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "heaplib.h"
#include <pthread.h>

#define HEAP_SIZE 1024
#define NUM_TESTS 24
#define NPOINTERS 100

// TODO: Add test descriptions as you add more tests...
const char *test_descriptions[] = {
    /* our SPEC tests */
    /* 0 */ "single init, should return without error",
    /* 1 */ "single init then single alloc, should pass",
    /* 2 */ "single alloc which should fail b/c heap is not big enough",
    /* 3 */ "multiple allocs, verifying no hard-coded block limit",
    /* your SPEC tests */
    /* 4  */ "non - 8 byte aligned heap, verifies blocks are 8 byte aligned",
    /* 5  */ "checks to make sure all allocs are 8-byte aligned",
    /* 6  */ "non - 8 byte aligned heap, verifies blocks are 8 byte aligned",
    /* 7  */ "Freeing (releasing) a block should make the same block usable again",
    /* 8  */ "If the block is NULL release should act as a NOP",
    /* 9  */ "If the block is NULL resize acts as alloc",
    /* 10 */ "After resizing a block the contents of the block is preserved",
    /* 11 */ "your description here",
    /* 12 */ "your description here",
    /* 13 */ "your description here",
    /* 14 */ "your description here",
    /* 15 */ "example threading test",
    /* STRESS tests */
    /* 16 */ "alloc & free, stay within heap limits",
    /* 17 */ "your description here",
    /* 18 */ "your description here",
    /* 19 */ "your description here",
    /* 20 */ "your description here",
    /* 21 */ "your description here",
    /* 22 */ "your description here",
    /* 23 */ "your description here",
};

/* ------------------ COMPLETED SPEC TESTS ------------------------- */

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init
 * SPECIFICATION BEING TESTED:
 * hl_init should successfully complete (without producing a seg
 * fault) for a HEAP_SIZE of 1024 or more.
 *
 * MANIFESTATION OF ERROR:
 * A basic test of hl_init.  If hl_init has an eggregious programming
 * error, this simple call would detect the problem for you by
 * crashing.
 *
 * Note: this shows you how to create a test that should succeed.
 */
int test00()
{

    // simulated heap is just a big array
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    return SUCCESS;
}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * If there is room in the heap for a request for a block, hl_alloc
 * should sucessfully return a pointer to the requested block.
 *
 * MANIFESTATION OF ERROR:
 * The call to hl_alloc will return NULL if the library cannot find a
 * block for the user (even though the test is set up such that the
 * library should be able to succeed).
 */
int test01()
{

    // simulated heap is just a big array
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    // if this returns null, test01 returns FAILURE (==0)
    return (hl_alloc(heap, HEAP_SIZE / 2) != NULL);
}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * If there is not enough room in the heap for a request for a block,
 * hl_alloc should return NULL.
 *
 * MANIFESTATION OF ERROR:
 * This test is designed to request a block that is definitely too big
 * for the library to find. If hl_alloc returns a pointer, the library is flawed.
 *
 * Notice that heaplame's hl_alloc does NOT return NULL. (This is one
 * of many bugs in heaplame.)
 *
 * Note: this shows you how to create a test that should fail.
 *
 * Surely it would be a good idea to test this SPEC with more than
 * just 1 call to alloc, no?
 */
int test02()
{

    // simulated heap is just an array
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    // if this returns NULL, test02 returns SUCCESS (==1)
    return !hl_alloc(heap, HEAP_SIZE * 2);
}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * There should be no hard-coded limit to the number of blocks heaplib
 * can support. So if the heap gets larger, so should the number of
 * supported blocks.
 *
 * MANIFESTATION OF ERROR:
 * See how many blocks are supported with heap size N. This number should
 * increase with heap size 2N. Otherwise fail!
 *
 * Note: unless it is fundamentally changed, heaplame will always fail
 * this test. That is fine. The test will have more meaning when run on
 * your heaplib.c
 */
int test03()
{

    // now we simulate 2 heaps, once 2x size of the first
    char heap[HEAP_SIZE], heap2[HEAP_SIZE * 2];
    int num_blocks = 0;
    int num_blocks2 = 0;

    hl_init(heap, HEAP_SIZE);

    while (true)
    {
        int *array = hl_alloc(heap, 8);
        if (array)
            num_blocks++;
        else
            break;
    }

    hl_init(heap2, HEAP_SIZE * 2);

    while (true)
    {
        int *array = hl_alloc(heap2, 8);
        if (array)
            num_blocks2++;
        else
            break;
    }
#ifdef PRINT_DEBUG
    printf("%d blocks (n), then %d blocks (2n) allocated\n", num_blocks, num_blocks2);
#endif

    // hoping to return SUCCESS (==1)
    return (num_blocks2 > num_blocks);
}

/* ------------------ YOUR SPEC TESTS ------------------------- */

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: init and alloc
 * SPECIFICATION BEING TESTED: Same number of blocks should be allocated
 * because the same number of 8 byte aligned blocks are possible.
 *
 * MANIFESTATION OF ERROR: They should both allocate the same number of blocks.
 * A broken malloc will only allocate a small number of blocks or will try to 
 * allocate more to the larger heap. (The larger heap is not 8-byte aligned 
 * which is allowed, however the left over memory is not usable because it's
 * impossible to be aligned properly. )
 *
 */
int test04()
{
    char heap[2052];

    char heap2[2048];
    int num_blocks = 0;
    int num_blocks2 = 0;

    hl_init(heap, 2052);

    while (true)
    {
        int *array = hl_alloc(heap, 8);
        if (array)
            num_blocks++;
        else
            break;
    }

    hl_init(heap2, 2048);

    while (true)
    {
        int *array = hl_alloc(heap2, 8);
        if (array)
            num_blocks2++;
        else
            break;
    }
#ifdef PRINT_DEBUG
    printf("%d blocks (n), then %d blocks (2n) allocated\n", num_blocks, num_blocks2);
#endif
    // hoping to return SUCCESS (==1)
    return (num_blocks2 == num_blocks && (num_blocks > 10));
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: init and alloc
 * SPECIFICATION BEING TESTED: Allocated blocks must be 8-byte aligned.
 *
 *
 * MANIFESTATION OF ERROR: Incorrect padding or other 8 byte alignment
 * issues. 
 *
 */
int test05()
{
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    int *block1 = hl_alloc(heap, 16);
    int *block2 = hl_alloc(heap, 48);
    int *block3 = hl_alloc(heap, 69);
    int *block4 = hl_alloc(heap, 15);

    // make sure all pointers are 8-byte aligned
    return (uintptr_t)block1 % 8 == 0 && (uintptr_t)block2 % 8 == 0 && (uintptr_t)block3 % 8 == 0 && (uintptr_t)block4 % 8 == 0;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: init, alloc
 * SPECIFICATION BEING TESTED: Checks blocks are 8 byte aligned even if the
 * heap is not. 
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test06()
{
    char heap[2052];
    hl_init(heap, 2052);
    int num_blocks = 0;
    int *block;
    while (num_blocks < 85)
    {
        block = hl_alloc(heap, 8);
        num_blocks += 1;
    }
    return (uintptr_t)block % 8 == 0;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: init, alloc, release
 * SPECIFICATION BEING TESTED: Freeing (releasing) a block should make the same
 * block usable again.
 *
 * (NOT SURE HOW TO TEST THIS SPEC)
 * 
 * MANIFESTATION OF ERROR: 
 *
 */
int test07()
{
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    hl_alloc(heap, 16);
    int *block = hl_alloc(heap, 32);
    hl_release(heap, block);
    int *block2 = hl_alloc(heap, 32);
    return block == block2;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: init, alloc, release
 * SPECIFICATION BEING TESTED: If the block is NULL release should act as a NOP
 *
 * (NOT SURE HOW TO TEST THIS SPEC)
 * 
 * MANIFESTATION OF ERROR: 
 *
 */
int test08()
{
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    int *block1 = hl_alloc(heap, 8);
    int *block2 = NULL;
    hl_release(heap, block2);
    int *block3 = hl_alloc(heap, 8);
    return (block1 < block3);
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: init, alloc, release
 * SPECIFICATION BEING TESTED: More extensive version of test07
 * 
 * MANIFESTATION OF ERROR: 
 *
 */
int test09()
{
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    hl_alloc(heap, 16);
    hl_alloc(heap, 32);
    int *block = hl_alloc(heap, 32);
    hl_alloc(heap, 32);
    hl_release(heap, block);
    int *block2 = hl_alloc(heap, 32);
    return block == block2;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: Checks that a single block can be resized to be 
 * smaller than before. 
 * 
 * SPECIFICATION BEING TESTED: After resizing a block the contents of the block 
 * are preserved.
 *
 * (SOMETHING IS WRONG WITH RESIZE THAT CAUSES A SEGFAULT HERE, WE STILL
 * NEED TO FIX RESIZE)
 * 
 * The test should be correct other than that.
 * 
 * MANIFESTATION OF ERROR:
 *
 */
int test10()
{
    char *heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    int *block = hl_alloc(heap, 8);
    block[0] = 'x';
    block[1] = 'y';
    block[2] = 'z';
    block[3] = 'k';
    hl_resize(heap, block, 7);
    return (block[0] == 'x' && block[1] == 'y' && block[2] == 'z' && block[3] == 'k');
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: resize
 * SPECIFICATION BEING TESTED: If the block is NULL resize acts as alloc
 *
 * (NOT SURE HOW TO TEST THIS SPEC)
 * 
 * MANIFESTATION OF ERROR:
 *
 */
int test11()
{
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    int *block = hl_alloc(heap, 16);
    hl_release(heap, block);
    int *dest = hl_alloc(heap, 128);

    return dest != 0 && dest != NULL;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: resize and release
 * SPECIFICATION BEING TESTED: If the block is NULL resize acts as alloc
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test12()
{
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    int *block = hl_alloc(heap, 16);
    hl_release(heap, block);
    int *block2 = hl_resize(heap, NULL, 16);
    return block == block2;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED: Checks blocks are 8-byte aligned even if the heap
 * is not. 
 *
 * 
 * 
 * MANIFESTATION OF ERROR:
 *
 */
int test13()
{
    char heap[HEAP_SIZE * 3];
    void *new_heap = &heap[97];
    hl_init(new_heap, HEAP_SIZE);
    int *block1 = hl_alloc(new_heap, 8);
    int *block2 = hl_alloc(new_heap, 147);
    int *block3 = hl_alloc(new_heap, 26);
    int *block4 = hl_alloc(new_heap, 16);
    return (uintptr_t)block1 % 8 == 0 && (uintptr_t)block2 % 8 == 0 && (uintptr_t)block3 % 8 == 0 && (uintptr_t)block4 % 8 == 0;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test14()
{
    return FAILURE;
}

typedef struct
{
    int arg1;
    void *heap;
    pthread_barrier_t *barrier;
} arg_struct;

/* The signature for a function run as a thread is a single void* argument returning a void*.
 * If you want to pass in arguments, you need to define a struct of arguments and cast manually.
 */

void *thread_function(void *ptr)
{
    arg_struct *args = (arg_struct *)ptr;
    pthread_barrier_t *barrier = args->barrier;
    //wait for all threads to synchronize at a barrier so they will run concurrently
    pthread_barrier_wait(barrier);
    hl_alloc(args->heap, 16);
    //do stuff with your arguments here
    return NULL;
}

/* THIS TEST IS NOT FINISHED.  It is a scaffold you can use for a threaded test.
 *
 * FUNCTIONS BEING TESTED: alloc
 * SPECIFICATION BEING TESTED:
 * Malloc library must be thread-safe: multiple threads simultaneously using the library
 * should not interfere with each other or cause a deadlock
 *
 * MANIFESTATION OF ERROR:
 * Test runs forever on a deadlock, or integrity is violated on a race condition.
 */
int test15()
{
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    int n_threads = 100;
    //int num_iter = 10000;
    pthread_t threads[n_threads];
    int irets[n_threads]; //return values of the threads

    //initialize a synchronization barrier
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, n_threads);

    arg_struct args[n_threads];
    for (int i = 0; i < n_threads; i++)
    {
        args[i] = (arg_struct){.arg1 = 1, .barrier = &barrier, .heap = &heap}; //inline initialization of a struct
    }

    //create threads to execute your function
    for (int i = 0; i < n_threads; i++)
    {
        irets[i] = pthread_create(&threads[i], NULL, thread_function, (void *)&args[i]);
    }
    //wait for all of the threads to finish before moving on
    for (int i = 0; i < n_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }
    //cleanup the barrier
    pthread_barrier_destroy(&barrier);
    printf("Thread 0 returned: %d\n", irets[0]);

    return FAILURE;
}

/* ------------------ STRESS TESTS ------------------------- */

/* THIS TEST IS NOT FINISHED. It is a stress test, but it does not
 * actually test to see whether the library or the user writes
 * past the heap. You are encouraged to complete this test.
 *
 * FUNCTIONS BEING TESTED: alloc, free
 * SPECIFICATION BEING TESTED:
 * The library should not give user "permission" to write off the end
 * of the heap. Nor should the library ever write off the end of the heap.
 *
 * MANIFESTATION OF ERROR:
 * If we track the data on each end of the heap, it should never be
 * written to by the library or a good user.
 *
 */
int test16()
{
    int n_tries = 10000;
    int block_size = 16;

    // 1024 bytes of padding
    // --------------------
    // 1024 bytes of "heap"
    // --------------------  <-- heap_start
    // 1024 bytes of "padding"
    char memarea[HEAP_SIZE * 3];

    char *heap_start = &memarea[1024]; // heap will start 1024 bytes in
    char *pointers[NPOINTERS];

    // zero out the pointer array
    memset(pointers, 0, NPOINTERS * sizeof(char *));

    hl_init(heap_start, HEAP_SIZE);
    srandom(0);
    for (int i = 0; i < n_tries; i++)
    {
        int index = random() % NPOINTERS;
        if (pointers[index] == 0)
        {
            pointers[index] = hl_alloc(heap_start, block_size);
        }
        else
        {
            hl_release(heap_start, pointers[index]);
            pointers[index] = 0;
        }
    }
    return SUCCESS;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test17()
{

    return FAILURE;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test18()
{

    return FAILURE;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test19()
{

    return FAILURE;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test20()
{

    return FAILURE;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test21()
{

    return FAILURE;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test22()
{

    return FAILURE;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test23()
{

    return FAILURE;
}
