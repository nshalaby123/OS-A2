
#include <stdint.h>

#include<stdio.h>
#include<stddef.h>
#include <stdbool.h>

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "rtos-alloc.h"





#define HEAP_CAP 25000
#define BLOCK_LIST_CAP 1024


uintptr_t  memory[HEAP_CAP] = {0};
size_t heap_size = 0;

void *global_stack = NULL;


typedef struct Block {
	size_t block_size;
	int free;
	uintptr_t *start;
	struct Block *next; 
} Block;


typedef struct{
	size_t count;
	Block chunk[BLOCK_LIST_CAP];


} Block_List;

Block_List alloced_blocks = {0};
Block_List freed_blocks = {
	.count = 1,
	.chunk = {
		[0] = {.start = memory, .block_size = sizeof(memory)}
	},
};

Block_List tmp_blocks = {0};


Block *freeblock = (void*)memory;

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

	list->count -= 1;
	



}


struct Block *free_block(struct Block  **finish, size_t size) {
	struct Block *curr = global_stack;
 	while (curr && !(curr->free && curr->block_size >= size)) {
    		*finish = curr;
    		curr = curr->next;
  	}
  	return curr;
}




struct Block *find_space(struct Block* finish, size_t size) {
 	struct Block *block;
  	block = sbrk(0);
	void *request = sbrk(size + sizeof(struct Block));
  	assert((void*)block == request);
  	if (request == (void*) -1) {
    		return NULL;
  	}

  	if (finish) { // NULL on finding space
    		finish->next = block;
  	}
 	block->block_size = size;
  	block->next = NULL;
  	block->free = 0;
  	return block;
}




// merge the free memory after the rtos_free
// dst - destination
// src - source

void block_list_merge(Block_List *dst, const Block_List *src){

	dst->count = 0;
	for(size_t i=0; i< src->count; ++i){

		const Block block = src->chunk[i];

		if(dst->count > 0) {
			Block *start_block = &dst->chunk[dst->count-1];

			if(start_block->start +start_block->block_size == block.start) {
				start_block->block_size += block.block_size;
			} else {
				block_list_insert(dst, block.start, block.block_size);
			}
		} else {
			block_list_insert(dst, block.start, block.block_size);
		}
	}
}



void split(Block *chunk, size_t size){
	Block *new = (void*)((void*)chunk + size + sizeof(Block));
	new->block_size = ((chunk->block_size) - size - sizeof(Block));
	new->free = 1;
	new->next = chunk->next;
	chunk->block_size = size;
	chunk->free = 0;
	chunk->next = new;
	
}
/**
 * Allocate @b size bytes of memory for the exclusive use of the caller,
 * as `malloc(3)` would.
 */



// sbrk() changes the location of the program break, which defined the end of the proccess's 
// data segment 
void*	rtos_malloc(size_t size){
	struct Block *block;

	if(size <= 0){

		return NULL;

	}

		if(!global_stack){
			block = find_space(NULL, size);
			if(!block){
				return NULL;
			} 

			global_stack = block;
		} else {

			struct Block *finish = global_stack;
			block = free_block(&finish, size);

			if(!block){
				block = find_space(finish, size);
				if(!block){

					return NULL;
				}
			} else {

			block->free = 0;
		}
	}

	return(block+1);

}
	




/**

	if(size == 0){

		return NULL;

	}

	
	
	Block *curr, *prev;
	void *start;

	if(!(freeblock->block_size)){
		freeblock->block_size = HEAP_CAP - sizeof(Block);
		freeblock->free = 1;
		freeblock->next = NULL;
	


	}

	curr = freeblock;
	while(((((curr->block_size < size) || ((curr->free == 0))) && (curr->next !=NULL)))){
		prev = curr;
		curr = curr->next;
		
	}

	if((curr->block_size) == size){
		curr->free = 0;
		start = (void*)(curr++);
		return start;

	}

	else if((curr->block_size) > (size + sizeof(Block))){
		split(curr, size);

		start = (void*)(curr++);
		return start;

	}

	else {
		start = NULL;
		

	}



	const size_t size_words = (size + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);
	if(size > 0) {
	
		block_list_merge(&tmp_blocks, &freed_blocks);
		freed_blocks = tmp_blocks;

		for(size_t i = 0; i < freed_blocks.count; ++i){
			const Block block = freed_blocks.chunk[i];
		//	if(block.block_size >= size_words) {
				block_list_remove(&freed_blocks, i);

				const size_t tail_size_words = block.block_size- size_words;

				block_list_insert(&alloced_blocks, block.start, size_words);
				if(tail_size_words > 0 && block.block_size >= size_words){
					block_list_insert(&freed_blocks, block.start + size_words, tail_size_words);

				}
			
			
			return block.start;
		//	}
		}
	}
	 return NULL;
***/	



void merge_2(){
	Block *curr, *prev;
	curr = freeblock;
	while((curr->next) != NULL){
		if((curr->free) && (curr->next->free)){
			curr->block_size = curr->block_size + ((curr->next->block_size) + sizeof(struct Block));
			curr->next = curr->next->next;

		}
		else if((curr->free == 1) && (curr->next->free == 0)){
			curr = curr->next;

		}
		prev = curr;
		curr = curr->next;

	}


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



int block_list_find(const Block_List *list, uintptr_t *ptr){

	for(size_t i = 0; i < list->count; ++i){
		if(list->chunk[i].start == ptr){
			return (int) i;
		}
	

	}
	return -1;
}



struct Block *get_block(void *ptr){
	return (struct Block*)ptr -1;



}
/**
 * Release the memory allocation starting at @b ptr for general use,
 * as `free(3)` would.
 * O(Alloced)
 */
void	rtos_free(void *ptr){
	// first we need to find the block to free.


	if(!ptr){
 	  return;

	}

  	struct Block* block = get_block(ptr);
 	assert(block->free == 0);
  

  	block->free = 1;

	
	assert(ptr == NULL);
/***

	if(ptr != NULL){
	
//		const int index = block_list_find(&alloced_blocks, ptr);
//		assert(index >= 0);
//		assert(ptr == alloced_blocks.chunk[index].start);
		

		if(((void*)memory <= ptr) && (ptr <= (void*)(memory + 25000 ))){
		
			Block *curr = ptr;
			curr = curr - 1;
			curr->free = 1;
			merge_2();
			
		}

		//block_list_insert(&freed_blocks, alloced_blocks.chunk[index].start, alloced_blocks.chunk[index].block_size);
		//block_list_remove(&alloced_blocks, (size_t) index); 	
	}

***/





}





/** How large is the allocation pointed at by this pointer?
 *
 * @pre   @b ptr points at a valid allocation (according to `rtos_allocated`)
 */
size_t  rtos_alloc_size(void *ptr){
	return sizeof(ptr);

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
	return true;
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
