# Timers and Time Management
The interrupt handler for this timer—called the timer interrupt—updates the system time and performs periodic work.The system timer and its timer interrupt are central to Linux and a large focus of this chapter.
The other focus of this chapter is dynamic timers, the facility used to schedule events that run once after a specified time has elapsed.


## Kernel Notion of Time
The hardware provides a system timer that the kernel uses to gauge the passing of time.This system timer works off of an electronic time source, such as a digital clock or the frequency of the processor.The system timer goes off (often called hitting or popping) at a preprogrammed frequency, called the tick rate.When the system timer goes off, it issues an interrupt that the kernel handles via a special interrupt handler.
Because the kernel knows the preprogrammed tick rate, it knows the time between any two successive timer interrupts.This period is called a tick and is equal to 1/(tick rate) seconds.This is how the kernel keeps track of both wall time and system uptime. 
Wall time—the actual time of day—is important to user-space applications.The kernel keeps track of it simply because the kernel controls the timer interrupt.A family of system calls provides the date and time of day to user-space.The system uptime—the relative time since the system booted—is useful to both kernel-space and user-space.A lot of code must be aware of the passing of time.The difference between two uptime readings—now and then—is a simple measure of this relativity.
The timer interrupt is important to the management of the operating system.A large number of kernel functions live and die by the passing of time. Some of the work executed periodically by the timer interrupt includes
* Updating the system uptime
* Updating the time of day
* On an SMP system, ensuring that the scheduler runqueues are balanced and, if not, balancing them
* Running any dynamic timers that have expired
* Updating resource usage and processor time statistics
Some of this work occurs on every timer interrupt—that is, the work is carried out with the frequency of the tick rate. Other functions execute periodically but only every n timer interrupts.That is, these functions occur at some fraction of the tick rate.

## The Tick Rate: HZ
The frequency of the system timer (the tick rate) is programmed on system boot based on a static preprocessor define, HZ .The value of HZ differs for each supported architecture. On some supported architectures, it even differs between machine types. The kernel defines the value in <asm/param.h> .The tick rate has a frequency of HZ hertz and a period of 1/HZ seconds.

### The Ideal HZ Value
Increasing the tick rate means the timer interrupt runs more frequently. Consequently, the work it performs occurs more often.This has the following benefits:
* The timer interrupt has a higher resolution and, consequently, all timed events have a higher resolution.
* The accuracy of timed events improves.
The resolution increases by the same factor as the tick rate increases.

### Advantages with a Larger HZ

This higher resolution and greater accuracy provides multiple advantages:
* Kernel timers execute with finer resolution and increased accuracy. (This provides a large number of improvements, one of which is the following.)
* System calls such as poll() and select() that optionally employ a timeout value execute with improved precision.
* Measurements, such as resource usage or the system uptime, are recorded with a finer resolution.
* Process preemption occurs more accurately.

Some of the most readily noticeable performance benefits come from the improved precision of poll() and select() timeouts.The improvement might be quite large; an application that makes heavy use of these system calls might waste a great deal of time waiting for the timer interrupt, when, in fact, the timeout has actually expired. Remember, the average error (that is, potentially wasted time) is half the period of the timer interrupt.
Another benefit of a higher tick rate is the greater accuracy in process preemption, which results in decreased scheduling latency.
The problem stems from the latency created by the delayed preemption. If the to-be-scheduled task had something time-sensitive to do, such as refill an audio buffer, the delay might not be acceptable.

### Disadvantages with a Larger HZ

A higher tick rate implies more frequent timer interrupts, which implies higher overhead, because the processor must spend more time executing the timer interrupt handler.The higher the tick rate, the more time the processor spends executing the timer interrupt.This adds up to not just less processor time available for other work, but also a more frequent thrashing of the processor’s cache and increase in power consumption.


## Jiffies

