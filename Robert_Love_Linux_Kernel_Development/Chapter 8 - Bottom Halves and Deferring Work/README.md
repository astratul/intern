# Bottom Halves and Deferring Work

## Bottom Halves
The job of bottom halves is to perform any interrupt-related work not performed by the interrupt handler.

### Why Bottom Halves?
Minimizing the time spent with interrupts disabled is important for system response and performance.Add to this the fact that interrupt handlers run asynchronously with respect to other code—even other interrupt handlers—and it is clear that you should work to minimize how long interrupt handlers run. Processing incoming network traffic should not prevent the kernel’s receipt of keystrokes.The solution is to defer some of the work until later.

### A World of Bottom Halves
Unlike the top half, which is implemented entirely via the interrupt handler, multiple mechanisms are available for implementing a bottom half.These mechanisms are different interfaces and subsystems that enable you to implement bottom halves. 

#### The Original “Bottom Half”
The infrastructure was also known as BH, which is what we will call it to avoid confusion with the generic term bottom half.The BH interface was simple, like most things in those good old days. It provided a statically created list of 32 bottom halves for the entire system.The top half could mark whether the bottom half would run by setting a bit in a 32-bit integer. Each BH was globally synchronized. No two could run at the same time, even on different processors.This was easy to use, yet inflexible; a simple approach, yet a bottleneck.
####  Task Queues
Later on, the kernel developers introduced task queues both as a method of deferring work and as a replacement for the BH mechanism.The kernel defined a family of queues. Each queue contained a linked list of functions to call.The queued functions were run at certain times, depending on which queue they were in. Drivers could register their bottom halves in the appropriate queue.This worked fairly well, but it was still too inflexible to replace the BH interface entirely. It also was not lightweight enough for performance critical subsystems, such as networking.

#### Softirqs and Tasklets
During the 2.3 development series, the kernel developers introduced softirqs and tasklets. With the exception of compatibility with existing drivers, softirqs and tasklets could completely replace the BH interface. 
Softirqs are a set of statically defined bottom halves that can run simultaneously on any processor; even two of the same type can run concurrently.Tasklets, which have an awful and confusing name, 2 are flexible, dynamically created bottom halves built on top of softirqs.Two different tasklets can run concurrently on different processors, but two of the same type of tasklet cannot run simultaneously.Thus, tasklets are a good trade-off between performance and ease of use. For most bottom-half processing, the tasklet is sufficient. Softirqs are useful when performance is critical, such as with networking. Using softirqs requires more care, however, because two of the same softirq can run at the same time. In addition, softirqs must be registered statically at compile time. Conversely, code can dynamically register tasklets.

#### Dispelling the Confusion
Bottom half is a generic operating system term referring to the deferred portion of interrupt processing, so named because it represents the second, or bottom, half of the interrupt processing solution. In Linux, the term currently has this meaning, too.All the kernel’s mechanisms for deferring work are “bottom halves.” Some people also confusingly call all bottom halves “softirqs.”

## Softirqs
Softirqs are rarely used directly; tasklets are a much more common form of bottom half. Nonetheless, because tasklets are built on softirqs, we cover them first.The softirq code lives in the file kernel/softirq.c in the kernel source tree.
### Implementing Softirqs
Softirqs are statically allocated at compile time. Unlike tasklets, you cannot dynamically register and destroy softirqs. Softirqs are represented by the softirq_action structure, which is defined in <linux/interrupt.h> :
```
struct softirq_action {
void (*action)(struct softirq_action *);
};
```

