# Summary - Processes and Signals

## What Is a Process?
process = “an address space with one or more threads executing within that address space, and the required system resources for those threads.”

## Process Structure

```
ps -ef
```
Each process is allocated a unique number, called a process identifier or PID. This is usually a positive integer between 2 and 32,768. When a process is started, the next unused number in sequence is chosen and the numbers restart at 2 so that they wrap around. The number 1 is typically reserved for the special init process, which manages other processes.

### The Process Table

The Linux process table is like a data structure describing all of the processes that are currently loaded with, for example, their PID, status, and command string, the sort of information output by ps . The operating system manages processes using their PIDs, and they are used as an index into the process table. The table is of limited size, so the number of processes a system will support is limited. Early UNIX systems were limited to 256 processes. More modern implementations have relaxed this restriction considerably and may be limited only by the memory available to construct a process table entry.

### Viewing Processes

```
ps -ef # The ps command shows the processes you’re running, the process another user is running, or all the processes on the system.
```
By default, the ps program shows only processes that maintain a connection with a terminal, a console, a serial line, or a pseudo terminal. Other processes run without needing to communicate with a user on a terminal. These are typically system processes that Linux uses to manage shared resources. You can use ps to see all such processes using the -e option and to get “full” information with -f .


### System Processes

The STAT output from ps provides codes indicating the current status.

STAT Code         - Description
* S               - Sleeping. Usually waiting for an event to occur, such as a signal or input to become available.
* R               -  Running. Strictly speaking, “runnable,” that is, on the run queue either executing or about to run.
* D               -  Uninterruptible Sleep (Waiting). Usually waiting for input or output to complete.
* T               -  Stopped. Usually stopped by shell job control or the process is under the control of a debugger.
* Z               -  Defunct or “zombie” process.
* N               -  Low priority task, “nice.”
* W               -  Paging. (Not for Linux kernel 2.6 onwards.)
* s               -  Process is a session leader.
* '+'             -    Process is in the foreground process group.
* l               -  Process is multithreaded.
* <               -  High priority task.
```
ps ax
```
init = the operating system process manager and the grandparent of all processes (Ss)
R+ = Ready to run, the program is a foreground task not waiting for other processes to finish or waiting for input or output to complete.
### Process Scheduling

Well-behaved programs are termed nice programs, and in a sense this “niceness” can be measured. The operating system determines the priority of a process based on a “nice” value, which defaults to 0 , and on the behavior of the program. Programs that run for long periods without pausing generally get lower priorities. Programs that pause while, for example, waiting for input, get rewarded. This helps keep a program that interacts with the user responsive; while it is waiting for some input from the user, the  system increases its priority, so that when it’s ready to resume, it has a high priority. You can set the process nice value using nice and adjust it using renice . The nice command increases the nice value of a process by 10, giving it a lower priority. You can view the nice values of active processes using the –l or –f (for long output) option to ps .

```
ps -l
nice oclock &
renice 10 1362
ps x
```
## Starting New Processes
You can cause a program to run from inside another program and thereby create a new process by using the system library function.

```
#include <stdlib.h>
int system (const char *string);
```
The system function runs the command passed to it as a string and waits for it to complete. The command is executed as if the command
```
$ sh -c string
```
has been given to a shell. system returns 127 if a shell can’t be started to run the command and -1 if another error occurs. Otherwise, system returns the exit code of the command.

#### Try It Out
* system1.c
* system2.c

## Replacing a Process Image

There is a whole family of related functions grouped under the exec heading. They differ in the way that they start processes and present program arguments. An exec function replaces the current process with a new process specified by the path or file argument. You can use exec functions to “hand off” execution of your program to another. For example, you could check the user’s credentials before starting another application that has a restricted usage policy. The exec functions are more efficient than system because the original program will no longer be running after the new one is started.

```
#include <unistd.h>
char **environ;
int execl(const char *path, const char *arg0, ..., (char *)0);
int execlp(const char *file, const char *arg0, ..., (char *)0);
int execle(const char *path, const char *arg0, ..., (char *)0, char *const envp[]);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execve(const char *path, char *const argv[], char *const envp[]);
```
These functions belong to two types. execl , execlp , and execle take a variable number of arguments ending with a null pointer. execv and execvp have as their second argument an array of strings. In both cases, the new program starts with the given arguments appearing in the argv array passed to main .
These functions are usually implemented using execve.

