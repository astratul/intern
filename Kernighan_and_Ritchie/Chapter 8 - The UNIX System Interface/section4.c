// 8.4 Random Access - Lseek


#include <sys/syscall.h>
#define BUFSIZE 1024
char buf[BUFSIZE];

/* The system call lseek provides a way to move around in a file without reading or writing any data:

long lseek(int fd, long offset, int origin);
lseek(fd, 0L, 2);
*/

/*get: read n bytes from position pos */
int get(int fd, long pos, char *buf, int n)
{
if (lseek(fd, pos, 0) >= 0) /* get to pos */
return read(fd, buf, n);
else
return -1;
}

int main(){
    // FILE *fd;
    int *fd=0;
    printf("GET %d ", get(fd, 0L, buf, 3)); // random call without a file that returns -1
    return 0;
}
