/*
Exercise 4-1. Write the function strindex(s,t) which returns the position of the rightmost
occurrence of t in s , or -1 if there is none.
*/
#include <stdio.h>
#define MAXLINE 1000 /* maximum input line length */
int getline2(char line[], int max);
int strindex2(char source[], char searchfor[]);
char pattern[] = "buna"; /* pattern to search for */
/* find all lines matching pattern */
int main()
{
    char line[MAXLINE] = "heibunamanumescAdrianabuna";
    int found;
    // while (getline2(line, MAXLINE) > 0)
    //     if ((f = strindex2(line, pattern)) > found) {
    //         // printf("This line contains the pattern %s : %s",pattern,line);
    //         found = f;
    //     }
    //     printf("The rightmost occurrence of %s is: %d\n",pattern,found);
    found = strindex2(line, pattern);
    printf("The rightmost occurrence of %s is: %d\n",pattern,found);
    return 0;
}

/* getline: get line into s, return length */
int getline2(char s[], int lim)
{
    int c, i;
    i = 0;
    while (--lim > 0 && (c=getchar()) != EOF && c != '\n')
        s[i++] = c;
    if (c == '\n')
        s[i++] = c;
    s[i] = '\0';
    return i;
}

/* strindex: return index of t in s, -1 if none */
int strindex2(char s[], char t[])
{
    int i, j, k;
    int result = -1;
    for (i = 0; s[i] != '\0'; i++) {
        for (j=i, k=0; t[k]!='\0' && s[j]==t[k]; j++, k++)
            ;
        if (k > 0 && t[k] == '\0')
            result = i;
    }
    return result;
}