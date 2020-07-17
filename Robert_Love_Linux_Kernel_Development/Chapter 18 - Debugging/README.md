# Debugging

Success in debugging the kernel—and ultimately, success in kernel development as a whole—is largely a function of your experience and understanding of the operating sys- tem.This chapter looks at approaches to debugging the kernel.

## Getting Started

The more information you have when tackling a bug, the better. Many times, if you can reliably reproduce the bug, you are more than halfway home.

## Bugs in the Kernel

Debugging the kernel might sound difficult, but in reality, the kernel is not unlike any other large software project.The kernel does have unique issues, such as timing constraints and race conditions, which are a consequence of allowing multiple threads of execution inside the kernel.

## Debugging by Printing

The kernel print function printk() behaves almost identically to the C library printf() function.

### Robustness

One property of printk() easily taken for granted is its robustness.The printk() function is callable from just about anywhere in the kernel at any time. It can be called from interrupt or process context. It can be called while any lock is held. It can be called simultaneously on multiple processors, yet it does not require the caller to hold a lock.
* A chink in the armor of printk() ’s robustness does exist. It is unusable before a certain point in the kernel boot process, prior to console initialization.
* Hardcore architecture hackers use the hardware that does work (say, a serial port) to communicate with the outside world.This is not fun for most people.The solution is a printk() variant that can output to the console early in the boot process: early_printk() .The behavior is the same as printk() , only the name and its capability to work earlier are changed.This is not a portable solution, however, because not all supported architectures have such a method implemented.
Unless you need to write to the console early in the boot process, you can rely on printk() to always work.

### Loglevels

The major difference in usage between printk() and printf() is the capability of the former to specify a loglevel.The kernel uses the loglevel to decide whether to print the message to the console.The kernel displays all messages with a loglevel below a specified value on the console.
You specify a loglevel like this:
```
printk(KERN_WARNING “This is a warning!\n”);
printk(KERN_DEBUG “This is a debug notice!\n”);
printk(“I did not specify a loglevel!\n”);
// or
printk(“<4>This is a warning!\n”);
printk(“<7>This is a debug notice!\n”);
printk(“<4>did not specify a loglevel!\n”);
```
The KERN_WARNING and KERN_DEBUG strings are simple defines found in <linux/kernel.h> .
##### Available Loglevels
* Loglevel - Description
* KERN_EMERG - An emergency condition; the system is probably dead. <0>
* KERN_ALERT - A problem that requires immediate attention. <1>
* KERN_CRIT - A critical condition. <2>
* KERN_ERR - An error. <3>
* KERN_WARNING - A warning. - it's default. <4>
* KERN_NOTICE - A normal, but perhaps noteworthy, condition. <5>
* KERN_INFO - An informational message. <6>
* KERN_DEBUG - A debug message—typically superfluous. <7>

### The Log Buffer

Kernel messages are stored in a circular buffer of size LOG_BUF_LEN .This size is configurable at compile time via the CONFIG_LOG_BUF_SHIFT option.The default for a uniprocessor machine is 16KB. In other words, the kernel can simultaneously store 16KB of kernel messages. If the message queue is at this maximum and another call to printk() is made, the new message overwrites the oldest one.The log buffer is called circular because the reading and writing occur in a circular pattern.
* Using a circular buffer has multiple advantages. Because it is easy to simultaneously write to and read from a circular buffer, even interrupt context can easily use printk() .
* The lone disadvantage of a circular buffer—the possibility of losing messages—is a small price to pay for the simplicity and robustness it affords.

### syslogd and klogd

On a standard Linux system, the user-space klogd daemon retrieves the kernel messages from the log buffer and feeds them into the system log file via the syslogd daemon.To read the log, the klogd program can either read the /proc/kmsg file or call the syslog() system call. By default, it uses the /proc approach. In either case, klogd blocks until there are new kernel messages to read. It then wakes up, reads any new messages, and processes them. By default, it sends the messages to the syslogd daemon.
The syslogd daemon appends all the messages it receives to a file, which is by default /var/log/messages . It is configurable via /etc/syslog.conf .
You can have klogd change the console loglevel when it loads by specifying the -c flag when you start it.

