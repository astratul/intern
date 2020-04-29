/*
Exercise 8-5. Modify the fsize program to print the other information contained in the inode
entry.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define NAME_MAX 14

typedef struct {
    long ino;
    char name[NAME_MAX + 1];
} Dirent;

typedef struct {
    int fd;
    Dirent d;
} MYDIR;

MYDIR *myopendir(char *dirname);

Dirent *myreaddir(MYDIR *dfd);

void myclosedir(MYDIR *dfd);


void fsize(char *);
void dirwalk(char *, void (*fcn)(char *));

// struct stat /* inode information returned by stat */
// {
//     dev_t st_dev;    /* device of inode */
//     ino_t st_ino;    /* inode number */
//     short st_mode;   /* mode bits */
//     short st_nlink;  /* number of links to file */
//     short st_uid;    /* owners user id */
//     short st_gid;    /* owners group id */
//     dev_t st_rdev;   /* for special files */
//     off_t st_size;   /* file size in characters */
//     time_t st_atime; /* time last accessed */
//     time_t st_mtime; /* time last modified */
//     time_t st_ctime; /* time originally created */
// };


/*fsize: print size of file "name" */
void fsize(char *name) {
    struct stat stbuf;

    if (stat(name, &stbuf) == -1) {
        fprintf(stderr, "fsize: can't access %s\n", name);
        return;
    }

    if ((stbuf.st_mode & S_IFMT) == S_IFDIR)
        dirwalk(name, fsize);
    // printf("%8ld %s\n", stbuf.st_size, name);
    printf("%8ld, %8ld, %8ld, %8ld, %8ld, %8ld, %s\n", stbuf.st_size, stbuf.st_blocks, stbuf.st_blksize, stbuf.st_nlink, stbuf.st_dev, stbuf.st_nlink, name);

}

#define MAX_PATH 1024

void dirwalk(char *dir, void (*fcn)(char *)) {
    char name[MAX_PATH];
    Dirent *dp;
    MYDIR *dfd;

    if ((dfd = myopendir(dir)) == NULL) {
        fprintf(stderr, "dirwalk: cant open %s\n", dir);
        return;
    }

    while((dp = myreaddir(dfd)) != NULL) {
        if (strcmp(dp->name, ".") == 0 || strcmp(dp->name, "..") == 0)
            continue;
        if (strlen(dir) + strlen(dp->name) + 2 > sizeof(name))
            fprintf(stderr, "dirwalk: name %s/%s too long\n", dir, dp->name);
        else {
            sprintf(name, "%s/%s", dir, dp->name);
            (*fcn)(name);
        }

    }
    myclosedir(dfd);
}

#ifndef DIRSIZ
#define DIRSIZE 14
#endif

struct direct {  /* directory entry */
    ino_t d_ino;
    char d_name[DIRSIZE];

};


MYDIR *myopendir(char *dirname) {
    int fd;
    struct stat stbuf;
    MYDIR *dp;

    if ((fd = open(dirname, O_RDONLY, 0)) == -1 || fstat(fd, &stbuf) == -1 || (stbuf.st_mode & S_IFMT) != S_IFDIR || (dp = (MYDIR *) malloc(sizeof(MYDIR))) == NULL)
        return NULL;
    dp->fd = fd;
    return dp;

}

void myclosedir(MYDIR *dp) {
    if (dp) {
        close(dp->fd);
        free(dp);
    }
}

#include <sys/dir.h>


#define DIRSIZE 14


Dirent *myreaddir(MYDIR *dp) {
    struct direct dirbuf;
    static Dirent d;

    while (read(dp->fd, (char *) &dirbuf, sizeof(dirbuf)) == sizeof(dirbuf)) {
        if (dirbuf.d_ino == 0)
            continue;
        d.ino = dirbuf.d_ino;
        strncpy(d.name, dirbuf.d_name, DIRSIZE);
        d.name[DIRSIZE] = '\0';
        return &d;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc == 1)
        fsize(".");
    else
        while (--argc > 0)
            fsize(*++argv);
    return 0;
}