#### Try It Out
* pexec.c

### Duplicating a Process Image

You can create a new process by calling fork . This system call duplicates the current process, creating a new entry in the process table with many of the same attributes as the current process. The new process is almost identical to the original, executing the same code but with its own data space, environment, and file descriptors. Combined with the exec functions, fork is all you need to create new processes.
```
#include <sys/types.h>
#include <unistd.h>
pid_t fork(void);
```
If fork fails, it returns -1 .
#### Try It Out
* fork1.c

### Waiting for a Process
When you start a child process with fork , it takes on a life of its own and runs independently. Sometimes, you would like to find out when a child process has finished. For example, in the previous program, the parent finishes ahead of the child and you get some messy output as the child continues to run. You can arrange for the parent process to wait until the child finishes before continuing by calling wait .

```
#include <sys/types.h>
#include <sys/wait.h>
pid_t wait(int *stat_loc);
```

The wait system call causes a parent process to pause until one of its child processes is stopped. The call returns the PID of the child process. This will normally be a child process that has terminated. The status information allows the parent process to determine the exit status of the child process, that is, the value returned from main or passed to exit . If stat_loc is not a null pointer, the status information will be written to the location to which it points. 
You can interpret the status information using macros defined in sys/wait.h , shown in the following table.
Macro  -  Definition
* WIFEXITED(stat_val) - Nonzero if the child is terminated normally.
* WEXITSTATUS(stat_val) - If WIFEXITED is nonzero, this returns child exit code.
* WIFSIGNALED(stat_val) - Nonzero if the child is terminated on an uncaught signal.
* WTERMSIG(stat_val) - If WIFSIGNALED is nonzero, this returns a signal number.
* WIFSTOPPED(stat_val) - Nonzero if the child has stopped.
* WSTOPSIG(stat_val) - If WIFSTOPPED is nonzero, this returns a signal number.

#### Try It Out
* wait.c
### Zombie Processes

Using fork to create processes can be very useful, but you must keep track of child processes. When a child process terminates, an association with its parent survives until the parent in turn either terminates normally or calls wait . The child process entry in the process table is therefore not freed up immediately. Although no longer active, the child process is still in the system because its exit code needs to be stored in case the parent subsequently calls wait . It becomes what is known as defunct, or a zombie process.
You can see a zombie process being created if you change the number of messages in the fork example program. If the child prints fewer messages than the parent, it will finish first and will exist as a zombie until the parent has finished.
#### Try It Out
* fork2.c

```
#include <sys/types.h>
#include <sys/wait.h>
pid_t waitpid(pid_t pid, int *stat_loc, int options);
```

### Input and Output Redirection
#### Try It Out
* upper.c
* useupper.c
* file.txt
```
./upper
cat file.txt
./upper < file.txt
./useupper file.txt
./upper < file.txt
```

### Threads

Linux processes can cooperate, can send each other messages, and can interrupt one another. They can even arrange to share segments of memory between themselves, but they are essentially separate entities within the operating system. They do not readily share variables.
There is a class of process known as a thread that is available in many UNIX and Linux systems. Though threads can be difficult to program, they can be of great value in some applications, such as multithreaded database servers. Programming threads on Linux (and UNIX generally) is not as common as using multiple processes, because Linux processes are quite lightweight, and programming multiple cooperation processes is much easier than programming threads.

## Signals

A signal is an event generated by the UNIX and Linux systems in response to some condition, upon receipt of which a process may in turn take some action. We use the term raise to indicate the generation of a signal, and the term catch to indicate the receipt of a signal. Signals are raised by some error conditions, such as memory segment violations, floating-point processor errors, or illegal instructions. They are generated by the shell and terminal handlers to cause interrupts and can also be explicitly sent from one process to another as a way of passing information or modifying behavior. In all these cases, the programming interface is the same. Signals can be raised, caught and acted upon, or (for some at least) ignored.
Signal names are defined by including the header file signal.h . They all begin with SIG and include those listed in the following table.