### Transposing printf() and printk()

When you first start developing kernel code, you most likely will often transpose printf() for printk() .This transposition is only natural, as you cannot deny years of repetition using printf() in user-space development.With luck, this mistake will not last long because the repeated linker errors will eventually grow rather annoying.
Someday, you might find yourself accidentally using printk() instead of printf() in your user-space code. When that day comes, you can say you are a true kernel hacker.

## Oops

An oops is the usual way a kernel communicates to the user that something bad happened. Because the kernel is the supervisor of the entire system, it cannot simply fix itself or kill itself as it can when user-space goes awry. Instead, the kernel issues an oops.This involves printing an error message to the console, dumping the contents of the registers, and providing a back trace.A failure in the kernel is hard to manage, so the kernel must jump through many hoops to issue the oops and clean up after itself. Often, after an oops the kernel is in an inconsistent state. For example, the kernel could have been in the middle of processing important data when the oops occurred. It might have held a lock or been in the middle of talking to hardware.The kernel must gracefully back out of its current context and try to resume control of the system. A panic results in an instant halt of the system. If the oops occurred in the idle task (pid zero) or the init task (pid one), the result is also a panic because the kernel cannot continue without these important processes. If the oops occurs in any other process, however, the kernel kills the process and tries to continue executing.

An oops might occur for multiple reasons, including a memory access violation or an illegal instruction.As a kernel developer, you will often deal with (and undoubtedly cause) oopses.


### ksymoops

The addresses in the back trace need to be converted into symbolic names.This is done via the ksymoops command in conjunction with the System.map generated during kernel compile. If you use modules, you also need some module information. ksymoops tries to figure out most of this information, so you can usually invoke it via 
```
ksymoops saved_oops.txt
```
The program then spits out a decoded version of the oops. If the default information ksymoops uses is unacceptable, or you want to provide alternative locations for the information, the program understands various options. ksymoops ’ manual page has a lot of information that you should read before using it.The ksymoops program most likely came with your distribution.

### kallsyms

Thankfully, dealing with ksymoops is no longer a requirement.This is a big deal, because although developers might have had little problem using it, end users often mismatched System.map files or failed to decode oopses altogether.The configuration option CONFIG_KALLSYMS_ALL additionally stores the symbolic name of all symbols, not only functions.This is generally needed only by specialized debuggers.
The CONFIG_KALLSYMS_EXTRA_PASS option causes the kernel build process to make a second pass over the kernel’s object code. It is useful only when debugging kallsyms itself.

## Kernel Debugging Options

Multiple configure options that you can set during compile to aid in debugging and testing kernel code are available.These options are in the Kernel Hacking menu of the Kernel Configuration Editor.They all depend on CONFIG_DEBUG_KERNEL .When hacking on the kernel, consider enabling as many of these options as practical.
Some of the options are rather useful, enabling slab layer debugging, high-memory debugging, I/O mapping debugging, spin-lock debugging, and stack-overflow checking. One of the most useful settings, however, is sleep-inside-spinlock checking, which actually does much more.Code that holds a spin lock or has disabled kernel preemption is atomic. Code cannot sleep while atomic—sleeping while holding a lock is a recipe for deadlock.
Thanks to kernel preemption, the kernel has a central atomicity counter.The kernel can be set such that if a task sleeps while atomic, or even does something that might sleep, the kernel prints a warning and provides a back trace. Potential bugs that are detectable include calling schedule() while holding a lock, issuing a blocking memory allocation while holding a lock, or sleeping while holding a reference to per-CPU data.This debugging infrastructure catches a lot of bugs and is highly recommended.
The following options make the best use of this feature:
```
CONFIG_PREEMPT=y
CONFIG_DEBUG_KERNEL=y
CONFIG_KALLSYMS=y
CONFIG_DEBUG_SPINLOCK_SLEEP=y
```