#### The Softirq Handler
The prototype of a softirq handler, action , looks like
```
void softirq_handler(struct softirq_action *)
```
When the kernel runs a softirq handler, it executes this action function with a pointer to the corresponding softirq_action structure as its lone argument.
A softirq never preempts another softirq.The only event that can preempt a softirq is an interrupt handler.Another softirq—even the same one—can run on another processor, however.
#### Executing Softirqs
A registered softirq must be marked before it will execute.This is called raising the softirq.
Usually, an interrupt handler marks its softirq for execution before returning.Then, at a suitable time, the softirq runs. Pending softirqs are checked for and executed in the following places:
* In the return from hardware interrupt code path
* In the ksoftirqd kernel thread
* In any code that explicitly checks for and executes pending softirqs, such as the networking subsystem
### Using Softirqs
Softirqs are reserved for the most timing-critical and important bottom-half processing on the system. Currently, only two subsystems—networking and block devices—directly use softirqs.Additionally, kernel timers and tasklets are built on top of softirqs. If you add a new softirq, you normally want to ask yourself why using a tasklet is insufficient.Tasklets are dynamically created and are simpler to use because of their weaker locking requirements, and they still perform quite well.
#### Assigning an Index
You declare softirqs statically at compile time via an enum in <linux/interrupt.h> .The kernel uses this index, which starts at zero, as a relative priority. Softirqs with the lowest numerical priority execute before those with a higher numerical priority. Creating a new softirq includes adding a new entry to this enum .When adding a new softirq, you might not want to simply add your entry to the end of the list, as you would elsewhere.
* Tasklet  -  Priority  -  Softirq Description
* HI_SOFTIRQ  -  0  -  High-priority tasklets
* TIMER_SOFTIRQ  -  1  -  Timers
* NET_TX_SOFTIRQ  -  2  -  Send network packets
* NET_RX_SOFTIRQ  -  3  -  Receive network packets
* BLOCK_SOFTIRQ  -  4  -  Block devices
* TASKLET_SOFTIRQ  -  5  -  Normal priority tasklets
* SCHED_SOFTIRQ  -  6  -  Scheduler
* HRTIMER_SOFTIRQ  -  7  -  High-resolution timers
* RCU_SOFTIRQ  -  8  -  RCU locking
#### Registering Your Handler
Next, the softirq handler is registered at run-time via open_softirq() , which takes two parameters: the softirq’s index and its handler function.The networking subsystem, for example, registers its softirqs like this, in net/core/dev.c :
```
open_softirq(NET_TX_SOFTIRQ, net_tx_action);
open_softirq(NET_RX_SOFTIRQ, net_rx_action);
```
The softirq handlers run with interrupts enabled and cannot sleep.While a handler runs, softirqs on the current processor are disabled.Another processor, however, can execute other softirqs. If the same softirq is raised again while it is executing, another processor can run it simultaneously.This means that any shared data—even global data used only within the softirq handler—needs proper locking (as discussed in the next two chapters).
This is an important point, and it is the reason tasklets are usually preferred. Simply preventing your softirqs from running concurrently is not ideal. If a softirq obtained a lock to prevent another instance of itself from running simultaneously, there would be no reason to use a softirq. Consequently, most softirq handlers resort to per-processor data (data unique to each processor and thus not requiring locking) and other tricks to avoid explicit locking and provide excellent scalability.
The raison d’être to softirqs is scalability. If you do not need to scale to infinitely many processors, then use a tasklet.Tasklets are essentially softirqs in which multiple instances of the same handler cannot run concurrently on multiple processors.

