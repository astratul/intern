# System Calls

## Communicating with the Kernel
System calls provide a layer between the hardware and user-space processes.This layer serves three primary purposes. First, it provides an abstracted hardware interface for userspace.When reading or writing from a file, for example, applications are not concerned with the type of disk, media, or even the type of filesystem on which the file resides. Second, system calls ensure system security and stability.With the kernel acting as a middleman between system resources and user-space, the kernel can arbitrate access based on permissions, users, and other criteria. For example, this arbitration prevents applications from incorrectly using hardware, stealing other processes’ resources, or otherwise doing harm to the system. Finally, a single common layer between user-space and the rest of the system allows for the virtualized system provided to processes.
Other interfaces, such as device files or /proc , are ultimately accessed via system calls. Interestingly, Linux implements far fewer system calls than most systems. 1 This chapter addresses the role and implementation of system calls in Linux.

## APIs, POSIX, and the C Library

Typically, applications are programmed against an Application Programming Interface (API) implemented in user-space, not directly to system calls.This is important because no direct correlation is needed between the interfaces that applications make use of and the actual interface provided by the kernel.An API defines a set of programming interfaces used by applications.Those interfaces can be implemented as a system call, implemented through multiple system calls, or implemented without the use of system calls at all.The same API can exist on multiple systems and provide the same interface to applications while the implementation of the API itself can differ greatly from system to system.

## Syscalls

System calls (often called syscalls in Linux) are typically accessed via function calls defined in the C library.They can define zero, one, or more arguments (inputs) and might result in one or more side effects, 3 for example writing to a file or copying some data into a provided pointer. System calls also provide a return value of type long 4 that signifies success or error—usually, although not always, a negative return value denotes an error.A return value of zero is usually (but again not always) a sign of success.The C library, when a system call returns an error, writes a special error code into the global errno variable.
The asmlinkage modifier on the function definition.This is a directive to tell the compiler to look only on the stack for this function’s arguments.This is a required modifier for all system calls.

### System Call Numbers

In Linux, each system call is assigned a syscall number.This is a unique number that is used to reference a specific system call.When a user-space process executes a system call, the syscall number identifies which syscall was executed; the process does not refer to the syscall by name.
The syscall number is important; when assigned, it cannot change, or compiled applications will break. Likewise, if a system call is removed, its system call number cannot be recycled, or previously compiled code would aim to invoke one system call but would in reality invoke another.

### System Call Performance

System calls in Linux are faster than in many other operating systems.This is partly because of Linux’s fast context switch times; entering and exiting the kernel is a streamlined and simple affair.The other factor is the simplicity of the system call handler and the individual system calls themselves.

## System Call Handler

It is not possible for user-space applications to execute kernel code directly.They cannot simply make a function call to a method existing in kernel-space because the kernel exists in a protected memory space. If applications could directly read and write to the kernel’s address space, system security and stability would be nonexistent.
Instead, user-space applications must somehow signal to the kernel that they want to execute a system call and have the system switch to kernel mode, where the system call can be executed in kernel-space by the kernel on behalf of the application.

### Denoting the Correct System Call

On x86, the syscall number is fed to the kernel via the eax register. Before causing the trap into the kernel, user-space sticks in eax the number corresponding to the desired system call.The system call handler then reads the value from eax . Other architectures do something similar.
The system_call() function checks the validity of the given system call number by comparing it to NR_syscalls . If it is larger than or equal to NR_syscalls , the function returns -ENOSYS . Otherwise, the specified system call is invoked:
```
call *sys_call_table(,%rax,8)
```
Because each element in the system call table is 64 bits (8 bytes), the kernel multiplies the given system call number by four to arrive at its location in the system call table.

### Parameter Passing
In addition to the system call number, most syscalls require that one or more parameters be passed to them. Somehow, user-space must relay the parameters to the kernel during the trap.The easiest way to do this is via the same means that the syscall number is passed: The parameters are stored in registers. On x86-32, the registers ebx , ecx , edx , esi , and edi contain, in order, the first five arguments. In the unlikely case of six or more arguments, a single register is used to hold a pointer to user-space where all the parameters are stored.
The return value is sent to user-space also via register. On x86, it is written into the eax register.

## System Call Implementation

### Implementing System Calls

The first step in implementing a system call is defining its purpose.What will it do? The syscall should have exactly one purpose. Multiplexing syscalls (a single system call that does wildly different things depending on a flag argument) is discouraged in Linux. Look at ioctl() as an example of what not to do.
The purpose of the system call will remain constant but its uses may change.

### Verifying the Parameters
System calls must carefully verify all their parameters to ensure that they are valid and legal.The system call runs in kernel-space, and if the user can pass invalid input into the kernel without restraint, the system’s security and stability can suffer.
For example, file I/O syscalls must check whether the file descriptor is valid. Process related functions must check whether the provided PID is valid. Every parameter must be checked to ensure it is not just valid and legal, but correct. Processes must not ask the kernel to access resources to which the process does not have access.

The pointer points to a region of memory in user-space. Processes must not be able to trick the kernel into reading data in kernel-space on their behalf.
The pointer points to a region of memory in the process’s address space.The process must not be able to trick the kernel into reading someone else’s data.
If reading, the memory is marked readable. If writing, the memory is marked writable. If executing, the memory is marked executable.The process must not be able to bypass memory access restrictions.
For writing into user-space, the method copy_to_user() is provided. It takes three parameters.The first is the destination memory address in the process’s address space.The second is the source pointer in kernel-space. Finally, the third argument is the size in bytes of the data to copy.
For reading from user-space, the method copy_from_user() is analogous to copy_to_user() .The function reads from the second parameter into the first parameter the number of bytes specified in the third parameter.
Both of these functions return the number of bytes they failed to copy on error. On success, they return zero. It is standard for the syscall to return -EFAULT in the case of such an error.

## System Call Context
The kernel is in process context during the execution of a system call.The current pointer points to the current task, which is the process that issued the syscall.
In process context, the kernel is capable of sleeping (for example, if the system call blocks on a call or explicitly calls schedule() ) and is fully preemptible.These two points are important. First, the capability to sleep means that system calls can make use of the majority of the kernel’s functionality.

### Final Steps in Binding a System Call

### Accessing the System Call from User-Space
