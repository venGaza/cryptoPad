/*
 * Description: This program generates a key to use for encoding/decoding. It is meant
 *              to be used with a redirection into a file for use with the other programs in this
 *              project.
 */

#include <stdio.h>
#include <stdlib.h> //for rand, srand, atoi, exit
#include <time.h>   //for time
#include <string.h> //for memset, size_t

int main(int argc, const char * argv[]) {
    int i;
    char letters[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int choice;
    srand((unsigned)time(0));                                 //Seed the random number generator
    
    if (argc < 2) {                                           //Check if argument entered
        printf("Error: keygen.c needs an length argument");
        fflush(stdout);
        exit(1);
    }
    
    int length = atoi(argv[1]);
    char key[length + 1];
    memset(key, '\0', length + 1);
    
    for (i = 0; i < length; i++) {                             //Create key
        choice = rand() % 27;
        key[i] = letters[choice];
    }
    
    printf("%s\n", key);                                       //Print key
    fflush(stdout);
    
    return 0;
}
