# Summary - Debugging

Topics we cover include
* Types of errors
* General debugging techniques
* Debugging with GDB and other tools
* Assertions
* Memory use debugging

## Types of Errors

* Specification errors: If a program is incorrectly specified, it will inevitably fail to perform as required.
* Design errors: Programs of any size need to be designed before they’re created.
* Coding errors: Of course, everyone makes typing errors.

## General Debugging Techniques

The five stages of debugging are:
* Testing: Finding out what defects or bugs exist
* Stabilization: Making the bugs reproducable
* Localization: Identifying the line(s) of code responsible
* Correction: Fixing the code
* Verification: Making sure the fix works

### A Program with Bugs

```
cc -o debug1 debug1.c
./debug1

cc -o debug2 debug2.c
./debug2

cc -o debug3 debug3.c
./debug3
```

### Code Inspection

There are tools that you can use to help with code reviews, the compiler being an obvious one. It will tell you if you have any syntax errors in your program.
```
gcc -Wall -pedantic –ansi
```
### Instrumentation

Instrumentation is the adding of code to a program for the purpose of collecting more information about the behavior of the program as it runs. It’s very common to add printf calls, as in the example, to print out the values of variables at different stages in a program’s execution. You could usefully add several printf calls, but you should be aware that the process entails an additional edit and compile whenever the program is changed, and of course, you will need to remove the code when the bugs are fixed.
There are two instrumentation techniques that can help here. The first uses the C preprocessor to selectively include instrumentation code so that you only need to recompile the program to include or exclude debugging code. You can do this quite simply with constructs such as:
```
#ifdef DEBUG
printf(“variable x has value = %d\n”, x);
#endif
```
You can compile the program with the compiler flag -DDEBUG to define the DEBUG symbol and include the extra code or without to exclude it. You can make more sophisticated use of a numeric debug macro, like this:
```
#define BASIC_DEBUG 1
#define EXTRA_DEBUG 2
#define SUPER_DEBUG 4
#if (DEBUG & EXTRA_DEBUG)
printf...
#endif
```
In this case, you must always define the DEBUG macro, but you can set it to represent a set of debug information, or a level of detail. The compiler flag -DDEBUG=5 would, in this case, enable BASIC_DEBUG and SUPER_DEBUG , but not EXTRA_DEBUG . The flag -DDEBUG=0 would disable all debug information. Alternatively, including the following lines eliminates the need to specify DEBUG on the command line in the case where no debugging is required:
```
#ifndef DEBUG
#define DEBUG 0
#endif
```

Several macros defined by the C preprocessor can help with debug information. These are macros that expand to give information about the current compilation:

Macro       Description
* __LINE__ - A decimal constant representing the current line number
* __FILE__ - A string representing the current filename
* __DATE__ - A string of the form “mmm dd yyyy” , the current date
* __TIME__ - A string of the form “hh:mm:ss” , the current time

#### Try It Out
* cinfo.c

### Debugging without Recompiling

Before we move on, it’s worth mentioning that there’s a way of using the printf function to help with debugging without using the #ifdef DEBUG technique, which requires a program to be recompiled before it can be used.
The method is to add a global variable as a debug flag, allow a -d option at the command line, which allows the user to switch debugging on even after the program has been released, and add a debug logging function. Now you can intersperse things like this in the program code:
```
if (debug) {
sprintf(msg, ...)
write_debug(msg)
}
```
You should write debug output to stderr , or, if this isn’t practical because of the nature of the program, use the logging facilities provided by the syslog function.
### Controlled Execution

A number of debuggers are available on commercial UNIX systems, depending on the vendor. Common ones are adb , sdb , idebug , and dbx . The more sophisticated ones allow you to look at the state of the program in some detail at a source-code level. This is the case with the GNU debugger, gdb , which can be used with Linux and many UNIX variants. There also exist “front ends” to gdb , which make it more user-friendly; xxgdb , KDbg , and ddd are such programs.
The -g flag is the usual one used to compile a program for debugging. You must use it for compiling each source file that needs to be debugged and also for the linker, so that special versions of the standard C library can be used to provide debug support in library functions. The compiler program will pass the flag to the linker automatically. Debugging can be used with libraries that aren’t compiled for the purpose, but with less flexibility.

Debug information can make the executable many (up to ten) times larger. Even though the executable may be larger (and take up more disk space) the amount of memory the program needs to run is effectively the same. It is usually a good idea to remove debug information before you release your programs, but only after they have been debugged.
You can remove debug information from an executable file without recompiling by running
```
strip <file> .
```
## Debugging with gdb

### Starting gdb
```
cc -g -o debug3 debug3.c
gdb debug3
```
gdb is itself a text-based application, but it does provide a few shortcuts to help with repetitive tasks. Many versions have command-line editing with a history so that you can scroll back and execute the same command again (try using the cursor keys).

### Running a Program
You can execute the program with the run command. Any arguments that you give to the run command are passed to the program as its arguments. In this case, you don’t need any arguments. We’ll assume here that your system, like both the authors’, is now generating a segmentation fault. If it isn’t, read on. You’ll find out what to do when one of your own programs does generate a segmentation violation. If you’re not getting a segmentation violation, but want to work though this example as you read the book, you can pick up the program again at debug4.c , when the first of the memory access problems has been fixed.

```
(gdb) run
```

### Stack Trace
```
(gdb) backtrace
```
The backtrace command may be abbreviated bt , and, for compatibility with other debuggers, the where command has the same function.

