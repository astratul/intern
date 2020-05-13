# Summary - Working with Files

## Linux File Structure

### Directories

```
ln –i # You can see the inode number for a file 
```
* With the ln command you can make links to the same file in different directories.
* The standard library functions unfortunately do not understand the shell’s tilde shorthand notation in filename parameters, so you must always use the real name of the file in your programs.

### Files and Devices

#### /dev/console
* The system console (Error messages and diagnostics are often sent to this device.)
* Each UNIX system has a designated terminal or screen to receive console messages. At one time, it might have been a dedicated printing terminal.

#### /dev/tty
* The special file /dev/tty is an alias (logical device) for the controlling terminal (keyboard and screen, or window) of a process, if it has one.
* For instance, processes and scripts run automatically by the system won’t have a controlling terminal, and therefore won’t be able to open /dev/tty.

#### /dev/null

* The /dev/null file is the null device. All output written to this device is discarded. 
* An immediate end of file is returned when the device is read, and it can be used as a source of empty files by using the cp command. Unwanted output is often redirected to /dev/null.

```
$ echo do not want to see this >/dev/null
$ cp /dev/null empty_file # create an empty file
```

#### /dev/zero

* Acts as a source of null bytes to create files full of zeros

## System Calls and Device Drivers


You can access and control files and devices using a small number of functions. These functions, known as system calls, are provided by UNIX (and Linux) directly, and are the interface to the operating system itself.

To provide a similar interface, device drivers encapsulate all of the hardware-dependent features. Idiosyncratic features of the hardware are usually available through the ioctl (for I/O control) system call.

The low-level functions used to access the device drivers, the system calls, include:
* open : Open a file or device
* read : Read from an open file or device
* write : Write to a file or device
* close : Close the file or device
* ioctl : Pass control information to a device driver

The ioctl system call is used to provide some necessary hardware-specific control (as opposed to regular input and output), so its use varies from device to device. For example, a call to ioctl can be used to rewind a tape drive or set the flow control characteristics of a serial port. For this reason, ioctl isn’t necessarily portable from machine to machine. In addition, each driver defines its own set of ioctl commands.

## Library Functions

* There’s a performance penalty in making a system call. System calls are therefore expensive compared to function calls because Linux has to switch from running your program code to executing its own kernel code and back again. It’s a good idea to keep the number of system calls used in a program to a minimum and get each call to do as much work as possible, for example, by reading and writing large amounts of data rather than a single character at a time.
* The hardware has limitations that can impose restrictions on the size of data blocks that can be read or written by the low-level system call at any one time. For example, tape drives often have a block size, say 10k, to which they can write. So, if you attempt to write an amount that is not an exact multiple of 10k, the drive will still advance the tape to the next 10k block, leaving gaps on the tape.

## Low-Level File Access

Each running program, called a process, has a number of file descriptors associated with it. These are small integers that you can use to access open files or devices. How many of these are available will vary depending on how the system has been configured. When a program starts, it usually has three of these descriptors already opened. These are:
- 0: Standard input
- 1: Standard output
- 2: Standard error

### write

```
#include <unistd.h>
size_t write(int fildes, const void *buf, size_t nbytes);
```

The write system call arranges for the first nbytes bytes from buf to be written to the file associated with the file descriptor fildes . It returns the number of bytes actually written. This may be less than nbytes if there has been an error in the file descriptor or if the underlying device driver is sensitive to block size. If the function returns 0, it means no data was written; if it returns –1, there has been an error in the write call, and the error will be specified in the errno global variable.

simple_write.c

### read

```
#include <unistd.h>
size_t read(int fildes, void *buf, size_t nbytes);
```

The read system call reads up to nbytes bytes of data from the file associated with the file descriptor fildes and places them in the data area buf . It returns the number of data bytes actually read, which may be less than the number requested. If a read call returns 0, it had nothing to read; it reached the end of the file. Again, an error on the call will cause it to return –1.

simple_read.c

### open

open establishes an access path to a file or device

```
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
int open(const char *path, int oflags);
int open(const char *path, int oflags, mode_t mode);
```

