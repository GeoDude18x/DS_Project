#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_SIZE 127
#define MAX_COUNTRY_LENGTH 20
#define MAX_WEIGHT 50000
#define MIN_WEIGHT 100
#define MAX_VALUATION 2000
#define MIN_VALUATION 10
#define SUCCESS 0
#define FAILURE -1
#define FILE_OPEN_ERROR -2
#define FILE_CLOSE_ERROR -3
#define INPUT_FILE_NAME "myth_units.txt"

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

// Prototypes
unsigned long GenerateHash(char* destination);
HashTable* InitializeHashTable(void);
int InsertWithSeparateChaining(HashTable* hashTable, char* destination, int weight, float valuation);
Parcel* InitializeParcelNode(char* destination, int weight, float valuation);
Parcel* InsertParcelIntoBST(Parcel* root, char* destination, int weight, float valuation);
int SearchParcelInBST(Parcel* root, int weightToSearch);

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
        printf("Insertion failed for destination: %s\n", destination);
        return;
    }
    hashTable->root[hash] = result;
}

//Function Definitions
// 
//
//Function: InitializeParcelNode()
//Description: This allocates memory for a parcel node, and feeds the node
//             the appropriate infromation recorded for a parcel node.
//Parameters:
//	char* destination: C-style string used to fill the destination attribute
//  int weight: The weight of the parcel, used to fill the weight attribute
//  float valuation: The valuation of the parcel, used to fill the valuation attribute
//Returns:
//  Parcel*: Pointer to the Parcel memory created and filled. NULL, in case of failure
//           in memory allocation
//
Parcel* InitializeParcelNode(char* destination, int weight, float valuation) {
    Parcel* node = (Parcel*)malloc(sizeof(Parcel));
    if (node == NULL) {
        printf("Memory allocation failure for InitializeParcelNode()");
        return NULL;
    }

    strcpy(node->destination, (const char*)destination);
    node->weight = weight;
    node->valuation = valuation;
    node->left = NULL;
    node->right = NULL;
    return node;
}

//Function Definitions
// 
//
//Function: InsertParcelIntoBST()
//Description: This function inserts a new element into the BST.
//             It does so by traversing the tree according to the
//             weight value of the parcel.
//Parameters:
//  Parcel* root: Pointer to the root of the BST. This is updated as the BST is
//                traversed according to the weight attribute.
//	char* destination: C-style string used to fill the destination attribute
//  int weight: The weight of the parcel, used to fill the weight attribute
//  float valuation: The valuation of the parcel, used to fill the valuation attribute
//Returns:
//  Parcel*: Pointer to the new Parcel object. NULL, in case of failure.
//
Parcel* InsertParcelIntoBST(Parcel* root, char* destination, int weight, float valuation) {
    if (root == NULL) { // parent is empty so node is inserted here
        return InitializeParcelNode(destination, weight, valuation);
    }

    if (weight < root->weight) { // weight is less than root's, attempting to insert again on the left node (with updated root)
        root->left = InsertParcelIntoBST(root->left, destination, weight, valuation);
    }
    else if (elementToInsert > root->Element) { // weight is greater than root's, attempting to insert again on the right node (with updated root)
        root->right = InsertParcelIntoBST(root->right, destination, weight, valuation);
    }

    return root;
}

int SearchParcelInBST(Parcel* root, int weightToSearch) {
    if (root == NULL) {
        printf("There are no parcels with that destination name.\n");
        return FAILURE;
    }

    if (root->weight == weightToSearch) {
        return root->weight;
    }

    if (elementToSearch < root->weight) {
        return SearchElementInBST(root->LeftChild, weightToSearch);
    }
    else if (elementToSearch > root->weight) {
        return SearchElementInBST(root->RightChild, weightToSearch);
    }

    return FAILURE;
}

int main(void) {
    return 0;
}
