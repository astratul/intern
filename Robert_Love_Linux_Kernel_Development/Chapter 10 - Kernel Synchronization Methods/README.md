# Kernel Synchronization Methods

## Atomic Operations

Atomic operations provide instructions that execute atomically—without interruption. Just as the atom was originally thought to be an indivisible particle, atomic operators are indivisible instructions.
The kernel provides two sets of interfaces for atomic operations—one that operates on integers and another that operates on individual bits.These interfaces are implemented on every architecture that Linux supports. Most architectures contain instructions that provide atomic versions of simple arithmetic operations. Other architectures, lacking direct atomic operations, provide an operation to lock the memory bus for a single operation, thus guaranteeing that another memory-affecting operation cannot occur simultaneously.

### Atomic Integer Operations
The atomic integer methods operate on a special data type, atomic_t .This special type is used, as opposed to having the functions work directly on the C int type, for several reasons. First, having the atomic functions accept only the atomic_t type ensures that the atomic operations are used only with these special types. Likewise, it also ensures that the data types are not passed to any nonatomic functions.The atomic_t type is defined in <linux/types.h> :
```
typedef struct {
    volatile int counter;
} atomic_t;
```
Despite being an integer, and thus 32 bits on all the machines that Linux supports, developers and their code once had to assume that an atomic_t was no larger than 24 bits in size.The SPARC port in Linux has an odd implementation of atomic operations:A lock was embedded in the lower 8 bits of the 32-bit int (it looked like Figure 10.1).The lock was used to protect concurrent access to the atomic type because the SPARC architecture lacks appropriate support at the instruction level. Consequently, only 24 usable bits were available on SPARC machines.
The declarations needed to use the atomic integer operations are in <asm/atomic.h> . Some architectures provide additional methods that are unique to that architecture, but all architectures provide at least a minimum set of operations that are used throughout the kernel.When you write kernel code, you can ensure that these operations are correctly implemented on all architectures. Defining an atomic_t is done in the usual manner. Optionally, you can set it to an initial value:
```
atomic_t v;
atomic_t u = ATOMIC_INIT(0);
/* define v */
/* define u and initialize it to zero */
```
Consequently, atomic_read() is usually just a macro returning the integer value of the atomic_t :
```
/**
* atomic_read - read atomic variable
* @v: pointer of type atomic_t
*
* Atomically reads the value of @v.
*/
static inline int atomic_read(const atomic_t *v)
{
return v->counter;
}
```
### 64-Bit Atomic Operations
the atomic64_t type is just a simple wrapper around an integer, this type a long :
```
typedef struct {
volatile long counter;
} atomic64_t;
```

### Atomic Bitwise Operations
In addition to atomic integer operations, the kernel also provides a family of functions that operate at the bit level. They are architecture-specific and defined in <asm/bitops.h> .
What might be surprising is that the bitwise functions operate on generic memory addresses.The arguments are a pointer and a bit number. Bit zero is the least significant bit of the given address. On 32-bit machines, bit 31 is the most significant bit, and bit 32 is the least significant bit of the following word.There are no limitations on the bit number supplied; although, most uses of the functions provide a word and, consequently, a bit number between 0 and 31 on 32-bit machines and 0 and 63 on 64-bit machines.

Atomicity requires that either instructions succeed in their entirety, uninterrupted, or instructions fail to execute at all. Therefore, if you issue two atomic bit operations, you expect two operations to succeed. After both operations complete, the bit needs to have the value as specified by the second operation. Moreover, however, at some point in time prior to the final operation, the bit needs to hold the value as specified by the first operation.
The kernel also provides routines to find the first set (or unset) bit starting at a given address:
```
int find_first_bit(unsigned long *addr, unsigned int size)
int find_first_zero_bit(unsigned long *addr, unsigned int size)
```
Both functions take a pointer as their first argument and the number of bits in total to search as their second.They return the bit number of the first set or first unset bit, respectively. If your code is searching only a word, the routines __ffs() and ffz() , which take a single parameter of the word in which to search, are optimal. 
Unlike the atomic integer operations, code typically has no choice whether to use the bitwise operations—they are the only portable way to set a specific bit.

