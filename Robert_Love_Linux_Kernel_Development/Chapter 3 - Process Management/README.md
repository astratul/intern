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
The state field of the process descriptor describes the current condition of the process.
TASK_RUNNING(Is runnable);
TASK_INTERRUPTIBLE(Is sleeping);
TASK_UNINTERRUPTIBLE(like TASK_INTERRUPTIBLE exept that it does not wake up and become runnable if it receives a signal.);
_TASK_TRACED(The process is being traced by another process, such as a debugger, via ptrace.);
_TASK_STOPPED(Proccess execution has stopped);

### Manipulating the Current Process State
Kernel code often needs to change a process’s state.
set_task_state(task, state);

### Process Context

### The Process Family Tree
A distinct hierarchy exists between processes in Unix systems, and Linux is no exception. All processes are descendants of the init process, whose PID is one.The kernel starts init in the last step of the boot process.The init process, in turn, reads the system initscripts and executes more programs, eventually completing the boot process.
Every process on the system has exactly one parent. Likewise, every process has zero or more children. Processes that are all direct children of the same parent are called siblings. The relationship between processes is stored in the process descriptor.
## Process Creation
### Copy-on-Write
Traditionally, upon fork(), all resources owned by the parent are duplicated and the copy is given to the child.This approach is naive and inefficient in that it copies much data that might otherwise be shared.Worse still, if the new process were to immediately execute a new image, all that copying would go to waste. In Linux, fork() is implemented through the use of copy-on-write pages. Copy-on-write (or COW) is a technique to delay or altogether prevent copying of the data. Rather than duplicate the process address space, the parent and the child can share a single copy.
### Forking
Linux implements fork() via the clone() system call.This call takes a series of flags that specify which resources, if any, the parent and child process should share.
The vfork() system call has the same effect as fork() , except that the page table entries of the parent process are not copied. Instead, the child executes as the sole thread in the parent’s address space, and the parent is blocked until the child either calls exec() or exits. The child is not allowed to write to the address space.

## The Linux Implementation of Threads
Threads are a popular modern programming abstraction.They provide multiple threads of execution within the same program in a shared memory address space.They can also share open files and other resources.Threads enable concurrent programming and, on multiple processor systems, true parallelism.

### Creating Threads
Threads are created the same as normal tasks, with the exception that the clone() system call is passed flags corresponding to the specific resources to be shared

### Kernel Threads
It is often useful for the kernel to perform some operations in the background.The kernel accomplishes this via kernel threads—standard processes that exist solely in kernelspace.The significant difference between kernel threads and normal processes is that kernel threads do not have an address space. (Their mm pointer, which points at their address space, is NULL .) They operate only in kernel-space and do not context switch into user-space. Kernel threads, however, are schedulable and preemptable, the same as normal processes.
Linux delegates several tasks to kernel threads, most notably the flush tasks and the ksoftirqd task.

## Process Termination
Generally, process destruction is self-induced. It occurs when the process calls the exit() system call, either explicitly when it is ready to terminate or implicitly on return from the main subroutine of any program.

### Removing the Process Descriptor
After do_exit() completes, the process descriptor for the terminated process still exists, but the process is a zombie and is unable to run.As discussed, this enables the system to obtain information about a child process after it has terminated. Consequently, the acts of cleaning up after a process and removing its process descriptor are separate.
The wait() family of functions are implemented via a single (and complicated) system call, wait4() .The standard behavior is to suspend execution of the calling task until one of its children exits, at which time the function returns with the PID of the exited child. Additionally, a pointer is provided to the function that on return holds the exit code of the terminated child.
### The Dilemma of the Parentless Task
If a parent exits before its children, some mechanism must exist to reparent any child tasks  to a new process, or else parentless terminated processes would forever remain zombies, wasting system memory.The solution is to reparent a task’s children on exit to either another process in the current thread group or, if that fails, the init process. do_exit() calls exit_notify() , which calls forget_original_parent() , which, in turn, calls find_new_reaper() to perform the reparenting.