#### Raising Your Softirq
After a handler is added to the enum list and registered via open_softirq() , it is ready to run. To mark it pending, so it is run at the next invocation of do_softirq() , call
```
raise_softirq() .
```
## Tasklets
Tasklets are a bottom-half mechanism built on top of softirqs. As mentioned, they have nothing to do with tasks.Tasklets are similar in nature and behavior to softirqs; however, they have a simpler interface and relaxed locking rules.
As a device driver author, the decision whether to use softirqs versus tasklets is simple: You almost always want to use tasklets. As we saw in the previous section, you can (almost) count on one hand the users of softirqs. Softirqs are required only for highfrequency and highly threaded uses.Tasklets, on the other hand, see much greater use. Tasklets work just fine for the vast majority of cases and are very easy to use.
### Implementing Tasklets
Because tasklets are implemented on top of softirqs, they are softirqs. As discussed, tasklets are represented by two softirqs: HI_SOFTIRQ and TASKLET_SOFTIRQ .The only difference in these types is that the HI_SOFTIRQ -based tasklets run prior to the TASKLET_SOFTIRQ - based tasklets.
#### The Tasklet Structure
Tasklets are represented by the tasklet_struct structure. Each structure represents a unique tasklet.The structure is declared in <linux/interrupt.h> :
```
struct tasklet_struct {
struct tasklet_struct *next; // next tasklet in the list
unsigned long state; // state of the tasklet
atomic_t count; // reference counter
void (*func)(unsigned long); // tasklet handler function
unsigned long data; // argument to the tasklet function
};
```
#### Scheduling Tasklets
Scheduled tasklets (the equivalent of raised softirqs) 5 are stored in two per-processor structures: tasklet_vec (for regular tasklets) and tasklet_hi_vec (for high-priority tasklets).
Both of these structures are linked lists of tasklet_struct structures. Each tasklet_struct structure in the list represents a different tasklet. Tasklets are scheduled via the tasklet_schedule() and tasklet_hi_schedule() functions, which receive a pointer to the tasklet’s tasklet_struct as their lone argument. Each function ensures that the provided tasklet is not yet scheduled and then calls __tasklet_schedule() and __tasklet_hi_schedule() as appropriate.


### Using Tasklets
In most cases, tasklets are the preferred mechanism with which to implement your bottom half for a normal hardware device.Tasklets are dynamically created, easy to use, and quick.

#### Declaring Your Tasklet
You can create tasklets statically or dynamically.What option you choose depends on whether you have (or want) a direct or indirect reference to the tasklet. If you are going to statically create the tasklet (and thus have a direct reference to it), use one of two macros in <linux/interrupt.h> :
```
DECLARE_TASKLET(name, func, data)
DECLARE_TASKLET_DISABLED(name, func, data);
```
Both these macros statically create a struct tasklet_struct with the given name.

#### Writing Your Tasklet Handler
The tasklet handler must match the correct prototype:
```
void tasklet_handler(unsigned long data)
```
As with softirqs, tasklets cannot sleep.This means you cannot use semaphores or other blocking functions in a tasklet.Tasklets also run with all interrupts enabled, so you must take precautions (for example, disable interrupts and obtain a lock) if your tasklet shares data with an interrupt handler.

#### Scheduling Your Tasklet
To schedule a tasklet for execution, tasklet_schedule() is called and passed a pointer to the relevant tasklet_struct :
```
tasklet_schedule(&my_tasklet);
```
You can remove a tasklet from the pending queue via tasklet_kill() .This function receives a pointer as a lone argument to the tasklet’s tasklet_struct . Removing a scheduled tasklet from the queue is useful when dealing with a tasklet that often reschedules itself.This function first waits for the tasklet to finish executing and then it removes the tasklet from the queue. Nothing stops some other code from rescheduling the tasklet, of course.This function must not be used from interrupt context because it sleeps.

### ksoftirqd
Softirq (and thus tasklet) processing is aided by a set of per-processor kernel threads.These kernel threads help in the processing of softirqs when the system is overwhelmed with softirqs. Because tasklets are implemented using softirqs, the following discussion applies equally to softirqs and tasklets. For brevity, we will refer mainly to softirqs. As already described, the kernel processes softirqs in a number of places, most commonly on return from handling an interrupt. Softirqs might be raised at high rates (such as during heavy network traffic). Further, softirq functions can reactivate themselves.

### The Old BH Mechanism
## Work Queues
Work queues are a different form of deferring work from what we have looked at so far. Work queues defer work into a kernel thread—this bottom half always runs in process context.Thus, code deferred to a work queue has all the usual benefits of process context. Most important, work queues are schedulable and can therefore sleep. Normally, it is easy to decide between using work queues and softirqs/tasklets. If the deferred work needs to sleep, work queues are used. If the deferred work need not sleep, softirqs or tasklets are used. Indeed, the usual alternative to work queues is kernel threads. Because the kernel developers frown upon creating a new kernel thread (and, in some locales, it is a punishable offense), work queues are strongly preferred.They are really easy to use, too.
If you need a schedulable entity to perform your bottom-half processing, you need work queues.They are the only bottom-half mechanisms that run in process context, and thus, the only ones that can sleep.This means they are useful for situations in which you need to allocate a lot of memory, obtain a semaphore, or perform block I/O. If you do not need a kernel thread to handle your deferred work, consider a tasklet instead.

