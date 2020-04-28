// 8.3 Open, Creat, Close, Unlink

/*
open is rather like the fopen discussed in Chapter 7, except that instead of returning a file pointer, it returns a file descriptor, which is just an int . open returns -1 if any error occurs.
The system call creat is provided to create new files, or to re-write old ones.
*/

#include <stdio.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <stdarg.h>
#include <unistd.h>

#define PERMS 0666
#define BUFSIZ 1024

/* error: print an error message and die */
void error1(char *fmt, ...)
{
va_list args;

va_start(args, fmt);
fprintf(stderr, "error: ");
// vprintf(stderr, fmt, args);
fprintf(stderr, "\n");
va_end(args);
exit(1);

}
int main(int argc, char *argv[]){
    int f1, f2, n;
    char buf[BUFSIZ];

    if (argc != 3)
        error1("Usage: cp from to");
    if ((f1 = open(argv[1], O_RDONLY, 0)) == -1)
        error1("cp: can't open %s", argv[1]);
    if ((f2 = creat(argv[2], PERMS)) == -1)
        error1("cp: can't create %s, mode %03o",
              argv[2], PERMS);
    while ((n = read(f1, buf, BUFSIZ)) > 0)
        if (write(f2, buf, n) != n)
            error1("cp: write error on file %s", argv[2]);
    return 0;
}