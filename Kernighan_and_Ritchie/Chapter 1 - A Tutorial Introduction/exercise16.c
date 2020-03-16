#include <stdio.h>
#define MAXLINE 1000 /* maximum input line length */
int getline2(char line[], int maxline);
void copy(char to[], char from[]); /* print the longest input */
/*

Exercise 1-16. Revise the main routine of the longest-line program so it will correctly print
the length of arbitrary long input lines, and as much as possible of the text.

*/

int main()
{
int len;
int max;
char line[MAXLINE];
char longest[MAXLINE];
max = 0;
while ((len = getline2(line, MAXLINE)) > 0)
{
    printf("Array of char: %s", line);
    printf("Length: %d\n", len);
    if (len > max) {
        max = len;
        copy(longest, line);
    }
}
if (max > 0) /* there was a line */
    {
	printf("\n---------------------------\n");
	printf("Longest: %s", longest);
	printf("Length: %d\n", max);}
return 0;
}
/* getline2: read a line into s, return length */
int getline2(char s[],int lim)
{
int c, i;
for (i=0; i < lim-1 && (c=getchar())!=EOF && c!='\n'; ++i)
    s[i] = c;
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