## Spin Locks
The most common lock in the Linux kernel is the spin lock.A spin lock is a lock that can be held by at most one thread of execution. If a thread of execution attempts to acquire a spin lock while it is already held, which is called contended, the thread busy loops— spins—waiting for the lock to become available. If the lock is not contended, the thread can immediately acquire the lock and continue.The spinning prevents more than one thread of execution from entering the critical region at any one time.The same lock can be used in multiple locations, so all access to a given data structure, for example, can be protected and synchronized.
### Spin Lock Methods
Spin locks are architecture-dependent and implemented in assembly.The architecture dependent code is defined in <asm/spinlock.h> .The actual usable interfaces are defined in <linux/spinlock.h> .The basic use of a spin lock is:
```
DEFINE_SPINLOCK(mr_lock);
spin_lock(&mr_lock);
/* critical region ... */
spin_unlock(&mr_lock);
```
The lock can be held simultaneously by at most only one thread of execution. Consequently, only one thread is allowed in the critical region at a time.This provides the needed protection from concurrency on multiprocessing machines. On uniprocessor machines, the locks compile away and do not exist; they simply act as markers to disable and enable kernel preemption. If kernel preempt is turned off, the locks compile away entirely.
Big Fat Rule: Locks that simply wrap code regions are hard to understand and prone to race conditions. Lock data, not code.

### Other Spin Lock Methods
Method - Description
* spin_lock() - Acquires given lock
* spin_lock_irq() - Disables local interrupts and acquires given lock
* spin_lock_irqsave() - Saves current state of local interrupts, disables local interrupts, and acquires given lock
* spin_unlock() - Releases given lock
* spin_unlock_irq() - Releases given lock and enables local interrupts
* spin_unlock_irqrestore() - Releases given lock and restores local interrupts to given previous state
* spin_lock_init() - Dynamically initializes given spinlock_t
* spin_trylock() - Tries to acquire given lock; if unavailable, returns nonzero
* spin_is_locked() - Returns nonzero if the given lock is currently acquired, otherwise it returns zero

### Spin Locks and Bottom Halves
Because a bottom half might preempt process context code, if data is shared between a bottom-half process context, you must protect the data in process context with both a lock and the disabling of bottom halves.
## Reader-Writer Spin Locks
When a data structure is neatly split into reader/writer or consumer/producer usage patterns, it makes sense to use a locking mechanism that provides similar semantics.To satisfy this use, the Linux kernel provides reader-writer spin locks. Reader-writer spin locks provide separate reader and writer variants of the lock. One or more readers can concurrently hold the reader lock.The writer lock, conversely, can be held by at most one writer with no concurrent readers. Reader/writer locks are sometimes called shared/exclusive or concurrent/exclusive locks because the lock is available in a shared (for readers) and an exclusive (for writers) form.
Usage is similar to spin locks.The reader-writer spin lock is initialized via
```
DEFINE_RWLOCK(mr_rwlock);
```
Then, in the reader code path:
```
read_lock(&mr_rwlock);
/* critical section (read only) ... */
read_unlock(&mr_rwlock);
```
Finally, in the writer code path:
```
write_lock(&mr_rwlock);
/* critical section (read and write) ... */
write_unlock(&mr_lock);
```
Normally, the readers and writers are in entirely separate code paths, such as in this example.
* Method - Description
* read_lock() - Acquires given lock for reading
* read_lock_irq() - Disables local interrupts and acquires given lock for reading
* read_lock_irqsave() - Saves the current state of local interrupts, disables local interrupts, and acquires the given lock for reading
* read_unlock() - Releases given lock for reading
* read_unlock_irq() - Releases given lock and enables local interrupts
* read_unlock_ irqrestore() - Releases given lock and restores local interrupts to the given previous state
* write_lock() - Acquires given lock for writing
* write_lock_irq() - Disables local interrupts and acquires the given lock for writing
* write_lock_irqsave() - Saves current state of local interrupts, disables local interrupts, and acquires the given lock for writing
* write_unlock() - Releases given lock
* write_unlock_irq() - Releases given lock and enables local interrupts
* write_unlock_irqrestore() - Releases given lock and restores local interrupts to given previous state
* write_trylock() - Tries to acquire given lock for writing; if unavailable, returns nonzero
* rwlock_init() - Initializes given rwlock_t
## Semaphores
Semaphores in Linux are sleeping locks.When a task attempts to acquire a semaphore that is unavailable, the semaphore places the task onto a wait queue and puts the task to sleep.The processor is then free to execute other code.When the semaphore becomes available, one of the tasks on the wait queue is awakened so that it can then acquire the semaphore.
Let’s jump back to the door and key analogy.When a person reaches the door, he can grab the key and enter the room.The big difference lies in what happens when another dude reaches the door and the key is not available. In this case, instead of spinning, the fellow puts his name on a list and takes a number.When the person inside the room leaves, he checks the list at the door. If anyone’s name is on the list, he goes over to the first name and gives him a playful jab in the chest, waking him up and allowing him to enter the room. In this manner, the key (read: semaphore) continues to ensure that there is only one person (read: thread of execution) inside the room (read: critical region) at one time.
This provides better processor utilization than spin locks because there is no time spent busy looping, but semaphores have much greater overhead than spin locks. Life is always a trade-off.