Signal Name - Description
* SIGABORT - *Process abort
* SIGALRM - Alarm clock
* SIGFPE - *Floating-point exception
* SIGHUP - Hangup
* SIGILL - *Illegal instruction
* SIGINT - Terminal interrupt
* SIGKILL - Kill (can’t be caught or ignored)
* SIGPIPE - Write on a pipe with no reader
* SIGQUIT - Terminal quit
* SIGSEGV - *Invalid memory segment access
* SIGTERM - Termination
* SIGUSR1 - User-defined signal 1
* SIGUSR2 - User-defined signal 2
* SIGCHLD - Child process has stopped or exited.
* SIGCONT - Continue executing, if stopped.
* SIGSTOP - Stop executing. (Can’t be caught or ignored.)
* SIGTSTP - Terminal stop signal.
* SIGTTIN - Background process trying to read.
* SIGTTOU - Background process trying to write.
* SIG_IGN - Ignore the signal.
* SIG_DFL - Restore default behavior.
```
kill –HUP 512
killall –HUP inetd
```
Programs can handle signals using the signal library function.
```
#include <signal.h>
void (*signal(int sig, void (*func)(int)))(int);
```
#### Try It Out
* ctrlc1.c

### Sending Signals

A process may send a signal to another process, including itself, by calling kill . The call will fail if the program doesn’t have permission to send the signal, often because the target process is owned by another user. This is the program equivalent of the shell command of the same name.
```
#include <sys/types.h>
#include <signal.h>
int kill(pid_t pid, int sig);
```
The kill function sends the specified signal, sig , to the process whose identifier is given by pid . It returns 0 on success.

```
#include <unistd.h>
unsigned int alarm(unsigned int seconds);
```
#### Try It Out
* alarm.c

This program introduces a new function, pause , which simply causes the program to suspend execution until a signal occurs. When it receives a signal, any established handler is run and execution continues as normal. It’s declared as
```
#include <unistd.h>
int pause(void);
```
and returns -1 (if the next received signal doesn’t cause the program to terminate) with errno set to EINTR when interrupted by a signal.

### A Robust Signals Interface
We’ve covered raising and catching signals using signal and friends in some depth, because they are very common in older UNIX programs. However, the X/Open and UNIX specifications recommend a newer programming interface for signals that is more robust: sigaction .

```
#include <signal.h>
int sigaction(int sig, const struct sigaction *act, struct sigaction *oact);
```
The sigaction function sets the action associated with the signal sig . If oact is not null, sigaction writes the previous signal action to the location it refers to. If act is null, this is all sigaction does. If act isn’t null, the action for the specified signal is set. As with signal , sigaction returns 0 if successful and -1 if not. The error variable errno will be set to EINVAL if the specified signal is invalid or if an attempt is made to catch or ignore a signal that can’t be caught or ignored.
Within the sigaction structure pointed to by the argument act , sa_handler is a pointer to a function called when signal sig is received. This is much like the function func you saw earlier passed to signal . You can use the special values SIG_IGN and SIG_DFL in the sa_handler field to indicate that the signal is to be ignored or the action is to be restored to its default, respectively.
The sa_mask field specifies a set of signals to be added to the process’s signal mask before the sa_handler function is called. These are the set of signals that are blocked and won’t be delivered to the process. This prevents the case you saw earlier where a signal is received before its handler has run to completion. Using the sa_mask field can eliminate this race condition.
#### Try It Out
* ctrlc2.c

### Signal Sets

The header file signal.h defines the type sigset_t and functions used to manipulate sets of signals. These sets are used in sigaction and other functions to modify process behavior on receipt of signals.
```
#include <signal.h>
int sigaddset(sigset_t *set, int signo);
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigdelset(sigset_t *set, int signo);
```
These functions perform the operations suggested by their names. sigemptyset initializes a signal set to be empty. sigfillset initializes a signal set to contain all defined signals. sigaddset and sigdelset add and delete a specified signal ( signo ) from a signal set. They all return 0 if successful and -1 with errno set on error. The only error defined is EINVAL if the specified signal is invalid.
The function sigismember determines whether the given signal is a member of a signal set. It returns 1 if the signal is a member of the set, 0 if it isn’t, and -1 with errno set to EINVAL if the signal is invalid.
```
#include <signal.h>
int sigismember(sigset_t *set, int signo);
```
The process signal mask is set or examined by calling the function sigprocmask . This signal mask is the set of signals that are currently blocked and will therefore not be received by the current process.
```
#include <signal.h>
int sigprocmask(int how, const sigset_t *set, sigset_t *oset);
```
sigprocmask can change the process signal mask in a number of ways according to the how argument. New values for the signal mask are passed in the argument set if it isn’t null, and the previous signal mask will be written to the signal set oset .

