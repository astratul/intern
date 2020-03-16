#include <stdio.h>
#define MAXLINE 1000 /* maximum input line length */
int getline2(char line[], int maxline);
/* void copy(char to[], char from[]); */

/*

Exercise 1-17. Write a program to print all input lines that are longer than 80 characters.

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
    if (len > 80) {
	printf("\n---------------------------\n");
    	printf("Line longer than 80 characters: %s", line);
    	printf("Length: %d\n", len);
	printf("---------------------------\n");
    }
}
/* if (max > 0) 
    {
	printf("\n---------------------------\n");
	printf("Longest: %s", longest);
	printf("Length: %d\n", max);}
*/
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
/* 
void copy(char to[], char from[])
{
int i;

i = 0;
while ((to[i] = from[i]) != '\0')
    ++i;
}
*/
