#include <stdio.h>
#include <stdlib.h>
#define KU_MAGIC 0x19311946U;	//Magic number

typedef struct __hnode_t{			//Free List node
	int size;
	struct __hnode_t *next;
	}hnode_t;
	
	
typedef struct __hheader_t{			//HEADER BLOCK(before segment)
	int size;
	unsigned long magic;		//magic = addr^KU_MAGIC;
	}hheader_t;
	
	
void* malloc(size_t size);


void* splitting(void * target_address, size_t newsize);

void* next_fit(size_t size);

void free(void *ptr);

void add_freelist(size_t newsize, hnode_t* address);

void coalescing();

int check_freelist(size_t size);

void print_freelist();
