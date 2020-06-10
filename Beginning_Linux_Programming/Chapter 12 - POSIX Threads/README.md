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
Creating a new thread has some distinct advantages over creating a new process in certain circumstances. The overhead cost of creating a new thread is significantly less than that of creating a new process (though Linux is particularly efficient at creating new processes compared with many other operating systems).
Following are some advantages of using threads:
* Sometimes it is very useful to make a program appear to do two things at once. The classic example is to perform a real-time word count on a document while still editing the text. One thread can manage the user’s input and perform editing. The other, which can see the same document content, can continuously update a word count variable. The first thread (or even a third one) can use this shared variable to keep the user informed. Another example is a multithreaded database server where an apparent single process serves multiple clients, improving the overall data throughput by servicing some requests while blocking others, waiting for disk activity. For a database server, this apparent multitasking is quite hard to do efficiently in different processes, because the requirements for locking and data consistency cause the different processes to be very tightly coupled. This can
be done much more easily with multiple threads than with multiple processes.
* The performance of an application that mixes input, calculation, and output may be improved by running these as three separate threads. While the input or output thread is waiting for a connection, one of the other threads can continue with calculations. A server application processing multiple network connects may also be a natural fit for a multithreaded program.
* Now that multi-cored CPUs are common even in desktop and laptop machines, using multiple threads inside a process can, if the application is suitable, enable a single process to better utilize the hardware resources available.
* In general, switching between threads requires the operating system to do much less work than switching between processes. Thus, multiple threads are much less demanding on resources than multiple processes, and it is more practical to run programs that logically require many threads of execution on single-processor systems. That said, the design difficulties of writing a multithreaded program are significant and should not be taken lightly.



Threads also have drawbacks:
* Writing multithreaded programs requires very careful design. The potential for introducing subtle timing faults, or faults caused by the unintentional sharing of variables in a multithreaded program is considerable. Alan Cox (the well respected Linux guru) has commented that threads are also known as ```“how to shoot yourself in both feet at once.”```
* Debugging a multithreaded program is much, much harder than debugging a single-threaded one, because the interactions between the threads are very hard to control.
* A program that splits a large calculation into two and runs the two parts as different threads will not necessarily run more quickly on a single processor machine, unless the calculation truly allows multiple parts to be calculated simultaneously and the machine it is executing on has multiple processor cores to support true multiprocessing.

## A First Threads Program

There is a whole set of library calls associated with threads, most of whose names start with pthread . To use these library calls, you must define the macro _REENTRANT , include the file pthread.h , and link with the threads library using -lpthread .

Re-entrant code can be called more than once, whether by different threads or by nested invocations in some way, and still function correctly. Thus, the re-entrant section of code usually must use local variables only in such a way that each and every call to the code gets its own unique copy of the data.
In multithreaded programs, you tell the compiler that you need this feature by defining the _REENTRANT macro before any #include lines in your program. This does three things, and does them so elegantly that usually you don’t even need to know what was done:
* Some functions get prototypes for a re-entrant safe equivalent. These are normally the same function name, but with _r appended so that, for example, gethostbyname is changed to gethostbyname_r .
* Some stdio.h functions that are normally implemented as macros become proper re-entrant safe functions.
* The variable errno , from errno.h , is changed to call a function, which can determine the real errno value in a multithread safe way.
```
#include <pthread.h>
int pthread_create(pthread_t *thread, pthread_attr_t *attr, void
*(*start_routine)(void *), void *arg);
```

pthread_create , like most pthread_ functions, is among the few Linux functions that do not follow the convention of using a return value of 1 for errors. Unless you are very sure, it’s always safest to double-check the manual before checking the return code.

```
#include <pthread.h>
void pthread_exit(void *retval); // This function terminates the calling thread, returning a pointer to an object
int pthread_join(pthread_t th, void **thread_return); // is the thread equivalent of wait that processes use to collect child processes.
```
### Try It Out:
* thread1.c
```
$ cc -D_REENTRANT -I/usr/include/nptl thread1.c  -L/usr/lib/nptl -lpthread
$ cc -D_REENTRANT thread1.c -lpthread
$ ./a.out
```

## Simultaneous Execution
The next example shows you how to write a program that checks that the execution of two threads occurs simultaneously. (Of course, if you are using a single-processor system, the CPU would be cleverly switched between the threads, rather than having the hardware simultaneously execute both threads using separate processor cores). Because you haven’t yet met any of the thread synchronization functions, this will be a very inefficient program that does what is known as a polling between the two threads. Again, you will make use of the fact that everything except local function variables are shared between the different threads in a process.

### Try It Out:
* thread2.c
```
$ cc -D_REENTRANT thread2.c -lpthread
$ ./a.out
```

## Synchronization

In the previous section, you saw that both threads execute together, but the method of switching between them was clumsy and very inefficient. Fortunately, there is a set functions specifically designed to provide better ways to control the execution of threads and access to critical sections of code.
We look at two basic methods here: semaphores, which act as gatekeepers around a piece of code, and mutexes, which act as a mutual exclusion (hence the name mutex) device to protect sections of code. These methods are similar; indeed, one can be implemented in terms of the other. However, there are some cases where the semantics of the problem suggest that one is more expressive than the other.