## Asserting Bugs and Dumping Information

A number of kernel routines make it easy to flag bugs, provide assertions, and dump information.Two of the most common are BUG() and BUG_ON() .When called, they cause an oops, which results in a stack trace and an error message dumped to the kernel.Why these statements cause an oops is architecture-dependent. Most architectures define BUG() and BUG_ON() as illegal instructions, which result in the desired oops.You normally use these routines as assertions, to flag situations that should not happen:
```
if (bad_thing)
BUG();
```
Or even better
```
BUG_ON(bad_thing);
```
Most kernel developers believe that BUG_ON() is easier to read and more self-documenting compared to BUG() .Also, BUG_ON() wraps its assertion in an unlikely() statement.
A more critical error is signaled via panic() .A call to panic() prints an error message and then halts the kernel. Obviously, you want to use it only in the worst of situations:
```
if (terrible_thing)
panic(“terrible_thing is %ld!\n”, terrible_thing);
```
Sometimes, you just want a simple stack trace issued on the console to help you in debugging. In those cases, dump_stack() is used. It simply dumps the contents of the registers and a function back trace to the console:
```
if (!debug_check) {
printk(KERN_DEBUG “provide some information...\n”);
dump_stack();
}
```

## Magic SysRq Key

A possible lifesaver is the Magic SysRq key, which is enabled via the CONFIG_MAGIC_SYSRQ configure option.The SysRq (system request) key is a standard key on most keyboards.
When this configure option is enabled, special combinations of keys enable you to communicate with the kernel regardless of what else it is doing.This enables you to perform some useful tasks in the face of a dying system.
In addition to the configure option, there is a sysctl to toggle this feature on and off.To turn it on:
```
echo 1 > /proc/sys/kernel/sysrq
```
From the console, you can hit SysRq-h for a list of available options. SysRq-s syncs dirty buffers to disk, SysRq-u unmounts all filesystems, and SysRq-b reboots the machine.
Issuing these three key combinations in a row is a safer way to reboot a dying machine than simply hitting the machine reset switch.

## The Saga of a Kernel Debugger

Many kernel developers have long demanded an in-kernel debugger. Unfortunately, Linus does not want a debugger in his tree.

### gdb

You can use the standard GNU debugger to glimpse inside a running kernel. Starting the debugger on the kernel is about the same as debugging a running process:
```
gdb vmlinux /proc/kcore
```

### kgdb

kgdb is a patch that enables gdb to fully debug the kernel remotely over a serial line. It requires two computers.The first runs a kernel patched with kgdb .The second debugs the first over the serial line (a null modem cable connecting the two machines) using gdb . With kgdb , the entire feature set of gdb is accessible: reading and writing any variables, settings breakpoints, setting watch points, single stepping, and so on! Special versions of kgdb even enable function execution.
Setting up kgdb and the serial line is a little tricky, but when complete, debugging is simple.The patch installs plenty of documentation in Documentation/ —check it out.

## Poking and Probing the System

### Using UID as a Conditional

If the code you are developing is process-related, sometimes you can develop alternative implementations without breaking the existing code.This is helpful if you are rewriting an important system call and would like a fully functional system with which to debug it.
```
if (current->uid != 7777) {
/* old algorithm .. */
} else {
/* new algorithm .. */
}
```
All users except UID 7777 will use the old algorithm.You can create a special user, with UID 7777, for testing the new algorithm.This makes it much easier to test critical process-related code.

### Using Condition Variables

Simply create a global variable and use it as a conditional check in your code. If the variable is zero, you follow one code path. If it is nonzero, you follow another.The variable can be set via an interface you export or a poke from the debugger.

### Using Statistics

