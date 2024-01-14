#include <stdio.h>
#include <stdlib.h>
#include "my_malloc.h"

int main(){
  //initialize memory with 1MB (1048576 bytes)
  mem_init(1024 * 1024);
  printMemory();

  //allocate memory blocks
  void* block1 = my_malloc(500);
  void* block2 = my_malloc(1000);
  void* block3 = my_malloc(5000);
  //void* block4 = my_malloc(1042076);
  //void* block5 = my_malloc(1);

  my_free(block2);
  void* block4 = my_malloc(999);
  //printMemory();

  //free all allocated blocks
  my_free(block1);
  my_free(block2);
  my_free(block3);
  my_free(block4);
  //my_free(block5);
  printMemory();
  
  return 0;
}