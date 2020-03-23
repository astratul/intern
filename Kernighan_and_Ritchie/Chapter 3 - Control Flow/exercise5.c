/* Exercise 3-5. Write the function itob(n,s,b) that converts the integer n into a base b character representation in the string s . In particular, itob(n,s,16) formats s as a hexadecimal integer in s . */
#include<stdio.h>
#include<string.h>
#define MAXLINE 100
void itob(int n,char s[],int b);
void reverse(char s[]);

int main(void)
{
    int number;
    char str[MAXLINE];
    number=12998;
    itob(number,str,16);
    printf("The number %d in base 16 is: %s.\n",number,str);
    return 0;
}

void itob(int n,char s[],int b)
{
    int i,j,sign;
    if((sign=n) < 0)
        n = -n;
    i = 0;
    do
    {
        j = n % b;
        if(j<=9)
             s[i++] = j+'0';
        else // for 10-15 / a-f
            s[i++] = j+'a'-10;
    }while((n /= b) > 0);

    if(sign < 0)
        s[i++]='-';
    
    s[i]='\0';
    reverse(s);
}

void reverse(char s[])
{
    int i,j,c;
    for(i=0,j=strlen(s)-1;i<j;i++,j--)
    {
        c=s[i];
        s[i]=s[j];
        s[j]=c;
    }
}