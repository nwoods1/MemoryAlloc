#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

//determines how many chunks of the memory can be allocated separately
#define MAX_BLOCKS 100

typedef struct{
    void* address_start;
    void* address_end;
    size_t size;
    bool available;
} MemoryBlock;

void mem_init(size_t initialSize);
void* my_malloc(size_t size);
void my_free(void* ptr);
void defragMemory();
void sortMemoryBySize();
void sortMemoryByAddress();
void printMemory();
