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

### WRITE

The write system call arranges for the first nbytes bytes from buf to be written to the file associated with the file descriptor fildes . It returns the number of bytes actually written. This may be less than nbytes if there has been an error in the file descriptor or if the underlying device driver is sensitive to block size. If the function returns 0, it means no data was written; if it returns –1, there has been an error in the write call, and the error will be specified in the errno global variable.

simple_write.c

### READ

The read system call reads up to nbytes bytes of data from the file associated with the file descriptor fildes and places them in the data area buf . It returns the number of data bytes actually read, which may be less than the number requested. If a read call returns 0, it had nothing to read; it reached the end of the file. Again, an error on the call will cause it to return –1.

simple_read.c

### OPEN