* Because the contending tasks sleep while waiting for the lock to become available, semaphores are well suited to locks that are held for a long time.
* Conversely, semaphores are not optimal for locks that are held for short periods because the overhead of sleeping, maintaining the wait queue, and waking back up can easily outweigh the total lock hold time.
* Because a thread of execution sleeps on lock contention, semaphores must be obtained only in process context because interrupt context is not schedulable.
* You can (although you might not want to) sleep while holding a semaphore because you will not deadlock when another process acquires the same semaphore. (It will just go to sleep and eventually let you continue.)
* You cannot hold a spin lock while you acquire a semaphore, because you might have to sleep while waiting for the semaphore, and you cannot sleep while holding a spin lock.

### Counting and Binary Semaphores
A final useful feature of semaphores is that they can allow for an arbitrary number of simultaneous lock holders.Whereas spin locks permit at most one task to hold the lock at a time, the number of permissible simultaneous holders of semaphores can be set at declaration time.This value is called the usage count or simply the count.The most common value is to allow, like spin locks, only one lock holder at a time. In this case, the count is equal to one, and the semaphore is called either a binary semaphore (because it is either held by one task or not held at all) or a mutex (because it enforces mutual exclusion).
The down() method is used to acquire a semaphore by decrementing the count by one. If the new count is zero or greater, the lock is acquired and the task can enter the critical region. If the count is negative, the task is placed on a wait queue, and the processor moves on to something else.These names are used as verbs:You down a semaphore to acquire it.The up() method is used to release a semaphore upon completion of a critical region.This is called upping the semaphore.The method increments the count value; if the semaphore’s wait queue is not empty, one of the waiting tasks is awakened and allowed to acquire the semaphore.

### Creating and Initializing Semaphores
The semaphore implementation is architecture-dependent and defined in <asm/semaphore.h> .The struct semaphore type represents semaphores. Statically declared semaphores are created via the following, where name is the variable’s name and count is the usage count of the semaphore:
 ```
struct semaphore name;
sema_init(&name, count);
```
As a shortcut to create the more common mutex, use the following, where, again, name is the variable name of the binary semaphore:
```
static DECLARE_MUTEX(name);
```
More frequently, semaphores are created dynamically, often as part of a larger structure. In this case, to initialize a dynamically created semaphore to which you have only an indirect pointer reference, just call sema_init() , where sem is a pointer and count is the usage count of the semaphore:
```
sema_init(sem, count);
```
Similarly, to initialize a dynamically created mutex, you can use
```
init_MUTEX(sem);
```

### Using Semaphores
The function down_interruptible() attempts to acquire the given semaphore. If the semaphore is unavailable, it places the calling process to sleep in the TASK_INTERRUPTIBLE state. Recall from Chapter 3 that this process state implies that a task can be awakened with a signal, which is generally a good thing. If the task receives a signal while waiting for the semaphore, it is awakened and down_interruptible() returns -EINTR .Alternatively, the function down() places the task in the TASK_UNINTERRUPTIBLE state when it sleeps.You most likely do not want this because the process waiting for the semaphore does not respond to signals.Therefore, use of down_interruptible() is much more common (and correct) than down() .Yes, again, the naming is not ideal.
You can use down_trylock() to try to acquire the given semaphore without blocking. If the semaphore is already held, the function immediately returns nonzero. Otherwise, it returns zero and you successfully hold the lock.
To release a given semaphore, call up() .

