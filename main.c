#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mhDefs.h"
#include "stuDefs.h"

int main()
{
  HeapType *heap;
  void  *ptr1, *ptr2, *ptr3, *ptr4;

  heap = (HeapType *) malloc(sizeof(HeapType));
  mh_init(heap);

  ptr1 = mh_alloc(heap, 5*sizeof(int), "ints");
  ptr2 = mh_alloc(heap, 10*sizeof(double), "doubles");
  ptr3 = mh_alloc(heap, 8*sizeof(char), "chars");
  ptr4 = mh_alloc(heap, 12*sizeof(StudentType), "Students");

  printf("\nDUMP 1, byte count = %d\n", mh_count(heap));
  mh_dump(heap);

  mh_dealloc(heap, ptr1);
  mh_dealloc(heap, ptr2);
  mh_dealloc(heap, ptr3);

  printf("\nDUMP 2, byte count = %d\n", mh_count(heap));
  mh_dump(heap);

  mh_collect(heap);

  printf("\nDUMP 3, byte count = %d\n", mh_count(heap));
  mh_dump(heap);
  printf("\n\n");

  mh_cleanup(heap);
  free(heap);

  return 0;
}

void mh_init(HeapType *heap) {
  heap->numBlocks = 0;
  heap->blocks    = (*BlockType) calloc(MAX_BLK, sizeof(BlockType) );
}
void mh_cleanup(HeapType *heap) {}
void *mh_alloc(HeapType *heap, int n, char *label) {}
void mh_dealloc(HeapType *heap, void *addr) {}
int  mh_count(HeapType *heap) {}
void mh_dump(HeapType *heap) {}
void mh_collect(HeapType *heap) {}
