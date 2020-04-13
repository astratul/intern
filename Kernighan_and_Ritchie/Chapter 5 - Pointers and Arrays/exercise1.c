/* 
Exercise 5-1. As written, getint treats a + or - not followed by a digit as a valid
representation of zero. Fix it to push such a character back on the input. 
*/

#include <stdio.h>
#include <ctype.h>
char buf[100];
int bufp = 0;

// from chapter 4
int getch(void)
{
    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c)
{
    if (bufp >= 100)
        printf("ungetch: too many characters\n");
    else
        buf[bufp++] = c;
}

int main(void)
{
    int n, s, v[1000], getint(int *);

    for (n = 0; n < 1000 && getint(&v[n]) != EOF; n++)
    {
        printf("Storing in n = %d, getint %d\n", n, v[n]);
    }
    printf("storing in n = %d, getint %d\n", n, v[n]);
    printf("\nThe values: ");
    for (s = 0; s <= n; s++)
        printf(" %d ", v[s]);
    printf("\n");
    return 0;
}

/* the function from page 87 */
int getint(int *pn)
{
    int c, sign;
    while (isspace(c = getch()))
        /* skip white space */
        ;
    if (!isdigit(c) && c != EOF && c != '+' && c != '-')
    {
        ungetch(c); /* it's not a number */
        return 0;
    }
    sign = (c == '-') ? -1 : 1;
    if (c == '+' || c == '-')
        c = getch();
    for (*pn = 0; isdigit(c); c = getch())
        *pn = 10 * *pn + (c - '0');
    *pn *= sign;
    if (c != EOF)
        ungetch(c);
    return c;
}