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
$ rm popen1
```
## Sending Output to popen

Now that you’ve seen an example of capturing output from an external program, let’s look at sending output to an external program. Here’s a program, popen2.c , that pipes data to another. Here, you’ll use od (octal dump).

####  Try It Out
* popen2.c
```
$ gcc -o popen2 popen2.c
$ ./popen2
$ echo “Once upon a time, there was...” | od -c
$ rm popen2
```
How It Works?
The program uses popen with the parameter “w” to start the od -c command, so that it can send data to that command. It then sends a string that the od -c command receives and processes; the od -c command then prints the result of the processing on its standard output.
## Passing More Data

The mechanism that you’ve used so far simply sends or receives all the data in a single fread or fwrite . Sometimes you may want to send the data in smaller pieces, or perhaps you may not know the size of the output. To avoid having to declare a very large buffer, you can just use multiple fread or fwrite calls and process the data in parts
####  Try It Out
* popen3.c
```
$ gcc -o popen3 popen3.c
$ ./popen3
$ rm popen3
```
## How popen Is Implemented

####  Try It Out
* popen4.c
```
$ gcc -o popen4 popen4.c
$ ./popen4
$ rm popen4
```
How It Works?
The program shows that the shell is being invoked to expand popen*.c to the list of all files starting popen and ending in .c and also to process the pipe ( | ) symbol and feed the output from cat into wc . You invoke the shell, the cat program, and wc and cause an output redirection, all in a single popen call. The program that invokes the command sees only the final output.

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
* pipe1.c
```
$ gcc -o pipe1 pipe1.c
$ ./pipe1
$ rm pipe1
```
How It Works
The program creates a pipe using the two file descriptors in the array file_pipes[] . It then writes data into the pipe using the file descriptor file_pipes[1] and reads it back from file_pipes[0] . Notice that the pipe has some internal buffering that stores the data in between the calls to write and read .
You should be aware that the effect of trying to write using file_descriptor[0] , or read using file_descriptor[1] , is undefined, so the behavior could be very strange and may change without warning. On the authors’ systems, such calls fail with a –1 return value, which at least ensures that it’s easy to catch this mistake.
At first glance, this example of a pipe doesn’t seem to offer us anything that we couldn’t have done with a simple file. The real advantage of pipes comes when you want to pass data between two processes.

####  Try It Out
* pipe2.c
```
$ gcc -o pipe2 pipe2.c
$ ./pipe2
$ rm pipe2
```

## Parent and Child Processes
The next logical step in our investigation of the pipe call is to allow the child process to be a different program from its parent, rather than just a different process running the same program. You do this using the exec call. One difficulty is that the new exec ed process needs to know which file descriptor to access. In the previous example, this wasn’t a problem because the child had access to its copy of the file_pipes data. After an exec call, this will no longer be the case, because the old process has been replaced by the new child process. You can get around this by passing the file descriptor (which is, after all, just a number) as a parameter to the newly exec ed program.
To show how this works, you need two programs. The first is the data producer. It creates the pipe and then invokes the child, the data consumer.
####  Try It Out
* pipe3.c
* pipe4.c
```
$ gcc -o pipe3 pipe3.c
$ ./pipe3
$ rm pipe3
$ gcc -o pipe4 pipe4.c
$ ./pipe4
$ rm pipe4
```
### Reading Closed Pipes
A read call will normally block; that is, it will cause the process to wait until data becomes available. If the other end of the pipe has been closed, then no process has the pipe open for writing, and the read blocks. Because this isn’t very helpful, a read on a pipe that isn’t open for writing returns zero rather than blocking. This allows the reading process to detect the pipe equivalent of end of file and act appropriately. Notice that this isn’t the same as reading an invalid file descriptor, which read considers an error and indicates by returning –1.
If you use a pipe across a fork call, there are two different file descriptors that you can use to write to the pipe: one in the parent and one in the child. You must close the write file descriptors of the pipe in both parent and child processes before the pipe is considered closed and a read call on the pipe will fail. You’ll see an example of this later when we return to this subject in more detail to look at the O_NONBLOCK flag and FIFOs.
#### Pipes Used as Standard Input and Output 

The one big advantage is that you can invoke standard programs, ones that don’t expect a file descriptor as a parameter. In order to do this, you need to use the dup function, which you met in Chapter 3. There are two closely related versions of dup that have the following prototypes:
```
#include <unistd.h>
int dup(int file_descriptor);
int dup2(int file_descriptor_one, int file_descriptor_two);
```
The purpose of the dup call is to open a new file descriptor, a little like the open call. The difference is that the new file descriptor created by dup refers to the same file (or pipe) as an existing file descriptor.
In the case of dup , the new file descriptor is always the lowest number available, and in the case of dup2 it’s the same as, or the first available descriptor greater than, the parameter file_descriptor_two . You can get the same effect as dup and dup2 by using the more general fcntl call, with a command F_DUPFD . Having said that, the dup call is easier to use because it’s tailored specifically to the needs of creating duplicate file descriptors. It’s also very commonly used, so you’ll find it more frequently in existing programs than fcntl and F_DUPFD .

#### File Descriptor Manipulation by close and dup
####  Try It Out
* pipe5.c
```
$ gcc -o pipe5 pipe5.c
$ ./pipe5
$ rm pipe5
```
## Named Pipes: FIFOs

So far, you have only been able to pass data between related programs, that is, programs that have been started from a common ancestor process. Often this isn’t very convenient, because you would like unrelated processes to be able to exchange data.
You do this with FIFOs, often referred to as named pipes. A named pipe is a special type of file (remember that everything in Linux is a file!) that exists as a name in the file system but behaves like the unnamed pipes that you’ve met already.
You can create named pipes from the command line and from within a program. Historically, the command-line program for creating them was mknod :
```
$ mknod filename p
```
However, the mknod command is not in the X/Open command list, so it may not be available on all UNIX-like systems. The preferred command-line method is to use
```
$ mkfifo filename
```
Some older versions of UNIX only had the mknod command. X/Open Issue 4 Version 2 has the mknod function call, but not the command-line program. Linux, friendly as ever, supplies both mknod and mkfifo .
From inside a program, you can use two different calls:
```
#include <sys/types.h>
#include <sys/stat.h>
int mkfifo(const char *filename, mode_t mode);
int mknod(const char *filename, mode_t mode | S_IFIFO, (dev_t) 0);
```
Like the mknod command, you can use the mknod function for making many special types of files. Using a dev_t value of 0 and ORing the file access mode with S_IFIFO is the only portable use of this function that creates a named pipe. We’ll use the simpler mkfifo function in the examples.

####  Try It Out
* fifo1.c
```
$ gcc -o fifo1 fifo1.c
$ ./fifo1
$ rm fifo1
```

### Accessing a FIFO

One very useful feature of named pipes is that, because they appear in the file system, you can use them in commands where you would normally use a filename. Before you do more programming using the FIFO file you created, let’s investigate the behavior of the FIFO file using normal file commands.
####  Try It Out
```
# 1. First, try reading the (empty) FIFO:
$ cat < /tmp/my_fifo
# 2. Now try writing to the FIFO. You will have to use a different terminal because the first command will now be hanging, waiting for some data to appear in the FIFO.
$ echo “Hello World” > /tmp/my_fifo
#You will see the output appear from the cat command. If you don’t send any data down the FIFO, the cat command will hang until you interrupt it, conventionally with Ctrl+C.
# 3. You can do both at once by putting the first command in the background:
$ cat < /tmp/my_fifo &
$ echo “Hello World” > /tmp/my_fifo
```

#### Opening a FIFO with open
The main restriction on opening FIFOs is that a program may not open a FIFO for reading and writing with the mode O_RDWR . If a program violates this restriction, the result is undefined. This is quite a sensible restriction because, normally, you use a FIFO only for passing data in a single direction, so there is no need for an O_RDWR mode. A process would read its own output back from a pipe if it were opened read/write.
####  Try It Out
```
open(const char *path, O_RDONLY);
open(const char *path, O_RDONLY | O_NONBLOCK);
open(const char *path, O_WRONLY);
open(const char *path, O_WRONLY | O_NONBLOCK);
```
* fifo2.c

#### O_RDONLY and O_WRONLY without O_NONBLOCK
You now have your test program, so you can try out a couple of combinations. Notice that the first program, the reader, has been put in the background:
```
$ ./fifo2 O_RDONLY &
[1] 152
Process 152 opening FIFO
$ ./fifo2 O_WRONLY
Process 153 opening FIFO
Process 152 result 3
Process 153 result 3
Process 152 finished
Process 153 finished
```
This is probably the most common use of named pipes. It allows the reader process to start and wait in the open call and then allows both programs to continue when the second program opens the FIFO.
Notice that both the reader and writer processes have synchronized at the open call. When a Linux process is blocked, it doesn’t consume CPU resources, so this method of process synchronization is very CPU-efficient.

#### O_RDONLY with O_NONBLOCK and O_WRONLY
In the following example, the reader process executes the open call and continues immediately, even though no writer process is present. The writer also immediately continues past the open call, because the FIFO is already open for reading.
```
$ ./fifo2 O_RDONLY O_NONBLOCK &
[1] 160
Process 160 opening FIFO
$ ./fifo2 O_WRONLY
Process 161 opening FIFO
Process 160 result 3
Process 161 result 3
Process 160 finished
Process 161 finished
[1]+ Done           ./fifo2 O_RDONLY O_NONBLOCK
```
These two examples are probably the most common combinations of open modes. Feel free to use the
example program to experiment with some other combinations.

### Reading and Writing FIFOs
####  Try It Out
* fifo3.c
* fifo4.c
```
$ gcc -o fifo3 fifo3.c
$ ./fifo3
$ rm fifo3
$ gcc -o fifo4 fifo4.c
$ ./fifo4
$ rm fifo4
```
### Advanced Topic: Client/Server Using FIFOs
For your final look at FIFOs, let’s consider how you might build a very simple client/server application using named pipes. You want to have a single-server process that accepts requests, processes them, and returns the resulting data to the requesting party: the client.
You want to allow multiple client processes to send data to the server. In the interests of simplicity, we’ll assume that the data to be processed can be broken into blocks, each smaller than PIPE_BUF bytes. Of course, you could implement this system in many ways, but we’ll consider only one method as an illustration of how named pipes can be used.
Because the server will process only one block of information at a time, it seems logical to have a single FIFO that is read by the server and written to by each of the clients. By opening the FIFO in blocking mode, the server and the clients will be automatically blocked as required.
####  Try It Out
* client.h
* server.c
* client.c
```
$ ./server &
$ for i in 1 2 3 4 5
do
./client &
done
$
```

## The CD Database Application

### The Pipe
### Client-Side Functions
