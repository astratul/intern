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


### Other Spin Lock Methods

### Spin Locks and Bottom Halves

## Reader-Writer Spin Locks

## Semaphores

### Counting and Binary Semaphores

### Creating and Initializing Semaphores
 
### Using Semaphores

## Reader-Writer Semaphores

## Mutexes

### Semaphores Versus Mutexes

### Spin Locks Versus Mutexes

## Completion Variables

## BKL: The Big Kernel Lock

## Sequential Locks

## Preemption Disabling

## Ordering and Barriers
