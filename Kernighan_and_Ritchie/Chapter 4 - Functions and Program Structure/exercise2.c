/* Exercise 4-2. Extend atof to handle scientific notation of the form 123.45e-6 where a floating-point number may be followed by e or E and an optionally signed exponent.*/

#include <stdio.h>
#include <ctype.h>
#define MAXLINE 100
/* atof: convert string s to double */

double atof(char s[]);
int getline2(char s[], int lim);
int power(int base,int exp);  

int main() {
    double sum, atof(char []);
    char line[MAXLINE];
    int getline2(char line[], int max);

    sum = 0;
    while (getline2(line, MAXLINE) > 0)
    printf("%g\t%g\n",atof(line), sum += atof(line));
    return 0;
}

double atof(char s[])
{
    double val,pow;
    int sign,i,esign,exp;     
        
    for(i=0;isspace(s[i]);i++) /* skip white space */
        ;

    sign=(s[i]=='-')?-1:1;

    if(s[i]=='+' || s[i] == '-')
        i++;

    for(val=0.0;isdigit(s[i]);i++)
        val = 10.0 * val + (s[i] - '0');
    
    if(s[i]=='.')
        i++;

    for(pow=1.0;isdigit(s[i]);i++)
    {
        val = 10.0 * val + (s[i] - '0');
        pow *= 10.0;
    }

    if(s[i]=='e' || s[i] =='E')
        i++;
    if(s[i]=='+' || s[i] =='-')
    {
        esign=s[i];
        i++;
    }

    for(exp=0;isdigit(s[i]);i++)
        exp=10.0 * exp + (s[i] - '0');
    
    if( esign == '-')
        return sign * (val / pow) / power(10,exp);
    else
    
        return sign * (val / pow) * power(10,exp);
}

/* getline: get line into s, return length */
int getline2(char s[], int lim)
{
    int c, i;
    i = 0;
    printf("Insert a string: ");
    while (--lim > 0 && (c=getchar()) != EOF && c != '\n')
        s[i++] = c;
    if (c == '\n')
        s[i++] = c;
    s[i] = '\0';
    return i;
}
int power(int base,int exp)
{
    int power;
    power = 1;
    while(exp-- > 0)
        power *=base;

    return power;
}
