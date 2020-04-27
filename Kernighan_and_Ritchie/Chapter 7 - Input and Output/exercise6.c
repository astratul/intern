/*
Exercise 7-6. Write a program to compare two files, printing the first line where they differ.
*/

#define MAXLINE 1000
#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>

int getline2(char *line, int max);
void diffile(FILE *fp1, FILE *fp2);
char *fgets(char *s, int n, FILE *iop);

char line[MAXLINE];
char line2[MAXLINE];

int main(int argc, char *argv[]) {
    FILE *fp1, *fp2;
    if (argc == 1)
    {
        printf("\tPlease provide two files\n");
        return 1;
    }
    else
        while (--argc > 0) {
            if ((fp1 = fopen(argv[1], "r")) == NULL) {
                printf("\tCan't open %s\n", *argv);
                return 1;
            }

            if ((fp2 = fopen(argv[2], "r")) == NULL) {
                printf("\tCan't open %s\n", *argv);
                return 1;
            }
            diffile(fp1, fp2);
            fclose(fp1);
            fclose(fp2);
        }

    return 0;
}

void diffile(FILE *fp1, FILE *fp2)
{
    int f1, f2;
    while (1) {
        if ((f1 = getc(fp1)) == EOF)
            break;
        if ((f2 = getc(fp2)) == EOF)
            break;
        if (f1 != f2) {
            printf("\nFirst file: ");
            while(fgets(line,MAXLINE, fp1) > 0)
                    printf("%s\n",line);
            printf("\nSecond file: ");
            while(fgets(line2,MAXLINE, fp2) > 0)
                    printf("%s\n",line2);
            printf("\n");
            break;
        }
    }
}
char *fgets(char *s, int n, FILE *iop){
    register int c;
    register char *cs;
    cs = s;
    while (--n > 0 && (c = getc(iop)) != EOF)
        if ((*cs++ = c) == '\n')
            break;
    *cs = '\0';
    return (c == EOF && cs == s) ? NULL : s;
}

int getline2(char *line, int max){
    if (fgets(line, max, stdin) == NULL)
        return 0;
    else
        return strlen(line);
}