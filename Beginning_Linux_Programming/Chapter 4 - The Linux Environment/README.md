# Summary - The Linux Environment

This chapter considers the environment in which programs operate, how they can use that environment to gain information about operating conditions, and how users of the programs can alter their behavior.

## Program Arguments

Whenever the operating system starts a new program, the parameters argc and argv are set up and passed to main . These parameters are usually supplied by another program, very often the shell that has requested that the operating system start the new program. The shell takes the command line that it’s given, breaks it up into individual words, and uses these for the argv array. Remember that a Linux shell normally performs wild card expansion of filename arguments before argc and argv are set, whereas the MS-DOS shell expects programs to accept arguments with wild cards and perform their own wild card expansion.

#### Try It Out
* args.c

### getopt

To help us adhere to these guidelines, Linux provides the getopt facility, which supports the use of options with and without values and is simple to use.

```
#include <unistd.h>
int getopt(int argc, char *const argv[], const char *optstring);
extern char *optarg;
extern int optind, opterr, optopt;
```
The getopt function takes the argc and argv parameters as passed to the program’s main function and an options specifier string that tells getopt what options are defined for the program and whether they have associated values. The optstring is simply a list of characters, each representing a single character option. If a character is followed by a colon, it indicates that the option has an associated value that will be taken as the next argument. The getopts command in bash performs a very similar function.

For example, the following call would be used to handle the preceding example:
```
getopt(argc, argv, “if:lr”);
```
The external variable, optind , is set to the index of the next argument to process. getopt uses it to remember how far it’s got. Programs would rarely need to set this variable. When all the option arguments have been processed, optind indicates where the remaining arguments can be found at the end of the argv array.

#### Try It Out
* argopt.c


### getopt_long

Many Linux applications also accept arguments that are more meaningful than the single character options used in the previous example. The GNU C library contains a version of getopt called getopt_long that accepts so-called long arguments that are introduced with a double dash.

#### Try It Out
* longopt.c

## Environment Variables

```
#include <stdlib.h>
char *getenv(const char *name);
int putenv(const char *string);
```
The environment consists of strings of the form name=value . The getenv function searches the environment for a string with the given name and returns the value associated with that name. It will return null if the requested variable doesn’t exist. If the variable exists but has no value, getenv succeeds and returns an empty string, one in which the first byte is null . The string returned by getenv is held in static storage owned by getenv , so to use it further you must copy it to another string because it will be overwritten by subsequent calls to getenv.
The putenv function takes a string of the form name=value and adds it to the current environment. It will fail and return -1 if it can’t extend the environment due to lack of available memory. When this happens,
the error variable errno will be set to ENOMEM. 
In the following example, you write a program to print out the value of any environment variable you
choose. You also arrange to set the value if you give the program a second argument.

#### Try It Out
* environ.c

#### Use of Environment Variables

Environment variables are a mixed blessing and you should use them with care. They are more “hidden” to the user than command-line options and, as such, this can make debugging harder. In a sense, environment variables are like global variables in that they may alter the behavior of a program, giving unexpected results.

#### The environ Variable

```
#include <stdlib.h>
extern char **environ;
```
##### Try It Out
* showenv.c // This program iterates through the environ variable, a null-terminated array of strings, to print out the whole environment.

##  Time and Date
Times are handled using a defined type, a time_t . This is an integer type intended to be large enough to contain dates and times in seconds. On Linux systems, it’s a long integer and is defined, together with functions for manipulating time values, in the header file time.h .

```
#include <time.h>
time_t time(time_t *tloc);
```
#### Try It Out
* envtime.c

The difftime function calculates the difference between two time values and returns a value equivalent to time1-time2 as a floating-point number. For Linux, the return value from time is a number of seconds and can be manipulated, but for the ultimate in portability you should use difftime .

```
#include <time.h>
double difftime(time_t time1, time_t time2);
```
The function gmtime breaks down a low-level time value into a structure containing more usual fields:

```
#include <time.h>
struct tm *gmtime(const time_t timeval);
```
tm Member Description
* int tm_sec : Seconds, 0-61
* int tm_min : Minutes, 0-59
* int tm_hour : Hours, 0-23
* int tm_mday : Day in the month, 1-31
* int tm_mon : Month in the year, 0-11 (January = 0)
* int tm_year : Years since 1900
* int tm_wday : Day in the week, 0-6 (Sunday = 0)
* int tm_yday : Day in the year, 0-365
* int tm_isdst : Daylight savings in effect

#### Try It Out
* gmtime.c

The localtime function is identical to gmtime , except that it returns a structure containing values adjusted for local time zone and daylight savings. If you try the gmtime program again, but use localtime in place of gmtime , you should see a correct time and date reported.

```
#include <time.h>
struct tm *localtime(const time_t *timeval);
```
To convert a broken-down tm structure into a raw time_t value, you can use the function mktime :

