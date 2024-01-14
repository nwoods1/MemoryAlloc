#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "my_malloc.h"

MemoryBlock memoryArray[MAX_BLOCKS];
size_t totalMemory;

void swap(MemoryBlock* x, MemoryBlock* y){ 
  MemoryBlock temp = *x; 
  *x = *y; 
  *y = temp; 
}

void flip(){
  int start = -1;
  for (start=0;start<MAX_BLOCKS;start++)
    if (memoryArray[start].address_start != NULL)
      break;
  for (int i=start;i<MAX_BLOCKS;i++)
    swap(&memoryArray[i], &memoryArray[i-start]);
}

void sortMemoryBySize(){
  //sort by size to determine best fit
  int i, j, min_idx; 
  for (i=0;i<MAX_BLOCKS;i++){
    min_idx = i; 
    for (j=i+1;j<MAX_BLOCKS;j++){
      if ((memoryArray[j].size < memoryArray[min_idx].size))
        min_idx = j;
      swap(&memoryArray[min_idx], &memoryArray[i]);
    }
  }
  flip();
}

void sortMemoryByAddress(){
  //sort by starting address to get everything all contiguous
  int i, j, min_idx; 
  for (i=0;i<MAX_BLOCKS;i++){
    min_idx = i; 
    for (j=i+1;j<MAX_BLOCKS;j++){
      if ((memoryArray[j].address_start < memoryArray[min_idx].address_start) && (memoryArray[i].address_start != NULL))
        min_idx = j;
      swap(&memoryArray[min_idx], &memoryArray[i]);
    }
  }
  flip();
}

void printMemory(){
  printf("Printing memory:\n");
  for (size_t i=0;i<MAX_BLOCKS;i++)
    if (memoryArray[i].address_start != NULL)
      printf("[%zu] %p - %p, %zu, %d\n", i, memoryArray[i].address_start, memoryArray[i].address_end, memoryArray[i].size, memoryArray[i].available);
}

void mem_init(size_t initialSize){
  totalMemory = initialSize;

  memoryArray[0].address_start = (MemoryBlock*)malloc(initialSize * sizeof(MemoryBlock));
  memoryArray[0].address_end = memoryArray[0].address_start + initialSize - 1;
  memoryArray[0].size = initialSize;

  //initialize all blocks to available
  for (int i=0;i<MAX_BLOCKS;i++)
    memoryArray[i].available = 1;
}

void* my_malloc(size_t size){
  //find the best-fit free block (if any)
  size_t bestFitIndex = -1;
  for (int i=0;i<MAX_BLOCKS;i++)
    if ((memoryArray[i].available == 1) && (memoryArray[i].size >= size)){
      //this block is free
      bestFitIndex = i;
      break;
    }

  //no more memory available (ie. need to increase memory allocated)
  if (bestFitIndex == -1){
    printf("Error: Memory allocation failed. Out of memory space.\n");
    return NULL;
  }

  //find the next empty space
  int free_spot = -1;
  for (int i=0;i<MAX_BLOCKS;i++)
    if ((memoryArray[i].address_start == NULL) && (memoryArray[i].address_end == NULL) && (memoryArray[i].size == 0) && (memoryArray[i].available == 1)){
      free_spot = i;
      break;
    }

  //no more blocks available (ie. need to increase MAX_BLOCKS)
  if (free_spot == -1){
    printf("Error: Memory allocation failed. No more blocks available.\n");
    return NULL;
  }
  
  //split the block
  void* backup;
  memoryArray[free_spot].address_start = memoryArray[bestFitIndex].address_end - size + 1;
  memoryArray[free_spot].address_end = memoryArray[bestFitIndex].address_end;
  memoryArray[free_spot].size = size;
  memoryArray[free_spot].available = 0;

  memoryArray[bestFitIndex].address_end = memoryArray[free_spot].address_start - 1;
  memoryArray[bestFitIndex].size = memoryArray[bestFitIndex].size - size;

  //if we've used up all available memory then mark as unavailable
  if (memoryArray[bestFitIndex].size == 0){
    memoryArray[bestFitIndex].address_start = NULL;
    memoryArray[bestFitIndex].address_end = NULL;
    memoryArray[bestFitIndex].available = 1;
  }

  backup = memoryArray[free_spot].address_start;
  sortMemoryBySize();
  printMemory();
  return backup;
}

void my_free(void* ptr){
  //find the block to free
  for (size_t i=0; i<MAX_BLOCKS; i++)
    if (memoryArray[i].address_start == ptr){
      //mark the block as free
      memoryArray[i].available = 1;
      defragMemory();
      printMemory();
      return;
    }

  //if the pointer is not found, print an error message
  printf("Error: Attempted to free memory that was not allocated by my_malloc.\n");
}

void defragMemory(){
  //run at least once so assume you can find one fragment
  bool fragment_found = 1;
  while (fragment_found == 1){
    //arrange memory matrix contiguously
    sortMemoryByAddress();

    //loop through the available contiguous memory blocks and combine (if possible) until no more fragments found
    fragment_found = 0;
    for (int i=0;i<MAX_BLOCKS-1;i++){
      //see if we can combine this row with the next row
      if ((memoryArray[i].address_start != NULL) && (memoryArray[i].available == 1) && (memoryArray[i+1].available == 1) && (memoryArray[i+1].size > 0)){
        //adjust the address fence accordingly and also add the two sizes together
        memoryArray[i].address_end = memoryArray[i+1].address_end;
        memoryArray[i].size = memoryArray[i].size + memoryArray[i+1].size;

        //put the next row back in the general pool of blocks as it is now incorporated with the current row
        memoryArray[i+1].address_start = NULL;
        memoryArray[i+1].address_end = NULL;
        memoryArray[i+1].size = 0;
        fragment_found = 1;
        i++;
      }
    }
  }
  sortMemoryBySize();
 }