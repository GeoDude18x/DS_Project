/*
FILE : project.cpp
PROJECT : SENG1050 - DATA STRUCTURES GROUP PROJECT
PROGRAMMERS : Behzad Afrasiabi & George Dall
FIRST VERSION : 2024-08-11
DESCRIPTION :
This program implements a hashTable with binary search tree entries, in order to manage various
parcels and display output based on a menu given to the user. The hashTable entries are the roots
of the BSTs, and each BST is sorted according to the Parcels' weight attributes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning (disable : 4996)

// Constants
#define HASH_TABLE_SIZE 127
#define MAX_COUNTRY_LENGTH 20
#define MAX_WEIGHT 50000
#define MIN_WEIGHT 100
#define MAX_VALUATION 2000
#define MIN_VALUATION 10
#define MAX_LINE_LENGTH 40
#define INPUT_FILE_NAME "couriers.txt"

// Return Values
#define SUCCESS 0
#define FAILURE -1
#define FILE_OPEN_ERROR -2
#define FILE_CLOSE_ERROR -3


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
void printDestinationParcels(Parcel* root);
void displayParcelsForDestination(HashTable* hashTable, char* destination);
void printOtherWeightedParcels(Parcel* root, int weight);
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


int main(void) {

    HashTable* hashTable = initializeHashTable();
    if (readAndSort((char*)INPUT_FILE_NAME, hashTable) == SUCCESS)
    {
        displayMenu(hashTable);
    }
    else
    {
        printf("Error loading file\n");
    }

    return SUCCESS;
}


// Function: generateHash()
// Description: This function generates a hash value for a given destination string 
//              using the djb2 algorithm. The hash value is used to determine the 
//              index in the hash table where the parcel data should be stored.
// Parameters:
//  char* destination: The destination string for which the hash value is to be generated.
// Returns:
//  unsigned long: The generated hash value, modulo the size of the hash table.
unsigned long generateHash(char* destination) {
    unsigned long hash = 5381;
    int c;

    while ((c = *destination++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_TABLE_SIZE;
}


// Function: initializeHashTable()
// Description: This function allocates memory for a hash table and initializes each 
//              bucket in the hash table to NULL, representing an empty table.
// Parameters:
//  void: This function does not take any parameters.
// Returns:
//  HashTable: A pointer to the newly allocated and initialized hash table. If memory 
//              allocation fails, the function will exit the program.
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


// Function: insertWithSeparateChaining()
// Description: This function inserts a parcel into the hash table based on the 
//              destination, weight, and valuation. It generates a hash value for the 
//              destination, locates the appropriate bucket in the hash table, and 
//              inserts the parcel into the binary search tree at that location. 
//              If a collision occurs (i.e., multiple parcels hash to the same bucket), 
//              the function uses separate chaining to handle the collision.
// Parameters:
//  HashTable hashTable: Pointer to the hash table where the parcel data will be stored.
//  char* destination: The destination of the parcel.
//  int weight: The weight of the parcel.
//  float valuation: The valuation of the parcel.
// Returns:
//  int: Returns SUCCESS if the parcel is successfully inserted into the hash table, 
//       and FAILURE if the insertion fails.
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
Parcel* insertParcelIntoBST(Parcel* root, char* destination, int weight, float valuation) {
    if (root == NULL) { // parent is empty so node is inserted here
        return initializeParcelNode(destination, weight, valuation);
    }

    if (weight < root->weight) { // weight is less than root's, attempting to insert again on the left node (with updated root)
        root->left = insertParcelIntoBST(root->left, destination, weight, valuation);
    }
    else if (weight > root->weight) { // weight is greater than root's, attempting to insert again on the right node (with updated root)
        root->right = insertParcelIntoBST(root->right, destination, weight, valuation);
    }

    return root;
}


//
//Function: displayParcelsForDestination()
//Description: This function is used to prepare the deployment of  rintDestinationParcels()
//             by checking and finding the BST root according to a hashValue generated using the
//             given destination.
//Parameters:
//  HashTable* hashTable: Pointer to the hashTable. Used to find the root of the destination's
//                        BST.
//  char* destination: C-style string used to generate the hash value for the root entry inside
//                     the hashTable.
//Returns:
//  void: Function only displays info and returns nothing.
//
void displayParcelsForDestination(HashTable* hashTable, char* destination) {
    unsigned long hash = generateHash(destination);
    Parcel* root = hashTable->root[hash];

    if (root == NULL) {
        printf("No parcels found for destination: %s\n", destination);
        return;
    }
    printDestinationParcels(root);
}


//
//Function: printDestinationParcels()
//Description: This function recursively prints parcels for a destination.
//Parameters:
//  Parcel* root: Pointer to the root of the BST. This is updated as the BST undergoes
//                a pre-order traversal.
//Returns:
//  void: Function only displays info and returns nothing.
//
void printDestinationParcels(Parcel* root) {
    if (root == NULL)
        return;

    printf("%s, %d, %.2f \n", root->destination, root->weight, root->valuation);
    printDestinationParcels(root->left);
    printDestinationParcels(root->right);

}


//
//Function: printOtherWeightedParcels()
//Description: This function recursively prints parcels for a weight, ignoring the parcel
//             with the weight given.
//Parameters:
//  Parcel* root: Pointer to the root of the BST. This is updated as the BST undergoes
//                a pre-order traversal.
//	int weight: The weight to be excluded from the displayed parcels.
//Returns:
//  void: Function only displays info and returns nothing.
//
void printOtherWeightedParcels(Parcel* root, int weight) {
    if (root == NULL)
        return;

    if (root->weight != weight)
        printf("%s, %d, %.2f \n", root->destination, root->weight, root->valuation);

    printOtherWeightedParcels(root->left, weight);
    printOtherWeightedParcels(root->right, weight);
}


//
//Function: checkOtherWeightParcels()
//Description: This function is used to prepare the deployment of  printOtherWeightedParcels()
//             by checking and finding the BST root according to a hashValue generated using the
//             given destination.
//Parameters:
//  HashTable* hashTable: Pointer to the hashTable. Used to find the root of the destination's
//                        BST.
//  char* destination: C-style string used to generate the hash value for the root entry inside
//                     the hashTable.
//	int weight: The weight to be excluded from the displayed parcels, passed to printOtherWeightedParcels()
//              directly.
//Returns:
//  void: Function only displays info and returns nothing.
//
void checkOtherWeightParcels(HashTable* hashTable, char* destination, int weight) {
    unsigned long hash = generateHash(destination);
    Parcel* root = hashTable->root[hash];

    if (root == NULL) {
        printf("No parcels found for destination: %s\n", destination);
        return;
    }

    printOtherWeightedParcels(root, weight);
}


//
//Function: checkOtherWeightParcels()
//Description: This function is used to display the total parcel load and valuation for a given
//             destination. The totals are calculated through calculateStatsTotal().
//Parameters:
//  HashTable* hashTable: Pointer to the hashTable. Used to find the root of the destination's
//                        BST.
//  char* destination: C-style string used to generate the hash value for the root entry inside
//                     the hashTable.
//Returns:
//  void: Function only displays info and returns nothing.
//
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

    printf("The culminative weight for parcels under destination %s is: %d \n", destination, result->weight);
    printf("The culminative valuation for parcels under destination %s is: %.2f \n", destination, result->valuation);
    free(result);
}


//
//Function: checkOtherWeightParcels()
//Description: This function is used to calculate the total parcel load and valuation for a given
//             destination. It updates the result parameter, as the totals are calculated.
//Parameters:
//  Parcel* root: Pointer to the root of the BST.
//  Parcel* result: Pointer to the result Parcel object being modified
//Returns:
//  void: Function only updates a Parcel object using its' pointer and returns nothing.
//
void calculateStatsTotal(Parcel* root, Parcel* result)
{
    if (root == NULL)
        return;

    result->weight += root->weight;
    result->valuation += root->valuation;

    calculateStatsTotal(root->left, result);
    calculateStatsTotal(root->right, result);
}


//
//Function: displayCheapestAndMostExpensiveParcels()
//Description: This function is used to show the cheapest and most expensive parcel details.
//             These details are gathered using the functions  findCheapestParcel(), and
//              findMostExpensiveParcel().
//             
//Parameters:
//  HashTable* hashTable: Pointer to the hashTable. Used to find the root of the destination's
//                        BST.
//  char* destination: C-style string used to generate the hash value for the root entry inside
//                     the hashTable.
//Returns:
//  void: Function only displays info and returns nothing.
//
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
    printf("The most expensive parcel for destination %s: weight - %d  valuation - %.2f\n", destination, mostExpensive->weight,
        (double)mostExpensive->valuation);
    free(cheapest);
    free(mostExpensive);
}


//
//Function: findCheapestParcel()
//Description: This function is used to find the cheapest parcel by taking the root 
//             of the BST and comparing its' valuation attribute with the other parcels.
//Parameters:
//  Parcel* root: Pointer to the root of the BST.
//  Parcel* result: Pointer to the result Parcel object being modified.
//Returns:
//  void: Function only updates a Parcel object using its' pointer and returns nothing.
//
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


//
//Function: findMostExpensiveParcel()
//Description: This function is used to find the most expensive parcel by taking the root 
//             of the BST and comparing its' valuation attribute with the other parcels.
//Parameters:
//  Parcel* root: Pointer to the root of the BST.
//  Parcel* result: Pointer to the result Parcel object being modified.
//Returns:
//  void: Function only updates a Parcel object using its' pointer and returns nothing.
//
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


//
//Function: displayLightestAndHeaviestParcel()
//Description: This function is used to show the lightest and heaviest parcel details.
//             These details are gathered using the functions findLightestParcel(), 
//             and findHeaviestParcel().           
//Parameters:
//  HashTable* hashTable: Pointer to the hashTable. Used to find the root of the destination's
//                        BST.
//  char* destination: C-style string used to generate the hash value for the root entry inside
//                     the hashTable.
//Returns:
//  void: Function only displays info and returns nothing.
//
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

    findLightestParcel(root, lightest);
    findHeaviestParcel(root, heaviest);

    printf("The lightest parcel for destination %s: weight - %d  valuation - %.2f\n", destination, lightest->weight,
        lightest->valuation);
    printf("The heaviest valuation for destination %s: weight - %d  valuation - %.2f\n", destination, heaviest->weight,
        heaviest->valuation);

    free(lightest);
    free(heaviest);
}


//
//Function: findMostExpensiveParcel()
//Description: This function is used to find the lightest parcel by taking the root 
//             of the BST and traversing down the left subtree.
//Parameters:
//  Parcel* root: Pointer to the root of the BST.
//  Parcel* result: Pointer to the result Parcel object being modified.
//Returns:
//  void: Function only updates a Parcel object using its' pointer and returns nothing.
//
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


//
//Function: findMostExpensiveParcel()
//Description: This function is used to find the heaviest parcel by taking the root 
//             of the BST and traversing down the left subtree.
//Parameters:
//  Parcel* root: Pointer to the root of the BST.
//  Parcel* result: Pointer to the result Parcel object being modified.
//Returns:
//  void: Function only updates a Parcel object using its' pointer and returns nothing.
//
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


// Function: displayMenu()
// Description: This function displays a menu to the user and handles various options
//              such as displaying parcel details, searching by weight and displaying lower/higher
//              weighted parcels for destination entered, displaying overall load and valuation, 
//              displaying cheapest and most expensive parcels and displaying lightest and heaviest parcels.
// Parameters:
//  HashTable* hashTable: Pointer to the hash table containing parcel data.
// Returns:
//  void: This function does not return a value.
void displayMenu(HashTable* hashTable) {
    int choice;
    char destination[MAX_COUNTRY_LENGTH + 1];
    int weight;

    do {
        printf("Menu:\n");
        printf("1. Enter country name and display all parels.\n");
        printf("2. Enter country and weight to show for parcels with a lower/higher weight.\n");
        printf("3. Enter country name to display total parcel load and valuation.\n");
        printf("4. Enter the country name and display cheapest and most expensive parcels.\n");
        printf("5. Enter the country name and display lightest and heaviest parcels.\n");
        printf("6. Exit the application.\n\n");
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
            checkOtherWeightParcels(hashTable, destination, weight);
            break;
        case 3:
            printf("Enter country name: ");
            if (scanf("%20s", destination) != 1) {
                printf("Invalid input. Please enter a valid country name.\n");
                while (getchar() != '\n');
                continue;
            }
            displayStatsTotal(hashTable, destination);
            break;
        case 4:
            printf("Enter country name: ");
            if (scanf("%20s", destination) != 1) {
                printf("Invalid input. Please enter a valid country name.\n");
                while (getchar() != '\n');
                continue;
            }
            displayCheapestAndMostExpensiveParcels(hashTable, destination);
            break;
        case 5:
            printf("Enter country name: ");
            if (scanf("%20s", destination) != 1) {
                printf("Invalid input. Please enter a valid country name.\n");
                while (getchar() != '\n');
                continue;
            }
            displayLightestAndHeaviestParcel(hashTable, destination);
            break;
        case 6:
            printf("Exiting the application...\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }

        printf("\n");

    } while (choice != 6);
}


//
//Function: readLine()
//Description: This function reads one line from a file
//Parameters:
//	FILE* file: pointer for representing file being read
//Returns:
//  char*: Pointer to C-style string representing line read. 
//         NULL if there's no line.
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
//Description: This function reads the content a file and populates
//	           the hashTable and its BSTs using  insertWithSeparateChaining()
//Parameters:
//	char* fileName: C-style string representing file name
//  HashTable* hashTable: Pointer to the hashTable. 
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
        printf("Failed to open file\n");
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
                    printf("error closing file\n");
                    return FILE_CLOSE_ERROR;
                }
                break;
            }
            printf("Error reading line.\n");
            continue;
        }

        if (sscanf(line, "%20[^,],%d,%f", destination, &weight, &valuation) == 3)
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
