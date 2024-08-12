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
int readAndSort(char* fileName, struct MythUnit** head, struct MythUnit** tail)
{
    char name[MAX_LINE_LENGTH] = { " " };
    int powerLevel = 0;

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

        if (sscanf(line, "%29[^,],%d", name, &powerLevel) == 2)
        {
            recordAndSort(head, tail, name, powerLevel); // reads and sorts into the data structure  
            free(line);
        }
        else
        {
            printf("Error parsing line: %s\n", line);
            free(line);
        }

    }

    printList(head);

    return SUCCESS;
}
