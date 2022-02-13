

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


/*
 * The following functions are declartions of standard C library functions
 * for you to implement:
 */

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