The global variable jiffies holds the number of ticks that have occurred since the system booted. On boot, the kernel initializes the variable to zero, and it is incremented by one during each timer interrupt.Thus, because there are HZ timer interrupts in a second, there are HZ jiffies in a second.The system uptime is therefore jiffies/HZ seconds.What actually happens is slightly more complicated:The kernel initializes jiffies to a special initial value, causing the variable to overflow more often, catching bugs.When the actual value of jiffies is sought, this “offset” is first subtracted.

In operating systems, especially Unix, a jiffy is the time between two successive clock ticks. Historically, this has been 10ms. As we have seen in this chapter, however, a jiffy in Linux can have various values.

The jiffies variable is declared in <linux/jiffies.h> as
```
extern unsigned long volatile jiffies;
```
The following expression converts from seconds to a unit of jiffies :
```
(seconds * HZ)
```
Likewise, this expression converts from jiffies to seconds:
```
(jiffies / HZ)
```
The former, converting from seconds to ticks, is more common. For example, code often needs to set a value for some time in the future, for example:
```
unsigned long time_stamp = jiffies; // now
unsigned long next_tick = jiffies + 1; // one tick from now
unsigned long later = jiffies + 5*HZ; // five seconds from now
unsigned long fraction = jiffies + HZ / 10; // a tenth of a second from now 
```
Converting from ticks to seconds is typically reserved for communicating with userspace, as the kernel itself rarely cares about any sort of absolute time.
Note that the jiffies variable is prototyped as unsigned long and that storing it in anything else is incorrect.


### Internal Representation of Jiffies

The jiffies variable has always been an unsigned long , and therefore 32 bits in size on 32-bit architectures and 64-bits on 64-bit architectures.With a tick rate of 100, a 32-bit jiffies variable would overflow in about 497 days.With HZ increased to 1000, however, that overflow now occurs in just 49.7 days! If jiffies were stored in a 64-bit variable on all architectures, then for any reasonable HZ value the jiffies variable would never overflow in anyone’s lifetime.
For performance and historical reasons—mainly compatibility with existing kernel code—the kernel developers wanted to keep jiffies an unsigned long . Some smart thinking and a little linker magic saved that day. As you previously saw, jiffies is defined as an unsigned long :
```
extern unsigned long volatile jiffies;
```
A second variable is also defined in <linux/jiffies.h> :
```
extern u64 jiffies_64;
```
### Jiffies Wraparound
The jiffies variable, like any C integer, experiences overflow when its value is increased beyond its maximum storage limit. For a 32-bit unsigned integer, the maximum value is 2^32 – 1 .Thus, a possible 4294967295 timer ticks can occur before the tick count overflows.When the tick count is equal to this maximum and it is incremented, it wraps around to zero.
Look at an example of a wraparound:
```
unsigned long timeout = jiffies + HZ/2; /* timeout in 0.5s */
/* do some work ... */
/* then see whether we took too long */
if (timeout > jiffies) {
/* we did not time out, good ... */
} else {
/* we timed out, error ... */
}
```
### User-Space and HZ
The function jiffies_to_clock_t() , defined in kernel/time.c , is then used to scale a tick count in terms of HZ to a tick count in terms of USER_HZ .The expression used depends on whether USER_HZ and HZ are integer multiples of themselves and whether USER_HZ is less than or equal to HZ . If both those conditions are true, and for most systems they usually are, the expression is rather simple:
```
return x / (HZ / USER_HZ);
```
the function jiffies_64_to_clock_t() is provided to convert a 64-bit jiffies value from HZ to USER_HZ units.
These functions are used anywhere a value in ticks-per-seconds needs to be exported to user-space. Following is an example:
```
unsigned long start;
unsigned long total_time;
start = jiffies;
/* do some work ... */
total_time = jiffies - start;
printk(“That took %lu ticks\n”, jiffies_to_clock_t(total_time));
```
User-space expects the previous value as if HZ=USER_HZ . If they are not equivalent, the macro scales as needed and everyone is happy. Of course, this example is silly: It would make more sense to print the message in seconds, not ticks. For example:
```
printk(“That took %lu seconds\n”, total_time / HZ);
```
## Hardware Clocks and Timers
Architectures provide two hardware devices to help with time keeping: the system timer, which we have been discussing, and the real-time clock.
### Real-Time Clock

