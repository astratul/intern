// 7.7 Line Input and Output
#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>

#define MAXLINE 1000
/* fgets: get at most n chars from iop */
char *fgets(char *s, int n, FILE *iop){
    register int c;
    register char *cs;
    cs = s;
    while (--n > 0 && (c = getc(iop)) != EOF)
        if ((*cs++ = c) == '\n')
            break;
    *cs = '\0';
    return (c == EOF && cs == s) ? NULL : s;
}
/* fputs: put string s on file iop 
For output, the function fputs writes a string (which need not contain a newline) to a file:
int fputs(char *line, FILE *fp)
It returns EOF if an error occurs, and non-negative otherwise.*/

int fputs2(char *s, FILE *iop){
    int c;
    while (c = *s++)
        putc(c, iop);
    return ferror(iop) ? EOF : 0;
}

/* getline: read a line, return length */
int getline2(char *line, int max){
    if (fgets(line, max, stdin) == NULL)
        return 0;
    else
        return strlen(line);
}

int main(int argc, char *argv[])
{
    char line[MAXLINE];
    FILE *fp;
    fp = fopen(argv[1], "r");
    int found=0;
    while((getline2(line,MAXLINE)) > 0)
        if(fgets(line,MAXLINE, fp) >= 0)
            printf("%s\n",line);
    // fputs2("hello", fp);
    return 0;

}