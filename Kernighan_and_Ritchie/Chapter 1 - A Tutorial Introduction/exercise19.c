#include <stdio.h>
#define MAXLINE 1000 /* maximum input line length */

int getline2(char line[],int lim);
void reverse(char rline[]);

/*

Exercise 1-19. Write a function reverse(s) that reverses the character string s . Use it to
write a program that reverses its input a line at a time.

*/

int main()
{	int len;
	char line[MAXLINE];
	while ((len = getline2(line, MAXLINE)) > 0)
	{
		reverse(line);
		printf("Reverse: %s\n",line);
	}
	return 0;
}

int getline2(char s[],int lim)
{	int c, i;
	for (i=0; i < lim-1 && (c=getchar())!=EOF && c!='\n'; ++i)
   		s[i] = c;
	if (c == '\n') {
    	s[i] = c;
   		++i;
   	}
    s[i] = '\0';
    return i;
}

void reverse(char rline[])
{
  int i,j;
  char temp;
  j = 0;
  int p = strlen(rline);
  if(rline[p-1]=='\n')
  	p=p-2;
  while(j < p)
  {
    temp = rline[j];
    rline[j] = rline[p];
    rline[p] = temp;
    --p;
    ++j;
  }
}