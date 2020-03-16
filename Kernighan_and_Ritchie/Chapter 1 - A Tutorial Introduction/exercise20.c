/*

Exercise 1-20. Write a program detab that replaces
 tabs in the input with the proper number
of blanks to space to the next tab stop. Assume a 
fixed set of tab stops, say every n columns.
Should n be a variable or a symbolic parameter?

*/

#include<stdio.h>
#define TAB 8

int main(void)
{
  int nb,pos,c;
  nb = 0;
  pos = 1;
  int t = 0;
  while((c=getchar())!=EOF)
  { 
    if( c == '\t')
    {
      t++;
      nb = TAB - (( pos - 1) % TAB);

      while( nb > 0)
      {
        // putchar(t);
        putchar('#');
        ++pos;
        --nb;
      }
    }
    else if( c == '\n')
    {
      putchar(c);
      pos = 1;
    }
    else
    {
      putchar(c);
      ++pos;
    }
  }
  return 0;
}