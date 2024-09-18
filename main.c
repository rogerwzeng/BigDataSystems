#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_RESULTS 5  //limit the # of returned nodes

int main() {
    hashtable* ht = NULL;
    int result = allocate(&ht, MAX_RESULTS);  // Start with a small size then rehash

    if (result == 0)
        printf("Hash Table Allocation Successful\n");
    else {
        printf("FAILED to Create Hash Table\n");
        return -1;
    }

    valType values[MAX_RESULTS] = {0};
    int num_values = MAX_RESULTS;
    int num_results;

    for (int i = 0; i < 20; i++) {
        put(ht, i, i * 10);
        printf("Inserted %d, Size: %d, Count: %d\n", i, ht->size, ht->count);
    }

    for (int i = 0; i < 20; i++) {
        int result = get(ht, i, values, num_values, &num_results);
        if (result == 0) {
            printf("Number of results for key %d: %d\n", i, num_results);
            for (int i = 0; i < num_results && i < num_values; i++) {
                printf("Value %d: %d\n", i + 1, values[i]);
            }
            if (num_results > num_values) {
                printf("Not all results could be stored in the buffer.\n");
            }
        } else {
            printf("Error retrieving values for key %d\n", i);
        }
    }

    // Try to get a non-existent key
    result = get(ht, 100, values, num_values, &num_results);
    if (result == 0) {
        printf("Number of results for key 100: %d\n", num_results);
    } else {
        printf("Error retrieving values for key 100\n");
    }

    deallocate(ht);
    return 0;
}