* Method - Description
* sema_init(struct semaphore *, int) - Initializes the dynamically created semaphore to the given count
* init_MUTEX(struct semaphore *) - Initializes the dynamically created semaphore with a count of one
* init_MUTEX_LOCKED(struct semaphore *) - Initializes the dynamically created semaphore with a count of zero (so it is initially locked)
* down_interruptible (struct semaphore *) - Tries to acquire the given semaphore and enter interruptible sleep if it is contended
* down(struct semaphore *) - Tries to acquire the given semaphore and enter uninterruptible sleep if it is contended
* down_trylock(struct semaphore *) - Tries to acquire the given semaphore and immediately return nonzero if it is contended
* up(struct semaphore *) - Releases the given semaphore and wakes a waiting task, if any

## Reader-Writer Semaphores
Reader-writer semaphores are represented by the struct rw_semaphore type, which is declared in <linux/rwsem.h> . Statically declared reader-writer semaphores are created via the following, where name is the declared name of the new semaphore:
```
static DECLARE_RWSEM(name);
```
Reader-writer semaphores created dynamically are initialized via
```
init_rwsem(struct rw_semaphore *sem)
```
All reader-writer semaphores are mutexes—that is, their usage count is one—although they enforce mutual exclusion only for writers, not readers.Any number of readers can concurrently hold the read lock, so long as there are no writers. Conversely, only a sole writer (with no readers) can acquire the write variant of the lock.All reader-writer locks use uninterruptible sleep, so there is only one version of each down() .
Reader-writer semaphores have a unique method that their reader-writer spin lock cousins do not have: downgrade_write() .This function atomically converts an acquired write lock to a read lock.

## Mutexes

The term “mutex” is a generic name to refer to any sleeping lock that enforces mutual exclusion, such as a semaphore with a usage count of one. In recent Linux kernels, the proper noun “mutex” is now also a specific type of sleeping lock that implements mutual exclusion.That is, a mutex is a mutex.
The mutex is represented by struct mutex . It behaves similar to a semaphore with a count of one, but it has a simpler interface, more efficient performance, and additional constraints on its use.To statically define a mutex, you do:
```
DEFINE_MUTEX(name);
```
To dynamically initialize a mutex, you call
```
mutex_init(&mutex);
```
Locking and unlocking the mutex is easy:
```
mutex_lock(&mutex);
/* critical region ... */
mutex_unlock(&mutex);
```
* Mutex Methods
* Method - Description
* mutex_lock(struct mutex *) - Locks the given mutex; sleeps if the lock is unavailable
* mutex_unlock(struct mutex *) - Unlocks the given mutex
* mutex_trylock(struct mutex *) - Tries to acquire the given mutex; returns one if successful and the lock is acquired and zero otherwise
* mutex_is_locked (struct mutex *) - Returns one if the lock is locked and zero otherwise

* Only one task can hold the mutex at a time.That is, the usage count on a mutex is always one.
* Whoever locked a mutex must unlock it.That is, you cannot lock a mutex in one context and then unlock it in another.This means that the mutex isn’t suitable for more complicated synchronizations between kernel and user-space. Most use cases, however, cleanly lock and unlock from the same context.
* Recursive locks and unlocks are not allowed.That is, you cannot recursively acquire the same mutex, and you cannot unlock an unlocked mutex.
* A process cannot exit while holding a mutex.
* A mutex cannot be acquired by an interrupt handler or bottom half, even with mutex_trylock() .
* A mutex can be managed only via the official API: It must be initialized via the methods described in this section and cannot be copied, hand initialized, or reinitialized.


### Semaphores Versus Mutexes

Unless one of mutex’s additional constraints prevent you from using them, prefer the new mutex type to semaphores.When writing new code, only specific, often low-level, uses need a semaphore. Start with a mutex and move to a semaphore only if you run into one of their constraints and have no other alternative.

### Spin Locks Versus Mutexes
Knowing when to use a spin lock versus a mutex (or semaphore) is important to writing optimal code. In many cases, however, there is little choice. Only a spin lock can be used in interrupt context, whereas only a mutex can be held while a task sleeps.
* Requirement - Recommended Lock
* Low overhead locking - Spin lock is preferred.
* Short lock hold time - Spin lock is preferred.
* Long lock hold time - Mutex is preferred.
* Need to lock from interrupt context - Spin lock is required.
* Need to sleep while holding lock - Mutex is required.



