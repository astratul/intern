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


## Timers

### Using Timers

### Timer Race Conditions

### Timer Implementation

## Delaying Execution

### Busy Looping

### Small Delays

### schedule_timeout()
