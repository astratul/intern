/*
Exercise 7-7. Modify the pattern finding program of Chapter 5 to take its input from a set of named files or, if no files are named as arguments, from the standard input. Should the file name be printed when a matching line is found?
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 1000
char *fgets(char *s, int n, FILE *iop);
int getline2(char *line, int max);
int strindex(char *s, char *t);
char pattern[] = "ould"; /* pattern to search for */

int main(int argc, char *argv[])
{
    char line[MAXLINE];
    char line1[MAXLINE];
    char line2[MAXLINE];
    int found = 0;
    int arg = 0;
    FILE *fp1, *fp2;
    FILE *fp;
    // char *name="";
    if (argc == 1)
    {
        printf("\tStandard input\n");
        while ((getline2(line, MAXLINE)) > 0)
            if (strindex(line, pattern) >= 0)
            {
                printf("%s\n", line);
                found++;
            }
    }
    else if (argc == 2)
        while (--argc > 0)
        {
            if ((fp = fopen(argv[1], "r")) == NULL)
            {
                printf("\tCan't open %s\n", *argv);
                return 1;
            }
            while (fgets(line, MAXLINE, fp) > 0)
                printf("%s\n", line);
            fclose(fp);
        }
    else if (argc == 3)
        while (--argc > 0)
        {
            if ((fp1 = fopen(argv[1], "r")) == NULL)
            {
                printf("\tCan't open %s\n", *argv);
                return 1;
            }
            if ((fp2 = fopen(argv[2], "r")) == NULL)
            {
                printf("\tCan't open %s\n", *argv);
                return 1;
            }
            // name = argv[1];
            printf(" from %s\t",argv[1]);
            while (fgets(line1, MAXLINE, fp1) > 0)
                printf("%s\n", line1);
            // name = argv[2];
            printf(" from %s\t",argv[2]);
            while (fgets(line2, MAXLINE, fp2) > 0)
                printf("%s\n", line2);
            fclose(fp1);
            fclose(fp2);
        }

    return 0;
}

int getline2(char *s, int lim)
{
    int c;
    char *t = s;
    while (--lim > 0 && (c = getchar()) != 'X' && c != '\n')
        *s++ = c;
    if (c == '\n')
        *s++ = c;
    *s = '\0';
    return s - t;
}

int strindex(char *s, char *t)
{
    char *b = s;
    char *p, *r;
    for (; *s != '\0'; s++)
    {
        for (p = s, r = t; *r != '\0' && *p == *r; p++, r++)
            ;
        if (r > t && *r == '\0')
            return s - b;
    }
    return -1;
}
char *fgets(char *s, int n, FILE *iop)
{
    register int c;
    register char *cs;
    cs = s;
    int pattern = 0;
    while (--n > 0 && (c = getc(iop)) != EOF)
    {
        if ((((*cs++ = c) == 'o')) && ((*cs++ = c) == 'u') && ((*cs++ = c) == 'l') && ((*cs++ = c) == 'd'))
        {
            pattern = pattern + 1;
        }
        else if ((*cs = c) == '\n')
        {
            *cs++;
            break;
        }
        
    }
    printf("\n %d x 'ould' ", pattern);
    *cs = '\0';
    return (c == EOF && cs == s) ? NULL : s;
}
