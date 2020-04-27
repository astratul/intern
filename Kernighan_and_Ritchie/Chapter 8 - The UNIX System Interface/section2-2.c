// 8.2 Low Level I/O - Read and Write

// #include "syscalls.h"
#include <sys/syscall.h>
#include <unistd.h>
#include<stdio.h>

int getchar2(void);
int getchar1(void);
#define BUFSIZ 1024

int main(){
    char c;
    while ((c = getchar1()) != 'x')
        putchar(c);
    while ((c = getchar2()) != 'x')
        putchar(c);
    return 0;
}
/* getchar1: unbuffered single character input */
int getchar1(void){
    char c;
    return (read(0, &c, 1) == 1) ? (unsigned char)c : EOF;
}

/* getchar2: simple buffered version */
int getchar2(void){
    static char buf[BUFSIZ];
    static char *bufp = buf;
    static int n = 0;
    if (n == 0){
        n = read(0, buf, sizeof buf);
        bufp = buf;
    }
    return (--n >= 0) ? (unsigned char)*bufp++ : EOF;
}
