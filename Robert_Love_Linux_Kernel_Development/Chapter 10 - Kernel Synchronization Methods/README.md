# Kernel Synchronization Methods

## Atomic Operations

Atomic operations provide instructions that execute atomically—without interruption. Just as the atom was originally thought to be an indivisible particle, atomic operators are indivisible instructions.
The kernel provides two sets of interfaces for atomic operations—one that operates on integers and another that operates on individual bits.These interfaces are implemented on every architecture that Linux supports. Most architectures contain instructions that provide atomic versions of simple arithmetic operations. Other architectures, lacking direct atomic operations, provide an operation to lock the memory bus for a single operation, thus guaranteeing that another memory-affecting operation cannot occur simultaneously.

### Atomic Integer Operations

### 64-Bit Atomic Operations

### Atomic Bitwise Operations

## Spin Locks

### Spin Lock Methods

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
