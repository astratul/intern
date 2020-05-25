# Summary - Data Management

This chapter covers the following:
* Dynamic memory management: what to do and what Linux won’t let you do
* File locking: cooperative locking, locking regions of shared files, and avoiding deadlocks
* The dbm database: a basic, non-SQL-based database library featured in most Linux systems

## Managing Memory

### Simple Memory Allocation

You allocate memory using the malloc call in the standard C library:
```
#include <stdlib.h>
void *malloc(size_t size);
```
You can allocate a great deal of memory on most Linux systems. Let’s start with a very simple program, but one that would defeat old MS-DOS-based programs, because they cannot access memory outside the base 640K memory map of PCs.

#### Try It Out
* memory1.c
How It Works? 
This program asks the malloc library to give it a pointer to a megabyte of memory. You check to ensure that malloc was successful and then use some of the memory to show that it exists. When you run the program, you should see Hello World printed out, showing that malloc did indeed return the megabyte of usable memory. We don’t check that all of the megabyte is present; we have to put some trust in the malloc code!
Notice that because malloc returns a void * pointer, you cast the result to the char * that you need.
The malloc function is guaranteed to return memory that is aligned so that it can be cast to a pointer of any type.

### Allocating Lots of Memory

#### Try It Out
* memory2.c
    * How It Works? The program is very similar to the previous example. It simply loops, asking for more and more memory, until it has allocated twice the amount of memory you said your machine had when you adjusted the define PHY_MEM_MEGS . The surprise is that it works at all, because we appear to have created a program that uses every single byte of physical memory on the author’s machine. Notice that we use the size_t type for our call to malloc .

* memory3.c
    * How It Works? 
This “killing the process” behavior is different from early versions of Linux and many other flavors of UNIX, where malloc simply fails. It’s termed the “out of memory (OOM) killer,” and although it may seem rather drastic, it is in fact a good compromise between letting processes allocate memory rapidly and efficiently and having the Linux kernel protect itself from a total lack of resources, which is a serious issue.

### Abusing Memory

Suppose you try to do “bad” things with memory. In this exercise, you allocate some memory and then attempt to write past the end, in memory4.c .
#### Try It Out
* memory4.c
    * How It Works? 
Each running program on a Linux system sees its own memory map, which is different from every other program’s. Only the operating system knows how physical memory is arranged, and not only manages it for user programs, but also protects user programs from each other.

### The Null Pointer
#### Try It Out
* memory5a.c
    * How It Works? 
    The first printf attempts to print out a string obtained from a null pointer; then the sprintf attempts to write to a null pointer. In this case, Linux (in the guise of the GNU “C” library) has been forgiving about the read and has simply provided a “magic” string containing the characters ( n u l l ) \0 . It hasn’t been so forgiving about the write and has terminated the program. This can sometimes be helpful in tracking down program bugs.
* memory5b.c
    * This time you attempt to read directly from location zero.

### Freeing Memory
```
#include <stdlib.h>
void free(void *ptr_to memory);
```
A call to free should be made only with a pointer to memory allocated by a call to malloc, calloc, or realloc.

#### Try It Out
* memory6.c

Remember that once you’ve called free on a block of memory, it no longer belongs to the process. It’s not being managed by the malloc library. Never try to read or write memory after calling free on it.

### Other Memory Allocation Functions
```
#include <stdlib.h>
void *calloc(size_t number_of_elements, size_t element_size);
void *realloc(void *existing_memory, size_t new_size);
```
Although calloc allocates memory that can be freed with free , it has somewhat different parameters from malloc : It allocates memory for an array of structures and requires the number of elements and the size of each element as its parameters. The allocated memory is filled with zeros; and if calloc is successful, a pointer to the first element is returned. Like malloc , subsequent calls are not guaranteed to return contiguous space, so you can’t enlarge an array created by calloc by simply calling calloc again and expecting the second call to return memory appended to that returned by the first call.
The realloc function changes the size of a block of memory that has been previously allocated. It’s passed a pointer to some memory previously allocated by malloc , calloc , or realloc and resizes it up or down as requested. The realloc function may have to move data around to achieve this, so it’s important to ensure that once memory has been realloc ed, you always use the new pointer and never try to access the memory using pointers set up before realloc was called.
Another problem to watch out for is that realloc returns a null pointer if it has been unable to resize the memory. This means that in some applications you should avoid writing code like this:

