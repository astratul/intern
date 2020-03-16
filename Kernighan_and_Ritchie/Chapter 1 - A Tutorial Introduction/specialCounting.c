/* 
Exercise 1-8. Write a program to count blanks, tabs, and newlines.

*/

#include <stdio.h>
/* count lines in input */
int main()
{
int c, nl, b, t;
nl = 0;
b = 0;
t = 0;
while ((c = getchar()) != EOF)
{
if (c == '\n')
	++nl;
if (c == ' ')
	++b;
if (c == '\t')
	++t;
}
printf("%d blanks.\n", b);
printf("%d tabs.\n", t);
printf("%d newlines.\n", nl);
return 0;
}
