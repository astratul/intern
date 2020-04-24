/*
Exercise 7-2. Write a program that will print arbitrary input in a sensible way. As a minimum, it should print non-graphic characters in octal or hexadecimal according to local custom, and break long text lines.
*/

#include <stdio.h>
#include <ctype.h>

#define MAXLINE 100 /* maximum number of chars in one line */
#define OCT_LEN 6    /* length of an octal value */

int main(){
    int c, pos;
    char *w;
    int inc(int pos, int n);
    pos = 0;
    while ((c = getchar()) != EOF)
        if (iscntrl(c) || c == ' '){
            if (pos + OCT_LEN < MAXLINE)
                pos = pos + OCT_LEN;
            else
                pos = OCT_LEN;
            // putchar(c);
            if (c == '\n')
                w = "New line";
            else if (c =='\t')
                w = "Tab";
            else if (c == ' ')
                w ="Space";
            printf("\t'%s' x This non-graphic character is in  hexadecimal Ox%x and in octal 0%o.\n", w, c, c);
            if (c == '\n')
                pos = 0;
            putchar('\n');
        }
        else{
            if (pos + 1 < MAXLINE)
                pos = pos + 1;
            else
                pos = 1;
            // putchar(c);
            }
    return 0;
}