```
my_ptr = malloc(BLOCK_SIZE);
....
my_ptr = realloc(my_ptr, BLOCK_SIZE * 10);
```
If realloc fails, then it returns a null pointer; my_ptr will point to null; and the original memory allocated with malloc can no longer be accessed via my_ptr . It may therefore be to your advantage to request the new memory first with malloc and then copy data from the old block to the new block using memcpy before free ing the old block. On error, this would allow the application to retain access to the data stored in the original block of memory, perhaps while arranging a clean termination of the program.

## File Locking
File locking is a very important part of multiuser, multitasking operating systems. Programs frequently need to share data, usually through files, and it’s very important that those programs have some way of establishing control of a file. The file can then be updated in a safe fashion, or a second program can stop itself from trying to read a file that is in a transient state while another program is writing to it.
Linux has several features that you can use for file locking. The simplest method is a technique to create lock files in an atomic way, so that nothing else can happen while the lock is being created. This gives a program a method of creating files that are guaranteed to be unique and could not have been simultaneously created by a different program. The second method is more advanced; it enables programs to lock parts of a file for exclusive access.

### Creating Lock Files
To create a file to use as a lock indicator, you can use the open system call defined in fcntl.h (which you met in an earlier chapter) with the O_CREAT and O_EXCL flags set. This enables you to check that the file doesn’t already exist and then create it in a single, atomic operation.

#### Try It Out
* lock1.c 
    * How It Works? On Linux systems at least, error 17 refers to EEXIST , an error used to indicate that a file already exists. Error numbers are defined in the header file errno.h or, more commonly, by files included by it. In this case, the definition, actually in /usr/include/asm-generic/errno-base.h , reads 
    ```#define EEXIST 17 /* File exists */``` This is an appropriate error for an open(O_CREAT | O_EXCL) failure. If a program simply needs a resource exclusively for a short period of its execution, often termed a critical section, it should create the lock file using the open system call before entering the critical section, and use the unlink system call to delete it afterward, when the program exits the critical section.
* lock2.c

### Locking Regions
```
#include <fcntl.h>
int fcntl(int fildes, int command, ...);
```
fcntl operates on open file descriptors and, depending on the command parameter, can perform different tasks. The three command options of interest for file locking are as follows:
* F_GETLK
* F_SETLK
* F_SETLKW
When you use these the third argument must be a pointer to a struct flock , so the prototype is effectively this:
```
int fcntl(int fildes, int command, struct flock *flock_structure);
```
The flock (file lock) structure is implementation dependent, but it will contain at least the following members:
* short l_type;
* short l_whence;
* off_t l_start;
* off_t l_len;
* pid_t l_pid;
The l_type member takes one of several values, also defined in fcntl.h .

#### The F_GETLK Command
It gets locking information about the file that fildes (the first parameter) has open. It doesn’t attempt to lock the file. The calling process passes information about the type of lock it might wish to create, and fcntl used with the F_GETLK command returns any information that would prevent the lock from occurring.

#### The F_SETLK Command
This command attempts to lock or unlock part of the file referenced by fildes .
#### The F_SETLKW Command
The F_SETLKW command is the same as the F_SETLK command above except that if it can’t obtain the lock, the call will wait until it can. Once this call has started waiting, it will return only when the lock can be obtained or a signal occurs.

### Use of read and write with Locking
When you’re using locking on regions of a file, it’s very important to use the lower-level read and write calls to access the data in the file, rather than the higher-level fread and fwrite . This is necessary because fread and fwrite perform buffering of data read or written inside the library, so executing an fread call to read the first 100 bytes of a file may (in fact almost certainly will) read more than 100 bytes and buffer the additional data inside the library. If the program then uses fread to read the next 100 bytes, it will actually read data already buffered inside the library and not allow a low-level read to pull more data from the file.
To see why this is a problem, consider two programs that wish to update the same file. Suppose the file consists of 200 bytes of data, all zeros. The first program starts first and obtains a write lock on the first 100 bytes of the file. It then uses fread to read in those 100 bytes. However, as shown in an earlier chapter, fread will read ahead by up to BUFSIZ bytes at a time, so it actually reads the entire file into memory, but only passes the first 100 bytes back to the program.
The second program then starts. It obtains a write lock on the second 100 bytes of the program. This is successful because the first program locked only the first 100 bytes. The second program writes twos to bytes 100 to 199, closes the file, unlocks it, and exits. The first program then locks the second 100 bytes of the file and calls fread to read them in. As that data was already buffered in memory by the library, what the program actually sees is 100 bytes of zeros, not the 100 twos that actually exist in the file on the hard disk. This problem doesn’t occur when you’re using read and write .
#### Try It Out
* lock3.c
* lock4.c