* O_RDONLY - Open for read-only
* O_WRONLY - Open for write-only
* O_RDWR - Open for reading and writing

The call may also include a combination (using a bitwise OR) of the following optional modes in the oflags parameter:
* O_APPEND : Place written data at the end of the file.
* O_TRUNC : Set the length of the file to zero, discarding existing contents.
* O_CREAT : Creates the file, if necessary, with permissions given in mode .
* O_EXCL : Used with O_CREAT , ensures that the caller creates the file. The open is atomic; that is, it’s performed with just one function call. This protects against two programs creating the file at the same time. If the file already exists, open will fail.

open returns the new file descriptor (always a nonnegative integer) if successful, or –1 if it fails, at which time open also sets the global variable errno to indicate the reason for the failure.


### Initial Permissions

When you create a file using the O_CREAT flag with open , you must use the three-parameter form. mode , the third parameter, is made from a bitwise OR of the flags defined in the header file sys/stat.h . These are:
* S_IRUSR : Read permission, owner
* S_IWUSR : Write permission, owner
* S_IXUSR : Execute permission, owner
* S_IRGRP : Read permission, group
* S_IWGRP : Write permission, group
* S_IXGRP : Execute permission, group
* S_IROTH : Read permission, others
* S_IWOTH : Write permission, others
* S_IXOTH : Execute permission, others

```
open (“myfile”, O_CREAT, S_IRUSR|S_IXOTH);
```
has the effect of creating a file called myfile , with read permission for the owner and execute permission for others, and only those permissions.
```
$ ls -ls myfile
0 -r-------x 1 neil software    0 Sep 22 08:11 myfile*
```
There are a couple of factors that may affect the file permissions. First, the permissions specified are used only if the file is being created. Second, the user mask (specified by the shell’s umask command) affects the created file’s permissions. The mode value given in the open call is ANDed with the inverse of the user mask value at runtime.

#### umask

* The umask is a system variable that encodes a mask for file permissions to be used when a file is created.
* You can change the variable by executing the umask command to supply a new value. The value is a three-digit octal value. Each digit is the result of ORing values from 1, 2, or 4; the meanings are shown in the following table. The separate digits refer to “user,” “group,” and “other” permissions.

### close

You use close to terminate the association between a file descriptor, fildes , and its file. The file descriptor becomes available for reuse. It returns 0 if successful and –1 on error.
```
#include <unistd.h>
int close(int fildes);
```

### ioctl

```
#include <unistd.h>
int ioctl(int fildes, int cmd, ...);
```

It provides an interface for controlling the behavior of devices and their descriptors and configuring underlying services. Terminals, file descriptors, sockets, and even tape drives may have ioctl calls defined for them and you need to refer to the specific device’s man page for details. POSIX defines only ioctl for streams, which are beyond the scope of this book.

Try It Out
1. First you will need to make a test input file, say 1Mb in size, and name it file.in .

```
$ truncate -s 1M file.in
```
2. Then compile copy_system.c.
```
TIMEFORMAT=”“ time ./copy_system
$ ls -ls file.in file.out
```

You can improve matters by copying in larger blocks. Take a look at this modified program, copy_block.c .

### Other System Calls for Managing Files

#### lseek

The lseek system call sets the read/write pointer of a file descriptor, fildes ; that is, you can use it to set where in the file the next read or write will occur. You can set the pointer to an absolute location in the file or to a position relative to the current position or the end of file.

```
#include <unistd.h>
#include <sys/types.h>
off_t lseek(int fildes, off_t offset, int whence);
```

The offset parameter is used to specify the position, and the whence parameter specifies how the offset is used. whence can be one of the following:
* SEEK_SET : offset is an absolute position
* SEEK_CUR : offset is relative to the current position
* SEEK_END : offset is relative to the end of the file

lseek returns the offset measured in bytes from the beginning of the file that the file pointer is set to, or –1 on failure. The type off_t , used for the offset in seek operations, is an implementation-dependent integer type defined in sys/types.h .


#### fstat, stat, and lstat

The fstat system call returns status information about the file associated with an open file descriptor. The information is written to a structure, buf , the address of which is passed as a parameter.

