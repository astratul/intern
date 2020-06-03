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


## More Debugging Tools



## Assertions



## Memory Debugging


