#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the size of the hash table
#define HASH_TABLE_SIZE 127

// Define the maximum length of a country name
#define MAX_COUNTRY_LENGTH 20

// Define the maximum and minimum weights
#define MAX_WEIGHT 50000
#define MIN_WEIGHT 100

// Define the maximum and minimum valuations
#define MAX_VALUATION 2000
#define MIN_VALUATION 10

// Define the structure for a Parcel
typedef struct Parcel {
    char destination[MAX_COUNTRY_LENGTH + 1];
    int weight;
    float valuation;
    struct Parcel* left;
    struct Parcel* right;
} Parcel;

// Define the structure for the hash table
typedef struct HashTable {
    Parcel* root[HASH_TABLE_SIZE];
} HashTable;

// Function to generate hash value using djb2 algorithm
unsigned long GenerateHash(char* destination) {
    unsigned long hash = 5381;
    int c;

    while ((c = *destination++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_TABLE_SIZE;
}

// Function to initialize the hash table
HashTable* InitializeHashTable(void) {
    HashTable* hashTable = (HashTable*)malloc(sizeof(HashTable));
    if (hashTable == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hashTable->root[i] = NULL;
    }
    return hashTable;
}

// Function to insert a parcel into the hash table
int InsertWithSeparateChaining(HashTable* hashTable, char* destination, int weight, float valuation) {
    unsigned long hash = GenerateHash(destination);
    Parcel* result = InsertParcelIntoBST(hashTable->root[hash], destination, weight, valuation);
    if (result == NULL) {
        return;
    }
    hashTable->root[hash] = result;
}
