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

### 

### 

### 

## The Linux Scheduling Implementation

### 

### 

### 

### 
## Preemption and Context Switching

### 

### 

## Real-Time Scheduling Policies

## Scheduler-Related System Calls

### 

### 

### 