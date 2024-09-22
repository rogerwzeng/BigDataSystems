// Function declarations
#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>

typedef int keyType;
typedef int valType;

typedef struct node {
    keyType key;
    valType value;
    struct node* next;
} node;

typedef struct hashtable {
    int size;
    int count;
    node** table;
} hashtable;

// Function declarations
bool isPrime(int n);
int nextPrime(int n);
int allocate(hashtable** ht, int size);
node* createNode(keyType key, valType value);
hashtable* rehash(hashtable* oldHt, int newSize);
int put(hashtable* ht, keyType key, valType value);
int get(hashtable* ht, keyType key, valType *values, int num_values, int* num_results);
int erase(hashtable* ht, keyType key);
int deallocate(hashtable* ht);

#endif // HASH_TABLE_H