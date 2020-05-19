# Summary - Terminals

Specifically, in this chapter, you learn about
* Reading and writing to the terminal
* Terminal drivers and the General Terminal Interface
* termios
* Terminal output and terminfo
* Detecting keystrokes

## Reading from and Writing to the Terminal

#### Try It Out
* menu1.c

getchoice prints the program introduction greet and the sample menu choices and asks the user to choose the initial character. The program then loops until getchar returns a character that matches the first letter of one of the option array’s entries.

### Canonical versus Non-Canonical Modes
The two problems are closely related. By default, terminal input is not made available to a program until the user presses Enter or Return. In most cases, this is a benefit because it allows the user to correct typing mistakes using Backspace or Delete. Only when they’re happy with what they see on the screen do they press Enter to make the input available to the program.

This behavior is called canonical, or standard, mode. All the input is processed in terms of lines. Until a line of input is complete (usually when the user presses Enter), the terminal interface manages all the key presses, including Backspace, and no characters may be read by the application.

The opposite of this is non-canonical mode, where the application has much greater control over the processing of input characters. We’ll come back to these two modes again a little later.

Among other things, the Linux terminal handler helps by translating interrupt characters to signals (for example, stopping your program when you press Ctrl+C) and it can automatically perform Backspace and Delete processing for you, so you don’t have to reimplement it in each program you write.

The character the program actually sees isn’t an ASCII carriage return, CR (decimal 13, hex 0D ), but a line feed, LF (decimal 10, hex 0A ). This is because, internally, Linux (like UNIX) always uses a line feed to end lines of text; that is, UNIX uses a line feed alone to mean a newline, where other systems, such as MS-DOS, use a carriage return and a line feed together as a pair. If the input or output device also sends or requires a carriage return, the Linux terminal processing takes care of it. This might seem a little strange if you’re used to MS-DOS or other environments, but one of the very considerable benefits is that there is no real difference between text and binary files on Linux. Only when you input or output to a terminal or some printers and plotters are carriage returns processed.

### Handling Redirected Output

#### Try It Out
* menu2.c

#### How It Works
The new section of code uses the isatty function to test whether the standard output is connected to
a terminal and halts execution if it isn’t. This is the same test the shell uses to decide whether to offer prompts. It’s possible, and quite common, to redirect both stdout and stderr away from the terminal. You can direct the error stream to a different file like this:
```
$ ./menu2 >file 2>file.error
```
Or combine the two output streams into a single file like this:
```
$ ./menu2 >file 2>&1
```

## Talking to the Terminal

Fortunately, Linux and UNIX make things easy by providing a special device, /dev/tty , which is always the current terminal, or login session. Because Linux treats everything as a file, you can use normal file operations to read and write to /dev/tty .

#### Try It Out
* menu3.c

## The Terminal Driver and the General Terminal Interface

Sometimes a program needs much finer control over the terminal than can be achieved using simple file operations. Linux provides a set of interfaces that allow you to control the behavior of the terminal driver, giving you much greater control of the processing of terminal input and output.

The main features that you can control are
* Line editing: Choose whether to allow Backspace for editing.
* Buffering: Choose whether to read characters immediately, or read them after a configurable delay.
* Echo: Allows you to control echoing, such as when reading passwords.
* CR/LF: Determine mapping for input and output: what happens when you print a line feed character ( \n ).
* Line speeds: Rarely used on a PC console, these speeds are very important for modems and terminals on serial lines.

### Hardware Model 


## The termios Structure

termios is the standard interface specified by POSIX and is similar to the System V interface termio . The terminal interface is controlled by setting values in a structure of type termios and using a small set of function calls. Both are defined in the header file termios.h .

### Input Modes

### Output Modes

### Control Modes

### Local Modes

### Special Control Characters

#### Characters

#### The TIME and MIN Values

#### Accessing Terminal Modes from the Shell

#### Setting Terminal Modes from the Command Prompt

### Terminal Speed

### Additional Functions

##### Try It Out
* 

## Terminal Output

## Detecting Keystrokes
