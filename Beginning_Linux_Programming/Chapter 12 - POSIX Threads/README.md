# Summary - POSIX Threads

In this chapter, you look at
* Creating new threads within a process
* Synchronizing data access between threads in a single process
* Modifying the attributes of a thread
* Controlling one thread from another in the same process

## What Is a Thread?

Multiple strands of execution in a single program are called threads. A more precise definition is that a thread is a sequence of control within a process.
It’s important to be clear about the difference between the fork system call and the creation of new threads. When a process executes a fork call, a new copy of the process is created with its own variables and its own PID. This new process is scheduled independently, and (in general) executes almost independently of the process that created it. When we create a new thread in a process, in contrast, the new thread of execution gets its own stack (and hence local variables) but shares global variables, file descriptors, signal handlers, and its current directory state with the process that created it.
The concept of threads has been around for some time, but until the IEEE POSIX committee published some standards, they had not been widely available in UNIX-like operating systems, and the implementations that did exist tended to vary between different vendors.


## Advantages and Drawbacks of Threads
Threads also have drawbacks:
❑ Writing multithreaded programs requires very careful design. The potential for introducing subtle timing faults, or faults caused by the unintentional sharing of variables in a multithreaded program is considerable. Alan Cox (the well respected Linux guru) has commented that threads are also known as “how to shoot yourself in both feet at once.”
❑ Debugging a multithreaded program is much, much harder than debugging a single-threaded one, because the interactions between the threads are very hard to control.
❑ A program that splits a large calculation into two and runs the two parts as different threads will not necessarily run more quickly on a single processor machine, unless the calculation truly allows multiple parts to be calculated simultaneously and the machine it is executing on has multiple processor cores to support true multiprocessing.

## A First Threads Program
There is a whole set of library calls associated with threads, most of whose names start with pthread . To use these library calls, you must define the macro _REENTRANT , include the file pthread.h , and link with the threads library using -lpthread .
### Try It Out:
* thread1.c
```
$ cc -D_REENTRANT -I/usr/include/nptl thread1.c
–o thread1 -L/usr/lib/nptl -lpthread
$ cc -D_REENTRANT thread1.c –o thread1 -lpthread
./thread1
```

## Simultaneous Execution

## Synchronization

### Synchronization with Semaphores

### Synchronization with Mutexes

## Thread Attributes

### Thread Attributes Scheduling

## Canceling a Thread

## Threads in Abundance

