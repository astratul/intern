
/*
Exercise 5-3. Write a pointer version of the function strcat that we showed in Chapter 2:
strcat(s,t) copies the string t to the end of s .
*/

#include<stdio.h>
#define MAXLINE 1000

int getline2(char s[],int lim)
{
    int c,i;
    
    for(i=0;i<lim-1 && (c=getchar()) !=EOF && c!='\n';++i)
        s[i] = c;
    if(c == '\n'){
        s[i] = c;
        ++i;
    }
    s[i] = '\0';
    return i;
}

void strcat2(char *s,char *t)
{
    while(*s!='\0')
        s++;
    s--; //until \0
    while ((*s++ = *t++) != '\0')
        ;
}

int main()
{
    int len;
    char s[MAXLINE],t[MAXLINE];
    printf("Insert the first string, s: ");
    getline2(s,MAXLINE);
    printf("Insert the second string, t: ");
    getline2(t,MAXLINE);
    strcat2(s,t);
    printf("The final string with t at the end of s : %s",s);
    return 0;
}
