# Process Management

## The Process
A process is a program (object code stored on some media) in the midst of execution.
They also include a set of resources such as open files and pending signals, internal kernel data, processor state, a memory address space with one or more memory mappings, one or more threads of execution, and a data section containing global variables.
Processes, in effect, are the living result of running program code.The kernel needs to manage all these details efficiently and transparently.
Threads of execution, often shortened to threads, are the objects of activity within the process. Each thread includes a unique program counter, process stack, and set of processor registers.The kernel schedules individual threads, not processes.
On modern operating systems, processes provide two virtualizations: a virtualized processor and virtual memory.The virtual processor gives the process the illusion that it alone monopolizes the system, despite possibly sharing the processor among hundreds of other processes.
A program itself is not a process; a process is an active program and related resources.
Indeed, two or more processes can exist that are executing the same program. In fact, two or more processes can exist that share various resources, such as open files or an address space.
A process begins its life when, not surprisingly, it is created. In Linux, this occurs by means of the fork() system call, which creates a new process by duplicating an existing one.The process that calls fork() is the parent, whereas the new process is the child.The parent resumes execution and the child starts execution at the same place: where the call to fork() returns.The fork() system call returns from the kernel twice: once in the parent process and again in the newborn child.
Often, immediately after a fork it is desirable to execute a new, different program.The exec() family of function calls creates a new address space and loads a new program into it. In contemporary Linux kernels, fork() is actually implemented via the clone() system call, which is discussed in a following section.

## Process Descriptor and the Task Structure
The kernel stores the list of processes in a circular doubly linked list called the task list. Each element in the task list is a process descriptor of the type struct task_struct , which is defined in <linux/sched.h> .The process descriptor contains all the information about a specific process.
The task_struct is a relatively large data structure, at around 1.7 kilobytes on a 32-bit machine.This size, however, is quite small considering that the structure contains all the information that the kernel has and needs about a process.

### Allocating the Process Descriptor
The task_struct structure is allocated via the slab allocator to provide object reuse and cache coloring.

### Storing the Process Descriptor

An opaque type is a data type whose physical representation is unknown or irrelevant.
The system identifies processes by a unique process identification value or PID.The PID is a numerical value represented by the opaque type pid_t , which is typically an int .

### Process State

### Manipulating the Current Process State

### Process Context

### The Process Family Tree


## Process Creation

## The Linux Implementation of Threads

## Process Termination
