/* 
Exercise 5-14. Modify the sort program to handle a -r flag, which indicates
sorting in reverse (decreasing) order. Be sure that -r works with -n .
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUMERIC 1      /* numeric sort */
#define DECR 2         /* sorts in decreasing order */
#define LINES 100      /* max # of lines to be sorted */
#define ALLOCSIZE 1000 /* size of available space */
#define MAXLEN 1000    /* max length of any input line */

int getline2(char *, int);
char *alloc(int);
static char allocbuf[ALLOCSIZE]; /* storage for alloc */
static char *allocp = allocbuf;  /* next free position */
int numcmp(char *, char *);
int readlines(char *lineptr[], int maxlines);
void qsort2(void *v[], int left, int right, int (*comp)(void *, void *));
void writelines(char *lineptr[], int nlines, int decr);
static char option = 0;

/* sort input lines */

int main(int argc, char *argv[]){
    char *lineptr[LINES];
    int nlines;    /* pointer to text lines */
    int c, rc = 0; /* number of input lines read */

    while (--argc > 0 && (*++argv)[0] == '-')
        while (c = *++argv[0])
            switch (c){
            case 'n': /* numeric sort */
                option |= NUMERIC;
                break;
            case 'r': /* sort in decreasing order */
                option |= DECR;
                break;
            default:
                printf("sort:illegal option %c\n", c);
                argc = 1;
                rc = -1;
                break;
            }
    if (argc)
        printf("Usage: sort -nr \n");
    else if ((nlines = readlines(lineptr, LINES)) > 0){
        if (option & NUMERIC)
            qsort2((void **)lineptr, 0, nlines - 1, (int (*)(void *, void *))numcmp);
        else
            qsort2((void **)lineptr, 0, nlines - 1, (int (*)(void *, void *))numcmp);
        writelines(lineptr, nlines, option & DECR);
    }
    else{
        printf("input too big to sort \n");
        rc = -1;
    }
    return rc;
}

void writelines(char *lineptr[], int nlines, int decr){
    int i;
    if (decr) /* print in decreasing order */
        for (i = nlines - 1; i >= 0; i--)
            printf("%s\n", lineptr[i]);
    else
        for (i = 0; i < nlines; i++)
            printf("%s\n", lineptr[i]);
}

int numcmp(char *s1, char *s2){
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

int readlines(char *lineptr[], int maxlines){
    int len, nlines;
    char *p, line[MAXLEN];
    nlines = 0;
    while ((len = getline2(line, MAXLEN)) > 0)
        if (nlines >= maxlines || (p = alloc(len)) == NULL)
            return -1;
        else{
            line[len - 1] = '\0'; /* delete newline */
            strcpy(p, line);
            lineptr[nlines++] = p;
        }
    return nlines;
}

char *alloc(int n){
    if (allocbuf + ALLOCSIZE - allocp >= n){
        allocp += n;
        return allocp - n;
    }
    else
        return 0;
}

void afree(char *p){
    if (p >= allocbuf && p < allocbuf + ALLOCSIZE)
        allocp = p;
}

void qsort2(void *v[], int left, int right, int (*comp)(void *, void *)){
    int i, last;
    void swap(void *v[], int, int);

    if (left >= right)
        return;

    swap(v, left, (left + right) / 2);

    last = left;

    for (i = left + 1; i <= right; i++)
        if ((*comp)(v[i], v[left]) < 0)
            swap(v, ++last, i);
    swap(v, left, last);

    qsort2(v, left, last - 1, comp);
    qsort2(v, last + 1, right, comp);
}

void swap(void *v[], int i, int j){
    void *temp;
    temp = v[i];
    v[i] = v[j];
    v[j] = temp;
}

int getline2(char s[], int lim){
    int c, i;
    for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
        s[i] = c;
    if (c == '\n'){
        s[i] = c;
        ++i;
    }
    s[i] = '\0';
    return i;
}
