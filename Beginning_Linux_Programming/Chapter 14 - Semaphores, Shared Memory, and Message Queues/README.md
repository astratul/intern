# Summary - Semaphores, Shared Memory, and Message Queues

We cover the following topics in this chapter:
* Semaphores, for managing access to resources
* Shared memory, for highly efficient data sharing between programs
* Messaging, for an easy way of passing data between programs

## Semaphores
When you write programs that use threads operating in multiuser systems, multiprocessing systems, or a combination of the two, you may often discover that you have critical sections of code, where you need to ensure that a single process (or a single thread of execution) has exclusive access to a resource.
Semaphores have a complex programming interface. Fortunately, you can easily provide a much simplified interface that is sufficient for most semaphore-programming problems.

A more formal definition of a semaphore is a special variable on which only two operations are allowed; these operations are officially termed wait and signal. Because “wait” and “signal” already have special meanings in Linux programming, we’ll use the original notation:
* P(semaphore variable) for wait
* V(semaphore variable) for signal
These letters come from the Dutch words for wait (passeren: to pass, as in a checkpoint before the critical section) and signal (vrijgeven: to give or release, as in giving up control of the critical section). You may also come across the terms “up” and “down” used in relation to semaphores, taken from the use of signaling flags.

### Semaphore Definition

The simplest semaphore is a variable that can take only the values 0 and 1, a binary semaphore. This is the most common form. Semaphores that can take many positive values are called general semaphores. For the remainder of this chapter, we concentrate on binary semaphores.
The definitions of P and V are surprisingly simple. Suppose you have a semaphore variable sv . The two operations are then defined as follows:
* P(sv) If sv is greater than zero, decrement sv . If sv is zero, suspend execution of this process.
* V(sv) If some other process has been suspended waiting for sv , make it resume execution. If no process is suspended waiting for sv , increment sv .

### A Theoretical Example

You can see how this works with a simple theoretical example. Suppose you have two processes proc1 and proc2 , both of which need exclusive access to a database at some point in their execution. You define a single binary semaphore, sv , which starts with the value 1 and can be accessed by both processes. Both processes then need to perform the same processing to access the critical section of code; indeed, the two processes could simply be different invocations of the same program.
The two processes share the sv semaphore variable. Once one process has executed P(sv) , it has obtained the semaphore and can enter the critical section. The second process is prevented from entering the critical section because when it attempts to execute P(sv) , it’s made to wait until the first process has left the critical section and executed V(sv) to release the semaphore

### Linux Semaphore Facilities

```
#include <sys/sem.h>
int semctl(int sem_id, int sem_num, int command, ...);
int semget(key_t key, int num_sems, int sem_flags);
int semop(int sem_id, struct sembuf *sem_ops, size_t num_sem_ops);
```
The header file sys/sem.h usually relies on two other header files, sys/types.h and sys/ipc.h . Normally they are automatically included by sys/sem.h and you do not need to explicitly add a #include for them.

#### semget

The semget function creates a new semaphore or obtains the semaphore key of an existing semaphore:
```
int semget(key_t key, int num_sems, int sem_flags);
```
The first parameter, key , is an integral value used to allow unrelated processes to access the same semaphore. All semaphores are accessed indirectly by the program supplying a key, for which the system then generates a semaphore identifier. The semaphore key is used only with semget . All other semaphore functions use the semaphore identifier returned from semget .
There is a special semaphore key value, IPC_PRIVATE , that is intended to create a semaphore that only the creating process could access, but this rarely has any useful purpose. You should provide a unique, non-zero integer value for key when you want to create a new semaphore.


#### semop 
The function semop is used for changing the value of the semaphore:
```
int semop(int sem_id, struct sembuf *sem_ops, size_t num_sem_ops);
```
The first parameter, sem_id , is the semaphore identifier, as returned from semget . The second parameter, sem_ops , is a pointer to an array of structures, each of which will have at least the following members:
```
struct sembuf {
short sem_num;
short sem_op;
short sem_flg;
}
```
#### semctl

The semctl function allows direct control of semaphore information:
```
int semctl(int sem_id, int sem_num, int command, ...);
```
The first parameter, sem_id , is a semaphore identifier, obtained from semget . The sem_num parameter is the semaphore number. You use this when you’re working with arrays of semaphores. Usually, this is 0, the first and only semaphore. The command parameter is the action to take, and a fourth parameter, if present, is a union semun , which according to the X/OPEN specification must have at least the following members:
```
union semun {
int val;
struct semid_ds *buf;
unsigned short *array;
}
```
### Using Semaphores

As you can see from the previous section’s descriptions, semaphore operations can be rather complex. This is most unfortunate, because programming multiple processes or threads with critical sections is quite a difficult problem on its own and having a complex programming interface simply adds to the intellectual burden.

#### Try It Out
* sem1.c
```
$ cc sem1.c -o sem1
$ ./sem1 1 &
$ ./sem1
```


## Shared Memory
Shared memory is the second of the three IPC facilities. It allows two unrelated processes to access the same logical memory. Shared memory is a very efficient way of transferring data between two running processes. Although the X/Open standard doesn’t require it, it’s probable that most implementations of shared memory arrange for the memory being shared between different processes to be the same physical memory.

## Message Queues


## The CD Database Application


## IPC Status Commands