```
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
int fstat(int fildes, struct stat *buf);
int stat(const char *path, struct stat *buf);
int lstat(const char *path, struct stat *buf);
```
#### dup and dup2

The dup system calls provide a way of duplicating a file descriptor, giving two or more different descriptors that access the same file.

```
#include <unistd.h>
int dup(int fildes);
int dup2(int fildes, int fildes2);
```

### The Standard I/O Librar y

The standard I/O library (stdio) and its header file, stdio.h , provide a versatile interface to low-level I/O system calls.

#### fopen, fread, fwrite, fclose, fseek

analog functions of the low-level open, read, write, close, lseek system call.You use it mainly for files and terminal input and output.

#### fgetc, getc, and getchar

The fgetc function returns the next byte, as a character, from a file stream. When it reaches the end of the file or there is an error, it returns EOF . You must use ferror or feof to distinguish the two cases.

```
#include <stdio.h>
int fgetc(FILE *stream);
int getc(FILE *stream);
int getchar();
```
The getc function is equivalent to fgetc , except that it may be implemented as a macro. In that case the stream argument may be evaluated more than once so it does not have side effects (for example, it shouldn’t affect variables). Also, you can’t guarantee to be able use the address of getc as a func-
tion pointer. The getchar function is equivalent to getc(stdin) and reads the next character from the standard input.

#### fputc, putc, and putchar

The fputc function writes a character to an output file stream. It returns the value it has written, or EOF on failure.

```
#include <stdio.h>
int fputc(int c, FILE *stream);
int putc(int c, FILE *stream);
int putchar(int c);
```

#### fgets and gets

The fgets function reads a string from an input file stream .
```
#include <stdio.h>
char *fgets(char *s, int n, FILE *stream);
char *gets(char *s);
```

fgets writes characters to the string pointed to by s until a newline is encountered, n-1 characters have been transferred, or the end of file is reached, whichever occurs first. Any newline encountered is transferred to the receiving string and a terminating null byte, \0 , is added. Only a maximum of n-1 characters are transferred in any one call because the null byte must be added to mark the end of the string and bring the total up to n bytes.

When it successfully completes, fgets returns a pointer to the string s . If the stream is at the end of a file, it sets the EOF indicator for the stream and fgets returns a null pointer. If a read error occurs, fgets returns a null pointer and sets errno to indicate the type of error.

The gets function is similar to fgets , except that it reads from the standard input and discards any newline encountered. It adds a trailing null byte to the receiving string.


## Formatted Input and Output

### printf, fprintf, and sprintf
### scanf, fscanf, and sscanf
### Other Stream Functions

Try It Out:
copy_stdio.c

### Stream Errors

```
#include <errno.h>
extern int errno;

#include <stdio.h>
int ferror(FILE *stream);
int feof(FILE *stream);
void clearerr(FILE *stream);
```
The ferror function tests the error indicator for a stream and returns nonzero if it’s set, but zero otherwise.
The feof function tests the end-of-file indicator within a stream and returns nonzero if it is set, zero otherwise. Use it like this:
```
if(feof(some_stream))
/* We’re at the end */
```
The clearerr function clears the end-of-file and error indicators for the stream to which stream points. It has no return value and no errors are defined. You can use it to recover from error conditions on streams. One example might be to resume writing to a stream after a “disk full” error has been resolved.

### Streams and File Descriptors

Each file stream is associated with a low-level file descriptor. You can mix low-level input and output operations with higher-level stream operations, but this is generally unwise, because the effects of buffering can be difficult to predict.

```
#include <stdio.h>
int fileno(FILE *stream);
FILE *fdopen(int fildes, const char *mode);
```
The fdopen function operates in the same way as the fopen function, but instead of a filename it takes a low-level file descriptor. This can be useful if you have used open to create a file, perhaps to get fine control over the permissions, but want to use a stream for writing to it. The mode parameter is the same as for the fopen function and must be compatible with the file access modes established when the file was originally opened. fdopen returns the new file stream or NULL on failure.

## File and Directory Maintenance

### chmod

You can change the permissions on a file or directory using the chmod system call. This forms the basis of the chmod shell program.

