/*
 * FILE : project.cpp
 * PROJECT : SENG1050 - DATA STRUCTURES GROUP PROJECT
 * PROGRAMMERS : George Dall & Behzad Afrasiabi
 * FIRST VERSION : 2024-08-11
 * DESCRIPTION :
 * This program implements a hash table and a binary search tree to manage various products
 * and display output based on specific requirements.
 */

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

#pragma warning (disable : 4996)

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
unsigned long generateHash(char* destination);
HashTable* initializeHashTable(void);
int insertWithSeparateChaining(HashTable* hashTable, char* destination, int weight, float valuation);
Parcel* initializeParcelNode(char* destination, int weight, float valuation);
Parcel* insertParcelIntoBST(Parcel* root, char* destination, int weight, float valuation);
int loadDataFromFile(HashTable* hashTable, char* filename);
void printDestinationParcels(Parcel* root);
void displayParcelsForDestination(HashTable* hashTable, char* destination);
void printOtherWeightedParcels(Parcel* root, int weight);
void displayParcelsForWeight(HashTable* hashTable, int weight);
void displayMenu(HashTable* hashTable);
void displayStatsTotal(HashTable* hashTable, char* destination);
void calculateStatsTotal(Parcel* root, Parcel* result);
void displayCheapestAndMostExpensiveParcels(HashTable* hashTable, char* destination);
void findCheapestParcel(Parcel* root, Parcel* result);
void findMostExpensiveParcel(Parcel* root, Parcel* result);
void displayLightestAndHeaviestParcel(HashTable* hashTable, char* destination);
void findLightestParcel(Parcel* root, Parcel* result);
void findHeaviestParcel(Parcel* root, Parcel* result);
int readAndSort(char* fileName, HashTable* hashTable);
char* readLine(FILE* file);

