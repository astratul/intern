# Portability

Linux is a portable operating system that supports a wide range of computer architecture. Portability refers to how easily—if at all—code can move from one system architecture to another.We know that Linux is portable because it has already been ported to various systems. But this portability did not occur overnight—it requires diligence and a constant eye toward writing portable code. Consequently, it is now easy, relatively speaking, to bring Linux up on a new system.This chapter discusses how to write portable code—the issues you need to keep in mind when writing both core kernel code and device drivers.

## Portable Operating Systems

Some operating systems are designed with portability as a primary feature.As little code as possible is machine-specific.Assembly is kept to a minimum, and interfaces and features are sufficiently general and abstract so that they work on a wide range of architectures. 
* The obvious benefit is the relative ease with which a new architecture can be supported.
* The downside is that architecture specific features are not supported, and code cannot be hand-tuned for a specific machine. With this design choice, optimal code is traded for portable code.

Linux takes the middle road toward portability.As much as practical, interfaces and core code are architecture-independent C code. This approach enables Linux to remain portable without foregoing optimizations.Where portability would hinder performance, performance generally wins.

## History of Portability in Linux

## Word Size and Data Types

A word is the amount of data that a machine can process at one time.This fits into the document analogy that includes characters (usually 8 bits) and pages (many words, often 4KB or 8KB worth) as other measurements of data.A word is an integer number of bytes—for example, one, two, four, or eight.When someone talks about the “n-bits” of a machine, they are generally talking about the machine’s word size.
The size of a processor’s general-purpose registers (GPRs) is equal to its word size.The widths of the components in a given architecture—for example, the memory bus—are usually at least as wide as the word size.
Typically, at least in the architectures that Linux supports, the virtual memory address space is equal to the word size, although the physical address space is sometimes less. Consequently, the size of a pointer is equal to the word size.Additionally, the size of the C type long is equal to the word size, whereas the size of the int type is sometimes less than that of the word size.
Each supported architecture under Linux defines BITS_PER_LONG in <asm/types.h> to the length of the C long type, which is the system word size.
Some rules to keep in mind:
* As dictated by the ANSI C standard, a char is always 1 byte.
* Although there is no rule that the int type be 32 bits, it is in Linux on all currently supported architectures.
* The same goes for the short type, which is 16 bits on all current architectures, although no rule explicitly decrees that.
* Never assume the size of a pointer or a long , which can be either 32 bits or 64 bits on the currently supported machines in Linux.
* Because the size of a long varies on different architectures, never assume that sizeof(int) is equal to sizeof(long).
* Likewise, do not assume that a pointer and an int are the same size.

### Opaque Types

Opaque data types do not reveal their internal format or structure.They are about as “black box” as you can get in C.There is not a lot of language support for them. Instead, developers declare a typedef , call it an opaque type, and hope no one typecasts it back to a standard C type.All use is generally through a special set of interfaces that the developer creates.An example is the pid_t type, which stores a process identification number.The actual size of this type is not revealed—although anyone can cheat and take a peak and see that it is an int . If no code makes explicit use of this type’s size, it can be changed without too much hassle.
Another example of an opaque type is atomic_t . This type holds an integer value that can be manipulated atomically.Although this type is an int , using the opaque type helps ensure that the data is used only in the special atomic operation functions.The opaque type also helps hide the usable size of atomic_t , which was not always the full 32 bits because of architectural limitations on 32-bit SPARC.
Keep the following rules in mind when dealing with opaque types:
* Do not assume the size of the type. It might be 32-bit on some systems and 64-bit on others. Moreover, kernel developers are free to change its size over time.
* Do not convert the type back to a standard C type.
* Be size agnostic.Write your code so that the actual storage and format of the type can change.

### Special Types

Some data in the kernel, despite not being represented by an opaque type, requires a specific data type. One example is the flags parameter used in interrupt control, which should always be stored in an unsigned long .

### Explicitly Sized Types

Often, as a programmer, you need explicitly sized data in your code.This is usually to match external requirements, such as those imposed by hardware, networking, or binary files.The kernel defines these explicitly sized data types in <asm/types.h> , which is included by <linux/types.h> .
##### Explicitly Sized Data Types
Type - Description
* s8 - Signed byte
* u8 - Unsigned byte
* s16 - Signed 16-bit integer
* u16 - Unsigned 16-bit integer
* s32 - Signed 32-bit integer
* u32 - Unsigned 32-bit integer
* s64 - Signed 64-bit integer
* u64 - Unsigned 64-bit integer

