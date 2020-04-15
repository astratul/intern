// 5.11 Pointers to Functions

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINES 5000
#define MAXLEN 1000
char *lineptr[MAXLINES];
/* max #lines to be sorted */
/* pointers to text lines */
int getline2(char s[],int lim);
int readlines(char *lineptr[], int nlines);
void writelines(char *lineptr[], int nlines);
void qsort(void *lineptr[], int left, int right, int (*comp)(void *, void *));
int numcmp(char *, char *);
void swap(void *v[], int, int);
/* sort input lines */
int main(int argc, char *argv[])
{
    int nlines;
    /* number of input lines read */
    int numeric = 0;
    /* 1 if numeric sort */
    if (argc > 1 && strcmp(argv[1], "-n") == 0)
        numeric = 1;
    if ((nlines = readlines(lineptr, MAXLINES)) >= 0)
    {
        qsort((void **)lineptr, 0, nlines - 1,
              (int (*)(void *, void *))(numeric ? numcmp : strcmp));
        writelines(lineptr, nlines);
        return 0;
    }
    else
    {
        printf("input too big to sort\n");
        return 1;
    }
}
/*qsort : sort v[left]... v[right] into increasing order */
// int (*comp)(void *, void *) - comp is a pointer to a function that has two void * arguments and returns an int
void qsort(void *v[], int left, int right, int (*comp)(void *, void *))
{
    int i, last;
    void swap(void *v[], int, int);

    if (left >= right)
        /* do nothing if array contains */
        return;
    /* fewer than two elements */
    swap(v, left, (left + right) / 2);
    last = left;
    for (i = left + 1; i <= right; i++)
        if ((*comp)(v[i], v[left]) < 0)
            swap(v, ++last, i);
    swap(v, left, last);
    qsort(v, left, last - 1, comp);
    qsort(v, last + 1, right, comp);
}

/* numcmp: compare s1 and s2 numerically */

int numcmp(char *s1, char *s2)
{
    double v1, v2;

    v1 = atof(s1);
    v2 = atof(s2);
    if (v1 < v2)
        return -1;
    else if (v1 > v2)
        return 1;
    else
        return 0;
}

/*
The swap function, which exchanges two pointers, is identical to what we presented earlier in
the chapter, except that the declarations are changed to void *
*/

void swap(void *v[],
{
    void *temp;
    int i, int j;)
temp = v[i];
    v[i] = v[j];
    v[j] = temp;
}


/* writelines: write output lines */
void writelines(char *lineptr[],int nlines,int decr)
{
    int i;
    if(decr)    /* print in decreasing order */
    for(i=nlines-1;i>=0;i--)
        printf("%s\n",lineptr[i]);
    else
        for(i=0;i<nlines;i++)
        printf("%s\n",lineptr[i]);
}


/* readlines: read input lines */
int readlines(char *lineptr[],int maxlines)
{
    int len,nlines;
    char *p,line[MAXLEN];
    nlines = 0;
    while((len = getline2(line,MAXLEN)) > 0)
        if(nlines >= maxlines || (p = alloc(len)) == NULL)
            return -1;
        else
        {
            line[len-1] = '\0'; /* delete newline */
            strcpy(p,line);
            lineptr[nlines++]=p;
        }
    return nlines;
}
int getline2(char s[],int lim)
{
    int c,i;

    for(i=0;i<lim-1 && (c=getchar())!= EOF && c!='\n';++i)
        s[i] = c;
    if( c == '\n')
    {
        s[i] = c;
        ++i;
    }

    s[i] = '\0';
    return i;
}