// Function to generate hash value using djb2 algorithm
unsigned long generateHash(char* destination) {
    unsigned long hash = 5381;
    int c;

    while ((c = *destination++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_TABLE_SIZE;
}

// Function to initialize the hash table
HashTable* initializeHashTable(void) {
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
int insertWithSeparateChaining(HashTable* hashTable, char* destination, int weight, float valuation) {
    unsigned long hash = generateHash(destination);
    Parcel* result = insertParcelIntoBST(hashTable->root[hash], destination, weight, valuation);
    if (result == NULL) {
        printf("Insertion failed for destination: %s\n", destination);
        return FAILURE;
    }
    hashTable->root[hash] = result;
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
Parcel* initializeParcelNode(char* destination, int weight, float valuation) {
    Parcel* node = (Parcel*)malloc(sizeof(Parcel));
    if (node == NULL) {
        printf("Memory allocation failure for initializeParcelNode()");
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
//Description: This function inserts a new parcel into the BST.
//             It does so by traversing the tree according to the
//             weight value of the parcel.
//Parameters:
//  Parcel* root: Pointer to the root of the BST. This is updated as the BST is
//                traversed according to the weight attribute.
//	char* destination: C-style string used to fill the destination attribute
//  int weight: The weight of the parcel, used to fill the weight attribute
//  float valuation: The valuation of the parcel, used to fill the valuation attribute
//Returns:
//  Parcel*: Pointer to the new Parcel object.
//
Parcel* insertParcelIntoBST(Parcel* root, char* destination, int weight, float valuation) {
    if (root == NULL) { // parent is empty so node is inserted here
        return initializeParcelNode(destination, weight, valuation);
    }

    if (weight < root->weight) { // weight is less than root's, attempting to insert again on the left node (with updated root)
        root->left = insertParcelIntoBST(root->left, destination, weight, valuation);
    }
    else if (elementToInsert > root->Element) { // weight is greater than root's, attempting to insert again on the right node (with updated root)
        root->right = insertParcelIntoBST(root->right, destination, weight, valuation);
    }

    return root;
}

// Function to display parcels for a destination
void displayParcelsForDestination(HashTable* hashTable, char* destination) {
    unsigned long hash = generateHash(destination);
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

// Function to recursivly check hashtable for parcels for different weights than the one entered
// and then print 
void checkOtherWeightParcels(HashTable* hashTable, char* destination, int weight) {
    unsigned long hash = generateHash(destination);
    Parcel* root = hashTable->root[hash];

    if (root == NULL) {
        printf("No parcels found for destination: %s\n", destination);
        return;
    }

    printOtherWeightedParcels(root, weight);
}

// Function to display the total parcel load and valuation for the country: 
// When user enters country name, display the cumulative parcel 
// load and total valuation of all the parcels.
void displayStatsTotal(HashTable* hashTable, char* destination)
{
    unsigned long hash = generateHash(destination);
    Parcel* root = hashTable->root[hash];

    if (root == NULL) {
        printf("No parcels found for destination: %s\n", destination);
        return;
    }

    Parcel* result = initializeParcelNode(destination, 0, 0);
    calculateStatsTotal(root, result);

    printf("The culminative weight for parcels under destination $s is: %d \n",
        destination, result->weight);
    printf("The culminative valuation for parcels under destination $s is: %.2f \n", destination,
        result->valuation);
        free(result);
}

// Function to add culminative weight and value for parcel and return as Parcel*
void calculateStatsTotal(Parcel* root, Parcel* result)
{
    if (root == NULL)
        return;

    result->weight += root->weight;
    result->valuation += result->valuation;

    calculateStatsTotal(root->left, result);
    calculateStatsTotal(root->right, result);
}

// Function to show the cheapest and most expensive parcel details
void displayCheapestAndMostExpensiveParcels(HashTable* hashTable, char* destination) {
    unsigned long hash = generateHash(destination);
    Parcel* root = hashTable->root[hash];

    if (root == NULL) {
        printf("No parcels found for destination: %s\n", destination);
        return;
    }


    Parcel* cheapest = initializeParcelNode(destination, 0, root->valuation);
    Parcel* mostExpensive = initializeParcelNode(destination, 0, root->valuation);

    findCheapestParcel(root, cheapest);
    findMostExpensiveParcel(root, mostExpensive);

    printf("The cheapest parcel for destination %s: weight - %d  valuation - %.2f\n", destination,
        cheapest->weight, cheapest->valuation);
    printf("The most expensive valuation for destination %s: weight - %d  valuation - %.2f\n", destination, mostExpensive->weight,
        mostExpensive->valuation);
    free(cheapest);
    free(mostExpensive);
}

// Function to find the cheapest parcel (by taking the root valuation and comparing)
void findCheapestParcel(Parcel* root, Parcel* result) {
    if (root == NULL)
        return;

    if (root->valuation < result->valuation)
    {
        result->valuation = root->valuation;
        result->weight = root->weight;
    }

    findCheapestParcel(root->left, result);
    findCheapestParcel(root->right, result);

}

// Function to find the most expensive parcel (by taking the root valuation and comparing)
void findMostExpensiveParcel(Parcel* root, Parcel* result) {
    if (root == NULL)
        return;

    if (root->valuation > result->valuation)
    {
        result->valuation = root->valuation;
        result->weight = root->weight;
    }

    findMostExpensiveParcel(root->left, result);
    findMostExpensiveParcel(root->right, result);
}

// Find lightest and heaviest parcels for country
void displayLightestAndHeaviestParcel(HashTable* hashTable, char* destination)
{
    unsigned long hash = generateHash(destination);
    Parcel* root = hashTable->root[hash];

    if (root == NULL) {
        printf("No parcels found for destination: %s\n", destination);
        return;
    }

    Parcel* lightest = initializeParcelNode(destination, root->weight, 0);
    Parcel* heaviest = initializeParcelNode(destination, root->weight, 0);
    printf("The lightest parcel for destination %s: weight - %d  valuation - %.2f\n", destination, lightest->weight,
        lightest->valuation);
    printf("The heaviest valuation for destination %s: weight - %d  valuation - %.2f\n", destination, heaviest->weight,
        heaviest->valuation);

    free(lightest);
    free(heaviest);
}


// Finds lightest parcel by traversing left
void findLightestParcel(Parcel* root, Parcel* result)
{
    if (root == NULL)
        return;

    if (root->left != NULL)
    {
        return findLightestParcel(root->left, result);
    }
    else
    {
        result->weight = root->weight;
        result->valuation = root->valuation;
    }
}

// Finds heaviest parcel by traversing right
void findHeaviestParcel(Parcel* root, Parcel* result)
{
    if (root == NULL)
        return;

    if (root->right != NULL)
    {
        return findHeaviestParcel(root->right, result);
    }
    else
    {
        result->weight = root->weight;
        result->valuation = root->valuation;
    }
}

// Function to load data into the hash table
int loadDataFromFile(HashTable* hashTable, char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return FILE_OPEN_ERROR;
    }
    char destination[MAX_COUNTRY_LENGTH + 1];
    int weight;
    float valuation;

    while (fscanf(file, "%20s %d %f", destination, &weight, &valuation) == 3) {
        if (insertWithSeparateChaining(hashTable, destination, weight, valuation) == FAILURE) {
            printf("Failed to insert parcel: %s\n", destination);
        }    
        else {
            printf("Inserted: %s %d %.2f\n", destination, weight, valuation);
        }
    }

    if (fclose(file) != 0) {
        printf("Error closing file %s\n", filename);
        return FILE_CLOSE_ERROR;
    }

    return SUCCESS;
}

// Function to display the menu
void displayMenu(HashTable* hashTable) {
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
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a valid choice.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
        case 1:
            printf("Enter country name: ");
            if (scanf("%20s", destination) != 1) {
                printf("Invalid input. Please enter a valid country name.\n");
                while (getchar() != '\n');
                continue;
            }
            displayParcelsForDestination(hashTable, destination);
            break;
        case 2:
            printf("Enter country name: ");
            if (scanf("%20s", destination) != 1) {
                printf("Invalid input. Please enter a valid country name.\n");
                while (getchar() != '\n');
                continue;
            }
            printf("Enter weight to search: ");
            if (scanf("%d", &weight) != 1) {
                printf("Invalid input. Please enter a valid weight.\n");
                while (getchar() != '\n');
                continue;
            }
            {
                unsigned long hash = generateHash(destination);
                Parcel* root = hashTable->root[hash];

                if (root != NULL) {
                    Parcel* foundParcel = insertParcelIntoBST(root, destination, weight, 0.0f);
                    if (foundParcel != NULL) {
                        printf("Parcel found with weight %d in %s.\n", weight, destination);
                    }
                    else {
                        printf("Parcel not found with weight %d in %s.\n", weight, destination);
                    }
                }
                else {
                    printf("No parcels found for destination: %s\n", destination);
                }
            }
            break;
        case 3:
            printf("Enter weight to exclude: ");
            if (scanf("%d", &weight) != 1) {
                printf("Invalid input. Please enter a valid weight.\n");
                while (getchar() != '\n');
                continue;
            }
            for (int i = 0; i < HASH_TABLE_SIZE; i++) {
                Parcel* root = hashTable->root[i];
                if (root != NULL) {
                    printOtherWeightedParcels(root, weight);
                }
            }
            break;
        case 4:
            printf("Enter the name of the file to load parcels: ");
            if (scanf("%s", fileName) != 1) {
                printf("Invalid input. Please enter a valid file name.\n");
                while (getchar() != '\n');
                continue;
            }
            if (loadDataFromFile(hashTable, fileName) == SUCCESS) {
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

    HashTable* hashTable = initializeHashTable();
    displayMenu(hashTable);
    
    return 0;
}

//
//Function: readLine()
//Description: This function reads one line from a file
//Parameters:
//	FILE* file: pointer for representing file being read
//Returns:
//  char*: Pointer to C-style string representing line read. 
//     NULL if there's no line
//
char* readLine(FILE* file)
{
    char* line = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
    if (line == NULL)
    {
        printf("Memory allocation error: readLine()\n");
        return NULL;
    }


    if (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {
        char* ptr = strchr(line, '\n');
        if (ptr != NULL)
        {
            *ptr = '\0';
        }

        return line;
    }

    free(line);
    return NULL;
}

//
//Function: readAndSort()
//Description: This function reads the content a file and assigns the correct values
//	to the MythUnit node within the doubly linked list. It also sorts the node 
//  according to the powerLevel property.
//Parameters:
//	char* fileName: C-style string representing file name
//Returns:
//  int: return code indicating success or failure, or whether file was opened or closed
//      successfully.
//
int readAndSort(char* fileName, HashTable* hashTable)
{
    char destination[MAX_COUNTRY_LENGTH] = { " " };
    int weight = 0;
    float valuation = 0;

    FILE* file = fopen(fileName, "r");
    if (file == NULL)
    {
        printf("Failed to open file");
        return FILE_OPEN_ERROR;
    }

    bool endOfFile = false;
    while (!endOfFile)
    {
        char* line = readLine(file);
        if (line == NULL)
        {
            endOfFile = feof(file);
            if (endOfFile)
            {
                if (fclose(file) != 0)
                {
                    printf("error closing file");
                    return FILE_CLOSE_ERROR;
                }
                break;
            }
            printf("Error reading line.");
            continue;
        }

        if (sscanf(line, "%19[^,],%d,%f", destination, &weight, &valuation) == 3)
        {
            insertWithSeparateChaining(hashTable, destination, weight, valuation);// reads and sorts into the BST  
            free(line);
        }
        else
        {
            printf("Error parsing line: %s\n", line);
            free(line);
        }

    }

    return SUCCESS;
}
