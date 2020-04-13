/* 
Exercise 5-5. Write versions of the library functions strncpy , strncat , and strncmp , which
operate on at most the first n characters of their argument strings. For example,
strncpy(s,t,n) copies at most n characters of t to s . Full descriptions are in Appendix B.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXSIZE 1000
#define MAXLINE 1000

int getline2(char s[], int lim)
{
    int c, i;

    for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
        s[i] = c;
    if (c == '\n')
    {
        s[i] = c;
        ++i;
    }
    s[i] = '\0';
    return i;
}

int strnlen2(char *s)
{
    char *p = s;
    while (*s != '\0')
    {
        s = s + 1;
    }
    return s - p;
}

void strncpy2(char *dest, char *source, int n)
{
    while (*source && n-- > 0)
        *dest++ = *source++;
    int extra = strnlen2(dest) - n;
    while (extra-- > 0)
    {
        *dest++;
    }
    *dest = '\0';
}
void strncat2(char *str1, char *str2, char *dest, int n)
{
    while (*str1)
    {
        *dest++ = *str1++;
    }
    while (n-- > 0)
    {
        *dest++ = *str2++;
    }
    *dest = '\0';
}

int strncmp2(char *lhs, char *rhs, int n)
{
    for (; *lhs == *rhs; lhs++, rhs++)
        if (*lhs == '\0' || --n <= 0)
            return 0;
    return *lhs - *rhs;
}

int main()
{

    /*
    strncpy copies n characters of source string to destination. It does this by copying or overwriting one character a time from source to destination and keeps track of count n.
    */
    char dest[MAXSIZE];
    char source[MAXSIZE];
    getline2(dest, MAXLINE);
    getline2(source, MAXLINE);
    strncpy2(dest, source, 3);
    printf("Strncpy: %s\n", dest);

    char s1[MAXSIZE];
    char t1[MAXSIZE];
    getline2(s1, MAXLINE);
    getline2(t1, MAXLINE);
    char *d;
    if ((d = (char *)malloc(sizeof(char) * (strlen(s1) + +4 + 1))) == NULL)
    {
        printf("unable to allocate memory \n");
        return -1;
    }
    strncat2(s1, t1, d, 4);
    printf("Strncat: %s\n", d);
    free(d);
    /*
    strncat, takes three arguments, str1, str2 and dest. It concatenates n characters from str2 to str1 into a new string dest. It does this by copying all characters from str1 to dest and then keeps a track of count n, and copies n characters of str2 to dest. After copying n characters, it closes the dest string by 0 character.
    */

    char s2[MAXSIZE];
    char t2[MAXSIZE];
    getline2(s2, MAXLINE);
    getline2(t2, MAXLINE);
    int result;
    result = strncmp2(s2, t2, 3);
    printf("Strcmp: %d\n", result);
    /*
    strncmp, compares the lhs string with rhs string. It compares one character at a time and as long as both characters are same, it keeps going and if the lhs is exhaused before n characters are compared, it means we still satisfy the criteria and we return 0. Otherwise, it returns the difference between lhs character and rhs character, which will be 0 if they are equal, negative if lhs is smaller than rhs or positive value if lhs is greater than rhs.
    */
    return 0;
}
