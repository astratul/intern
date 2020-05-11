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