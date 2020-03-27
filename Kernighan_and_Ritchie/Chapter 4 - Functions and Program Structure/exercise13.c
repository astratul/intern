/* 
Exercise 4-13. Write a recursive version of the function reverse(s) , which reverses the string s in place.
*/
#include<stdio.h>
#include<string.h>

#define MAXLINE 100

int getline2(char line[],int maxline);
void reverse(char s[]);

int main(void)
{
    char s[MAXLINE];
    getline2(s,MAXLINE);
    reverse(s);
    printf("%s\n",s);
    return 0;
}

int getline2(char s[],int lim)
{
    int i,c;

    for(i=0;i<lim-1 && (c=getchar())!=EOF && c!='\n';++i)
        s[i] = c;
    if(c=='\n')
        s[i++]='\n';
    s[i]='\0';
}

static int i = 0; 

void reverse(char s[])
{
    int c,j;
    j = strlen(s) - (i + 1);
    if( i < j )
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
        ++i;
        reverse(s);
    }

}