## Completion Variables
Using completion variables is an easy way to synchronize between two tasks in the kernel when one task needs to signal to the other that an event has occurred. One task waits on the completion variable while another task performs some work.When the other task has completed the work, it uses the completion variable to wake up any waiting tasks. If you think this sounds like a semaphore, you are right—the idea is much the same. In fact, completion variables merely provide a simple solution to a problem whose answer is otherwise semaphores.

Completion variables are represented by the struct completion type, which is defined in <linux/completion.h> .A statically created completion variable is created and initialized via
```
DECLARE_COMPLETION(mr_comp);
```
A dynamically created completion variable is initialized via init_completion() . On a given completion variable, the tasks that want to wait call wait_for_completion() .After the event has occurred, calling complete() signals all waiting tasks to wake up.
* Method Description
* init_completion(struct completion *)  - Initializes the given dynamically created completion variable
* wait_for_completion(struct completion *) - Waits for the given completion variable to be signaled
* complete(struct completion *) - Signals any waiting tasks to wake up

For sample usages of completion variables, see kernel/sched.c and kernel/fork.c . A common usage is to have a completion variable dynamically created as a member of a data structure. Kernel code waiting for the initialization of the data structure calls wait_for_completion() .When the initialization is complete, the waiting tasks are awakened via a call to completion() .

## BKL: The Big Kernel Lock

The Big Kernel Lock (BKL) is a global spin lock that was created to ease the transition from Linux’s original SMP implmentation to fine-grained locking.The BKL has some interesting properties:
* You can sleep while holding the BKL.The lock is automatically dropped when the task is unscheduled and reacquired when the task is rescheduled. Of course, this does not mean it is always safe to sleep while holding the BKL, merely that you can and you will not deadlock.
* The BKL is a recursive lock.A single process can acquire the lock multiple times and not deadlock, as it would with a spin lock.
* You can use the BKL only in process context. Unlike spin locks, you cannot acquire the BKL in interrupt context.
* New users of the BKL are forbidden.With every kernel release, fewer and fewer drivers and subsystems rely on the BKL.

Function -Description
* lock_kernel () - Acquires the BKL.
* unlock_ kernel() - Releases the BKL.
* kernel_ locked() - Returns nonzero if the lock is held and zero otherwise. (UP always returns nonzero.)
The BKL is seemingly associated with code (for example,“it synchronizes callers to foo() ”) instead of data (“it protects the foo structure”).This makes replacing BKL uses with a spin lock difficult because it is not easy to determine just what is being locked.The replacement is made even harder in that the relationship between all BKL users needs to be determined.


## Sequential Locks
It provides a simple mechanism for reading and writing shared data. It works by maintaining a sequence counter.Whenever the data in question is written to, a lock is obtained and a sequence number is incremented. Prior to and after reading the data, the sequence number is read. If the values are the same, a write did not begin in the middle of the read. Further, if the values are even, a write is not underway.

To define a seq lock:
```
seqlock_t mr_seq_lock = DEFINE_SEQLOCK(mr_seq_lock);
```
The write path is then
```
write_seqlock(&mr_seq_lock);
/* write lock is obtained... */
write_sequnlock(&mr_seq_lock);
```
This looks like normal spin lock code.The oddness comes in with the read path, which is quite a bit different:
```
unsigned long seq;
do {
seq = read_seqbegin(&mr_seq_lock);
/* read data here ... */
} while (read_seqretry(&mr_seq_lock, seq));
```
Seq locks are useful to provide a lightweight and scalable lock for use with many readers and a few writers. Seq locks, however, favor writers over readers.An acquisition of the write lock always succeeds as long as there are no other writers. Readers do not affect the write lock, as is the case with reader-writer spin locks and semaphores. Furthermore, pending writers continually cause the read loop (the previous example) to repeat, until there are no longer any writers holding the lock. Seq locks are ideal when your locking needs meet most or all these requirements:
* Your data has a lot of readers.
* Your data has few writers.
* Although few in number, you want to favor writers over readers and never allow readers to starve writers.
* Your data is simple, such as a simple structure or even a single integer that, for whatever reason, cannot be made atomic.

