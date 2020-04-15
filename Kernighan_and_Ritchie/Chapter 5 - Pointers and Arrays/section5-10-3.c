// 5.10 Command-line Arguments

/*
As a second example, let us make some enhancements to the pattern-finding program from Section 4.1. If you recall, we wired the search pattern deep into the program, an obviously unsatisfactory arrangement. Following the lead of the UNIX program grep , let us enhance the program so the pattern to be matched is specified by the first argument on the command line.
*/

#include <stdio.h>
#include <string.h>
#define MAXLINE 1000

int getline2(char s[], int lim)
{
    int c, i;

    for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
        s[i] = c;
    if (c == '\n')
    {
        s[i] = c;
        ++i;
    }
    s[i] = '\0';
    return i;
}

/* find: print lines that match pattern from 1st arg */
// main(int argc, char *argv[])
// {
//     char line[MAXLINE];
//     long lineno = 0;
//     int c, except = 0, number = 0, found = 0;

//     if (argc != 2)
//         printf("Usage: find pattern\n");
//     else
//         while (getline2(line, MAXLINE) > 0)
//             if (strstr(line, argv[1]) != NULL)
//             {
//                 printf("%s", line);
//                 found++;
//             }
//     return found;
// }
int main(int argc, char *argv[])
{
    char line[MAXLINE];
    long lineno = 0;
    int c, except = 0, number = 0, found = 0;
    while (--argc > 0 && (*++argv)[0] == '-')
        while (c = *++argv[0])
            switch (c)
            {
            case 'x':
                except = 1;
                break;
            case 'n':
                number = 1;
                break;
            default:
                printf("find: illegal option %c\n", c);
                argc = 0;
                found = -1;
                break;
            }
    if (argc != 1)
        printf("Usage: find -x -n pattern\n");
    else
        while (getline2(line, MAXLINE) > 0)
        {
            lineno++;
            if ((strstr(line, *argv) != NULL) != except)
            {
                if (number)
                    printf("%ld:", lineno);
                printf("%s", line);
                found++;
            }
        }
    return found;
}