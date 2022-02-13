

#include<stdio.h>
#include<stddef.h>

#include "rtos-alloc.h"

#define CAPACITY 640000

char heap[CAPACITY];

char memory[20000];

struct block{
 size_t block_size;
 int free;
 struct block *next; 
};

struct block *freeList=(void*)memory;

void initialize(){
	freeList->block_size=20000-sizeof(struct block);
	freeList->free=1;
	freeList->next=NULL;
}


void split(struct block *fitting_slot,size_t size){
	struct block *new=(void*)((void*)fitting_slot+size+sizeof(struct block));
 	new->block_size=(fitting_slot->size)-size-sizeof(struct block);
 	new->free=1;
 	new->next=fitting_slot->next;
 	fitting_slot->block_size=size;
 	fitting_slot->free=0;
 	fitting_slot->next=new;
}	

/**
 * Allocate @b size bytes of memory for the exclusive use of the caller,
 * as `malloc(3)` would.
 */
void*	rtos_malloc(size_t size){
	struct block *curr,*prev;
	void *result;
	if(!(freeList->block_size)){
		initialize();
				
	
	}
	curr = freeList;
	while((((curr->block_size)<size)||((curr->free)==0))&&(curr->next!=NULL)){
		prev=curr;
		curr=curr->next;
		
	}
	if((curr->block_size)==size){
		curr->free=0;
		result=(void*)(++curr);
		return result;
	}
	else if((curr->block_size)>(size+sizeof(struct block))){
		split(curr, size);
		result=(void*)(curr++);
		return result;
	}
	else{
		result=NULL;
		return result;
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

	return CAPACITY;
}
