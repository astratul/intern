# Summary

## KISS - “Keep It Small and Simple”

Note that Linux, like UNIX, uses the colon ( : ) character to separate entries in the PATH variable, rather than the semicolon ( ; ) that MS-DOS and Windows use. (UNIX chose : first, so ask Microsoft why Windows is different, not why UNIX is different!) Here’s a sample PATH variable:

/usr/local/bin:/bin:/usr/bin:.:/home/neil/bin:/usr/X11R6/bin


## The C Compiler

```
$ gcc -o hello hello.c
$ ./hello
```

You can use header files in subdirectories or nonstandard places by specifying the -I flag (for include ) to the C compiler. For example,

## Header Files

You can use header files in subdirectories or nonstandard places by specifying the -I flag (for include ) to the C compiler. For example,

```
$ gcc -I/usr/openwin/include fred.c
```

It’s often convenient to use the grep command to search header files for particular definitions and function prototypes. Suppose you need to know the name of the #define s used for returning the exit status from a program. Simply change to the /usr/include directory and grep for a probable part of the name like this:

```
$ grep EXIT_ *.h
```

Here grep searches all the files in the directory with a name ending in .h for the string EXIT_ . In this example, it has found (among others) the definition you need in the file stdlib.h .

## Library Files 

Libraries are collections of precompiled functions that have been written to be reusable. Typically, they consist of sets of related functions to perform a common task. Examples include libraries of screen-handling functions (the curses and ncurses libraries) and database access routines (the dbm library).

```
$ gcc -o fred fred.c /usr/lib/libm.a
$ gcc -o fred fred.c -lm
```

-tells the compiler to compile file fred.c , call the resulting program file fred , and search the mathematical library in addition to the standard C library to resolve references to functions.

Although libraries are usually found in standard places in the same way as header files, you can add to the search directories by using the -L (uppercase letter) flag to the compiler. For example,

```
$ gcc -o x11fred -L/usr/openwin/lib x11fred.c -lX11
```

will compile and link a program called x11fred using the version of the library libX11 found in the /usr/openwin/lib directory.

## Static Libraries
```
$ gcc -c bill.c fred.c
$ ls *.o

// we create lib.h that declares the functions bill(char *) and void fred(int) for users

$ gcc -c program.c // that cals bill.c
$ gcc -o program program.o bill.o
$ ./program
```

Now you’ll create and use a library. Use the ar program to create the archive and add your object files to it. The program is called ar because it creates archives, or collections, of individual files placed together in one large file. Note that you can also use ar to create archives of files of any type. (Like many UNIX utilities, ar is a generic tool.)

```
$ ar crv libfoo.a bill.o fred.o
$ ranlib libfoo.a
$ gcc -o program program.o libfoo.a
$ ./program
$ gcc –o program program.o –L. –lfoo
```

–L. option tells the compiler to look in the current directory ( . ) for libraries. The –lfoo option tells the compiler to use a library called libfoo.a (or a shared library, libfoo.so , if one is present). To see which functions are included in an object file, library, or executable program, you can use the nm com-
mand.

## Shared Libraries

Shared libraries are stored in the same places as static libraries, but shared libraries have a different filename suffix. On a typical Linux system, the shared version of the standard math library is /lib/libm.so .
When a program uses a shared library, it is linked in such a way that it doesn’t contain function code itself, but references to shared code that will be made available at run time. When the resulting program
is loaded into memory to be executed, the function references are resolved and calls are made to the shared library, which will be loaded into memory if needed.
```
$ ldd program
$ man gcc
$ info gcc
```