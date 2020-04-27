// 8.2 Low Level I/O - Read and Write

#include <sys/syscall.h>
#include <unistd.h>
#define BUFSIZ 1024

// int n_read = read(int fd, char *buf, int n);
// int n_written = write(int fd, char *buf, int n);

int main() /* copy input to output */
{
    char buf[BUFSIZ];
    int n;

    while ((n = read(0, buf, BUFSIZ)) > 0)
        write(1, buf, n);
    return 0;
}
