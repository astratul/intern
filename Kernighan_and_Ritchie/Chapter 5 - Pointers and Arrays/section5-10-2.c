// 5.10 Command-line Arguments

#include <stdio.h>
/* echo command-line arguments; 2nd version */
int main(int argc, char *argv[])
{
    while (--argc > 0)
        printf("%s%s", *++argv, (argc > 1) ? " " : "");
    printf("\n");
    return 0;
}
/*
Since argv is a pointer to the beginning of the array of argument strings, incrementing it by 1 ( ++argv ) makes it point at the original argv[1] instead of argv[0] . Each successive increment moves it along to the next argument; *argv is then the pointer to that argument. At the same time, argc is decremented; when it becomes zero, there are no arguments left to print.

Alternatively, we could write the printf statement as
printf((argc > 1) ? "%s " : "%s", *++argv);
This shows that the format argument of printf can be an expression too.
*/