#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "hash_table.h"

#define START_SIZE 30000000
#define NUM_TESTS 50000000

int main(void) {

  hashtable* ht=NULL;
  //assert(allocate(&ht, num_tests)==0);
  assert(allocate(&ht, START_SIZE)==0); //start small and dynamically grow

  int seed = 2;
  srand(seed);
  printf("Performing stress test with starting hash table size of %d\n", START_SIZE);
  printf("Inserting %d keys...\n", NUM_TESTS);

  struct timeval stop, start;
  gettimeofday(&start, NULL);

  for (int i = 0; i < NUM_TESTS; i += 1) {
    int key = rand();
    int val = rand();
    assert(put(ht, key, val)==0);
  }

  gettimeofday(&stop, NULL);
  double secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec); 
  printf("%d insertions took %f seconds\n", NUM_TESTS, secs);
  printf("Hash table size = %d & count = %d\n", ht->size, ht->count);

  assert(deallocate(ht)==0);

  return 0;
}