```
#include <sys/stat.h>
int chmod(const char *path, mode_t mode);
```

### chown

A superuser can change the owner of a file using the chown system call.

```
#include <sys/types.h>
#include <unistd.h>
int chown(const char *path, uid_t owner, gid_t group);
```
POSIX actually allows systems where non-superusers can change file ownerships.

### unlink, link, and symlink

You can remove a file using unlink. The unlink system call removes the directory entry for a file and decrements the link count for it. It returns 0 if the unlinking was successful, –1 on an error. You must have write and execute permissions in the directory where the file has its directory entry for this call to function.

```
#include <unistd.h>
int unlink(const char *path);
int link(const char *path1, const char *path2);
int symlink(const char *path1, const char *path2);
```

If the count reaches zero and no process has the file open, the file is deleted. In fact, the directory entry is always removed immediately, but the file’s space will not be recovered until the last process (if any) closes it. The rm program uses this call. Additional links represent alternative names for a file, normally created by the ln program. You can create new links to a file programmatically by using the link system call.

Creating a file with open and then calling unlink on it is a trick some programmers use to create transient files. These files are available to the program only while they are open; they will effectively be automatically deleted when the program exits and the file is closed.

The link system call creates a new link to an existing file, path1 . The new directory entry is specified by path2 . You can create symbolic links using the symlink system call in a similar fashion. Note that symbolic links to a file do not increment a file’s reference count and so do not prevent the file from being effectively deleted as normal (hard) links do.

### mkdir and rmdir

You can create and remove directories using the mkdir and rmdir system calls.

```
#include <sys/types.h>
#include <sys/stat.h>
int mkdir(const char *path, mode_t mode);
```
The mkdir system call is used for creating directories and is the equivalent of the mkdir program. mkdir makes a new directory with path as its name. The directory permissions are passed in the parameter mode and are given as in the O_CREAT option of the open system call and, again, subject to umask.

```
#include <unistd.h>
int rmdir(const char *path);
```
The rmdir system call removes directories, but only if they are empty. The rmdir program uses this system call to do its job.

### chdir and getcwd

A program can navigate directories in much the same way as a user moves around the file system. As you use the cd command in the shell to change directory, so a program can use the chdir system call.

```
#include <unistd.h>
int chdir(const char *path);
char *getcwd(char *buf, size_t size);
```
A program can determine its current working directory by calling the getcwd function.
getcwd may also return NULL if the directory is removed ( EINVAL ) or permissions changed ( EACCESS ) while the program is running.

## Scanning Directories

The directory functions are declared i  a header file dirent.h . They use a structure, DIR , as a basis for directory manipulation. A pointer to this structure, called a directory stream (a DIR * ), acts in much the same way as a file steam ( FILE * ) does for regular file manipulation. Directory entries themselves are returned in dirent structures, also declared in dirent.h , because one should never alter the fields in the DIR structure directly.

### opendir

The opendir function opens a directory and establishes a directory stream. If successful, it returns a pointer to a DIR structure to be used for reading directory entries. opendir returns a null pointer on failure. Note that a directory stream uses a low-level file descriptor to access the directory itself, so opendir could fail with too many open files.

```
#include <sys/types.h>
#include <dirent.h>
DIR *opendir(const char *name);
```

### readdir

The readdir function returns a pointer to a structure detailing the next directory entry in the directory stream dirp . Successive calls to readdir return further directory entries. On error, and at the end of the directory, readdir returns NULL . POSIX-compliant systems leave errno unchanged when returning NULL at end of directory and set it when an error occurs.

```
#include <sys/types.h>
#include <dirent.h>
struct dirent *readdir(DIR *dirp);
```
dirent {
    ino_t d_ino; //The inode of the file
    char d_name[]; // The name of the file
}

### telldir

The telldir function returns a value that records the current position in a directory stream. You can use this in subsequent calls to seekdir to reset a directory scan to the current position.

```
#include <sys/types.h>
#include <dirent.h>
long int telldir(DIR *dirp);
```

### seekdir

The seekdir function sets the directory entry pointer in the directory stream given by dirp . The value of loc , used to set the position, should have been obtained from a prior call to telldir.