### Examining Variables
```
(gdb) print j
(gdb) print a[j]
(gdb) print a[3]
(gdb) print a[$-1].key

```
### Listing the Program
You can view the source code of the program from within gdb by using the list command. This prints out a portion of the code around the current position. Subsequent uses of list will print out more. You can also give list a function name as an argument and it will show the code at that position, or a pair of line numbers and it will list the code between those line numbers.
```
(gdb) list
cc -g -o debug4 debug4.c
./debug4
```
### Setting Breakpoints 

To find out where the program is failing, you need to be able to see what it’s doing as it runs. You can stop the program at any point by setting breakpoints. These cause the program to stop and return control to the debugger. You’ll be able to inspect variables and then allow the program to continue.
There are two loops in the sort function. The outer loop, with loop variable i , is run once for each element in the array. The inner loop swaps the element with those further down the list. This has the effect of bubbling up the smaller elements to the top. After each run of the outer loop, the largest element should have made its way to the bottom. You can confirm this by stopping the program at the outer loop and examining the state of the array.

```
(gdb) help breakpoint

gdb debug4
(gdb) break 24
(gdb) run
(gdb) print array[0]
(gdb) print array[0]@5
# To print a number of consecutive items, you can use the construction @<number> to cause gdb to print a number of array elements. To print all five elements of array , you can use
(gdb) cont
(gdb) print array[0]@5
(gdb) display array[0]@5
(gdb) commands
> cont
> end
(gdb) cont
```
gdb reports that the program exits with an unusual exit code. This is because the program itself doesn’t call exit and doesn’t return a value from main . The exit code is effectively meaningless in this case, and a meaningful one ought to be provided by a call to exit .

### Patching with the Debugger
```
(gdb) info display
(gdb) info break
# You can either disable these or delete them entirely. If you disable them, you retain the option to reenable them at a later time if you need to:
(gdb) disable break 1
(gdb) disable display 1
(gdb) break 33
(gdb) commands 2
>set variable n = n+1
>cont
>end
(gdb) run

```
### Learning More about gdb

## More Debugging Tools

### Lint: Removing the Fluff from Your Programs
```
debug0.c
splint -strict debug0.c
```
### Function Call Tools

### ctags
The ctags program creates an index of functions. For each function, you get a list of the places it’s used, like the index of a book.
```
ctags [-a] [-f filename] sourcefile sourcefile ...
ctags -x sourcefile sourcefile ...
```
By default, ctags creates a file, called tags , in the current directory, which contains, for each function declared in any of the input source files, lines of the form

### cxref

The cxref program analyzes C source code and produces a cross-reference. It shows where each symbol (variable, #define , and function) is mentioned in the program. It produces a sorted list with each symbol’s definition location marked with an asterisk, as shown on next page.
```
cxref *.c *.h
```
### cflow
The cflow program prints a function call tree, a diagram that shows which function calls which others, and which functions are called by them, and so on. It can be useful to find out the structure of a program to understand how it operates and to see what impact changes to a function will have. Some versions of cflow can operate on object files as well as source code.

### Execution Profiling with prof/gprof
A technique that is often useful when you’re trying to track down performance problems with a program is execution profiling. Normally supported by special compiler options and ancillary programs, a profile of a program shows where it’s spending its time.
The prof program (and its GNU equivalent, gprof ) prints a report from an execution trace file that is produced when a profiled program is run. A profiled executable is created by specifying the –p flag (for prof ) or -pg flag (for gprof ) to the compiler:
```
cc -pg -o program program.c
./program
ls -ls
```
## Assertions

```
#include <assert.h>
void assert(int expression)
```
The assert macro evaluates the expression and, if it’s nonzero, writes some diagnostic information to the standard error and calls abort to end the program.

#### Try It Out
* assert.c
```
cc -o assert -DNDEBUG assert.c -lm
./assert
```

## Memory Debugging
Typically, memory blocks are allocated by malloc and assigned to pointer variables. If the pointer variable is changed and there are no other pointers pointing to the memory block, it will become inaccessible. This is a memory leak and it causes your program to grow in size. If you leak a lot of memory, your system will eventually slow down and run out of memory.
If you write beyond the end of an allocated block (or before the beginning of a block), you’ll very likely corrupt the data structures used by the malloc library to keep track of allocations. In this case, at some future time, a call to malloc , or even free , will cause a segmentation violation and your program will crash. Tracking down the precise point at which the fault occurred can be very difficult, because it may have happened a long time before the event that caused the crash. 
Unsurprisingly, there are tools, commercial and free, that can help with these two problem types. There are, for example, many different versions of malloc and free , some of which contain additional code to check on allocations and deallocations to try to cater for the cases where a block is freed twice and some other types of misuse.

### ElectricFence
The ElectricFence library was developed by Bruce Perens. It is available as an optional component in some Linux distributions such as Red Hat (Enterprise and Fedora), SUSE and openSUSE, and can be readily found on the Internet. It attempts to use the virtual memory facilities of Linux to protect the memory used by malloc and free to halt the program at the point of memory corruption.

#### Try It Out
* efence.c
```
cc -o efence efence.c
./efence

cc -g -o efence efence.c -lefence
gdb efence

```
### valgrind
valgrind is a tool that is capable of detecting many of the problems that we have discussed. In particular, it can detect array access errors and memory leaks. It may not be included with your Linux distribution, but you can find it at http://valgrind.org .

#### Try It Out
* checker.c
```
valgrind --leak-check=yes -v ./checker
```

