// gcc -o system1 system1.c

#include <stdlib.h>
#include <stdio.h>

int main()
{
    printf("Running ps with system\n");
    system("ps ax");
    // system("ps ax &");
    printf("Done.\n");
    exit(0);
}