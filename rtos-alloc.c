

#include<stdio.h>
#include<stddef.h>

#include "rtos-alloc.h"
char memory[20000];

struct block{
 size_t size;
 int free;
 struct block *next; 
};

struct block *freeList=(void*)memory;


/**
 * Allocate @b size bytes of memory for the exclusive use of the caller,
 * as `malloc(3)` would.
 */
void*	rtos_malloc(size_t size){
	struct block *curr,*prev;
	void *result;
	if(!(freeList->size)){
	
	
	}
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
}