### Implementing Work Queues
In its most basic form, the work queue subsystem is an interface for creating kernel threads to handle work queued from elsewhere.These kernel threads are called worker threads.Work queues let your driver create a special worker thread to handle deferred work.The work queue subsystem, however, implements and provides a default worker thread for handling work.Therefore, in its most common form, a work queue is a simple interface for deferring work to a generic kernel thread. The default worker threads are called events/n where n is the processor number; there is one per processor.

This might be advantageous if you perform large amounts of processing in the worker thread. Processor intense and performance-critical work might benefit from its own thread.This also lightens the load on the default threads, which prevents starving the rest of the queued work.

#### Data Structures Representing the Threads
The worker threads are represented by the workqueue_struct structure:
```
/*
* The externally visible workqueue abstraction is an array of
* per-CPU workqueues:
*/
struct workqueue_struct {
    struct cpu_workqueue_struct cpu_wq[NR_CPUS];
    struct list_head list;
    const char *name;
    int singlethread;
    int freezeable;
    int rt;
};
struct cpu_workqueue_struct {
    spinlock_t lock;
    /* lock protecting this structure */
    struct list_head worklist;
    /* list of work */
    wait_queue_head_t more_work;
    struct work_struct *current_struct;
    struct workqueue_struct *wq; /* associated workqueue_struct */
    task_t *thread;
    /* associated thread */
};
```
#### Data Structures Representing the Work
All worker threads are implemented as normal kernel threads running the worker_thread() function.After initial setup, this function enters an infinite loop and goes to sleep.When work is queued, the thread is awakened and processes the work. When there is no work left to process, it goes back to sleep. The work is represented by the work_struct structure, defined in <linux/workqueue.h> :
```
struct work_struct {
    atomic_long_t data;
    struct list_head entry;
    work_func_t func;
};
```
These structures are strung into a linked list, one for each type of queue on each processor.
#### Work Queue Implementation Summary
At the highest level, there are worker threads.There can be multiple types of worker threads; there is one worker thread per processor of a given type. Parts of the kernel can create worker threads as needed. By default, there is the events worker thread. Each worker thread is represented by the cpu_workqueue_struct structure.The workqueue_struct structure represents all the worker threads of a given type.
Now, let’s approach from the lowest level, which starts with work.Your driver creates work, which it wants to defer to later.The work_struct structure represents this work. Among other things, this structure contains a pointer to the function that handles the deferred work.The work is submitted to a specific worker thread—in this case, a specific falcon thread.The worker thread then wakes up and performs the queued work. Most drivers use the existing default worker threads, named events.

### Using Work Queues
#### Creating Work
The first step is actually creating some work to defer.To create the structure statically at runtime, use DECLARE_WORK :
```
DECLARE_WORK(name, void (*func)(void *), void *data);
```
This statically creates a work_struct structure named name with handler function func 
and argument data . Alternatively, you can create work at runtime via a pointer:
```
INIT_WORK(struct work_struct *work, void (*func)(void *), void *data);
```
This dynamically initializes the work queue pointed to by work with handler function
funvc and argument data .

#### Your Work Queue Handler
The prototype for the work queue handler is
```
void work_handler(void *data)
```
A worker thread executes this function, and thus, the function runs in process context.
By default, interrupts are enabled and no locks are held. If needed, the function can sleep. Note that, despite running in process context, the work handlers cannot access user-space memory because there is no associated user-space memory map for kernel threads.The kernel can access user memory only when running on behalf of a user-space process, such as when executing a system call. Only then is user memory mapped in.
Locking between work queues or other parts of the kernel is handled just as with any other process context code.This makes writing work handlers much easier.The next two chapters cover locking.

