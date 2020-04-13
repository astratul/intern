/*Exercise 5-2. Write getfloat , the floating-point analog of getint . What type does
getfloat return as its function value? */

#include <stdio.h>
#include <ctype.h>

char buf[100];
int bufp = 0;

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

int getfloat(float *); //return int

int main(void)
{
    int n, j;
    float v[1000];

    for (n = 0; n < 1000 && getfloat(&v[n]) != EOF; n++)
        ;
    printf("\nThe values: ");
    for (j = 0; j < n; j++)
        printf("%f ", v[j]);
    printf("\n");
    return 0;
}

int getfloat(float *pn)
{
    int c, sign;
    float power;

    while (isspace(c = getch()))
        /* skip white space */
        ;
    if (!isdigit(c) && c != EOF && c != '+' && c != '-' && c != '.')
    {
        ungetch(c); /* it's not a number */
        return 0;
    }
    sign = (c == '-') ? -1 : 1;
    if (c == '+' || c == '-')
        c = getch();
    for (*pn = 0.0; isdigit(c); c = getch())
        *pn = 10.0 * *pn + (c - '0');
    if (c == '.') // decimals
        c = getch();
    for (power = 1; isdigit(c); c = getch())
    {
        *pn = 10 * *pn + (c - '0'); // fractional part
        power *= 10;
    }
    *pn *= sign / power;
    if (c != EOF)
        ungetch(c);

    return c;
}
