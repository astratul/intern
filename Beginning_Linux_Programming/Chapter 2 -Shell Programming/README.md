# Summary - Shell Programming

## A Bit of Philosophy

This command uses the ls and more utilities and pipes the output of the file listing to a screen-at-a-time
display. Each utility is one more building block. You can often use many small scripts together to create
large and complex suites of programs.
```
$ ls -al | more
```

## What is a Shell?

A shell is a program that acts as the interface between you and the Linux system, enabling you to enter commands for the operating system to execute. In that respect, it resembles the Windows command prompt, but as mentioned earlier, Linux shells are much more powerful. For example, input and output can be redirected using < and > , data piped between simultaneously executing programs using | , and output from a subprocess grabbed by using $(...) . On Linux it’s quite feasible to have multiple shells installed, with different users able to pick the one they prefer.

You can check the version of bash you have with the following command:
```
$ /bin/bash --version
```

More common shells available:

1. sh (Bourne) - The original shell from early versions of UNIX
2. csh , tcsh, zsh - The C shell, and its derivatives, originally created by Bill Joy of Berkeley UNIX fame. The C shell is probably the third most popular
type of shell after bash and the Korn shell.
3. ksh , pdksh - The Korn shell and its public domain cousin. Written by David Korn, this is the default shell on many commercial UNIX versions.
4. bash - The Linux staple shell from the GNU project. bash, or Bourne Again SHell, has the advantage that the source code is freely available, and even if it’s not currently running on your UNIX system, it has probably been ported to it. bash has many similarities to the Korn shell.

## Pipes and Redirection
### Redirecting Output
```
$ ls -l > lsoutput.txt
$ set -o noclobber 
//(or set -C ), which sets the  noclobber option to prevent a file from being overwritten using redirection
$ ps >> lsoutput.txt //append the output of the ps command to the end of the file
```
 ps - report a snapshot of the current processes.

To redirect the standard error output, preface the > operator with the number of the file descriptor you wish to redirect. Because the standard error is on file descriptor 2, use the 2> operator. This is often useful to discard error information and prevent it from appearing on the screen.

```
$ kill -HUP 1234 >killout.txt 2>killerr.txt  //By redirecting both the standard output and the error, you can prevent the kill command from writing any text to the screen.
$ kill -1 1234 >killouterr.txt 2>&1 //capture both sets of output into a single file, you can use the >& operator to combine the two outputs.
$ kill -1 1234 >/dev/null 2>&1 // discard the entire output
```
### Redirecting Input

```
$ more < killout.txt
```

### Pipes

You can connect processes using the pipe operator ( | )- by pipes can run simultaneously and are automatically rescheduled as data flows between them
```
//without pipes
$ ps > psout.txt
$ sort psout.txt > pssort.out
//with pipes
$ ps | sort > pssort.out
$ ps | sort | more
$ ps –xo comm | sort | uniq | grep -v sh | more
// This takes the output of ps , sorts it into alphabetical order, extracts processes using uniq , uses grep -v sh to remove the process named sh , and finally displays it paginated on the screen.
```
## The Shell as a Programming Language

### Interactive Programs
```
$ for file in *
> do
> if grep -l POSIX $file
> then
> more $file
> fi
> done
```
The shell also performs wildcard expansion (often referred to as globbing).
```
ls my{file,data}*
```
Experienced Linux users would probably perform this simple operation in a much more efficient way, perhaps with a command such as
```
$ more `grep -l POSIX *`
// or the synonymous  construction
$ more $(grep -l POSIX *)
$ grep -l POSIX * | more
// will output the name of the file whose contents contained the string POSIX .
```
## Creating a Script
```
#!/bin/sh
#first
#This file looks through all the files in the current
#directory for the string POSIX, and then prints the names of
#those files to the standard output.
for file in *
do
if grep -q POSIX $file
then
echo $file
fi
done
exit 0
```
#!/bin/sh , is a special form of comment; the #! characters tell the system that the argument that follows on the line is the program to be used to execute this file. In this case, /bin/sh is the default shell program.

