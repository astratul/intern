# Getting Started with the Kernel
## Obtaining the Kernel Source
### Using Git
You can use Git to obtain a copy of the latest “pushed” version of Linus’s tree:
```
$ git clone git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux-2.6.git
```
When checked out, you can update your tree to Linus’s latest:
```
$ git pull
```
With these two commands, you can obtain and subsequently keep up to date with the official kernel tree.
### Installing the Kernel Source

The kernel tarball is distributed in both GNU zip (gzip) and bzip2 format. Bzip2 is the default and preferred format because it generally compresses quite a bit better than gzip.
The Linux kernel tarball in bzip2 format is named linux- x . y . z .tar.bz2 , where x.y.z is the version of that particular release of the kernel source.After downloading the source, uncompressing and untarring it is simple. If your tarball is compressed with bzip2, run
```
$ tar xvjf linux-x.y.z.tar.bz2
```
If it is compressed with GNU zip, run
```
$ tar xvzf linux-x.y.z.tar.gz
```
This uncompresses and untars the source to the directory linux-x.y.z . If you use git to obtain and manage the kernel source, you do not need to download the tarball. Just run the git clone command as described and git downloads and unpacks the latest source.

### The Kernel Source Tree
The kernel source tree is divided into a number of directories, most of which contain many more subdirectories.

## Building the Kernel

Thankfully, the kernel provides multiple tools to facilitate configuration.The simplest tool is a text-based command-line utility:
```
$ make config
```
This utility goes through each option, one by one, and asks the user to interactively select yes, no, or (for tristates) module. Because this takes a long time, unless you are paid by the hour, you should use an ncurses-based graphical utility:
```
$ make menuconfig
```
Or a gtk+-based graphical utility:
```
$ make gconfig
```
These three utilities divide the various configuration options into categories, such as “Processor Type and Features.” You can move through the categories, view the kernel options, and of course change their values.
This command creates a configuration based on the defaults for your architecture:
```
$ make defconfig
```

### Minimizing Build Noise
A trick to minimize build noise, but still see warnings and errors, is to redirect the output from make :
```
$ make > ../detritus
```
If you need to see the build output, you can read the file. Because the warnings and errors are output to standard error, however, you normally do not need to. In fact, I just do
```
$ make > /dev/null
```
This redirects all the worthless output to that big, ominous sink of no return, /dev/null .

### Spawning Multiple Build Jobs
The make program provides a feature to split the build process into a number of parallel jobs. Each of these jobs then runs separately and concurrently, significantly speeding up the build process on multiprocessing systems. It also improves processor utilization because the time to build a large source tree includes significant time in I/O wait (time in which the process is idle waiting for an I/O request to complete).
By default, make spawns only a single job because Makefiles all too often have incorrect dependency information.With incorrect dependencies, multiple jobs can step on each other’s toes, resulting in errors in the build process.The kernel’s Makefiles have correct dependency information, so spawning multiple jobs does not result in failures.To build the kernel with multiple make jobs, use
```
$ make -jn
```
Here, n is the number of jobs to spawn. Usual practice is to spawn one or two jobs per processor. For example, on a 16-core machine, you might do
```
$ make -j32 > /dev/null
```
Using utilities such as the excellent distcc or kernel build time.

### Installing the New Kernel
After the kernel is built, you need to install it. How it is installed is architecture- and boot loader-dependent—consult the directions for your boot loader on where to copy the kernel image and how to set it up to boot.Always keep a known-safe kernel or two around in case your new kernel has problems!

Installing modules, thankfully, is automated and architecture-independent. As root,
simply run 
```
% make modules_install
```
This installs all the compiled modules to their correct home under /lib/modules .
The build process also creates the file System.map in the root of the kernel source tree. It contains a symbol lookup table, mapping kernel symbols to their start addresses.This is used during debugging to translate memory addresses to function and variable names.

## A Beast of a Different Nature
The Linux kernel has several unique attributes as compared to a normal user-space application.Although these differences do not necessarily make developing kernel code harder than developing user-space code, they certainly make doing so different.

These characteristics make the kernel a beast of a different nature. Some of the usual rules are bent; other rules are entirely new.Although some of the differences are obvious (we all know the kernel can do anything it wants), others are not so obvious.The most important of these differences are

* The kernel has access to neither the C library nor the standard C headers.
* The kernel is coded in GNU C.
* The kernel lacks the memory protection afforded to user-space.
* The kernel cannot easily execute floating-point operations.
* The kernel has a small per-process fixed-size stack.
* Because the kernel has asynchronous interrupts, is preemptive, and supports SMP, synchronization and concurrency are major concerns within the kernel.
* Portability is important.

