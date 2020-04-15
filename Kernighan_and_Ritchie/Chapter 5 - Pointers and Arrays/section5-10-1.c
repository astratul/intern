// 5.10 Command-line Arguments

#include <stdio.h>
/* echo command-line arguments; 1st version */
int main(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; i++)
        printf("%s%s", argv[i], (i < argc - 1) ? " " : "");
    printf("\n");
    return 0;
}
/* Since argv is a pointer to an array of pointers, we can manipulate the pointer rather than index the array. This next variant is based on incrementing argv , which is a pointer to pointer to char , while argc is counted down.
*/