```
#include <sys/types.h>
#include <dirent.h>
void seekdir(DIR *dirp, long int loc);
```

## closedir

The closedir function closes a directory stream and frees up the resources associated with it. It returns 0 on success and –1 if there is an error.

```
#include <sys/types.h>
#include <dirent.h>
int closedir(DIR *dirp);
```
#### Try It Out
* printdir.c

## Errors

The values and meanings of the errors are listed in the header file errno.h . They include
* EPERM : Operation not permitted
* ENOENT : No such file or directory
* EINTR : Interrupted system call
* EIO : I/O Error
* EBUSY : Device or resource busy
* EEXIST : File exists
* EINVAL : Invalid argument
* EMFILE : Too many open files
* ENODEV : No such device
* EISDIR : Is a directory
* ENOTDIR : Isn’t a directory

### strettor
This function maps an error number into a string describing the type of error that has occurred. This can be useful for logging error conditions.
```
#include <string.h>
char *strerror(int errnum);
```
### perror
The perror function also maps the current error, as reported in errno , into a string and prints it on the standard error stream. It’s preceded by the message given in the string s (if not NULL ), followed by a colon and a space.
```
#include <stdio.h>
void perror(const char *s);
```
##  The /proc File System

These /dev files are used to access hardware in a specific way using low-level system calls.
In many cases, the files can just be read and will give status information. For example, /proc/cpuinfo gives details of the processors available:
```
$ cat /proc/cpuinfo
```

Similarly, /proc/meminfo and /proc/version give information about memory usage and kernel version, respectively:
```
$ cat /proc/meminfo
$ cat /proc/version
```

You can find more information from specific kernel functions in subdirectories of /proc . For example, you can get network socket usage statistics from /proc/net/sockstat :
```
$ cat /proc/net/sockstat
```
the total number of files that all running programs can open at the same time is a Linux kernel parameter. The current value can be read at /proc/sys/fs/file-max :
```
$ cat /proc/sys/fs/file-max
```
Writing /proc files requires superuser access. You must take great care when writing /proc files; it’s possible to cause severe problems including system crashes and loss of data by writing inappropriate values.
To increase the system-wide file handle limit to 80,000, you can simply write the new limit to the file-max file:
```
$ echo 80000 >/proc/sys/fs/file-max
```
The process identifier for ftp here is given as 9118, so you need to look in /proc/9118 for details about it:
```
$ ls -l /proc/9118
```
The fd subdirectory provides information about the open file descriptors in use by the process. This information can be useful in determining how many files a program has open at one time. There is one entry per open descriptor; the name matches the number of the descriptor. In this case, you can see that ftp has open descriptors 0, 1, 2, and 3, as we might expect. These are the standard input, output, and error descriptors plus a connection to the remote server.
```
$ ls /proc/9118/fd
0 1 2 3
```

## Advanced Topics: fcntl and mmap

### fcntl
The fcntl system call provides further ways to manipulate low-level file descriptors.

```
#include <fcntl.h>
int fcntl(int fildes, int cmd);
int fcntl(int fildes, int cmd, long arg);
```

### mmap

UNIX provides a useful facility that allows programs to share memory, and the good news is that it’s been included in versions 2.0 and later of the Linux kernel. The mmap (for memory map) function sets up
a segment of memory that can be read or written by two or more programs. Changes made by one program are seen by the others.

You can use the same facility to manipulate files. You can make the entire contents of a disk file look like an array in memory. If the file consists of records that can be described by C structures, you can update
the file using structure array accesses.

This is made possible by the use of virtual memory segments that have special permissions set. Reading from and writing to the segment causes the operating system to read and write the appropriate part of the disk file.

The mmap function creates a pointer to a region of memory associated with the contents of the file accessed through an open file descriptor.

```
#include <sys/mman.h>
void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
```
The msync function causes the changes in part or all of the memory segment to be written back to (or read from) the mapped file.

```
#include <sys/mman.h>
int msync(void *addr, size_t len, int flags);
```

The munmap function releases the memory segment.

```
#include <sys/mman.h>
int munmap(void *addr, size_t len);
```

#### Try It Out

* record.c
