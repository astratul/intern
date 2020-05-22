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
* 
* 

### Abusing Memory
#### Try It Out
* 

### The Null Pointer
#### Try It Out
* 

### Freeing Memory
#### Try It Out
* 

### Other Memory Allocation Functions
#### Try It Out
* 

## File Locking

### Creating Lock Files
#### Try It Out
* 
* 

### Locking Regions

#### The F_GETLK Command

#### The F_SETLK Command
#### The F_SETLKW Command
### Use of read and write with Locking
#### Try It Out
* 
* 
### Competing Locks
#### Try It Out
* 

### Other Lock Commands

### Deadlocks

## Databases
## The CD Application