### Competing Locks

Now that you’ve seen how to test for existing locks on a file, let’s see what happens when two programs compete for locks on the same section of the file. You’ll use the lock3 program again for locking the file, and a new program, lock5 , to try to lock it again. To complete the example you’ll also add some calls for unlocking into lock5 .
#### Try It Out
* lock5.c

### Other Lock Commands
There is a second method of locking files: the lockf function. This also operates using file descriptors.
It has the prototype:
```
#include <unistd.h>
int lockf(int fildes, int function, off_t size_to_lock);
```
It can take the following function values:
* F_ULOCK : Unlock
* F_LOCK : Lock exclusively
* F_TLOCK : Test and lock exclusively
* F_TEST : Test for locks by other processes

The size_to_lock parameter is the number of bytes to operate on, from the current offset in the file.

lockf has a simpler interface than the fcntl interface, principally because it has rather less functionality and flexibility. To use the function, you must seek the start of the region you wish to lock and then call it with the number of bytes to lock.
Like the fcntl method of file locking, all locks are only advisory; they won’t actually prevent reading from or writing to the file. It’s the responsibility of programs to check for locks. The effect of mixing fcntl locks and lockf locks is undefined, so you must decide which type of locking you wish to use and stick to it.

### Deadlocks
No discussion of locking would be complete without a mention of the dangers of deadlocks. Suppose two programs wish to update the same file. They both need to update byte 1 and byte 2 at the same time. Program A chooses to update byte 2 first, then byte 1. Program B tries to update byte 1 first, then byte 2. Both programs start at the same time. Program A locks byte 2 and program B locks byte 1. Program A tries for a lock on byte 1. Since this is already locked by program B, program A waits. Program B tries for a lock on byte 2. Since this is locked by program A, program B too waits.

This situation, when neither program is able to proceed, is called a deadlock, or deadly embrace. It is a common problem with database applications in which many users are frequently trying to access the same data. Most commercial relational databases detect deadlocks and break them automatically; the Linux kernel doesn’t. Some external intervention, perhaps forcibly terminating one of the programs, is required to sort out the resulting mess.

Programmers must be wary of this situation. When you have multiple programs waiting for locks, you need to be very careful to consider whether a deadlock could occur. In this example it’s quite easy to avoid: Both programs should simply lock the bytes they require in the same order, or use a larger region to lock. 

##### We don’t have the space to consider the difficulties of concurrent programs here. If you’re interested in reading further, you might like to consider obtaining a copy of Principles of Concurrent and Distributed Programming by M. Ben-Ari (Prentice Hall, 1990).

## Databases

Using a database is better than storing files for two reasons:
* You can store data records that vary in size, which can be a little difficult to implement using flat, unstructured files.
* Databases store and retrieve data efficiently using an index. The big advantage is that this index need not be a simple record number, which would be quite easy to implement in a flat file, but can be an arbitrary string.

### The dbm Database

#### Introduction to dbm

#### Getting dbm

#### Troubleshooting and Reinstalling dbm

### The dbm Routines

### dbm Access Functions

#### dbm_open

#### dbm_store

#### dbm_fetch

#### dbm_close

#### Try It Out
* 

### Additional dbm Functions

#### dbm_delete

#### dbm_error

#### dbm_clearerr

#### dbm_firstkey and dbm_nextkey

#### Try It Out
* 
## The CD Application
### Updating the Design

### The CD Database Application Using dbm

#### Try It Out
* cd_data.h
* app_ui.c
* cd_access.c
* Makefile
