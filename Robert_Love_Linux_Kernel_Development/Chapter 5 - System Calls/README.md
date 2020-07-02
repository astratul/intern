# System Calls

## Communicating with the Kernel
System calls provide a layer between the hardware and user-space processes.This layer serves three primary purposes. First, it provides an abstracted hardware interface for userspace.When reading or writing from a file, for example, applications are not concerned with the type of disk, media, or even the type of filesystem on which the file resides. Second, system calls ensure system security and stability.With the kernel acting as a middleman between system resources and user-space, the kernel can arbitrate access based on permissions, users, and other criteria. For example, this arbitration prevents applications from incorrectly using hardware, stealing other processes’ resources, or otherwise doing harm to the system. Finally, a single common layer between user-space and the rest of the system allows for the virtualized system provided to processes.
Other interfaces, such as device files or /proc , are ultimately accessed via system calls. Interestingly, Linux implements far fewer system calls than most systems. 1 This chapter addresses the role and implementation of system calls in Linux.

## APIs, POSIX, and the C Library
Typically, applications are programmed against an Application Programming Interface (API) implemented in user-space, not directly to system calls.This is important because no direct correlation is needed between the interfaces that applications make use of and the actual interface provided by the kernel.An API defines a set of programming interfaces used by applications.Those interfaces can be implemented as a system call, implemented through multiple system calls, or implemented without the use of system calls at all.The same API can exist on multiple systems and provide the same interface to applications while the implementation of the API itself can differ greatly from system to system.

## Syscalls

System calls (often called syscalls in Linux) are typically accessed via function calls defined in the C library.They can define zero, one, or more arguments (inputs) and might result in one or more side effects, 3 for example writing to a file or copying some data into a provided pointer. System calls also provide a return value of type long 4 that signifies success or error—usually, although not always, a negative return value denotes an error.A return value of zero is usually (but again not always) a sign of success.The C library, when a system call returns an error, writes a special error code into the global errno variable.

### System Call Numbers

### System Call Performance

## System Call Handler

## System Call Implementation

## System Call Context

