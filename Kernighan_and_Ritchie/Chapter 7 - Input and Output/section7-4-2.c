// 7.4 Formatted Input - Scanf
// input : 25 Dec 1988

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#define MAXLINE 1000
int getline1(char *line, int max);
char line[MAXLINE];

int main() /* rudimentary calculator */
{
    // int day, year;
    char monthname[20];
    // scanf("%d %s %d", &day, monthname, &year);
    int day, month, year;
    scanf("%d/%d/%d", &month, &day, &year);
    while (getline1(line, sizeof(line)) > 0)
    {
        if (sscanf(line, "%d %s %d", &day, monthname, &year) == 3)
            printf("valid: %s\n", line); /* 25 Dec 1988 form */
        else if (sscanf(line, "%d/%d/%d", &month, &day, &year) == 3)
            printf("valid: %s\n", line); /* mm/dd/yy form */
        else
            printf("invalid: %s\n", line); /* invalid form */
    }
    return 0;
}

int getline1(char s[], int lim)
{
    int c, i;
    i = 0;
    while (--lim > 0 && (c = getchar()) != EOF && c != '\n')
        s[i++] = c;
    if (c == '\n')
        s[i++] = c;
    s[i] = '\0';
    return i;
}