

#include<stdio.h>
#include<stddef.h>
#include <stdbool.h>
#include <assert.h>
#include "rtos-alloc.h"





#define HEAP_CAP 640000

#define BLOCK_LIST_CAP 1024


char memory[HEAP_CAP] = {0};

size_t heap_size = 0;


typedef struct {
	size_t block_size;
	char *start;
	int free;
	struct block *next; 
} Block;


typedef struct{
	size_t count;
	Block chunk[BLOCK_LIST_CAP];


} Block_List;

Block_List alloced_blocks = {0};
Block_List freed_block = {0};


void block_list_insert(Block_List *list, void *start, size_t size){
	assert(list->count < BLOCK_LIST_CAP);
	list->chunk[list->count].start = start;
	list->chunk[list->count].block_size = size;
	for(size_t i = list->count; i> 0 && list->chunk[i].start < list->chunk[i-1].start; --i){
		// swap here!
		const Block t = list->chunk[i];
		list->chunk[i] = list->chunk[i-1];
		list->chunk[i-1] = t;
	
	}
	list->count += 1;

}


void block_list_remove(Block_List *list, void *ptr, size_t size){
}

/**
 * Allocate @b size bytes of memory for the exclusive use of the caller,
 * as `malloc(3)` would.
 */
void*	rtos_malloc(size_t size){

	if(size > 0) {

	assert(heap_size + size <= HEAP_CAP);
	
	void *ptr = memory + heap_size;
	
	heap_size += size;
	block_list_insert(&alloced_blocks, ptr,size);
	
	return ptr;

	} else {
		 return NULL;
	}

}
/**
 * Change the size of the allocation starting at @b ptr to be @b size bytes,
 * as `realloc(3)` would.
 */
void*	rtos_realloc(void *ptr, size_t size){
	

}



int block_start_compar(const void *a, const void *b){
	const Block *a_block = a;
	const Block *b_block = b;
	

	return a_block->start - b_block->start;


}
int block_list_find(const Block_List *list, void *ptr){
	//binary search? - bsearch
	Block key = {
	

		.start = ptr

	};

	Block *result =  *bsearch(&key, list->chunk, list->count, sizeof(list->chunk[0]), block_start_compar);
	if(result !=0) {
		assert(list->chunk <= result);
		return (result - list->chunk) /sizeof(list->chunk[0]);



	} else {
		return -1;


	}


}

/**
 * Release the memory allocation starting at @b ptr for general use,
 * as `free(3)` would.
 * O(Alloced)
 */
void	rtos_free(void *ptr){
	// first we need to find the block to free.

	if(ptr != NULL){
		


	
	const int index = block_list_find(&alloced_blocks, ptr);
	assert(index >= 0);


	block_list_insert(&freed_blocks, alloced_blocks.chunk[index].start, alloced_blocks[index].size);
	block_list_remove(&alloced_blocks, (size_t) index); 	
}


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
	assert(false && "TODO");
	return false;

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
