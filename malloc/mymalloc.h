#include <stdio.h>

typedef enum {bestFit, worstFit, firstFit} Strategy;
Strategy strategy = firstFit;

typedef struct{
  unsigned int size;
  unsigned int isfree;
}Info;

typedef struct{
  Info info;            /*number of  16 byte blocks*/
  struct Block *next;  /*next free*/
  char data[0];        /*start of the allocated memory*/
  Info *bTag;
  struct Block *prev;
 }Block;
/*Block: |4byte(size)|4byte(isfree)|8-byte next|0-byte(data)|8byte bTag|8byte prev|*/

static Block *free_list = NULL;     /*start of the free list*/
static Block *heap_start = NULL;    /*head of allocated memory from sbrk */
static Block *end = NULL;


void *mymalloc(size_t size);
void *myfree(void *p);
Block *split(Block *b, size_t size);
int test(int fitStrategy);
int roundingnumber(size_t size);
Block *firstFitAlg(Block *block, int size);
Block *bestFitAlg(Block *block, int size);
Block *worstFitAlg(Block *block, int size);
void deleteBlock(Block *block);
void addBlock(Block *block);