#### Scheduling Work
Now that the work is created, we can schedule it.To queue a given work’s handler function with the default events worker threads, simply call 
```
schedule_work(&work);
```
The work is scheduled immediately and is run as soon as the events worker thread on the current processor wakes up.
Sometimes you do not want the work to execute immediately, but instead after some delay. In those cases, you can schedule work to execute at a given time in the future:
```
schedule_delayed_work(&work, delay);
```
#### Flushing Work
Queued work is executed when the worker thread next wakes up. Sometimes, you need to ensure that a given batch of work has completed before continuing.This is especially important for modules, which almost certainly want to call this function before  unloading. Other places in the kernel also might need to make certain no work is pending, to prevent race conditions.
For these needs, there is a function to flush a given work queue:
```
void flush_scheduled_work(void);
```
This function waits until all entries in the queue are executed before returning.While waiting for any pending work to execute, the function sleeps.Therefore, you can call it only from process context.
Note that this function does not cancel any delayed work.
To cancel delayed work, call
```
int cancel_delayed_work(struct work_struct *work);
```
This function cancels the pending work, if any, associated with the given work_struct .

#### Creating New Work Queues
You create a new work queue and the associated worker threads via a simple function:
```
struct workqueue_struct *create_workqueue(const char *name);
```
The parameter name is used to name the kernel threads. For example, the default events
queue is created via
```
struct workqueue_struct *keventd_wq;
keventd_wq = create_workqueue(“events”);
```
This function creates all the worker threads (one for each processor in the system) and prepares them to handle work.
Creating work is handled in the same manner regardless of the queue type. After the work is created, the following functions are analogous to schedule_work() and schedule_delayed_work() , except that they work on the given work queue and not the
default events queue.
```
int queue_work(struct workqueue_struct *wq, struct work_struct *work)
int queue_delayed_work(struct workqueue_struct *wq,
    struct work_struct *work,
    unsigned long delay)
```
Finally, you can flush a wait queue via a call to the function
```
flush_workqueue(struct workqueue_struct *wq)
```
### The Old Task Queue Mechanism

## Which Bottom Half Should I Use?
Softirqs, by design, provide the least serialization.This requires softirq handlers to go through extra steps to ensure that shared data is safe because two or more softirqs of the same type may run concurrently on different processors. If the code in question is already highly threaded, such as in a networking subsystem that is chest-deep in per-processor variables, softirqs make a good choice.They are certainly the fastest alternative for timingcritical and high-frequency uses.
Tasklets make more sense if the code is not finely threaded.They have a simpler interface and, because two tasklets of the same type might not run concurrently, they are easier to implement.Tasklets are effectively softirqs that do not run concurrently. A driver developer should always choose tasklets over softirqs, unless prepared to utilize per-processor variables or similar magic to ensure that the softirq can safely run concurrently on multiple processors.

Bottom Half - Context - Inherent Serialization
* Softirq - Interrupt - None
* Tasklet - Interrupt - Against the same tasklet
* Work queues - Process - None (scheduled as process context)
## Locking Between the Bottom Halves
If process context code and a bottom half share data, you need to disable bottom-half processing and obtain a lock before accessing the data. Doing both ensures local and SMP protection and prevents a deadlock.
If interrupt context code and a bottom half share data, you need to disable interrupts and obtain a lock before accessing the data.This also ensures both local and SMP protection and prevents a deadlock.
Any shared data in a work queue requires locking, too.The locking issues are no different from normal kernel code because work queues run in process context.

## Disabling Bottom Halves
To disable all bottom-half processing (specifically, all softirqs and thus all tasklets), call local_bh_disable() .To enable bottom-half processing, call local_bh_enable() .

#### Bottom Half Control Methods
```Method - Description```
* ```void local_bh_disable()``` - Disables softirq and tasklet processing on the local
processor
* ```void local_bh_enable()```  - Enables softirq and tasklet processing on the local
processor

