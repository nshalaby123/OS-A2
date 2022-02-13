

#include<stdio.h>
#include<stddef.h>
#include <stdbool.h>
#include <assert.h>
#include "rtos-alloc.h"

#define HEAP_CAP 640000
#define HEAP_ALLOCED_CAP 1024

char heap[HEAP_CAP];

char memory[20000];

size_t heap_size = 0;


typedef struct {
	size_t block_size;
	void *start;
	int free;
	struct block *next; 
} Heap_Block;

Heap_Block heap_alloced[HEAP_ALLOCED_CAP] = {0};
size_t heap_alloced_size = 0;



	

/**
 * Allocate @b size bytes of memory for the exclusive use of the caller,
 * as `malloc(3)` would.
 */
void*	rtos_malloc(size_t size){

	assert(heap_size + size <= HEAP_CAP);
	
	void *result = memory + heap_size;
	
	heap_size += size;

	const Heap_Block chunk = {
		.start = result,
		.block_size = size,
		
	};
	
	assert(heap_alloced_size < HEAP_ALLOCED_CAP);
	heap_alloced[heap_alloced_size++] = chunk;


	
	
	
	
	
	return result;

}
/**
 * Change the size of the allocation starting at @b ptr to be @b size bytes,
 * as `realloc(3)` would.
 */
void*	rtos_realloc(void *ptr, size_t size){
}

/**
 * Release the memory allocation starting at @b ptr for general use,
 * as `free(3)` would.
 */
void	rtos_free(void *ptr){
}





/** How large is the allocation pointed at by this pointer?
 *
 * @pre   @b ptr points at a valid allocation (according to `rtos_allocated`)
 */
size_t  rtos_alloc_size(void *ptr){
}

/**
 * Does this pointer point at the beginning of a valid allocation?
 *
 * @param   ptr    any virtual address
 *
 * @returns whether or not @b ptr is a currently-allocated address returned
 *          from @b my_{m,re}alloc
 */
bool    rtos_allocated(void *ptr){
}

/**
 * How much memory has been allocated by this allocator?
 *
 * @returns the number of bytes that have been allocated to user code,
 *          **not** including any allocator overhead
 */
size_t  rtos_total_allocated(void){

	return HEAP_CAP;
}