The real-time clock (RTC) provides a nonvolatile device for storing the system time. The RTC continues to keep track of time even when the system is off by way of a small battery typically included on the system board. On the PC architecture, the RTC and the CMOS are integrated, and a single battery keeps the RTC running and the BIOS settings preserved.
On boot, the kernel reads the RTC and uses it to initialize the wall time, which is stored in the xtime variable.The kernel does not typically read the value again; however, some supported architectures, such as x86, periodically save the current wall time back to the RTC. Nonetheless, the real time clock’s primary importance is only during boot, when the xtime variable is initialized.
### System Timer
The system timer serves a much more important (and frequent) role in the kernel’s timekeeping.The idea behind the system timer, regardless of architecture, is the same—to provide a mechanism for driving an interrupt at a periodic rate. Some architectures implement this via an electronic clock that oscillates at a programmable frequency. Other systems provide a decrementer:A counter is set to some initial value and decrements at a fixed rate until the counter reaches zero.When the counter reaches zero, an interrupt is triggered. In any case, the effect is the same.
On x86, the primary system timer is the programmable interrupt timer (PIT).The PIT exists on all PC machines and has been driving interrupts since the days of DOS.The kernel programs the PIT on boot to drive the system timer interrupt (interrupt zero) at HZ frequency. It is a simple device with limited functionality, but it gets the job done.
Other x86 time sources include the local APIC timer and the processor’s time stamp counter (TSC).

## The Timer Interrupt Handler
The timer interrupt is broken into two pieces: an architecture-dependent and an architecture-independent routine.
The architecture-dependent routine is registered as the interrupt handler for the system timer and, thus, runs when the timer interrupt hits. Its exact job depends on the given architecture, of course, but most handlers perform at least the following work:
* Obtain the xtime_lock lock, which protects access to jiffies_64 and the wall time value, xtime .
* Acknowledge or reset the system timer as required.
* Periodically save the updated wall time to the real time clock.
* Call the architecture-independent timer routine, tick_periodic() .

The architecture-independent routine, tick_periodic() , performs much more work:
* Increment the jiffies_64 count by one. (This is safe, even on 32-bit architectures, because the xtime_lock lock was previously obtained.)
* Update resource usages, such as consumed system and user time, for the currently running process.
* Run any dynamic timers that have expired (discussed in the following section).
* Execute scheduler_tick().
* Update the wall time, which is stored in xtime .
* Calculate the infamous load average.

## The Time of Day

The current time of day (the wall time) is defined in kernel/time/timekeeping.c :
```
struct timespec xtime;
```
The timespec data structure is defined in <linux/time.h> as:
```
struct timespec {
__kernel_time_t tv_sec; /* seconds */
long tv_nsec; /* nanoseconds */
}; 
```
The kernel also implements the time() 5 system call, but gettimeofday() largely supersedes it.The C library also provides other wall time–related library calls, such as ftime() and ctime() . The settimeofday() system call sets the wall time to the specified value. It requires the CAP_SYS_TIME capability. Other than updating xtime , the kernel does not make nearly as frequent use of the current wall time as user-space does.

## Timers

Timers—sometimes called dynamic timers or kernel timers—are essential for managing the flow of time in kernel code. Kernel code often needs to delay execution of some function until a later time.
The purpose of bottom halves is not so much to delay work, but simply to not do the work now.What we need is a tool for delaying work a specified amount of time—certainly no less, and with hope, not much longer.The solution is kernel timers. A timer is easy to use.You perform some initial setup, specify an expiration time, specify a function to execute upon said expiration, and activate the timer.The given function runs after the timer expires.Timers are not cyclic.The timer is destroyed after it expires. This is one reason for the dynamic nomenclature:Timers are constantly created and destroyed, and there is no limit on the number of timers.Timers are popular throughout the entire kernel.