### No libc or Standard Headers

Of the missing functions, the most familiar is printf() .The kernel does not have
access to printf() , but it does provide printk() , which works pretty much the same as its more familiar cousin.The printk() function copies the formatted string into the kernel log buffer, which is normally read by the syslog program. Usage is similar to printf() :
```
printk("Hello world! A string '%s' and an integer '%d'\n", str, i);
```
One notable difference between printf() and printk() is that printk() enables you to specify a priority flag.This flag is used by syslogd to decide where to display kernel messages. Here is an example of these priorities:
```
printk(KERN_ERR "this is an error!\n");
```
### GNU C
Like any self-respecting Unix kernel, the Linux kernel is programmed in C. Perhaps surprisingly, the kernel is not programmed in strict ANSI C. Instead, where applicable, the kernel developers make use of various language extensions available in gcc (the GNU Compiler Collection, which contains the C compiler used to compile the kernel and most everything else written in C on a Linux system).

#### Inline Functions
* support inline functions.An inline function is, as its name suggests, inserted inline into each function call site.This eliminates the overhead of function invocation and return (register saving and restore) and allows for potentially greater optimization as the compiler can optimize both the caller and the called function as one.As a downside (nothing in life is free), code size increases because the contents of the function are copied into all the callers, which increases memory consumption and instruction cache footprint. Kernel developers use inline functions for small time-critical functions.
In the kernel, using inline functions is preferred over complicated macros for reasons of type safety and readability.
#### Inline Assembly
The gcc C compiler enables the embedding of assembly instructions in otherwise normal C functions.This feature, of course, is used in only those parts of the kernel that are unique to a given system architecture.
The asm() compiler directive is used to inline assembly code. For example, this inline assembly directive executes the x86 processor’s rdtsc instruction, which returns the value of the timestamp ( tsc ) register:
```
unsigned int low, high;
asm volatile("rdtsc" : "=a" (low), "=d" (high));
/* low and high now contain the lower and upper 32-bits of the 64-bit tsc */
```
The Linux kernel is written in a mixture of C and assembly, with assembly relegated to low-level architecture and fast path code.The vast majority of kernel code is programmed in straight C.

#### Branch Annotation
The gcc C compiler has a built-in directive that optimizes conditional branches as either very likely taken or very unlikely taken.The compiler uses the directive to appropriately optimize the branch.The kernel wraps the directive in easy-to-use macros, likely() and unlikely() .

### No Memory Protection

When a user-space application attempts an illegal memory access, the kernel can trap the error, send the SIGSEGV signal, and kill the process. If the kernel attempts an illegal memory access, however, the results are less controlled.
Additionally, kernel memory is not pageable.Therefore, every byte of memory you consume is one less byte of available physical memory. Keep that in mind the next time you need to add one more feature to the kernel!
### No (Easy) Use of Floating Point
When a user-space process uses floating-point instructions, the kernel manages the transition from integer to floating point mode.What the kernel has to do when using floating-point instructions varies by architecture, but the kernel normally catches a trap and then initiates the transition from integer to floating point mode.
### Small, Fixed-Size Stack
The kernel stack is neither large nor dynamic; it is small and fixed in size.The exact size of the kernel’s stack varies by architecture.Each process receives its own stack.

### Synchronization and Concurrency
The kernel is susceptible to race conditions. Unlike a single-threaded user-space application, a number of properties of the kernel allow for concurrent access of shared resources and thus require synchronization to prevent races. Specifically

* Linux is a preemptive multitasking operating system. Processes are scheduled and rescheduled at the whim of the kernel’s process scheduler.The kernel must synchronize between these tasks.
* Linux supports symmetrical multiprocessing (SMP).Therefore, without proper pro tection, kernel code executing simultaneously on two or more processors can con currently access the same resource.
* Interrupts occur asynchronously with respect to the currently executing code. Therefore, without proper protection, an interrupt can occur in the midst of accessing a resource, and the interrupt handler can then access the same resource.
* The Linux kernel is preemptive.Therefore, without protection, kernel code can be preempted in favor of different code that then accesses the same resource. 
Typical solutions to race conditions include spinlocks and semaphores.
### Importance of Portability
Although user-space applications do not have to aim for portability, Linux is a portable operating system and should remain one.This means that architecture-independent C code must correctly compile and run on a wide range of systems, and that architecture-dependent code must be properly segregated in system-specific directories in the kernel source tree.