Sometimes you want to get a feel for how often a specific event occurs. Sometimes you want to compare multiple events and generate some ratios for comparison.You can do this easily by creating statistics and a mechanism to export their values.
For instance, say you want to look at the occurrence of foo and the occurrence of bar. In a file, ideally the one where these events occur, declare two global variables:
```
unsigned long foo_stat = 0;
unsigned long bar_stat = 0;
```
For each occurrence of these events, increment the appropriate variable.Then export the data however you feel fit. For example, you can create a file in /proc with the values or write a system call.Alternatively, simply read them via a debugger.
Note that this approach is not particularly SMP-safe. Ideally, you would use atomic operations. For a trivial one-time debugging statistic, however, you normally do not need such protection.

### Rate and Occurrence Limiting Your Debugging

* rate limiting - which is useful when you want to watch the progression of an event, but the event occurs rather often.To prevent a deluge of debugging output, you print your debug message (or do whatever you are doing) only every few seconds. For example
```
static unsigned long prev_jiffy = jiffies;
/* rate limiting */
if (time_after(jiffies, prev_jiffy + 2*HZ)) {
prev_jiffy = jiffies;
printk(KERN_ERR “blah blah blah\n”);
}
```
In this example, the debug message is printed at most every 2 seconds.This prevents any flood of information on the console, and the computer remains usable.You might need the rate limiting to be larger or smaller, depending on your needs.

If you are only using printk() ,you can use a special function to rate limit your printk() calls:
```
if (error && printk_ratelimit())
printk(KERN_DEBUG “error=%d\n”, error);
```
The printk_ratelimit() function returns zero if rate limiting is in effect and nonzero otherwise. By default, the function allows only one message every 5 seconds but allows an initial burst of up to ten messages before that cap is enforced.These parameters are tunable via the printk_ratelimit and printk_ratelimit_burst sysctl , respectively.

* Another sticky situation arises if you try to determine if a codepath is exercised in a particular way. Unlike the previous example, you do not want real-time notification.This is an especially sticky situation if it is of the sort where if it is triggered once, it is triggered a lot.The solution here is not to rate limit the debugging, but occurrence limit it:
```
static unsigned long limit = 0;
if (limit < 5) {
limit++;
printk(KERN_ERR “blah blah blah\n”);
}
```

This example caps the debugging output to five.After five such messages, the conditional is always false.

## Binary Searching to Find the Culprit Change

Many times, however, you do not know what kernel version introduced the bug.You know that the bug is in the current kernel, but it seemed to have always been in the current kernel! With a little effort, you can find the offending change.With the change in hand, the bug fix is usually near.
To start, you need a reliably reproducible problem—preferably, a bug that you can verify immediately after boot. Next, you need a known-good kernel.You might already know this. For example, if you know a couple months back the kernel worked, grab a kernel from that period. If you are wrong, try an earlier release. It shouldn’t be too hard to find a kernel without the bug.
Next, you need a known-bad kernel.To make things easier, start with the earliest kernel you know to have the bug.
Now, you begin a binary search from the known-bad kernel down to the known-good kernel.

## Binary Searching with Git

The git source management tool provides a useful mechanism for performing binary searches. If you use git to control your copy of the Linux source tree, it can automate the binary search process. Moreover, the git tool performs the binary search at the revision level, actually pinpointing the specific commit that introduced the bug. Unlike many git related tasks, binary searching with git is not hard.To start, you tell git you want to begin a binary search:
```
$ git bisect start
// You then provide git with the earliest broken revision:
$ git bisect bad <revision>
// If the latest version of the kernel is your known-earliest offender, you do not need to provide a revision:
$ git bisect bad
// You then provide git with the latest working revision:
$ git bisect good v2.6.28
// You then compile, run, and test that revision. If it works, you run:
$ git bisect good
// If it does not work—that is, if the given kernel revision demonstrates the bug—you run
$ git bisect bad
// You repeat the process until there are no more bisections possible
// If you think you know the source of the bug—say, it is clear in x86-specific boot code—you can instruct git to only bisect among commits touching a specified list of directories:
$ git bisect start — arch/x86
```

## When All Else Fails: The Community

The Community - Linux Kernel Mailing List (LKML).