### Signedness of Chars

The C standard says that the char data type can be either signed or unsigned. It is the responsibility of the compiler, the processor, or both to decide what the suitable default for the char type is.
On most architectures, char is signed by default and thus has a range from –128 to 127. On a few other architectures, such as ARM, char is unsigned by default and has a range from 0 to 255.

## Data Alignment

Alignment refers to a piece of data’s location in memory.A variable is naturally aligned if it exists at a memory address that is a multiple of its size.

### Avoiding Alignment Issues

The compiler generally prevents alignment issues by naturally aligning all data types. In fact, alignment issues are normally not major concerns of the kernel developers—the gcc developers worry about them so other programmers need not. Issues arise, however, when a programmer plays too closely with pointers and accesses data outside the environment anticipated by the compiler.
Accessing an aligned address with a recast pointer of a larger-aligned address causes an alignment issue (whatever that might mean for a particular architecture).

### Alignment of Nonstandard Types

As mentioned, the aligned address of a standard data type is a multiple of the size of that data type. Nonstandard (complex) C types have the following alignment rules:
* The alignment of an array is the alignment of the base type; thus, each element is further aligned correctly.
* The alignment of a union is the alignment of the largest included type.
* The alignment of a structure is such that an array of the structure will have each element of the array properly aligned.

Structures also introduce padding, which introduces other issues.

### Structure Padding

Structures are padded so that each element of the structure is naturally aligned.This ensures that when the processor accesses a given element in the structure, that element is aligned.
Kernel developers need to be aware of structure padding when using structures wholesale—that is, when sending them out over the network or when saving a structure directly to disk, because the required padding might differ among various architectures.This is one reason C does not have a native structure comparison operator.The padding in a structure might contain gibberish, and it is not possible to do a byte-by-byte comparison of one structure to another.The C designers (correctly) felt it is best for the programmer to write a comparison function unique to each situation, taking advantage of the structure’s layout.

## Byte Order

Byte ordering is the order of bytes within a word. Processors can number the bytes in a word such that the least significant bit is either the first (left-most) or last (right-most) value in the word.The byte ordering is called big-endian if the most significant byte is encoded first, with the remaining bytes decreasing in significance.The byte ordering is called little-endian if the least significant byte is encoded first, with the remaining bytes growing in significance.
Never assume any given byte ordering when writing kernel code (unless you are writing code for a specific architecture, of course).The Linux kernel supports machines of both byte orders—including machines that can select from either ordering upon boot—and generic code must be compatible with either. 
Each supported architecture in Linux defines one of __BIG_ENDIAN or __LITTLE_ENDIAN  in <asm/byteorder.h> in correspondence to the machine’s byte order.

## Time

The measurement of time is another kernel concept that can differ between architectures or even kernel revisions. Never assume the frequency of the timer interrupt or the number of jiffies per second. Instead, always use HZ to scale your units of time correctly.This is important because not only can the timer frequency differ among the various architectures, but it can also change on a given architecture from one kernel release to the next.
HZ is defined in <asm/param.h> .

## Page Size

When working with pages of memory, never assume the page size. It is a common mis- take for x86-32 programmers to assume that the page size is 4KB.Although this is true on x86-32 machines, other architectures have different sizes. Some architectures support multiple page sizes, in fact!
When working with pages of memory, use PAGE_SIZE as the size of a page, in bytes. The value PAGE_SHIFT is the number of bits to left-shift an address to derive its page number.

## Processor Ordering

In your code, if you depend on data ordering, ensure that even the weakest ordered processor commits your load and stores in the right order by using the appropriate barriers, such as rmb() and wmb() .

## SMP, Kernel Preemption, and High Memory

In addition to the previous portability rules, you need to follow these as well:
* Always assume your code will run on an SMP system and use appropriate locking.
* Always assume your code will run with kernel preemption enabled and use appropriate locking and kernel preemption statements.
* Always assume your code will run on a system with high memory (memory not permanently mapped) and use kmap() as needed.