A prominent user of the seq lock is jiffies, the variable that stores a Linux machine’s uptime (see Chapter 11,“Timers and Time Management”). Jiffies holds a 64-bit count of the number of clock ticks since the machine booted. On machines that cannot atomically read the full 64-bit jiffies_64 variable, get_jiffies_64() is implemented using seq locks.

## Preemption Disabling

Because the kernel is preemptive, a process in the kernel can stop running at any instant to enable a process of higher priority to run.This means a task can begin running in the same critical region as a task that was preempted.To prevent this, the kernel preemption code uses spin locks as markers of nonpreemptive regions. If a spin lock is held, the kernel is not preemptive. Because the concurrency issues with kernel preemption and SMP are the same, and the kernel is already SMP-safe; this simple change makes the kernel preempt-safe, too.
Or so we hope. In reality, some situations do not require a spin lock, but do need kernel preemption disabled.The most frequent of these situations is per-processor data. If the data is unique to each processor, there might be no need to protect it with a lock because only that one processor can access the data. If no spin locks are held, the kernel is preemptive, and it would be possible for a newly scheduled task to access this same variable.

Function - Description
* preempt_disable() - Disables kernel preemption by incrementing the preemption counter
* preempt_enable() - Decrements the preemption counter and checks and services any pending reschedules if the count is now zero
* preempt_enable_no_resched() - Enables kernel preemption but does not check for any pending reschedules
* preempt_count() - Returns the preemption count
## Ordering and Barriers

When dealing with synchronization between multiple processors or with hardware devices, it is sometimes a requirement that memory-reads (loads) and memory-writes (stores) issue in the order specified in your program code.When talking with hardware, you often need to ensure that a given read occurs before another read or write.Additionally, on symmetrical multiprocessing systems, it might be important for writes to appear in the order that your code issues them (usually to ensure subsequent reads see the data in the same order). Complicating these issues is the fact that both the compiler and the processor can reorder reads and writes 4 for performance reasons.Thankfully, all processors that do reorder reads or writes provide machine instructions to enforce ordering requirements. It is also possible to instruct the compiler not to reorder instructions around a given point.These instructions are called barriers.
* The rmb() method provides a read memory barrier. It ensures that no loads are reordered across the rmb() call.That is, no loads prior to the call will be reordered to after the call, and no loads after the call will be reordered to before the call.
* The wmb() method provides a write barrier. It functions in the same manner as rmb() , but with respect to stores instead of loads—it ensures no stores are reordered across the barrier.
* The mb() call provides both a read barrier and a write barrier. No loads or stores will be reordered across a call to mb() . It is provided because a single instruction (often the same instruction used by rmb() ) can provide both the load and store barrier. A variant of rmb() , read_barrier_depends() , provides a read barrier but only for loads on which subsequent loads depend.

The macros smp_rmb() , smp_wmb() , smp_mb() , and smp_read_barrier_depends() provide a useful optimization. On SMP kernels they are defined as the usual memory barriers, whereas on UP kernels they are defined only as a compiler barrier.You can use these SMP variants when the ordering constraints are specific to SMP systems. The barrier() method prevents the compiler from optimizing loads or stores across the call.The compiler knows not to rearrange stores and loads in ways that would change the effect of the C code and existing data dependencies. It does not have knowledge, however, of events that can occur outside the current context. For example, the compiler cannot know about interrupts that might read the same data you are writing.

Barrier - Description
* rmb() - Prevents loads from being reordered across the barrier
* read_barrier_depends() - Prevents data-dependent loads from being reordered across the barrier
* wmb() - Prevents stores from being reordered across the barrier
* mb() - Prevents load or stores from being reordered across the barrier
*smp_rmb() - Provides an rmb() on SMP, and on UP provides a barrier()
* smp_read_barrier_depends() - Provides a read_barrier_depends() on SMP, and provides a barrier() - on UP
* smp_wmb() - Provides a wmb() on SMP, and provides a barrier() - on UP
* smp_mb() - Provides an mb() on SMP, and provides a
* barrier() - on UP
* barrier() - Prevents the compiler from optimizing stores or loads across the barrier

