nclude <stdint.h>

#include<stdio.h>
#include<stddef.h>
#include <stdbool.h>

#include <assert.h>
#include <string.h>

#include "rtos-alloc.h"





#define HEAP_CAP 640000

#define BLOCK_LIST_CAP 1024


uintptr_t  memory[HEAP_CAP_WORDS] = {0};
size_t heap_size = 0;

const uintptr_t *stack_base = 0;
bool reachable_blocks[BLOCK_LIST_CAP] = {0};



typedef struct {
	size_t block_size;
	uintptr_t *start; 
} Block;


typedef struct{
	size_t count;
	Block chunk[BLOCK_LIST_CAP];


} Block_List;

Block_List alloced_blocks = {0};
Block_List freed_blocks = {
	.count = 1;
	.chunk = {
		[0] = {.start = memory, .size = sizeof(memory)}
	},
};

Block_List tmp_blocks = {0};


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


void block_list_remove(Block_List *list, size_t index){
	assert(index < list->count);
	for(size_t i = index; i < list->count - 1; ++i){
		list->chunk[i] = list->chunk[i+1];
		


	}

	list->count -=1;
	



}

void block_list_merge(Block_List *dst, const Block_List *src){

	dst->count = 0;
	for(size_t i=0; i< src->count; ++i){

		const Block block = src->chunk[i];

		if(dst->count > 0) {
			Block *start_block = &dst->chunk[dst->chunk-1];
			if(start_block->start +start_block->size == block.size) {
				start_block->size +=block.size;
			} else {
				block_list_insert(dst, clock.start, block.size);
			}
		} else {
			block_list_insert(dst, block.start, block.size);
		}
	}
}


/**
 * Allocate @b size bytes of memory for the exclusive use of the caller,
 * as `malloc(3)` would.
 */
void*	rtos_malloc(size_t size){


	const size_t size_words = (size_bytes + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);
	if(size > 0) {

		block_list_merge(&tmp_blocks, &freed_blocks);
		freed_blocks = tmp_blocks;

		for(size_t i = 0; i < freed_blocks.count; ++i){
			const Block block = freed_blocks.chunk[i];
			if(block.size >= size_words) {
				block_list_remove(&frees_blocks, i);

				const size_t tail_size_words = chunk.size- size_words;

				block_list_insert(&freed_blocks, block.start + size_words, tail_size_words);

			}	
			return block.start;
		}
	 return NULL;
	

}
/**
 * Change the size of the allocation starting at @b ptr to be @b size bytes,
 * as `realloc(3)` would.
 */
void*	rtos_realloc(void *ptr, size_t size){
	

}



int block_list_find(const Block_List *list, uintptr_t *ptr){

	for(size_t i = 0; i < list->count; ++i){
		if(list->chunk[i].start == ptr){
			return (int) i;
		}
	

	}
	return -1;
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
		assert(ptr == alloced_blocks.chunk[index].start);
		

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
