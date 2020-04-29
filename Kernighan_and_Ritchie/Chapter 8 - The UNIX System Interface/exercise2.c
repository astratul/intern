/*
Exercise 8-2. Rewrite fopen and _fillbuf with fields instead of explicit bit operations.
Compare code size and execution speed.
*/

#include <fcntl.h>
#include <stdarg.h>
// The functions available from stdio.h are implemented here.
//#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <fcntl.h>

#ifdef NULL
#undef NULL
#endif

#define NULL 0
#define EOF (-1)
#define BUFSIZ 1024
#define OPEN_MAX 20 /* max # files open at once */

typedef struct _iobuf{
    int cnt;    /* characters left */
    char *ptr;  /* next character position */
    char *base; /* location of the buffer */
    int flag;   /* mode of the file access */
    int fd;     /* file descriptor */
} FILE;

extern FILE _iob[OPEN_MAX];

#define stdin (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])

enum _flags{
    _READ = 01,
    /* file open for reading */ /* binary 1 */
    _WRITE = 02,
    /* file open for writing */ /* binary 10 */
    _UNBUF = 03,
    /* file is unbuffered */ /* binary 11 */
    _EOF = 010,
    /* EOF has occurred on this file */ /* binary 1000 */
    _ERR = 020,
    /* error occurred on this file */ /* binary 10000*/
};

int _fillbuf(FILE *);

#define feof(p) (((p)->flag & _EOF) != 0)
#define ferror(p) (((p)->flag & _ERR) != 0)
#define fileno(p) ((p)->fd)

#define getc(p) (--(p)->cnt >= 0                  \
                     ? (unsigned char)*(p)->ptr++ \
                     : _fillbuf(p))

#define getchar() getc(stdin)

#define PERMS 0666 /* RW for owner, group and others */

FILE *fopen(char *name, char *mode){ /* fopen: open file, return file ptr */
    int fd;
    FILE *fp;

    if (*mode != 'r' && *mode != 'w' && *mode != 'a')
        return NULL;
    for (fp = _iob; fp < _iob + OPEN_MAX; fp++)
        if ((fp->flag & (_READ | _WRITE)) == 0)
            break; /* found free slot */
    if (fp >= _iob + OPEN_MAX) /* no free slots */
        return NULL;
    if (*mode == 'w')
        fd = creat(name, PERMS);
    else if (*mode == 'a'){
        if ((fd = open(name, O_WRONLY, 0)) == -1)
            fd = creat(name, PERMS);
        lseek(fd, 0L, 2);
    }else
        fd = open(name, O_RDONLY, 0);
    if (fd == -1) /* couldn't access name */
        return NULL;
    fp->fd = fd;
    fp->cnt = 0;
    fp->base = NULL;
    fp->flag = (*mode == 'r') ? _READ : _WRITE;
    return fp;
}

int _fillbuf(FILE *fp){ /* _fillbuf: allocate and fill input buffer */
    int bufsize;

    if ((fp->flag & (_READ | _EOF | _ERR)) != _READ)
        return EOF;
    bufsize = (fp->flag & _UNBUF) ? 1 : BUFSIZ;
    if (fp->base == NULL) /* no buffer yet */
        if ((fp->base = (char *)malloc(bufsize)) == NULL)
            return EOF; /* can't get buffer */
    fp->ptr = fp->base;
    fp->cnt = read(fp->fd, fp->ptr, bufsize);
    if (--fp->cnt < 0){
        if (fp->cnt == -1)
            fp->flag |= _EOF;
        else
            fp->flag |= _ERR;
        fp->cnt = 0;
        return EOF;
    }
    return (unsigned char)*fp->ptr++;
}

FILE _iob[OPEN_MAX] = {/* stdin, stdout, stderr */
                       {0, (char *)0, (char *)0, _READ, 0},
                       {0, (char *)0, (char *)0, _WRITE, 1},
                       {0, (char *)0, (char *)0, _WRITE | _UNBUF, 2}};

int main(int argc, char *argv[])
{
    int c;
    while ((c = getchar()) != 'x')
        ;
}