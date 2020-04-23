/*
Exercise 7-1. Write a program that converts upper case to lower or lower case to upper, depending on the name it is invoked with, as found in argv[0] .
*/

// LOW - convert upper case to lower case
// UP - convert lower case to upper case
#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[]){
    int c;
    if(argc == 1)
        while ((c = getchar()) != EOF)
            putchar(tolower(c));
    else if(argc == 2){
        if(strcmp(argv[1],"LOW")==0)
            while ((c = getchar()) != EOF)
                putchar(tolower(c));
        else if(strcmp(argv[1],"UP")==0)
            while ((c = getchar()) != EOF)
                putchar(toupper(c));
        }
    else if(argc >2)
        printf("Wrong number of arguments.\n");    
    return 0;
}