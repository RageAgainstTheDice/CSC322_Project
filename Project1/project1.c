//
// Created by Arian Djahed from March 28, 2024 to April 4, 2024
//
//-------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define BOATMAX 120 // maximum number of boats allowed
#define NAMEMAX 127 // maximum character length of a boat's name
#define LINEMAX 150 // the maximum number of characters allowed in a line from the input file

typedef char String[NAMEMAX];

typedef enum {
    slip,
    land,
    trailor,
    storage,
    no_place
} PlaceType;

typedef union {
    int SlipNum;
    char BayChar;
    String Trailortag;
    int StorageNum;
} ExtraInfo;

typedef struct {
    String Name;
    int Length;
    PlaceType Place;
    ExtraInfo TypeInfo;
    float Owed;
} Boat;
//-------------------------------------------------------------------------------------------------
//----For use with qsort (so that adjacent boat entries can be compared alphabetically)
int cmpfunc (const void * a, const void * b) {
    Boat* boat1 = *(Boat**)a;
    Boat* boat2 = *(Boat**)b;
    return strcasecmp(boat1->Name, boat2->Name);
}
//-------------------------------------------------------------------------------------------------
//----Convert a string to a place
PlaceType StringToPlaceType(String PlaceString) {

    if (!strcasecmp(PlaceString,"slip")) {
        return(slip);
    }
    if (!strcasecmp(PlaceString,"land")) {
        return(land);
    }
    if (!strcasecmp(PlaceString,"trailor")) {
        return(trailor);
    }
    if (!strcasecmp(PlaceString,"storage")) {
        return(storage);
    }
    return(no_place);
}
//-------------------------------------------------------------------------------------------------
//----Convert a place to a string
char* PlaceToString(PlaceType Place) {

    switch (Place) {
        case slip:
            return("slip");
        case land:
            return("land");
        case trailor:
            return("trailor");
        case storage:
            return("storage");
        case no_place:
            return("no_place");
        default:
            // printf("How the frick did I get here?\n"); // this is here for debugging purposes
            exit(EXIT_FAILURE);
            break;
    } // end of switch block
}
//-------------------------------------------------------------------------------------------------
//----Read the file lines and put them in the dedicated array
int ReadFromFile(FILE* file, Boat* array[BOATMAX]) {
    // initialize necessary variables
    String line;
    char* token;
    char* tokens[5];
    int linecount = 0, tokenIndex;

    // set loop conditions so that it doesn't read more than the maximum amount of allowed boats
    while ((fgets(line, LINEMAX, file)) != NULL && linecount <= BOATMAX) {
        // allocate memory for each new boat read
        array[linecount] = (Boat*)malloc(sizeof(Boat));

        // tokenize the line to prime it for the next part
        token = strtok(line, ",");
        tokenIndex = 0;
        while (token != NULL) {
            tokens[tokenIndex++] = token;
            token = strtok(NULL, ",");
        }

        // set each token to its respective value in the struct in the boat array
        strcpy(array[linecount]->Name, tokens[0]);
        array[linecount]->Length = atoi(tokens[1]);
        array[linecount]->Place = StringToPlaceType(tokens[2]);
        switch(array[linecount]->Place) {
            case slip:
                array[linecount]->TypeInfo.SlipNum = atoi(tokens[3]);
                break;
            case land:
                array[linecount]->TypeInfo.BayChar = tokens[3][0];
                break;
            case trailor:
                strcpy(array[linecount]->TypeInfo.Trailortag, tokens[3]);
                break;
            case storage:
                array[linecount]->TypeInfo.StorageNum = atoi(tokens[3]);
                break;
            default:
                printf("type does not exist"); // this is here for debugging purposes
        } // end of switch block
        array[linecount]->Owed = atof(tokens[4]);

        linecount++; // increment linecount to get ready to read the next line
    } // end of while loop

    return(linecount);
}
//-------------------------------------------------------------------------------------------------
//----Write the data in the array back into the .csv file in csv format
void SaveToFile(FILE* file, Boat* array[BOATMAX], int numBoats) {
    // set loop conditions so that it only loops over array indices with boats in them
    for (int i = 0; i < numBoats; i++) {
        switch (array[i]->Place) {
        case slip:
            fprintf(file, "%s,%d,%s,%d,%.2f\n", array[i]->Name, array[i]->Length,
                PlaceToString(array[i]->Place), array[i]->TypeInfo.SlipNum, array[i]->Owed);
            break;
        case land:
            fprintf(file, "%s,%d,%s,%c,%.2f\n", array[i]->Name, array[i]->Length,
                PlaceToString(array[i]->Place), array[i]->TypeInfo.BayChar, array[i]->Owed);
            break;
        case trailor:
            fprintf(file, "%s,%d,%s,%s,%.2f\n", array[i]->Name, array[i]->Length,
                PlaceToString(array[i]->Place), array[i]->TypeInfo.Trailortag, array[i]->Owed);
            break;
        case storage:
            fprintf(file, "%s,%d,%s,%d,%.2f\n", array[i]->Name, array[i]->Length,
                PlaceToString(array[i]->Place), array[i]->TypeInfo.StorageNum, array[i]->Owed);
            break;
        default:
            printf("huh?\n"); // this is here for debugging purposes
            break;
        } // end of switch block
    }    
}
//-------------------------------------------------------------------------------------------------
//----Print the contents of the boat array in an easy-to-read fashion
void Inventory(Boat* array[BOATMAX], int numBoats) {
    // make sure array is sorted before you print it out
    qsort(array, numBoats, sizeof(Boat*), cmpfunc);

    // set loop conditions so that it only loops over array indices with boats in them
    for (int i = 0; i < numBoats; i++) {
        switch (array[i]->Place) {
            case slip:
                printf("%-20s %3d' %7s   #%3d   Owes $%7.2lf\n", array[i]->Name, array[i]->Length,
                    PlaceToString(array[i]->Place), array[i]->TypeInfo.SlipNum, array[i]->Owed);
                break;
            case land:
                printf("%-20s %3d' %7s %6c   Owes $%7.2lf\n", array[i]->Name, array[i]->Length,
                    PlaceToString(array[i]->Place), array[i]->TypeInfo.BayChar, array[i]->Owed);
                break;
            case trailor:
                printf("%-20s %3d' %7s %6s   Owes $%7.2lf\n", array[i]->Name, array[i]->Length,
                    PlaceToString(array[i]->Place), array[i]->TypeInfo.Trailortag, array[i]->Owed);
                break;
            case storage:
                printf("%-20s %3d' %7s   #%3d   Owes $%7.2lf\n", array[i]->Name, array[i]->Length,
                    PlaceToString(array[i]->Place), array[i]->TypeInfo.StorageNum, array[i]->Owed);
                break;
            default:
                printf("what are you doing here?"); // this is here for debugging purposes
                break;
       } // end of switch block
    }
}
//-------------------------------------------------------------------------------------------------
//----Add a new boat into the array and sort
void Add(Boat* array[BOATMAX], int* numBoats) {
    // initialize necessary variables
    String line;
    char* token;
    char* tokens[5];
    int tokenIndex = 0;

    // recieve the input, allocate more memory in the array for it, and then tokenize each part
    printf("Please enter the boat data in CSV format                 : ");
    scanf("%s", line);
    array[*numBoats] = (Boat*)malloc(sizeof(Boat));
    token = strtok(line, ",");
    while (token != NULL) {
        tokens[tokenIndex++] = token;
        token = strtok(NULL, ",");
    }

    // set each token to its respective aspect of the boat structure
    strcpy(array[*numBoats]->Name, tokens[0]);
    array[*numBoats]->Length = atoi(tokens[1]);
    array[*numBoats]->Place = StringToPlaceType(tokens[2]);
    switch(array[*numBoats]->Place) {
        case slip:
            array[*numBoats]->TypeInfo.SlipNum = atoi(tokens[3]);
            break;
        case land:
            array[*numBoats]->TypeInfo.BayChar = tokens[3][0];
            break;
        case trailor:
            strcpy(array[*numBoats]->TypeInfo.Trailortag, tokens[3]);
            break;
        case storage:
            array[*numBoats]->TypeInfo.StorageNum = atoi(tokens[3]);
            break;
        default:
            printf("type does not exist"); // this is here for debugging purposes
    } // end of switch block
    array[*numBoats]->Owed = atof(tokens[4]);

    // sort the array again just in case the added boat messes up the order
    qsort(array, *numBoats, sizeof(Boat*), cmpfunc);

    // increment numBoats so that the program recognizes the new boat
    (*numBoats)++;
}
//-------------------------------------------------------------------------------------------------
//----Remove a pre-existing boat from the array and sort
void Remove(Boat* array[BOATMAX], int* numBoats) {
    // initialize necessary variables
    String name;
    int index = -1;
    Boat* temp;

    // read input in such a way that it allows for spaces and flushes any newlines in the stream
    printf("Please enter the boat name                               : ");
    scanf(" %[^\n]s", name);

    // check to see if the specified boat is in the array (case-insensitive)
    for (int i = 0; i < *numBoats; i++) {
        if(strcasecmp(array[i]->Name, name) == 0) {
            index = i;
            break;
            }
    }

    // if not, exit back to main
    if(index < 0) {
        printf("No boat with that name\n");
            return;
    }

    // swap out the boat in the last index with the boat you want to remove so you can remove the
    // last index; afterwards, free and decrement so the program "forgets" about the boat you're
    // removing
    temp = array[*numBoats-1];
    array[*numBoats-1] = array[index];
    array[index] = temp;
    free(array[*numBoats-1]);
    (*numBoats)--;

    // sort the array again just in case the removed boat messes up the order
    qsort(array, *numBoats, sizeof(Boat*), cmpfunc);
}
//-------------------------------------------------------------------------------------------------
//----"Pay" one of the boat rates (i.e. subtract that value from the boat rate)
void Payment(Boat* array[BOATMAX], int* numBoats) {
    // initialize necessary variables
    String name;
    int index = -1;
    float amount = 0.0;

    // read input in such a way that it allows for spaces and flushes any newlines in the stream
    printf("Please enter the boat name                               : ");
    scanf(" %[^\n]s", name);

    // check to see if the specified boat is in the array (case-insensitive)
    for (int i = 0; i < *numBoats; i++) {
        if(strcasecmp(array[i]->Name, name) == 0) {
            index = i;
            break;
        }
    }

    // if not, exit back to main
    if(index < 0) {
        printf("No boat with that name\n");
        return;
    }

    // recieve the amount and check that it's a valid value; if so, subtract; else, indicate
    printf("Please enter the amount to be paid                       : ");
    scanf("%f", &amount);
    if(amount > array[index]->Owed) {
        printf("That is more than the amount owed, $%.2f\n", array[index]->Owed);
    } else {
        array[index]->Owed -= amount;
    }
}
//-------------------------------------------------------------------------------------------------
//----Increase the rate for each boat in the array based on the boat's place
void Month(Boat* array[BOATMAX], int* numBoats) {
    // set loop conditions so that it only loops over array indices with boats in them
    for (int i = 0; i < *numBoats; i++) {
        switch(array[i]->Place) {
            case slip:
                array[i]->Owed += (12.50 * array[i]->Length);
                break;
            case land:
                array[i]->Owed += (14.00 * array[i]->Length);
                break;
            case trailor:
                array[i]->Owed += (25.00 * array[i]->Length);
                break;
            case storage:
                array[i]->Owed += (11.20 * array[i]->Length);
                break;
            default:
                printf("watchu doin' g\n"); // this is here for debugging purposes
                break;
        } // end of switch block
    } // end of for loop
}
//-------------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    // initialize necessary variables
    Boat* BoatArray[BOATMAX];
    FILE* TheFile;
    char input;
    bool exit = false;

    // check that the user has inputted the necessary number of command line arguments
    if(argc != 2) {
        printf("Must have a command line argument indicating the file");
        return(EXIT_FAILURE);
    } else {TheFile = fopen(argv[1], "r");}

    // check that the file opens, reads, and closes properly (all in read mode)
    if(TheFile == NULL) {
        printf("error opening file in read mode");
        return(EXIT_FAILURE);
    }
    int numBoats = ReadFromFile(TheFile, BoatArray);
    // printf("number of boats: %d\n", numBoats); // this is here for debugging purposes
    if(numBoats < 0) {
        printf("error reading from file");
        return(EXIT_FAILURE);
    }
    if(fclose(TheFile) == EOF) {
        printf("error closing file");
        return(EXIT_FAILURE);
    }

    // produce the options menu and loop it so it keeps showing until the user manually exits
    printf("Welcome to the Boat Management System\n");
    printf("-------------------------------------\n");
    while (true) {
        printf("(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
        scanf(" %c", &input);
        switch (input) {
        case 'I': case 'i':
            Inventory(BoatArray, numBoats);
            break;
        case 'A': case 'a':
            Add(BoatArray, &numBoats);
            break;
        case 'R': case 'r':
            Remove(BoatArray, &numBoats);
            break;
        case 'P': case 'p':
            Payment(BoatArray, &numBoats);
            continue;
        case 'M': case 'm':
            Month(BoatArray, &numBoats);
            break;
        case 'X': case 'x':
            exit = true;
            printf("Exiting the boat management system\n");
            break;
        default:
            printf("Invalid option %c\n", input);
            break;
        }

        // if the user selected exit, break out of the loop
        if(exit) {break;}
    }
    
    // check that the file opens, writes, and closes properly (all in write mode)
    if(TheFile != fopen(argv[1], "w")) {
        printf("error opening in write mode");
        return(EXIT_FAILURE);
    }
    qsort(BoatArray, numBoats, sizeof(Boat*), cmpfunc); // sort the array before saving
    SaveToFile(TheFile, BoatArray, numBoats); // save the array so new values are not lost
    if(fclose(TheFile) == EOF) {
        printf("error closing file");
        return(EXIT_FAILURE);
    }

    // free only the space in the array that was actually used
    for (int i = 0; i < numBoats; i++) {free(BoatArray[i]);}

    // terminate the program
    return(EXIT_SUCCESS);
}
//-------------------------------------------------------------------------------------------------