```
/bin/sh first.sh
chmod +x first
first
```
## Shell Syntax
### Variables
```
$ salutation=Hello
$ echo $salutation
Hello
$ salutation="Yes Dear"
$ echo $salutation
Yes Dear
$ salutation=7+5
$ echo $salutation
7+5
$ read salutation
Wie geht’s?
$ echo $salutation
Wie geht’s?
```

```
#!/bin/sh
myvar=”Hi there”
echo
echo
echo
echo
$myvar
“$myvar”
‘$myvar’
\$myvar
echo Enter some text
read myvar
echo ‘$myvar’ now equals $myvar
exit 0
```
### Environment Variables

1. $HOME - The home directory of the current user
2. $PATH - A colon-separated list of directories to search for commands
3. $PS1 - A command prompt, frequently $ , but in bash you can use some more complex values; for example, the string [\u@\h \W]$ is a popular default that tells you the user, machine name, and current directory, as well as providing a $ prompt.
4. $PS2 - A secondary prompt, used when prompting for additional input; usually > .
5. $IFS - An input field separator. This is a list of characters that are used to separate words when the shell is reading input, usually space, tab, and newline characters.
6. $0 - The name of the shell script
7. $# - The number of parameters passed
8. $$ - The process ID of the shell script, often used inside a script for generating unique temporary filenames; for example /tmp/tmpfile_$$

### Parameter Variables

1. $1, $2, ... -  The parameters given to the script 
2. $* - A list of all the parameters, in a single variable, separated by the first character in the environment variable IFS . If IFS is modified, then the way $* separates the command line into parameters will change.
3. $@ - A subtle variation on $* ; it doesn’t use the IFS environment variable, so parameters are not run together even if IFS is empty.

```
#!/bin/sh
salutation="Hello"
echo $salutation
echo "The program $0 is now running"
echo "The second parameter was $2"
echo "The first parameter was $1"
echo "The parameter list was $*"
echo "The user’s home directory is $HOME"
echo "Please enter a new greeting"
read salutation
echo $salutation
echo "The script is now complete"
exit 0
```
### Conditions
```
if test -f fred.c # or if [ -f fred.c ]
then
...
fi
```
#### String Comparison
1. string1 = string2 - True if the strings are equal
2. string1 != string2 - True if the strings are not equal
3. -n string - True if the string is not null
4. -z string - True if the string is null (an empty string)
#### Arithmetic Comparison
1. expression1 -eq expression2 - True if the expressions are equal
2. expression1 -ne expression2 - True if the expressions are not equal
3. expression1 -gt expression2 - True if expression1 is greater than expression2
4. expression1 -ge expression2 - True if expression1 is greater than or equal to expression2
5. expression1 -lt expression2 - True if expression1 is less than expression2
6. expression1 -le expression2 - True if expression1 is less than or equal to expression2
7. ! expression - True if the expression is false, and vice versa
#### File Conditional
1. -d file - True if the file is a directory
2. -e file - True if the file exists. Note that historically the - e option has not been portable, so -f is usually used.
3. -f file - True if the file is a regular file
4. -g file - True if set-group-id is set on file
5. -r file - True if the file is readable
6. -s file - True if the file has nonzero size
7. -u file - True if set-user-id is set on file
8. -w file - True if the file is writable
9. -x file - True if the file is executable

### Control Structures
- testif.sh
- testelif.sh
- controlStructures.sh
- testfor.sh
- testforWildcard.sh 
- testUntil.sh 
- testCase.sh
- patterns.sh
### Lists
- andlists.sh
- orlists.sh

### Functions
- func1.sh
- func2.sh
- func3.sh 

## Commands
- break: testbreak.sh
- ":" alias for true: testcom.sh 
- continue: testContinue.sh
- "." executes the command in the current shell:  classic_set and latest_set
- echo = printf;
- eval - enables you to evaluate arguments;
- exec - to replace the current shell with a different program;
- exit n - causes the script to exit with exit code n: testExit.sh;