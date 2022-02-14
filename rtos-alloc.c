#include <sys/mman.h>
#include <stdint.h>

#include<stdio.h>
#include<stddef.h>
#include <stdbool.h>

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "rtos-alloc.h"



size_t heap_size = 0;
size_t tmp_heap_size = 0;


int counter = 0;


typedef struct Block {
	size_t block_size;
	int free;
	void *ptr; 
} Block;



/**
 * Allocate @b size bytes of memory for the exclusive use of the caller,
 * as `malloc(3)` would.
 */



// sbrk() changes the location of the program break, which defined the end of the proccess's 
// data segment 

// for small chunks use sbrk and brk
// for large chunks - mmap an munmap
// using first - fit
void*	rtos_malloc(size_t size){
	size_t *p;
	if(size == 0) return NULL;

	p = mmap(NULL, size + sizeof(size_t), PROT_READ| PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0,0);
	if(p == (void*)-1){
		counter = 0;
		return NULL;
	}
	
	counter = 2;
	*p = size + sizeof(size_t);
	tmp_heap_size = *p;
	heap_size = heap_size + *p;
	p++;

	return p;

}
	


/**
 * Change the size of the allocation starting at @b ptr to be @b size bytes,
 * as `realloc(3)` would.
 */
void*	rtos_realloc(void *ptr, size_t size){
	

	
	if(ptr == NULL){
		return rtos_malloc(size);
	}


	if(size == 0){
		rtos_free(ptr);
		return NULL;


	}

	void* temp = rtos_malloc(size);
	memcpy(temp,ptr,size);

	rtos_free(ptr);

	ptr = temp;

	return ptr;


}




/**
 * Release the memory allocation starting at @b ptr for general use,
 * as `free(3)` would.
 * O(Alloced)
 */
void	rtos_free(void *ptr){
	if(ptr == NULL) return;

	
	size_t size;
	int *p = ptr;

	ptr--;
	size = *p;
	heap_size = heap_size-size;
	tmp_heap_size = 0;
	counter = 0;
	munmap(ptr, size);
	
}






/** How large is the allocation pointed at by this pointer?
 *
 * @pre   @b ptr points at a valid allocation (according to `rtos_allocated`)
 */
size_t  rtos_alloc_size(void *ptr){
	return tmp_heap_size;

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
	
	if(ptr == NULL){
		return false;
	}	

	// hacky - ish :(
		
	// if counter is 0 that mean the memory is freed
	else if(counter == 2){
		
		return true;
	}
	else {
		return false;
	}
	
	
}

/**
 * How much memory has been allocated by this allocator?
 *
 * @returns the number of bytes that have been allocated to user code,
 *          **not** including any allocator overhead
 */
size_t  rtos_total_allocated(void){

	return heap_size;
}
