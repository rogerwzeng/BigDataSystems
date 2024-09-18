#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>
#include "hash_table.h"

#define RATIO_THRESHOLD 2

// Helper function to check if a number is prime
bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    //all primes > 3 satisfies n = 6k ± 1, also the "square root trick"
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

// Helper function to find the next prime number
int nextPrime(int n) {
    if (n <= 1) return 2;
    int prime = n;
    bool found = false;
    while (!found) {
        prime++;
        if (isPrime(prime)) found = true;
    }
    return prime;
}

// Hash function (modular hashing)
unsigned int hashFunction(keyType key, int size) {
    // the default modulus function
    // return key % size;

    // A better hash function
    unsigned int hash = (unsigned int)key;

    //Uses bit manipulation and multiplication to spread the influence of all bits of the key.
    hash = ((hash >> 16) ^ hash) * 0x45d9f3b;
    hash = ((hash >> 16) ^ hash) * 0x45d9f3b;
    hash = (hash >> 16) ^ hash;
    return hash % size;
}

// Allocate a new hash table
int allocate(hashtable** ht, int size) {
    if (ht == NULL || size <= 0) {
        return -1;  // Invalid input
    }

    hashtable* newTable = (hashtable*) malloc(sizeof(hashtable));
    if (newTable == NULL) {
        return -1;  // Memory allocation failed
    }

    newTable->size = size;
    newTable->count = 0;
    newTable->table = (node**) calloc(size, sizeof(node*));

    if (newTable->table == NULL) {
        free(newTable);
        return -1;  // Memory allocation failed
    }

    *ht = newTable;  // Set the pointer to the new hashtable

    return 0;
}

// Create a new node
node* createNode(keyType key, valType value) {
    node* newNode = (node*) malloc(sizeof(node));
    if (newNode == NULL) {
        return NULL;  // Memory allocation failed
    }
    newNode->key = key;
    newNode->value = value;
    newNode->next = NULL;
    return newNode;
}

// Rehash function
hashtable* rehash(hashtable* oldHt, int newSize) {
    hashtable* newHt = NULL;
    int result = allocate(&newHt, newSize);

    if (result == -1 || newHt == NULL) {
        printf("Hash Table Allocation Failed");
        return NULL;
    }

    // Traverse the old hash table and insert all elements into the new one
    for (int i = 0; i < oldHt->size; i++) {
        node* current = oldHt->table[i];
        while (current != NULL) {
            if (put(newHt, current->key, current->value) != 0) {
                // If put fails, clean up and return NULL
                deallocate(newHt);
                return NULL;
            }
            current = current->next;
        }
    }

    // Deallocate the old hash table
    //deallocate(oldHt);

    return newHt;
}

// Modified put function
int put(hashtable* ht, keyType key, valType value) {
    if (ht == NULL) {
        errno = EINVAL;  // empty hash table
        return -1;
    }

    // Check if rehashing is needed
    if ((float)ht->count / ht->size > RATIO_THRESHOLD) {
        int newSize = nextPrime(ht->size * 2);
        hashtable* newHt = rehash(ht, newSize);
        if (newHt == NULL) {
            // Rehashing failed
            errno = ENOMEM;  // Out of memory
            return -1;
        }
        // Save old table
        hashtable oldTable = *ht;

        // Update ht with the new table's contents
        ht->size = newHt->size;
        ht->count = newHt->count;
        ht->table = newHt->table;

        //*ht = *newHt;
        free(newHt); //free up the pointer

        //deallocate the old table's contents
        for (int i = 0; i < oldTable.size; i++) {
            node* current = oldTable.table[i];
            while (current != NULL) {
                node* next = current->next;
                free(current);
                current = next;
            }
        }
        free(oldTable.table);
    }

    // insert the new node
    int index = hashFunction(key, ht->size);
    node* newNode = createNode(key, value);
    if (newNode == NULL) {
        return -1;  //New node creation failed
    }

    if (ht->table[index] == NULL) { //no existing node at this index
        ht->table[index] = newNode;
    } else { //there is collision, add to the end of chain
        node* temp = ht->table[index];
        while (temp->next != NULL) {
            // If the key already exists, update its value
            if (temp->key == key) {
                temp->value = value;
                free(newNode);
                return 0;
            }
            temp = temp->next;
        }
        // Check the last node
        if (temp->key == key) {
            temp->value = value;
            free(newNode);
            return 0;
        }
        // Attached to the end of chain in a collision
        temp->next = newNode;
    }
    ht->count++;
    return 0;
}

// Get a value by key
int get(hashtable* ht, keyType key, valType *values, int num_values, int* num_results) {
    if (ht == NULL || values == NULL || num_values <= 0 || num_results == NULL) {
        return -1;  // Error: invalid input
    }

    int index = hashFunction(key, ht->size);
    node* temp = ht->table[index];
    int count = 0;

    while (temp != NULL) {
        if (temp->key == key) {
            if (count < num_values) {
                values[count] = temp->value;
            }
            count++;
        }
        temp = temp->next;
    }

    *num_results = count;

    return 0;  // Success
}

// Erase a key-value pair from hash table
int erase(hashtable* ht, keyType key) {
    int index = hashFunction(key, ht->size);
    node* temp = ht->table[index];
    node* prev = NULL;

    while (temp != NULL && temp->key != key) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Key not found\n");
        return -1;
    }

    if (prev == NULL) {
        ht->table[index] = temp->next;
    } else {
        prev->next = temp->next;
    }

    free(temp);
    ht->count--;

    return 0;
}

// Deallocate the hash table
int deallocate(hashtable* ht) {
    if (ht == NULL) {
        return -1;
    }

    for (int i = 0; i < ht->size; i++) {
        node* temp = ht->table[i];
        while (temp != NULL) {
            node* toDelete = temp;
            temp = temp->next;
            free(toDelete);
        }
    }

    free(ht->table);
    free(ht);

    return 0;
}
