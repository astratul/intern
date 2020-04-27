/*
Exercise 7-9. Functions like isupper can be implemented to save space or to save time. Explore both possibilities.
*/

#include <stdio.h>
#include <stdlib.h>

int isupper2(int c)
{
    if (c >= 'A' && c <= 'Z')
        return 1;
    else
        return 0;
}


int main() {
    int c;
    while ((c = getchar()) != 'x'){
        if (c == '\n')
            continue;
        if (isupper2(c) == 1)
            printf("Is upper case!\n");
        else
            printf("Is not upper case!\n");
    }
    return EXIT_SUCCESS;
}

