// 7.5 File Access
// gcc -o section7-5 section7-5.c
// ./section7-5 file1.txt file2.txt

#include <stdio.h>
// FILE *fp;
// FILE *fopen(char *name, char *mode);
// fp = fopen(name, mode);
// int getc(FILE *fp) //getc returns the next char from the stream referred to by fp
// int putc(int c, FILE *fp)
// #define getchar() getc(stdin)
// #define putchar(c) putc((c), stdout)

// int fscanf(FILE *fp, char *format, ...);
// int fprintf(FILE *fp, char *format, ...);

/* cat:concatenate files, version 1 */
int main(int argc, char *argv[]){
    FILE *fp;
    void filecopy(FILE *, FILE *);
    if (argc == 1) /* no args; copy standard input */
        filecopy(stdin, stdout);
    else
        while (--argc > 0)
            if ((fp = fopen(*++argv, "r")) == NULL){
                printf("cat: can't open %s\n", *argv);
                return 1;
            }
            else{
                filecopy(fp, stdout);
                fclose(fp);
            }
    return 0;
}

/* filecopy: copy file ifp to file ofp */
void filecopy(FILE *ifp, FILE *ofp){
    int c;
    while ((c = getc(ifp)) != EOF)
        putc(c, ofp);
}