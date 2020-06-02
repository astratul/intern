# Summary - Development Tools

## Problems of Multiple Source Files

#### The make command is not used only to compile programs. It can be used whenever you produce output files from several input files. Other uses include document processing (such as with troff or TeX).

## The make Command and Makefiles

The combination of the make command and a makefile provides a very powerful tool for managing projects. It’s often used not only to control the compilation of source code, but also to prepare manual pages and to install the application into a target directory.

### The Syntax of Makefiles

A makefile consists of a set of dependencies and rules. A dependency has a target (a file to be created) and a set of source files upon which it is dependent. The rules describe how to create the target from the dependent files. Typically, the target is a single executable file.
The makefile is read by the make command, which determines the target file or files that are to be made and then compares the dates and times of the source files to decide which rules need to be invoked to construct the target. Often, other intermediate targets have to be created before the final target can be made. The make command uses the makefile to determine the order in which the targets have to be made and the correct sequence of rules to invoke.

#### Options and Parameters to make

The make program itself has several options. The three most commonly used are
* -k , which tells make to keep going when an error is found, rather than stopping as soon as the first problem is detected. You can use this, for example, to find out in one go which source files fail to compile.
* -n , which tells make to print out what it would have done without actually doing it.
* -f <filename> , which allows you to tell make which file to use as its makefile. If you don’t use this option, the standard version of make looks first for a file called makefile in the current directory. If that doesn’t exist, it looks for a file called Makefile . However if you are using GNU Make, which you probably are on Linux, that version of make looks for GNUmakefile first, before searching for makefile and then Makefile . By convention, many Linux programmers use Makefile ; this allows the makefile to appear first in a directory listing of a directory filled with lowercase-named files. We suggest you don’t use the name GNUmakefile , because it is specific to the GNU implementation of make .
 
To tell make to build a particular target, which is usually an executable file, you can pass the target name to make as a parameter. If you don’t, make will try to make the first target listed in the makefile. Many programmers specify all as the first target in their makefile and then list the other targets as being dependencies for all . This convention makes it clear which target the makefile should attempt to build by default when no target is specified. We suggest you stick to this convention.

#### Dependencies

The dependencies specify how each file in the final application relates to the source files.

#### Rules

### Try It Out
``` 
make -f makefile1 
```

#### Comments in a Makefile
A comment in a makefile starts with # and continues to the end of the line.

#### Macros in a Makefile
You define a macro in a makefile by writing MACRONAME=value , then accessing the value of MACRONAME by writing either $(MACRONAME) or ${MACRONAME} . Some versions of make may also accept $MACRONAME . You can set the value of a macro to blank (which expands to nothing) by leaving the rest of the line after the = blank.

### Try It Out
``` 
rm *.o myapp
make -f makefile2 
```
The make program replaces macro references $(CC) , $(CFLAGS) , and $(INCLUDE) with the appropriate definitions, rather like the C compiler does with #define . Now if you want to change the compile command, you need to change only a single line of the makefile.
In fact, make has several special internal macros that you can use to make makefiles even more succinct. We list the more common ones in the following table; you will see them in use in later examples. Each of these macros is only expanded just before it’s used, so the meaning of the macro may vary as the makefile progresses. In fact, these macros would be of very little use if they didn’t work this way.

* $? - List of prerequisites (files the target depends on) changed more recently than the current target
* $@ - Name of the current target
* $< - Name of the current prerequisite
* $* - Name of the current prerequisite, without any suffix
* '-'  - tells make to ignore any errors.
* @ - tells make not to print the command to standard output before executing it. This character is handy if you want to use echo to display some instructions.

#### Multiple Targets

### Try It Out
``` 
rm *.o myapp
make -f makefile3
make -f makefile3
rm myapp
make -f makefile3 install
make -f makefile3 clean
```
#### Built-in Rules

### Try It Out
* foo.c
```
make foo
rm foo
make CC=gcc CFLAGS=”-Wall -g” foo
```
### Suffix and Pattern Rules

###  Managing Libraries with make

### Try It Out
```
rm -f myapp *.o mylib.a
make -f Makefile5
touch c.h
make -f Makefile5
```
### Advanced Topic: Makefiles and Subdirectories

### GNU make and gcc
#### Try It Out
```
gcc -MM main.c 2.c 3.c
```

## Source Code Control

### RCS
#### The rcs Command
```
rcs -i important.c
ls -l
```
#### The ci Command
```
ci important.c
ls -l
```

#### The co Command

```
co -l important.c
ls -l
ci important.c
ls -l
```
#### The rlog Command
```
rlog important.c
co -r1.1 important.c
```
#### The rcsdiff Command
```
rcsdiff -r1.1 -r1.2 important.c
```
### Identifying Revisions
```
co -l important.c
# edit the  file important.c
ci important.c
ls -l
rm -f important.c
make important
```
#### The ident Command
##### Try It Out
```
./important
ident important
```
### SCCS

#### Comparing RCS and SCCS

## Writing a Manual Page

## Distributing Software

## RPM Packages

## Other Package Formats

## Development Environments
