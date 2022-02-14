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





#define HEAP_CAP 25000
#define BLOCK_LIST_CAP 1024


uintptr_t  memory[HEAP_CAP] = {0};
size_t heap_size = 0;

int counter = 0;

void *global_stack = NULL;


typedef struct Block {
	size_t block_size;
	int free;
	uintptr_t *start;
	struct Block *next;
	struct Block *prev;
	char data[1];
	void *ptr; 
	bool alloced;
} Block;
#define BLOCK_SIZE sizeof(struct Block)

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

void *base = NULL;
typedef struct Block *block;

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


block find_block (block *last , size_t size ){
	block b=base;
	while (b && !(b->free && b->block_size >= size )) {
		*last = b;
		b = b->next;
	}
	return (b);
}





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
	block b;
	if(size == 0) return NULL;
	p = mmap(NULL, size + sizeof(size_t), PROT_READ| PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0,0);
	if(p == (void*)-1) return NULL;
	
	*p = size + sizeof(size_t);
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

int valid_addr(void *p){

	if(base)
	{
		if(p>base && p<sbrk(0)){
		
			return(p == (get_block(p)->ptr));
		}	
	}
	return 0;

}

/**
 * Release the memory allocation starting at @b ptr for general use,
 * as `free(3)` would.
 * O(Alloced)
 */
void	rtos_free(void *ptr){
	if(ptr == NULL) return;
	
	ptr--;

	munmap(ptr, ptr);
	
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
	else if(counter == 0){
		return true;
	}
	counter++;
	return false;
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
