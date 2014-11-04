#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mhDefs.h"
#include "stuDefs.h"
#include "newDataTypes.h"

void mh_freeblock(HeapType *, int);

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


  /* Additional tests:
   *    70  floats
   *     9  MatrixTypes
   *    20  ServerEntryTypes
   *  32+2  Point3DTypes
   *    10  longs
   *    64  HeapTypes (lol)
   **/
  printf("\n--> ADDITIONAL TESTING:\n");

  mh_alloc(heap, 70*sizeof(float), "floats");

  printf("\nDUMP 4, byte count = %d\n", mh_count(heap));
  mh_dump(heap);

  void *server_entries;
  server_entries = mh_alloc(heap, 20*sizeof(ServerEntryType), "ServerEntries");
  mh_alloc(heap, 9*sizeof(MatrixType), "Matrices");

  printf("\nDUMP 5, byte count = %d\n", mh_count(heap));
  mh_dump(heap);

  mh_dealloc(heap, server_entries);
  mh_alloc(heap, 32*sizeof(Point3DType), "3D points");
  mh_alloc(heap, 2*sizeof(Point3DType), "3D Points");

  printf("\nDUMP 6, byte count = %d\n", mh_count(heap));
  mh_dump(heap);

  mh_collect(heap);
  mh_alloc(heap, 10*sizeof(long), "longs");

  printf("\nDUMP 7, byte count = %d\n", mh_count(heap));
  mh_dump(heap);

  mh_alloc(heap, 64*sizeof(HeapType), "heaps"); // not initialized

  printf("\nDUMP 8, byte count = %d\n", mh_count(heap));
  mh_dump(heap);

  printf("\n\n");
  mh_cleanup(heap);
  free(heap);

  return 0;
}

/*  Function:  mh_init
 *   Purpose:  initializes HeapType with empty blocks ready for alloc
 *    in/out:  HeapType pointer
 **/
void mh_init(HeapType *heap) {
  heap->numBlocks = MAX_BLK;
  heap->blocks = (BlockType *) calloc(MAX_BLK, sizeof(BlockType));

  int i;
  for (i = 0; i < heap->numBlocks; i++)
  {
    heap->blocks[i].rsv  = C_FALSE;
    heap->blocks[i].addr = NULL;
    heap->blocks[i].size = 0;
  }
}

/*  Function:  mh_cleanup
 *   Purpose:  frees block data (see mh_collect) and frees block array
 *    in/out:  HeapType pointer
 **/
void mh_cleanup(HeapType *heap) {
  int i;
  mh_collect(heap);
  free(heap->blocks);
}

/*  Function:  mh_alloc
 *   Purpose:  Allocates memory in the first free block and stores info
 *             about the block.
 *    in/out:  HeapType pointer
 *        in:  size of memory block to be allocated (goes directly to malloc)
 *        in:  label used in mh_dump to identify blocks
 *    return:  void-casted pointer to allocated heap
 **/
void *mh_alloc(HeapType *heap, int n, char *label) {
  int i;
  for (i = 0; i<MAX_BLK; i++) {
    if (heap->blocks[i].rsv == C_FALSE) {
      heap->blocks[i].rsv  = C_TRUE;
      heap->blocks[i].addr = malloc(n);
      heap->blocks[i].size = n;
      strcpy(heap->blocks[i].tag, label);
      break;
    }
  }

  if (i == MAX_BLK) {
    printf("Memory hunter heap space exceeded. Exiting.\n");
    exit(EXIT_FAILURE);
  }
  return heap->blocks[i].addr;
}

/*  Function:  mh_dealloc
 *   Purpose:  scans tracked blocks for given pointer and frees it if found.
 *    in/out:  HeapType pointer
 *        in:  void pointer to memory (initially supplied by mh_alloc)
 * 
 **/
void mh_dealloc(HeapType *heap, void *addr) {
  int i;
  for (i = 0; i < MAX_BLK; i++) 
  {
    if (heap->blocks[i].addr == addr) {
      mh_freeblock(heap, i);
      break;
    }
  }

  if (i == MAX_BLK) {
    printf("Address not tracked by memory hunter. Exiting.\n");
    exit(EXIT_FAILURE);
  }
}

/*  Function:  mh_count
 *   Purpose:  Finds the total stored bytes within the blocks of a heap.
 *        in:  HeapType pointer
 *    return:  int sum
 **/
int mh_count(HeapType *heap) {
  int sum = 0;
  int i;

  for(i = 0; i<MAX_BLK; i++) {
    sum += heap->blocks[i].size * heap->blocks[i].rsv;
  }

  return sum;
}

/*  Function:  mh_dump
 *   Purpose:  Pretty-prints a complete memory dump of each block in a heap.
 *        in:  HeapType pointer
 **/
void mh_dump(HeapType *heap) {
  int i;
  for(i = 0; i<MAX_BLK; i++) {
    if(heap->blocks[i].rsv == C_TRUE) {
      printf("%20s:%7d bytes stored at %p\n", heap->blocks[i].tag,
                                              heap->blocks[i].size,
                                              heap->blocks[i].addr);
    }
  }
}

/*  Function:  mh_collect
 *   Purpose:  frees all data stored within blocks of a heap.
 *    in/out:  HeapType pointer
 **/
void mh_collect(HeapType *heap) {
  int i;
  for (i = 0; i < MAX_BLK; i++) {
    mh_freeblock(heap, i);
  }
}

/*  Function:  mh_freeblock
 *   Purpose:  frees and cleans up a block at a given index in a heap.
 *    in/out:  HeapType pointer
 *        in:  index of block in heap->blocks
 **/
void mh_freeblock(HeapType *heap, int i) {
  heap->blocks[i].rsv  = C_FALSE;
  free(heap->blocks[i].addr);
  heap->blocks[i].addr = NULL;
  heap->blocks[i].size = 0;
  strcpy(heap->blocks[i].tag, "");
}