### Using Timers
Timers are represented by struct timer_list , which is defined in <linux/timer.h> :
```
struct timer_list {
struct list_head entry; // entry in linked list of timers 
unsigned long expires; // expiration value, in jiffies 
void (*function)(unsigned long); // the timer handler function 
unsigned long data; // lone argument to the handler 
struct tvec_t_base_s *base; // internal timer field, do not touch 
};
```
The first step in creating a timer is defining it:
```
struct timer_list my_timer;
```
Next, the timer’s internal values must be initialized.This is done via a helper function and must be done prior to calling any timer management functions on the timer:
```
init_timer(&my_timer);
```
Now you fill out the remaining values as required:
```
my_timer.expires = jiffies + delay; /* timer expires in delay ticks */
my_timer.data = 0; /* zero is passed to the timer handler */
my_timer.function = my_function; /* function to run when timer expires */
```
The my_timer.expires value specifies the timeout value in absolute ticks.When the current jiffies count is equal to or greater than my_timer.expires , the handler function my_timer.function is run with the lone argument of my_timer.data .As you can see from the timer_list definition, the function must match this prototype:
```
void my_timer_function(unsigned long data); 
```
The data parameter enables you to register multiple timers with the same handler, and differentiate between them via the argument. If you do not need the argument, you can simply pass zero (or any other value). Finally, you activate the timer:
```
add_timer(&my_timer);
```
If you need to deactivate a timer prior to its expiration, use the del_timer() function:
```
del_timer(&my_timer);
```

### Timer Race Conditions

Because timers run asynchronously with respect to the currently executing code, several potential race conditions exist. First, never do the following as a substitute for a mere mod_timer() , because this is unsafe on multiprocessing machines:
```
del_timer(my_timer)
my_timer->expires = jiffies + new_delay;
add_timer(my_timer);
```
Second, in almost all cases, you should use del_timer_sync() over del_timer() . Otherwise, you cannot assume the timer is not currently running, and that is why you made the call in the first place! Imagine if, after deleting the timer, the code went on to free or otherwise manipulate resources used by the timer handler.Therefore, the synchronous version is preferred. Finally, you must make sure to protect any shared data used in the timer handler function.The kernel runs the function asynchronously with respect to other code.

### Timer Implementation

The kernel executes timers in bottom-half context, as softirqs, after the timer interrupt completes.The timer interrupt handler runs update_process_times() , which calls run_local_timers() : 
```
void run_local_timers(void)
{
hrtimer_run_queues();
raise_softirq(TIMER_SOFTIRQ); /* raise the timer softirq */
softlockup_tick();
}
```
The TIMER_SOFTIRQ softirq is handled by run_timer_softirq() .This function runs all the expired timers (if any) on the current processor. Timers are stored in a linked list.

## Delaying Execution

Often, kernel code (especially drivers) needs a way to delay execution for some time without using timers or a bottom-half mechanism.This is usually to enable hardware time to complete a given task.The time is typically quite short. 

### Busy Looping
The simplest solution to implement (although rarely the optimal solution) is busy waiting or busy looping.This technique works only when the time you want to delay is some integer multiple of the tick rate or precision is not important. The idea is simple: Spin in a loop until the desired number of clock ticks pass.
```
unsigned long timeout = jiffies + 10; /* ten ticks */
while (time_before(jiffies, timeout))
;
```
The loop continues until jiffies is larger than delay , which occurs only after 10 clock ticks have passed.
This approach is not nice to the rest of the system.While your code waits, the processor is tied up spinning in a silly loop—no useful work is accomplished! You rarely want to take this brain-dead approach, and it is shown here because it is a clear and simple method for delaying execution.You might also encounter it in someone else’s not-so-pretty code.
The kernel requires, however, that jiffies be reread on each iteration, as the value is incremented elsewhere: in the timer interrupt. Indeed, this is why the variable is marked volatile in <linux/jiffies.h> .The volatile keyword instructs the compiler to reload the variable on each access from main memory and never alias the variable’s value in a register, guaranteeing that the previous loop completes as expected.