### Synchronization with Semaphores

There are two sets of interface functions for semaphores: One is taken from POSIX Realtime Extensions and used for threads, and the other is known as System V semaphores, which are commonly used for process synchronization.
Dijkstra: A semaphore is a special type of variable that can be incremented or decremented, but crucial access to the variable is guaranteed to be atomic, even in a multithreaded program. 
This means that if two (or more) threads in a program attempt to change the value of a semaphore, the system guarantees that all the operations will in fact take place in sequence. With normal variables the result of conflicting operations from different threads within the same program is undefined.
Counting semaphores are a logical extension of a binary semaphore and that the actual function calls needed are identical.
The semaphore functions do not start with pthread_ , as most thread-specific functions do, but with sem_ . Four basic semaphore functions are used in threads. They are all quite simple.
A semaphore is created with the sem_init function, which is declared as follows:
```
#include <semaphore.h>
int sem_init(sem_t *sem, int pshared, unsigned int value);
```
The next pair of functions controls the value of the semaphore and is declared as follows:
```
#include <semaphore.h>
int sem_wait(sem_t * sem);
int sem_post(sem_t * sem);
```
These both take a pointer to the semaphore object initialized by a call to sem_init .
The sem_post function atomically increases the value of the semaphore by 1. Atomically here means that if two threads simultaneously try to increase the value of a single semaphore by 1, they do not interfere with each other, as might happen if two programs read, increment, and write a value to a file at the same time. If both programs try to increase the value by 1, the semaphore will always be correctly increased in value by 2.
The sem_wait function atomically decreases the value of the semaphore by one, but always waits until the semaphore has a nonzero count first. Thus, if you call sem_wait on a semaphore with a value of 2, the thread will continue executing but the semaphore will be decreased to 1. If sem_wait is called on a  semaphore with a value of 0 , the function will wait until some other thread has incremented the value so that it is no longer 0 .
The last semaphore function is sem_destroy . This function tidies up the semaphore when you have finished with it. It is declared as follows:
```
#include <semaphore.h>
int sem_destroy(sem_t * sem);
```
### Try It Out:
* thread3.c
```
$ cc -D_REENTRANT thread3.c -lpthread
$ ./a.out
```
### Synchronization with Mutexes

The other way of synchronizing access in multithreaded programs is with mutexes (short for mutual exclusions), which act by allowing the programmer to “lock” an object so that only one thread can access it. To control access to a critical section of code you lock a mutex before entering the code section and then unlock it when you have finished.
The basic functions required to use mutexes are very similar to those needed for semaphores. They are declared as follows:
```
#include <pthread.h>
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
int pthread_mutex_lock(pthread_mutex_t *mutex));
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```
As usual, 0 is returned for success, and on failure an error code is returned, but errno is not set; you must use the return code.
### Try It Out:
* thread4.c
```
$ cc -D_REENTRANT thread4.c -lpthread
$ ./a.out
```
## Thread Attributes
You can create threads that behave like this. They are called detached threads, and you create them by modifying the thread attributes or by calling pthread_detach . Because we want to demonstrate attributes, we use the former method here.
The most important function that you need is pthread_attr_init , which initializes a thread attribute object.
```
#include <pthread.h>
int pthread_attr_init(pthread_attr_t *attr);

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);
int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param);
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inherit);
int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inherit);
int pthread_attr_setscope(pthread_attr_t *attr, int scope);
int pthread_attr_getscope(const pthread_attr_t *attr, int *scope);
int pthread_attr_setstacksize(pthread_attr_t *attr, int scope);
int pthread_attr_getstacksize(const pthread_attr_t *attr, int *scope);
```
Once again, 0 is returned for success and an error code is returned on failure.
There is also a destroy function: pthread_attr_destroy . Its purpose is to allow clean destruction of the attribute object. Once the object has been destroyed, it cannot be used again until it has been reinitialized.
### Try It Out:
* thread5.c
```
$ cc -D_REENTRANT thread5.c -lpthread
$ ./a.out
```

### Thread Attributes Scheduling
### Try It Out:
* thread6.c
```
$ cc -D_REENTRANT thread6.c -lpthread
$ ./a.out
```
## Canceling a Thread

Sometimes, you want one thread to be able to ask another thread to terminate, rather like sending it a signal. There is a way to do this with threads, and, in parallel with signal handling, threads get a way of modifying how they behave when they are asked to terminate.
Let’s look first at the function to request a thread to terminate:
```
#include <pthread.h>
int pthread_cancel(pthread_t thread);
int pthread_setcanceltype(int type, int *oldtype);
```
Again, the oldtype allows the previous state to be retrieved, or a NULL can be passed if you are not interested in knowing the previous state. By default, threads start with the cancellation state PTHREAD_CANCEL_ENABLE and the cancellation type PTHREAD_CANCEL_DEFERRED .

### Try It Out:
* thread7.c
```
$ cc -D_REENTRANT thread7.c -lpthread
$ ./a.out
```

## Threads in Abundance

Up until now, we have always had the normal thread of execution of a program create just one other thread. However, we don’t want you to think that you can create only one extra thread.

### Try It Out:
* thread8.c
```
$ cc -D_REENTRANT thread8.c -lpthread
$ ./a.out
```