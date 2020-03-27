/*
Exercise 4-9. Our getch and ungetch do not handle a pushed-back EOF correctly. Decide what their properties ought to be if an EOF is pushed back, then implement your design.
*/

#include<stdio.h>
#define BUFSIZE 100

int getch(void);
void ungetch(int c);

int buf[BUFSIZE]; /* ungetch */
int bufp = 0;     /* next free position in buf */

int main(void)
{
    int c;
    c = '>';
    ungetch(c);
    while((c=getch())!=EOF)
        putchar(c);
    return 0;
}

int getch(void)
{
    if(bufp > 0)
        return buf[--bufp];
    else
        return getchar();
}

void ungetch(int c)
{
    if (bufp >= BUFSIZE)
        printf("ungetch: too many characters \n");
    else
        buf[bufp++] = c;
}
    
