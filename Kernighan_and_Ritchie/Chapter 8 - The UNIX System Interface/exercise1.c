/*
Exercise 8-1. Rewrite the program cat from Chapter 7 using read , write , open , and close instead of their standard library equivalents. Perform experiments to determine the relative speeds of the two versions.
*/

#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define BUFSIZE 1024
char buf[BUFSIZE];

void error2(char *fmt, ...);
void filecopy(int ifd, int ofd);

int main(int argc, char *argv[]){
    int fd;
    if (argc == 1)
        filecopy(STDIN, STDOUT);
    else
        while (--argc > 0)
            if ((fd = open(*++argv, O_RDONLY)) == -1)
                error2("cat:can't open %s", *argv);
            else{
                filecopy(fd, STDOUT);
                close(fd);
            }
    return 0;
}

void error2(char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    fprintf((FILE *) STDERR, "error: ");
    vfprintf((FILE *) STDERR, fmt, args);
    fprintf((FILE *) STDERR, "\n");
    va_end(args);
    exit(1);
}

void filecopy(int ifd, int ofd){
    int n;
    while ((n = read(ifd, buf, BUFSIZE)) > 0)
        if (write(ofd, buf, n) != n)
            error2("cat: write error");
}
