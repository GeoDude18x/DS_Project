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
int LoadDataFromFile(HashTable* hashTable, char* filename);
int SearchParcelInBST(Parcel* root, int weightToSearch);
Parcel* SearchWithSeparateChaining(HashTable* hashTable, char* destination, int weight);
void printDestinationParcels(Parcel* root);
void DisplayParcelsForDestination(HashTable* hashTable, char* destination);
void printOtherWeightedParcels(Parcel* root, int weight);
void DisplayParcelsForWeight(HashTable* hashTable, int weight);
void DisplayMenu(HashTable* hashTable);

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

// Function to search for a parcel in the hash table
Parcel* SearchWithSeparateChaining(HashTable* hashTable, char* destination, int weight) {
    unsigned long hash = GenerateHash(destination);
    Parcel* root = hashTable->root[hash];

    if (root == NULL) {
        printf("ERROR: Cannot find parcel for destination: %s\n", destination);
        return NULL;
    }

    Parcel* foundParcel = SearchParcelInBST(root, weight);
    if (foundParcel == NULL) {
        printf("ERROR: Cannot find parcel with weight %d for destination: %s\n", weight, destination);
        return NULL;
    }
    printf("Parcel with weight %d found for destination: %s\n", weight, destination);
    return SUCCESS;
}

// Function to load data into the hash table
int LoadDataFromFile(HashTable* hashTable, char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return FILE_OPEN_ERROR;
    }
    char destination[MAX_COUNTRY_LENGTH + 1];
    int weight;
    float valuation;

    while (fscanf(file, "%20s %d %f", destination, &weight, &valuation) == 3) {
        if (InsertWithSeparateChaining(hashTable, destination, weight, valuation) == FAILURE) {
            printf("Failed to insert parcel: %s\n", destination);
        }
    }

    if (fclose(file) != 0) {
        printf("Error closing file %s\n", filename);
        return FILE_CLOSE_ERROR;
    }

    return SUCCESS;
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

// Function to search for a parcel in the BST
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

// Function to display parcels for a destination
void DisplayParcelsForDestination(HashTable* hashTable, char* destination) {
    unsigned long hash = GenerateHash(destination);
    Parcel* root = hashTable->root[hash];

    if (root == NULL) {
        printf("No parcels found for destination: %s\n", destination);
        return;
    }
    printDestinationParcels(root);
}

// Function to recursively print parcels for a destination
void printDestinationParcels(Parcel* root) {
    if (root == NULL)
        return;

    printf("%s, %d, %.2f \n", root->destination, root->weight, root->valuation);
    printDestinationParcels(root->left);
    printDestinationParcels(root->right);

}

// Function to recursively print parcels for a weight
void printOtherWeightedParcels(Parcel* root, int weight) {
    if (root == NULL)
        return;

    if (root->weight != weight)
        printf("%s, %d, %.2f \n", root->destination, root->weight, root->valuation);

    printOtherWeightedParcels(root->left);
    printOtherWeightedParcels(root->right);
}

// Function to display the menu
void DisplayMenu(HashTable* hashTable) {
    int choice;
    char destination[MAX_COUNTRY_LENGTH + 1];
    int weight;
    char fileName[256];

    do {
        printf("Menu:\n");
        printf("1. Enter country name and display all parcels.\n");
        printf("2. Enter country and weight to search for a parcel.\n");
        printf("3. Enter weight to display all parcels excluding that weight.\n");
        printf("4. Load parcels from a file.\n");
        printf("5. Exit.\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            printf("Enter country name: ");
            scanf("%s", destination);
            DisplayParcelsForDestination(hashTable, destination);
            break;
        case 2:
            printf("Enter country name: ");
            scanf("%s", destination);
            printf("Enter weight to search: ");
            scanf("%d", &weight);
            Parcel* foundParcel = SearchWithSeparateChaining(hashTable, destination, weight);
            if (foundParcel != NULL) {
                printf("Parcel found.\n");
            }
            else {
                printf("Parcel not found.\n");
            }
            break;
        case 3:
            printf("Enter weight to exclude: ");
            scanf("%d", &weight);
            DisplayParcelsForWeight(hashTable, weight);
            break;
        case 4:
            printf("Enter the name of the file to load parcels: ");
            scanf("%s", fileName);
            if (LoadDataFromFile(hashTable, fileName) == SUCCESS) {
                printf("Data loaded successfully from %s\n", fileName);
            }
            else {
                printf("Failed to load data from %s\n", fileName);
            }
            break;
        case 5:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5);
}

int main(void) {

    HashTable* hashTable = InitializeHashTable();
    DisplayMenu(hashTable);
    
    return 0;
}