```
#include <time.h>
time_t mktime(struct tm *timeptr);
```
mktime will return -1 if the structure can’t be represented as a time_t value.

For “friendly” (as opposed to machine) time, and date output provided by the date program, you can use the functions asctime and ctime :
```
#include <time.h>
char *asctime(const struct tm *timeptr);
char *ctime(const time_t *timeval);
```
#### Try It Out
* ctime.c

To gain more control of the exact formatting of time and date strings, Linux and modern UNIX-like systems provide the strftime function. This is rather like a sprintf for dates and times and works in a similar way:
```
#include <time.h>
size_t strftime(char *s, size_t maxsize, const char *format, struct tm *timeptr);
```
The strftime function formats the time and date represented by the tm structure pointed to by timeptr and places the result in the string s . This string is specified as (at least) maxsize characters long. The format string is used to control the characters written to the string. Like printf , it contains ordinary characters that will be transferred to the string and conversion specifiers for formatting time and date elements. 

the usual date as given by the date program corresponds to a strftime format string of “%a %b %d %H:%M:%S %Y”
```
#include <time.h>
char *strptime(const char *buf, const char *format, struct tm *timeptr);
```
The format string is constructed in exactly the same way as the format string for strftime . strptime acts in a similar way to sscanf in that it scans a string, looking for identifiable fields, and writes them into variables. Here it’s the members of a tm structure that are filled in according to the format string.
However, the conversion specifiers for strptime are a little more relaxed than those for strftime because strptime will allow both abbreviated and full names for days and months. Either representation will match a %a specifier in strptime . Also, where strftime always uses leading zeros on numbers less than 10, strptime regards them as optional.
strptime returns a pointer to the character following the last one consumed in the conversion process. If it encounters characters that can’t be converted, the conversion simply stops at that point. The calling program needs to check that enough of the passed string has been consumed to ensure that meaningful values have been written to the tm structure.

#### Try It Out
* strftime.c

## Temporary Files

This popular use of temporary files has a hidden disadvantage. You must take care to ensure that the applications choose a unique filename to use for the temporary file. If they don’t there may be a problem. Because Linux is a multitasking system, another program could choose the same name and the two will interfere with each other.
A unique filename can be generated by the tmpnam function:

```
#include <stdio.h>
char *tmpnam(char *s);
FILE *tmpfile(void);
```
The tmpfile function returns a stream pointer that refers to a unique temporary file. The file is opened for reading and writing (via fopen with w+ ), and it will be automatically deleted when all references to the file are closed.
tmpfile returns a null pointer and sets errno on error.

#### Try It Out
* tmpnam.c

Versions of UNIX introduced another way to generate temporary filenames using the functions mktemp and mkstemp . These are supported by Linux and are similar to tmpnam , except that you can specify a template for the temporary filename, which gives you a little more control over their location and name:
```
#include <stdlib.h>
char *mktemp(char *template);
int mkstemp(char *template);
```
The mktemp function creates a unique filename from the given template . The template argument must be a string with six trailing X characters. The mktemp function replaces these X characters with a unique combination of valid filename characters. It returns a pointer to the generated string or a null pointer if it couldn’t generate a unique name.
The mkstemp function is similar to tmpfile in that it creates and opens a temporary file. The filename is generated in the same way as mktemp , but the returned result is an open, low-level file descriptor. 

#### You should always use the “create and open” functions tmpfile and mkstemp in your own programs rather than tmpnam and mktemp .

## User Information

You can set up programs to run as if a different user had started them. When a program has its UID permission set, it will run as if started by the owner of the executable file. When the su command is executed, the program runs as if it had been started by the superuser. It then validates the user’s access, changes the UID to that of the target account, and executes that account’s login shell. This also allows a program to be run as if a different user had started it and is often used by system administrators to perform maintenance tasks.
Because the UID is key to the user’s identity, let’s start with that.
The UID has its own type — uid_t — defined in sys/types.h . It’s normally a small integer. Some are predefined by the system; others are created by the system administrator when new users are made known to the system. Normally, users usually have UID values larger than 100.

```
#include <sys/types.h>
#include <unistd.h>
uid_t getuid(void);
char *getlogin(void);
```
The getuid function returns the UID with which the program is associated. This is usually the UID of the user who started the program.
The getlogin function returns the login name associated with the current user.
The system file /etc/passwd contains a database dealing with user accounts. It consists of lines, one per user, that contain the username, encrypted password, user identifier (UID), group identifier (GID), full name, home directory, and default shell. Here’s an example line:
neil:zBqxfqedfpk:500:100:Neil Matthew:/home/neil:/bin/bash

```
#include <sys/types.h>
#include <pwd.h>
struct passwd *getpwuid(uid_t uid);
struct passwd *getpwnam(const char *name);
```

#### Try It Out
* user.c


## Host Information

