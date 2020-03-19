/* 
Exercise 2-5. Write the function any(s1,s2) , which returns the first location in a string s1
where any character from the string s2 occurs, or -1 if s1 contains no characters from s2 .
(The standard library function strpbrk does the same job but returns a pointer to the
location.)
*/

#include<stdio.h>
#define MAXLINE 1000

int getline2(char line[],int maxline);
int any(char s1[],char s2[]);

int main(void)
{
    char s1[MAXLINE],s2[MAXLINE];
    putchar('s');
    putchar('1');
    putchar(' ');
    getline2(s1,MAXLINE);
    putchar('s');
    putchar('2');
    putchar(' ');
    getline2(s2,MAXLINE);
    printf("The first location in s1 where any character from the string s2 occurs is: %d :) \n",any(s1,s2));
    return 0;
}

int getline2(char s[],int lim)
{
    int i,c;
    for(i=0;i<lim-1 && (c=getchar())!=EOF && c!='\n';++i)
        s[i]=c;

    if(c=='\n')
        s[i++]=c;
    s[i]='\0';
}

int any(char s1[],char s2[])
{
    int i,j,check;
    check=1;
    for(i=0;s1[i]!='\0'&& (check);) 
    {
        for(j=0;s2[j]!='\0'&& (s1[i]!=s2[j]);++j) 
            ; 
        if(s2[j]=='\0') {
            check=1;
            i++;
        }
        else
            check=0;
    }

    if(s1[i]=='\0')
        return -1;
    else
        return i;
}