The how argument can be one of the following:
* SIG_BLOCK The signals in set are added to the signal mask.
* SIG_SETMASK The signal mask is set from set .
* SIG_UNBLOCK The signals in set are removed from the signal mask.

If a signal is blocked by a process, it won’t be delivered, but will remain pending. A program can determine which of its blocked signals are pending by calling the function sigpending .
```
#include <signal.h>
int sigpending(sigset_t *set);
```
This writes a set of signals that are blocked from delivery and are pending into the signal set pointed to by set . It returns 0 if successful, otherwise, -1 with errno set to indicate the error. This function can be useful when a program needs to handle signals and to control when the handling function is called.
A process can suspend execution until the delivery of one of a set of signals by calling sigsuspend . This is a more general form of the pause function you met earlier.
```
#include <signal.h>
int sigsuspend(const sigset_t *sigmask);
```
The sigsuspend function replaces the process signal mask with the signal set given by sigmask and then suspends execution. It will resume after the execution of a signal handling function. If the received signal terminates the program, sigsuspend will never return. If a received signal doesn’t terminate the program, sigsuspend returns -1 with errno set to EINTR .

### sigaction Flags

The sa_flags field of the sigaction structure used in sigaction may contain the values shown in the following table to modify signal behavior:

* SA_NOCLDSTOP Don’t generate SIGCHLD when child processes stop.
* SA_RESETHAND Reset signal action to SIG_DFL on receipt.
* SA_RESTART Restart interruptible functions rather than error with EINTR .
* SA_NODEFER Don’t add the signal to the signal mask when caught

### Common Signal Reference

The default action for the signals in the following table is abnormal termination of the process with all the consequences of _exit (which is like exit but performs no cleanup before returning to the kernel). However, the status is made available to wait , and waitpid indicates abnormal termination by the specified signal.
Signal Name - Description
* SIGALRM Generated by the timer set by the alarm function.
* SIGHUP Sent to the controlling process by a disconnecting terminal, or by the controlling process on termination to each foreground process.
* SIGINT Typically raised from the terminal by typing Ctrl+C or the configured interrupt character.
* SIGKILL Typically used from the shell to forcibly terminate an errant process, because this signal can’t be caught or ignored.
* SIGPIPE Generated by an attempt to write to a pipe with no associated reader.
* SIGTERM Sent as a request for a process to finish. Used by UNIX when shutting down to request that system services stop. This is the default signal sent from the kill command.
* SIGUSR1, SIGUSR2 May be used by processes to communicate with each other, possibly to cause them to report status information.

By default, the signals in the next table also cause abnormal termination:
* SIGFPE Generated by a floating-point arithmetic exception.
* SIGILL An illegal instruction has been executed by the processor. Usually caused by a corrupt program or invalid shared memory module.
* SIGQUIT Typically raised from the terminal by typing Ctrl+\ or the configured quit character.
* SIGSEGV A segmentation violation, usually caused by reading or writing at an illegal location in memory either by exceeding array bounds or dereferencing an invalid pointer. Overwriting a local array variable and corrupting the stack can cause a SIGSEGV to be raised when a function returns to an illegal address.

A process is suspended by default on receipt of one of the signals in the following:

* SIGSTOP Stop executing (can’t be caught or ignored).
* SIGTSTP Terminal stop signal, often raised by typing Ctrl+Z.
* SIGTTIN, SIGTTOU Used by the shell to indicate that background jobs have stopped because they need to read from the terminal or produce output.

SIGCONT restarts a stopped process and is ignored if received by a process that is not stopped. 
The SIGCHLD signal is ignored by default.
* SIGCONT Continue executing, if stopped.
* SIGCHLD Raised when a child process stops or exits.