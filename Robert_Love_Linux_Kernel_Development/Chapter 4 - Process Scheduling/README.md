# Process Scheduling

## Multitasking
A multitasking operating system is one that can simultaneously interleave execution of more than one process. On single processor machines, this gives the illusion of multiple processes running concurrently. On multiprocessor machines, such functionality enables processes to actually run concurrently, in parallel, on different processors. On either type of machine, it also enables many processes to block or sleep, not actually executing until work is available.These processes, although in memory, are not runnable. Instead, such processes utilize the kernel to wait until some event (keyboard input, network data, passage of time, and so on) occurs. Consequently, a modern Linux system can have many processes in memory but, say, only one in a runnable state.
Multitasking operating systems come in two flavors: cooperative multitasking and preemptive multitasking.

## Linux’s Process Scheduler

## Policy
Policy is the behavior of the scheduler that determines what runs when.A scheduler’s policy often determines the overall feel of a system and is responsible for optimally utilizing processor time.

### I/O-Bound Versus Processor-Bound Processes
Processes can be classified as either I/O-bound or processor-bound.The former is characterized as a process that spends much of its time submitting and waiting on I/O requests.
Consequently, such a process is runnable for only short durations, because it eventually blocks waiting on more I/O.

### Process Priority
A common type of scheduling algorithm is priority-based scheduling.The goal is to rank processes based on their worth and need for processor time.The general idea, which isn’t exactly implemented on Linux, is that processes with a higher priority run before those with a lower priority, whereas processes with the same priority are scheduled round-robin.

### Timeslice
The timeslice 2 is the numeric value that represents how long a task can run until it is preempted.The scheduler policy must dictate a default timeslice, which is not a trivial exercise.Too long a timeslice causes the system to have poor interactive performance; the system will no longer feel as if applications are concurrently executed.Too short a timeslice causes significant amounts of processor time to be wasted on the overhead of switching processes because a significant percentage of the system’s time is spent switching from one process with a short timeslice to the next.

### The Scheduling Policy in Action
Consider a system with two runnable tasks: a text editor and a video encoder.The text editor is I/O-bound because it spends nearly all its time waiting for user key presses.

## The Linux Scheduling Algorithm

### Scheduler Classes

### Process Scheduling in Unix Systems

### Fair Scheduling

## The Linux Scheduling Implementation

### Time Accounting

### Process Selection

### The Scheduler Entry Point

### Sleeping and Waking Up

## Preemption and Context Switching
Context switching, the switching from one runnable task to another, is handled by the context_switch() function defined in kernel/sched.c . It is called by schedule() when a new process has been selected to run. It does two basic jobs:
* Calls switch_mm() , which is declared in <asm/mmu_context.h> , to switch the virtual memory mapping from the previous process’s to that of the new process.
* Calls switch_to() , declared in <asm/system.h> , to switch the processor state from the previous process’s to the current’s.This involves saving and restoring stack information and the processor registers and any other architecture-specific state that must be managed and restored on a per-process basis.
The kernel, however, must know when to call schedule() . If it called schedule() only when code explicitly did so, user-space programs could run indefinitely. Instead, the kernel 0provides the need_resched flag to signify whether a reschedule should be performed

### User Preemption
User preemption occurs when the kernel is about to return to user-space, need_resched is set, and therefore, the scheduler is invoked. If the kernel is returning to user-space, it knows it is in a safe quiescent state. In other words, if it is safe to continue executing the current task, it is also safe to pick a new task to execute. Consequently, whenever the kernel is preparing to return to user-space either on return from an interrupt or after a system call, the value of need_resched is checked. If it is set, the scheduler is invoked to select a new (more fit) process to execute.
In short, user preemption can occur
* When returning to user-space from a system call
* When returning to user-space from an interrupt handler


### Kernel Preemption
Kernel preemption can occur
* When an interrupt handler exits, before returning to kernel-space
* When kernel code becomes preemptible again
* If a task in the kernel explicitly calls schedule()
* If a task in the kernel blocks (which results in a call to schedule() )

## Real-Time Scheduling Policies
Linux provides two real-time scheduling policies, SCHED_FIFO and SCHED_RR .The normal, not real-time scheduling policy is SCHED_NORMAL .Via the scheduling classes framework, these real-time policies are managed not by the Completely Fair Scheduler, but by a special real-time scheduler, defined in kernel/sched_rt.c .

## Scheduler-Related System Calls
Linux provides a family of system calls for the management of scheduler parameters. These system calls allow manipulation of process priority, scheduling policy, and processor affinity, as well as provide an explicit mechanism to yield the processor to other tasks.

System - Call Description
* nice() - Sets a process’s nice value
* sched_setscheduler() - Sets a process’s scheduling policy
* sched_getscheduler() - Gets a process’s scheduling policy
* sched_setparam() - Sets a process’s real-time priority
* sched_getparam() - Gets a process’s real-time priority
* sched_get_priority_max() - Gets the maximum real-time priority
* sched_get_priority_min() - Gets the minimum real-time priority
* sched_rr_get_interval() - Gets a process’s timeslice value
* sched_setaffinity() - Sets a process’s processor affinity
* sched_getaffinity() - Gets a process’s processor affinity
* sched_yield() - Temporarily yields the processor

The sched_setscheduler() and sched_getscheduler() system calls set and get a given process’s scheduling policy and real-time priority, respectively.
The sched_setparam() and sched_getparam() system calls set and get a process’s real-time priority.These calls merely encode rt_priority in a special sched_param structure.The calls sched_get_priority_max() and sched_get_priority_min() return the maximum and minimum priorities, respectively, for a given scheduling policy.The maximum priority for the real-time policies is MAX_USER_RT_PRIO minus one; the minimum is one.
For normal tasks, the nice() function increments the given process’s static priority by the given amount. Only root can provide a negative value, thereby lowering the nice value and increasing the priority.The nice() function calls the kernel’s set_user_nice() function, which sets the static_prio and prio values in the task’s task_struct as appropriate.

### Processor Affinity System Calls


### Yielding Processor Time

Linux provides the sched_yield() system call as a mechanism for a process to explicitly yield the processor to other waiting processes. It works by removing the process from the active array (where it currently is, because it is running) and inserting it into the expired array.
Kernel code, as a convenience, can call yield() , which ensures that the task’s state is TASK_RUNNING and then call sched_yield() . User-space applications use the sched_yield() system call.
