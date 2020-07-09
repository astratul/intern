# An Introduction to Kernel Synchronization
Shared resources require protection from concurrent access because if multiple threads of execution access and manipulate the data at the same time, the threads may overwrite each other’s changes or access data while it is in an inconsistent state. Concurrent access of shared data is a recipe for instability that often proves hard to track down and debug—getting it right at the start is important.
## Critical Regions and Race Conditions

### Why Do We Need Protection?

### The Single Variable

## Locking

### Causes of Concurrency

### Knowing What to Protect

## Deadlocks

## Contention and Scalability