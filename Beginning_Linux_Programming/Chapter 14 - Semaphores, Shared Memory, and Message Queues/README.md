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

```
#include <sys/shm.h>
void *shmat(int shm_id, const void *shm_addr, int shmflg);
int shmctl(int shm_id, int cmd, struct shmid_ds *buf);
int shmdt(const void *shm_addr);
int shmget(key_t key, size_t size, int shmflg);

```
As with semaphores, the include files sys/types.h and sys/ipc.h are normally automatically included by shm.h .
### shmget
You create shared memory using the shmget function:
```
int shmget(key_t key, size_t size, int shmflg);
```
If the shared memory is successfully created, shmget returns a nonnegative integer, the shared memory identifier. On failure, it returns –1.

### shmat

When you first create a shared memory segment, it’s not accessible by any process. To enable access to the shared memory, you must attach it to the address space of a process. You do this with the shmat function:
```
void *shmat(int shm_id, const void *shm_addr, int shmflg);
```
### shmdt
The shmdt function detaches the shared memory from the current process. It takes a pointer to the address returned by shmat . On success, it returns 0, on error –1. Note that detaching the shared memory doesn’t delete it; it just makes that memory unavailable to the current process.

### shmctl
The control functions for shared memory are (thankfully) somewhat simpler than the more complex ones for semaphores:
```
int shmctl(int shm_id, int command, struct shmid_ds *buf);
```
The shmid_ds structure has at least the following members:
```
struct shmid_ds {
uid_t shm_perm.uid;
uid_t shm_perm.gid;
mode_t shm_perm.mode;
}
```
The first parameter, shm_id , is the identifier returned from shmget .
The second parameter, command , is the action to take. It can take three values:
IPC_STAT Sets the data in the shmid_ds structure to reflect the values associated with the shared memory.
IPC_SET Sets the values associated with the shared memory to those provided in the shmid_ds data structure, if the process has permission to do so.
IPC_RMID Deletes the shared memory segment.

The third parameter, buf , is a pointer to the structure containing the modes and permissions for the shared memory.

#### Try It Out
* shm_com.h
* shm1.c 
* shm2.c
```
$ ./shm1 &
[1] 294
Memory attached at 40017000
$ ./shm2
Memory attached at 40017000
Enter some text: hello
You wrote: hello
waiting for client...
waiting for client...
Enter some text: Linux!
You wrote: Linux!
waiting for client...
waiting for client...
waiting for client...
Enter some text: end
You wrote: end
$
```

## Message Queues
We’ll now take a look at the third and final System V IPC facility: message queues. In many ways, message queues are like named pipes, but without the complexity associated with opening and closing the pipe.
However, using messages doesn’t get you away from the problems that you have with named pipes, such as blocking on full pipes.
Message queues provide a reasonably easy and efficient way of passing data between two unrelated processes. They have the advantage over named pipes that the message queue exists independently of both the sending and receiving processes, which removes some of the difficulties that occur in synchronizing the opening and closing of named pipes.

```
#include <sys/msg.h>

int msgctl(int msqid,int cmd, struct msqid_ds *buf);
int msgget(key_t key,int msgflg);
int msgrcv(int msqid,void *msg_ptr, size_t msg_sz, long int msgtype, int msgflg);
int msgsnd(int msqid,const void *msg_ptr, size_t msg_sz, int msgflg);
```
### msgget
You create and access a message queue using the msgget function:
```
int msgget(key_t key, int msgflg);

```
### msgsnd
The msgsnd function allows you to add a message to a message queue:
```
int msgsnd(int msqid, const void *msg_ptr, size_t msg_sz, int msgflg);
```
The structure of the message is constrained in two ways. First, it must be smaller than the system limit, and second, it must start with a long int , which will be used as a message type in the receive function.
When you’re using messages, it’s best to define your message structure something like this:
```
struct my_message {
long int message_type;
/* The data you wish to transfer */
}
```
Because the message_type is used in message reception, you can’t simply ignore it. You must declare your data structure to include it, and it’s also wise to initialize it so that it contains a known value.
The first parameter, msqid , is the message queue identifier returned from a msgget function. 
The second parameter, msg_ptr , is a pointer to the message to be sent, which must start with a long int type as described previously.
The third parameter, msg_sz , is the size of the message pointed to by msg_ptr . This size must not include the long int message type.
The fourth parameter, msgflg , controls what happens if either the current message queue is full or the systemwide limit on queued messages has been reached.
### msgrcv
The msgrcv function retrieves messages from a message queue:
```
int msgrcv(int msqid, void *msg_ptr, size_t msg_sz, long int msgtype, int msgflg);
```
### msgctl
The final message queue function is msgctl , which is very similar to that of the control function for shared memory:
```
int msgctl(int msqid, int command, struct msqid_ds *buf);
```
The msqid_ds structure has at least the following members:
```
struct msqid_ds {
uid_t msg_perm.uid;
uid_t msg_perm.gid
mode_t msg_perm.mode;
}
```

#### Try It Out
* msg1.c
* msg2.c
```
$ ./msg2
$ ./msg1
$
```
## The CD Database Application


## IPC Status Commands
Although they’re not required for X/Open compliance, most Linux systems provide a set of commands that allow command-line access to IPC information, and to tidy up stray IPC facilities. These are the ipcs and ipcrm commands, which are very useful when you’re developing programs.
One of the irritations of the IPC facilities is that a poorly written program, or a program that fails for some reason, can leave its IPC resources (such as data in a message queue) loitering on the system long after the program completes. This can cause a new invocation of the program to fail, because the program expects to start with a clean system, but actually finds some leftover resource. The status ( ipcs ) and remove ( ipcrm ) commands provide a way of checking and tidying up IPC facilities.
### Displaying Semaphore Status
To examine the state of semaphores on the system, use the ipcs -s command. If any semaphores are present, the output will have this form:
```
$ ./ipcs -s
```
You can use the ipcrm command to remove any semaphores accidentally left by programs. To delete the preceding semaphore, the command (on Linux) is
```
$ ./ipcrm -s 768
```
Some much older Linux systems used to use a slightly different syntax:
```
$ ./ipcrm sem 768
```
but that style is now rare. Check the manual pages for your system to see which format is valid on your particular system.

### Displaying Shared Memory Status
Like semaphores, many systems provide command-line programs for accessing the details of shared memory. These are ipcs -m and ipcrm -m <id> (or ipcrm shm <id> ).
Here’s some sample output from ipcs -m :
```
$ ipcs -m
```
The ipcrm -m <id> command allows shared memory to be removed.

### Displaying Message Queue Status
For message queues the commands are ipcs -q and ipcrm -q <id> (or ipcrm msg <id> ).
Here’s some sample output from ipcs -q :
```
$ ipcs -q
```
The ipcrm -q <id> command allows a message queue to be removed.