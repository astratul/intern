# An Introduction to Kernel Synchronization
Shared resources require protection from concurrent access because if multiple threads of execution access and manipulate the data at the same time, the threads may overwrite each other’s changes or access data while it is in an inconsistent state. Concurrent access of shared data is a recipe for instability that often proves hard to track down and debug—getting it right at the start is important.

## Critical Regions and Race Conditions
Code paths that access and manipulate shared data are called critical regions (also called critical sections). It is usually unsafe for multiple threads of execution to access the same resource simultaneously.To prevent concurrent access during critical regions, the programmer must ensure that code executes atomically—that is, operations complete without interruption as if the entire critical region were one indivisible instruction. It is a bug if it is possible for two threads of execution to be simultaneously executing within the same critical region.When this does occur, we call it a race condition, so-named because the threads raced to get there first
### Why Do We Need Protection?

### The Single Variable

## Locking

Notice that locks are advisory and voluntary. Locks are entirely a programming construct that the programmer must take advantage of. Nothing prevents you from writing code that manipulates the fictional queue without the appropriate lock. Such a practice, of course, would eventually result in a race condition and corruption.
Locks come in various shapes and sizes—Linux alone implements a handful of different locking mechanisms.The most significant difference between the various mechanisms is the behavior when the lock is unavailable because another thread already holds it— some lock variants busy wait, 2 whereas other locks put the current task to sleep until the lock becomes available.
### Causes of Concurrency
In user-space, the need for synchronization stems from the fact that programs are scheduled preemptively at the will of the scheduler. Because a process can be preempted at any time and another process can be scheduled onto the processor, a process can be involuntarily preempted in the middle of accessing a critical region. If the newly scheduled process then enters the same critical region (say, if the two processes manipulate the same shared memory or write to the same file descriptor), a race can occur.The same problem can occur with multiple single-threaded processes sharing files, or within a single program with signals, because signals can occur asynchronously.This type of concurrency—in which two things do not actually happen at the same time but interleave with each other such that they might as well—is called pseudo-concurrency.
If you have a symmetrical multiprocessing machine, two processes can actually be executed in a critical region at the exact same time.That is called true concurrency.Although the causes and semantics of true versus pseudo concurrency are different, they both result in the same race conditions and require the same sort of protection.
The kernel has similar causes of concurrency:
* Interrupts— An interrupt can occur asynchronously at almost any time, interrupting the currently executing code.
* Softirqs and tasklets— The kernel can raise or schedule a softirq or tasklet at almost any time, interrupting the currently executing code.
* Kernel preemption— Because the kernel is preemptive, one task in the kernel can preempt another.
* Sleeping and synchronization with user-space— A task in the kernel can sleep and thus invoke the scheduler, resulting in the running of a new process.
* Symmetrical multiprocessing— Two or more processors can execute kernel code at exactly the same time.
### Knowing What to Protect

## Deadlocks
A deadlock is a condition involving one or more threads of execution and one or more resources, such that each thread waits for one of the resources, but all the resources are already held.The threads all wait for each other, but they never make any progress toward releasing the resources that they already hold.Therefore, none of the threads can continue, which results in a deadlock.
A good analogy is a four-way traffic stop. If each car at the stop decides to wait for the other cars before going, no car will ever proceed, and we have a traffic deadlock.
The simplest example of a deadlock is the self-deadlock: 4 If a thread of execution attempts to acquire a lock it already holds, it has to wait for the lock to be released. But it will never release the lock, because it is busy waiting for the lock, and the result is deadlock:
```
acquire lock
acquire lock, again
wait for lock to become available
...
```

## Contention and Scalability

The term lock contention, or simply contention, describes a lock currently in use but that another thread is trying to acquire.A lock that is highly contended often has threads waiting to acquire it. High contention can occur because a lock is frequently obtained, held for a long time after it is obtained, or both. Because a lock’s job is to serialize access to a resource, it comes as no surprise that locks can slow down a system’s performance.A highly contended lock can become a bottleneck in the system, quickly limiting its performance. Of course, the locks are also required to prevent the system from tearing itself to shreds, so a solution to high contention must continue to provide the necessary concurrency protection.
Scalability is a measurement of how well a system can be expanded. In operating systems, we talk of the scalability with a large number of processes, a large number of processors, or large amounts of memory.

