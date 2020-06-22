# Summary - Standards for Linux

Linux started as just a kernel. Unfortunately, a kernel on its own is not very useful; programs are needed for logging in, managing files, compiling new programs, and so forth. To make a useful system, tools were added from the GNU project. These were clones of familiar programs available on the UNIX and UNIX-like systems around at the time. Making Linux look and feel like UNIX set the first standards for Linux, providing a familiar environment for C programmers.

In particular, we look at
* The C programming language standard
* The UNIX standards, particularly the POSIX standard developed by the IEEE and The Single UNIX Specification developed by the Open Group
* Work by the Free Standards Group, particularly the Linux Standard Base, which defines a standard Linux file system layout

## The C Programming Language

The programming language C is the de facto language for programming Linux, so in order to write portable C programs for Linux, it’s worth understanding a little of its origins, how it has changed, and, most important, how to check that your program is conforming correctly to standards.

### A Brief History Lesson

### The GNU Compiler Collection

### gcc Options

### Compiler Options for Standards Tracking

These options are passed to gcc on the command line; we show only the most important options here:
* -ansi:  and tells the compiler to work to the ISO C90 standard of the language. It turns off certain gcc extensions that are incompatible with the standard, disables C++ (//) style comments in C programs, and enables the ANSI trigraph features. It also defines the macro __STRICT_ANSI__ , which turns off some gcc extensions in header files that are incompatible with the standard. Later versions of the compiler may change the language standard targeted.
* -std=: This option allows more fine-grained control of the standard in use by supplying a parameter that sets the exact standard required.

### Define Options for Standard Tracking
These are constants ( #defines ) that can either be set by options on the compiler command line or, alternatively, as definitions in the source code. We generally suggest using the compiler command line for these options.
* __STRICT_ANSI__: Force the use of ISO standard C. Defined when -ansi is given as a compile line option.
* _POSIX_C_SOURCE=2: Turn on features defined by the IEEE Std 1003.1 and 1003.2 standards.
* _BSD_SOURCE: This enables BSD-type features. If those features conflict with POSIX definitions, the BSD definitions take precedence.
* _GNU_SOURCE: Allows a wide range of features, including GNU extensions. If those features conflict with POSIX definitions, the POSIX definitions take precedence.

### Compiler Options for Warnings
These options are passed to the compiler on the command line. Again we just list the main options; you can find a full list in the gcc manual pages.
* -pedantic: This is the most powerful compiler option for checking clean C code. Apart from turning on the option to check for standard conformant C, it also turns off some traditional C constructs that are not permitted by the standard, and disables all the GNU extensions to the standard. This is the option to use if you want your C code to be as portable as possible. The downside is that the compiler is very fussy indeed about your code being clean, and sometimes it can require you to think very carefully in order to get rid of the last few warnings.
* -Wformat: Checks that the argument types to the printf family of functions are correct.
* -Wparentheses: Checks that parentheses are always provided, even in some circumstances where they are not needed. This is quite a useful option for checking that initialization of complex structures is as intended.
* -Wswitch-default: Checks that all switch statements have a default case, which is generally good coding practice.
* -Wunused: Checks a variety of cases such as static functions declared but never defined, unused parameters, and discarded results.
* -Wall: Turns on most of gcc’s warnings, including all of the preceding -W options (it does not select -pedantic ) and is a good way to keep your code clean.

## Interfaces and the Linux Standards Base
The Linux Standards Base defines (as of version 3.1) three areas for compliance:
* Core: The main libraries, utilities and some key file system locations.
* C++: The C++ libraries.
* Desktop: Additional files for desktop installs, principally various graphic libraries.
The main area we are interested in is the Core part of the specification. The LSB specification covers a number of areas as part of its own documentation, but also refers to some external standards for particular interface definitions. The areas covered are
* Object formats for binary compatibility
* Dynamic linking standards
* Standard libraries, both base libraries and the X Window System libraries
* The shell and other command-line programs
* The execution environment, including users and groups
* System initialization and run levels
In this chapter, we are really only interested in the standard libraries, users, and system initialization, so those are the areas we look at here.
### LSB Standard Libraries
The Linux Standard Base defines the interfaces that must be present in two ways. For some functions, primarily those that are implemented by the GNU C library or tend to be Linux-only standards, it defines the interface and its behavior. For other interfaces, mostly those that come from Linux being UNIX-like, the specification simply states that a particular interface must be present and must behave as defined by another standard, usually that of the Common Application Environment (CAE) or more commonly The Single UNIX Specification, which is available from The Open Group, http://www.opengroup.org . Some parts are available (currently registration is required) on the web at http://www.unix.org/online.html .
Unfortunately, the underlying standards for Linux, the UNIX standards, have a rather complex past, and rather too many standards to choose from, although mostly the various versions of the standards are closely compatible.

### A Brief History Lesson

### Using the LSB Standard for Libraries

### LSB Users and Groups
* The specification tells us never to read files like /etc/passwd directly, but to always use the standard library calls such as getpwent , or standard utilities like passwd for accessing user details.
* It tells us that there must be a user called root in the root group, and that root is an administrative user with full privileges. We also discover that there are a number of optional user and group names that should never be used by standard applications; they are intended for use by distributions.
* It also tells us that user IDs below 100 are system accounts, 100–499 are allocated by system administrators and post-install scripts, and 500 and higher are for normal user accounts.

### LSB System Initialization
The area of system initialization has always, at least to us, been an annoyance because of subtle differences between distributions.
Linux has inherited from UNIX-like operating systems the idea of run levels that define the services that are running at any time. For Linux, the usual definitions are given in the following table.
Run Level - Used for
* 0 Halt. Used as a logical state to change to when the system is shut down.
* 1 Single user mode, directories other than / may not be mounted, and networking will not be enabled. It is normally used for system maintenance.
* 2 Multiuser mode; however, networking is not enabled.
* 3 Normal multiuser mode with networking, using a text mode login screen.
* 4 Reserved.
* 5 Normal multiuser mode with networking and a graphical login screen.
* 6 A pseudo-level used for rebooting.
The LSB lists these levels but doesn’t require them to be used, although they are very common.

## The Filesystem Hierarchy Standard
The purpose of this standard is to define standard places in the Linux file system, so that developers and users alike can have reasonable expectations of where to find things. Long-time users of the various UNIX-like operating systems have long bemoaned the subtle differences between the way file systems are laid out, and the FHS provides a way for Linux distributions to avoid going down the same fragmented path.

The arrangement of files in a Linux system may seem at first to be a semi-arbitrary arrangement of files and directories, based on historic practice. To an extent that’s true, but over the years the layout has evolved for good reasons into the hierarchy we see today. The general idea is to separate files and directories into three groups:
* Files and directories that are unique to a particular system running Linux, such as start-up scripts and configuration files
* Files and directories that are read-only and may be shared between systems running Linux, such as application executables
* Directories that are read/write, but may be shared between systems running Linux or other operating systems, such as user home directories

## Further Reading about Standards
