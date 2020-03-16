#include <stdio.h>
#define MAXLINE 1000 /* maximum input line length */

int getline2(char line[],int lim);
int removetrail(char rline[]);

/*

Exercise 1-18. Write a program to remove trailing blanks and tabs from each line of input,
and to delete entirely blank lines.

*/

int main()
{	int len;
	char line[MAXLINE];
	while ((len = getline2(line, MAXLINE)) > 0)
		if(removetrail(line) > 0)
			printf("%s",line);
	return 0;
}

int getline2(char s[],int lim)
{	int c, i;
	for (i=0; i < lim-1 && (c=getchar())!=EOF && c!='\n'; ++i)
   		{
		   while(c == ' ') {
			   c=getchar();
		   	}
		   while(c == '\t') {
    			c=getchar();
   			}
			if (c != '\t' || c != ' ') 
		   		s[i] = c;
		}
	if (c == '\n') {
    	s[i] = c;
   		++i;
   	}
    s[i] = '\0';
    return i;
}

int removetrail(char s[])
{	int i;
	for(i=0; s[i]!='\n'; ++i)
		;
	--i;  /* To consider raw line without \n */
	for(i >0; ((s[i] == ' ') || (s[i] =='\t'));--i)   
    		; /* Removing the Trailing Blanks and Tab Spaces */
	if( i >= 0) /* Non Empty Line */
  	{	++i;
		s[i] = '\n';
		++i;
		s[i] = '\0';
  	}
  	return i;}
