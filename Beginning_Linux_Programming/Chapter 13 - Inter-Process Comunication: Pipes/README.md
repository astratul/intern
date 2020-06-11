# Summary - Inter-Process Communication: Pipes

We cover the following topics in this chapter:
* The definition of a pipe
* Process pipes
* Pipe calls
* Parent and child processes
* Named pipes: FIFOs
* Client/server considerations

## What Is a Pipe?

We use the term pipe to mean connecting a data flow from one process to another. Generally you attach, or pipe, the output of one process to the input of another.
Most Linux users will already be familiar with the idea of a pipeline, linking shell commands together so that the output of one process is fed straight to the input of another. For shell commands, this is done using the pipe character to join the commands, such as
```
cmd1 | cmd2
```

The shell arranges the standard input and output of the two commands, so that
* The standard input to cmd1 comes from the terminal keyboard.
* The standard output from cmd1 is fed to cmd2 as its standard input.
* The standard output from cmd2 is connected to the terminal screen.
What the shell has done, in effect, is reconnect the standard input and output streams so that data flows
from the keyboard input through the two commands and is then output to the screen.

## Process Pipes
Perhaps the simplest way of passing data between two programs is with the popen and pclose functions. These have the following prototypes:
```
#include <stdio.h>
FILE *popen(const char *command, const char *open_mode);
int pclose(FILE *stream_to_close);
```
### popen

The popen function allows a program to invoke another program as a new process and either pass data to it or receive data from it. The command string is the name of the program to run, together with any parameters. open_mode must be either “r” or “w” .
If the open_mode is “r” , output from the invoked program is made available to the invoking program and can be read from the file stream FILE * returned by popen , using the usual stdio library functions for reading (for example, fread ). However, if open_mode is “w” , the program can send data to the invoked command with calls to fwrite . The invoked program can then read the data on its standard input. Normally, the program being invoked won’t be aware that it’s reading data from another process;it simply reads its standard input stream and acts on it.
A call to popen must specify either “r” or “w” ; no other option is supported in a standard implementation of popen . This means that you can’t invoke another program and both read from and write to it. On failure, popen returns a null pointer. If you want bidirectional communication using pipes, the normal solution is to use two pipes, one for data flow in each direction.

### pclose

When the process started with popen has finished, you can close the file stream associated with it using pclose . The pclose call will return only when the process started with popen finishes. If it’s still running when pclose is called, the pclose call will wait for the process to finish.
The pclose call normally returns the exit code of the process whose file stream it is closing. If the invoking process has already executed a wait statement before calling pclose , the exit status will be lost because the invoked process has finished and pclose will return –1, with errno set to ECHILD .

####  Try It Out
* popen1.c
```
$ gcc -o popen1 popen1.c
$ ./popen1
```
## Sending Output to popen

Now that you’ve seen an example of capturing output from an external program, let’s look at sending output to an external program. Here’s a program, popen2.c , that pipes data to another. Here, you’ll use od (octal dump).

####  Try It Out
* popen2.c
```
$ gcc -o popen2 popen2.c
$ ./popen2
$ echo “Once upon a time, there was...” | od -c
```
## Passing More Data

The mechanism that you’ve used so far simply sends or receives all the data in a single fread or fwrite . Sometimes you may want to send the data in smaller pieces, or perhaps you may not know the size of the output. To avoid having to declare a very large buffer, you can just use multiple fread or fwrite calls and process the data in parts
####  Try It Out
* popen3.c
```
$ gcc -o popen3 popen3.c
$ ./popen3
```
## How popen Is Implemented

####  Try It Out
* popen4.c
```
$ gcc -o popen4 popen4.c
$ ./popen4
```

## The Pipe Call

You’ve seen the high-level popen function, but now let’s move on to look at the lower-level pipe function. This function provides a means of passing data between two programs, without the overhead of invoking a shell to interpret the requested command. It also gives you more control over the reading and writing of data.

```
#include <unistd.h>
int pipe(int file_descriptor[2]);
```
pipe is passed (a pointer to) an array of two integer file descriptors. It fills the array with two new file descriptors and returns a zero. On failure, it returns -1 and sets errno to indicate the reason for failure. Errors defined in the Linux manual page for pipe (in section 2 of the manual) are
* EMFILE : Too many file descriptors are in use by the process.
* ENFILE : The system file table is full.
* EFAULT : The file descriptor is not valid.

####  Try It Out
* pipe2.c
```
$ gcc -o pipe2 pipe2.c
$ ./pipe2
```

## Parent and Child Processes
The next logical step in our investigation of the pipe call is to allow the child process to be a different program from its parent, rather than just a different process running the same program. You do this using the exec call. One difficulty is that the new exec ed process needs to know which file descriptor to access. In the previous example, this wasn’t a problem because the child had access to its copy of the file_pipes data. After an exec call, this will no longer be the case, because the old process has been replaced by the new child process. You can get around this by passing the file descriptor (which is, after all, just a number) as a parameter to the newly exec ed program.
To show how this works, you need two programs. The first is the data producer. It creates the pipe and then invokes the child, the data consumer.
####  Try It Out
* 
```

```


## Named Pipes: FIFOs

## The CD Database Application

