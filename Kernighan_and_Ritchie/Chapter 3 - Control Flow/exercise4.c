/*
Exercise 3-4. In a two's complement number representation, our version of itoa does not
handle the largest negative number, that is, the value of n equal to -(2 wordsize-1 ). Explain why
not. Modify it to print that value correctly, regardless of the machine on which it runs.
*/

#include <stdio.h>
#include <string.h>
#define MAXLINE 1000
#define abs(x) ( (x) > 0 ? (x): -(x))

void itoa_old(int n, char s[]);
void itoa(int n,char s[]);
void reverse(char s[]);

/* 
itoa: convert n to characters in s 
The new itoa can handle the situation of -2(2^(wordsize-1)) = -2147483648 ; In the old itoa would fail at -2345645.
* sign is stored as  the number itself
* absolute value of each digit is stored in the string and while loop is tested not for 0
*/

int main(void)
{
    int number, number2;
    char str1[MAXLINE], str2[MAXLINE];
    number = -2147483648;
    number2 = -2345645;

    printf("\nInteger: %d printed as\nString :\n",number);
    itoa(number,str1);
    printf("\t* with the new function: %s.\n",str1);
    itoa_old(number,str2);
    printf("\t* with the old function: %s.\n\n",str2);

    printf("\nInteger: %d printed as\nString :\n",number2);
    itoa(number2,str1);
    printf("\t* with the new function: %s.\n",str1);
    itoa_old(number2,str2);
    printf("\t* with the old function: %s.\n\n",str2);
    return 0;
}

void itoa(int n,char s[])
{
    int i,sign;
    sign=n; 
    i = 0;
    do
    {
        s[i++]= abs(n%10) + '0';
    }while((n /= 10) != 0);
    if( sign < 0)
        s[i++]='-';
    s[i]='\0';
    reverse(s);
}

/* itoa: convert n to characters in s */
void itoa_old(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0) /* record sign */
        n = -n;
    /* make n positive */
    i = 0;
    do {
        /* generate digits in reverse order */
        s[i++] = n % 10 + '0'; /* get next digit */
    } while ((n /= 10) > 0);
    /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

void reverse(char s[])
{
    int c,i,j;

    for(i=0,j=strlen(s)-1;i<j;i++,j--)
    {    
        c=s[i];
        s[i]=s[j];
        s[j]=c;
    }
}