### Small Delays
Sometimes, kernel code (again, usually drivers) requires short (smaller than a clock tick) and rather precise delays.This is often to synchronize with hardware, which again usually lists some minimum time for an activity to complete—often less than a millisecond. It would be impossible to use jiffies -based delays, as in the previous examples, for such a short wait.
Thankfully, the kernel provides three functions for microsecond, nanosecond, and millisecond delays, defined in <linux/delay.h> and <asm/delay.h >, which do not use jiffies :
```
void udelay(unsigned long usecs)
void ndelay(unsigned long nsecs)
void mdelay(unsigned long msecs)
```
The former function delays execution by busy looping for the specified number of microseconds.The latter function delays execution for the specified number of milliseconds.
The udelay() function is implemented as a loop that knows how many iterations can be executed in a given period of time.The mdelay() function is then implemented in terms of udelay() . Because the kernel knows how many loops the processor can complete in a second (see the sidebar on BogoMIPS), the udelay() function simply scales that value to the correct number of loop iterations for the given delay.

The udelay() function should be called only for small delays because larger delays on fast machines might result in overflow.As a rule, do not use udelay() for delays more than one millisecond in duration. For longer durations, mdelay() works fine. Like the other busy waiting solutions for delaying execution, neither of these functions (especially mdelay() , because it is used for such long delays) should be used unless absolutely needed.

### schedule_timeout()
A more optimal method of delaying execution is to use schedule_timeout() .This call puts your task to sleep until at least the specified time has elapsed.There is no guarantee that the sleep duration will be exactly the specified time—only that the duration is at least as long as specified.When the specified time has elapsed, the kernel wakes the task up and places it back on the runqueue. Usage is easy:
```
/* set task’s state to interruptible sleep */
set_current_state(TASK_INTERRUPTIBLE);
/* take a nap and wake up in “s” seconds */
schedule_timeout(s * HZ);
```
The lone parameter is the desired relative timeout, in jiffies.This example puts the task in interruptible sleep for s seconds. Because the task is marked TASK_INTERRUPTIBLE , it wakes up prematurely if it receives a signal. If the code does not want to process signals, you can use TASK_UNINTERRUPTIBLE instead.The task must be in one of these two states before schedule_timeout() is called or else the task will not go to sleep. Note that because schedule_timeout() invokes the scheduler, code that calls it must be capable of sleeping.

#### schedule_timeout() Implementation

The function creates a timer with the original name timer and sets it to expire in timeout clock ticks in the future. It sets the timer to execute the process_timeout() function when the timer expires. It then enables the timer and calls schedule() . Because the task is supposedly marked TASK_INTERRUPTIBLE or TASK_UNINTERRUPTIBLE , the scheduler does not run the task, but instead picks a new one. When the timer expires, it runs process_timeout() :
```
void process_timeout(unsigned long data)
{
wake_up_process((task_t *) data);
}
```
This function puts the task in the TASK_RUNNING state and places it back on the runqueue.
When the task reschedules, it returns to where it left off in schedule_timeout() (right after the call to schedule() ). In case the task was awakened prematurely (if a signal was received), the timer is destroyed.The function then returns the time slept.
The code in the switch() statement is for special cases and is not part of the general usage of the function.The MAX_SCHEDULE_TIMEOUT check enables a task to sleep indefinitely. In that case, no timer is set (because there is no bound on the sleep duration), and the scheduler is immediately invoked. If you do this, you must have another method of waking your task up!

#### Sleeping on a Wait Queue, with a Timeout
Sometimes it is desirable to wait for a specific event or wait for a specified time to elapse—whichever comes first. In those cases, code might simply call schedule_timeout() instead of schedule() after placing itself on a wait queue.The task wakes up when the desired event occurs or the specified time elapses.The code needs to check why it woke up—it might be because of the event occurring, the time elapsing, or a received signal—and continue as appropriate.
