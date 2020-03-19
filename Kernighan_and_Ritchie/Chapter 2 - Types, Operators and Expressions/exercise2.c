/*
Exercise 2-2. Write a loop equivalent to the for loop above without using && or || .
*/

#include <stdio.h>
#define MAXLINE 1000 /* maximum input line length */
int getline2(char line[], int maxline);
void copy(char to[], char from[]); /* print the longest input */

int main()
{
int len;
int max;
char line[MAXLINE];
char longest[MAXLINE];
max = 0;
while ((len = getline2(line, MAXLINE)) > 0)
    if (len > max) {
        max = len;
        copy(longest, line);
    }
if (max > 0) /* there was a line */
    printf("%s", longest);
return 0;
}
/* getline2: read a line into s, return length */
int getline2(char s[],int lim)
{
int c, i;
    for(i=0; i < lim - 1 ;++i) {
        c = getchar();
        if (c == EOF)
            break;
        if (c == '\n')
            break;
        s[i] = c;
    }
if (c == '\n') {
    s[i] = c;
    ++i;
    }
    s[i] = '\0';
    return i;
}
/* copy: copy 'from' into 'to'; assume to is big enough */
void copy(char to[], char from[])
{
int i;
i = 0;
while ((to[i] = from[i]) != '\0')
    ++i;
}
