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

Programs that use the function calls defined in termios.h will need to be linked with an appropriate function library. This may be simply the standard C library or it may be the curses library, depending on your installation. If necessary, when compiling the examples in this chapter, add -lcurses to thebend of the compiler command line. On some older Linux systems, the curses library is provided by a version known as “new curses.” I

The values that can be manipulated to affect the terminal are grouped into various modes:
* Input
* Output
* Control
* Local
* Special control characters

```
#include <termios.h>
struct termios {
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_cflag;
    tcflag_t c_lflag;
    cc_t
    c_cc[NCCS];
};
```
You can initialize a termios structure for the terminal by calling the function tcgetattr , which has the following prototype:
```
#include <termios.h>
int tcgetattr(int fd, struct termios *termios_p);
```
This call writes the current values of the terminal interface variables into the structure pointed to by termios_p . If these values are then altered, you can reconfigure the terminal interface with the tcsetattr function as follows:
```
#include <termios.h>
int tcsetattr(int fd, int actions, const struct termios *termios_p);
```
The actions field for tcsetattr controls how any changes are applied. The three possibilities are
* TCSANOW : Changes values immediately.
* TCSADRAIN : Changes values when current output is complete.
* TCSAFLUSH : Changes values when current output is complete, but discards any input currently available and not yet returned in a read call.

### Input Modes 

The input modes control how input (characters received by the terminal driver at a serial port or keyboard) is processed before being passed on to the program. You control them by setting flags in the c_iflag member of the termios structure. All the flags are defined as macros and can be combined with a bitwise OR. This is the case for all the terminal modes.

The macros that can be used for c_iflag are
* BRKINT : Generate an interrupt when a break condition (loss of connection) is detected on the line
* IGNBRK : Ignore break conditions on the line
* ICRNL : Convert a received carriage return to a newline
* IGNCR : Ignore received carriage returns
* INLCR : Convert received newlines to carriage returns
* IGNPAR : Ignore characters with parity errors
* INPCK : Perform parity checking on received characters
* PARMRK : Mark parity errors
* ISTRIP : Strip (set to seven bits) all incoming characters
* IXOFF : Enable software flow control on input
* IXON : Enable software flow control on output
If neither BRKINT nor IGNBRK is set, a break condition on the line is read as a NULL (0x00) character.

### Output Modes

These modes control how output characters are processed; that is, how characters sent from a program are processed before being transmitted to the serial port or screen. As you might expect, many of these are counterparts of the input modes. Several additional flags exist, which are mainly concerned with allowing for slow terminals that require time to process characters such as carriage returns. Almost all of these are either redundant (as terminals get faster) or better handled using the terminfo database of terminal capabilities, which you use later in this chapter.

You control output modes by setting flags in the c_oflag member of the termios structure. The macros 
that you can use in c_oflag are
* OPOST : Turn on output processing
* ONLCR : Convert any output newline to a carriage return/line feed pair
* OCRNL : Convert any output carriage return to a newline
* ONOCR : No carriage return output in column 0
* ONLRET : A newline also does a carriage return
* OFILL : Send fill characters to provide delays
* OFDEL : Use DEL as a fill character, rather than NULL
* NLDLY : Newline delay selection
* CRDLY : Carriage return delay selection
* TABDLY : Tab delay selection
* BSDLY : Backspace delay selection
* VTDLY : Vertical tab delay selection
* FFDLY : Form feed delay selection
If OPOST is not set, all the other flags are ignored.

### Control Modes
These modes control the hardware characteristics of the terminal. You specify control modes by setting flags in the c_cflag member of the termios structure, which has the following macros:
* CLOCAL : Ignore any modem status lines.
* CREAD : Enable the receipt of characters.
* CS5 : Use five bits in sent or received characters.
* CS6 : Use six bits in sent or received characters.
* CS7 : Use seven bits in sent or received characters.
* CS8 : Use eight bits in sent or received characters.
* CSTOPB : Use two stop bits per character, rather than one.
* HUPCL : Hang up modem on close.
* PARENB : Enable parity generation and detection.
* PARODD : Use odd parity rather than even parity.
If HUPCL is set, when the terminal driver detects that the last file descriptor referring to the terminal has been closed, it will set the modem control lines to “hang-up” the line.

### Local Modes
These modes control various characteristics of the terminal. You specify local modes by setting flags in the c_lflag member of the termios structure, with the following macros:
* ECHO : Enable local echoing of input characters
* ECHOE : Perform a Backspace, Space, Backspace combination on receiving ERASE
* ECHOK : Perform erase line on the KILL character
* ECHONL : Echo newline characters
* ICANON : Enable canonical input processing (see the text following this list)
* IEXTEN : Enable implementation-specific functions
* ISIG : Enable signals
* NOFLSH : Disable flush on queue
* TOSTOP : Send background processes a signal on write attempts
The two most important flags here are ECHO , which allows you to suppress the echoing of typed characters, and ICANON , which switches the terminal between two very distinct modes of processing received characters. If the ICANON flag is set, the line is said to be in canonical mode; if not, the line is in non-canonical mode.

### Special Control Characters 

Special control characters are a collection of characters, like Ctrl+C, acted upon in particular ways when the user types them. The c_cc array member of the termios structure contains the characters mapped to each of the supported functions. The position of each character (its index into the array) is defined by a macro, but there’s no limitation that they must be control characters.
The c_cc array is used in two very different ways, depending on whether the terminal is set to canonical mode (that is, the setting of the ICANON flag in the c_lflag member of termios ).
It’s important to realize that there is some overlap in the way the array index values are used for the two different modes. Because of this, you should never mix values from these two modes. For canonical mode, the array indices are 
* VEOL : EOL character
* VERASE : ERASE character
* VINTR : INTR character
* VKILL : KILL character
* VQUIT : QUIT character
* VSUSP : SUSP character
* VSTART : START character
* VSTOP : STOP character

For non-canonical mode, the array indices are
* VINTR : INTR character
* VMIN : MIN value
* VQUIT : QUIT character
* VSUSP : SUSP character
* VTIME : TIME value
* VSTART : START character
* VSTOP : STOP character

#### Characters
Character Description
* INTR - Causes the terminal driver to send a SIGINT signal to processes connected to the terminal.
* QUIT - Causes the terminal driver to send a SIGQUIT signal to processes connected to the terminal.
* ERASE - Causes the terminal driver to delete the last character on the line.
* KILL - Causes the terminal driver to delete the entire line.
* EOF - Causes the terminal driver to pass all characters on the line to the application reading input. If the line is empty, a read call will return zero characters as though a read had been attempted at the end of a file.
* EOL - Acts as a line terminator in addition to the more usual newline character. SUSP Causes the terminal driver to send a SIGSUSP signal to processes connected to the terminal. If your UNIX supports job control, the current application will be suspended.
* STOP - Acts to “flow off”; that is, prevent further output to the terminal. It’s used to support XON/XOFF flow control and is usually set to the ASCII XOFF character, Ctrl+S.
* START - Restarts output after a STOP character, often the ASCII XON character.

#### The TIME and MIN Values

The values of TIME and MIN are used only in non-canonical mode and act together to control the reading of input. Together, they control what happens when a program attempts to read a file descriptor associated with a terminal.
There are four cases:
* MIN = 0 and TIME = 0 : In this case, a read will always return immediately. If some characters are available, they will be returned; if none are available, read will return zero and no characters will have been read.
* MIN = 0 and TIME > 0 : In this case, the read will return when any character is available to be read or when TIME tenths of a second have elapsed. If no character was read because the timer expired, read will return zero. Otherwise, it will return the number of characters read.
* MIN > 0 and TIME = 0 : In this case, the read will wait until MIN characters can be read and then return that number of characters. Zero is returned on end of file.
* MIN > 0 and TIME > 0 : This is the most complex case. When read is called, it waits for a character to be received. When the first character is received, and every subsequent time a character is received, an inter-character timer is started (or restarted if it was already running). The read will return when either MIN characters can be read or the inter-character time of TIME tenths of a second expires. This can be useful for telling the difference between a single press of the Escape key and the start of a function key escape sequence. Be aware, though, that network communications or high processor loads neatly erase such fine timing information.

By setting non-canonical mode and using the MIN and TIME values, programs can perform character-by-character processing of input.

#### Accessing Terminal Modes from the Shell

If you want to see the termios settings that are being used while you’re using the shell, you can get a list using the command:
```
$ stty -a
# if your version of stty supports it, is to use the command:
$ stty sane
# stty -g command to write the current stty setting in a form ready to reread. 
$ stty -g > save_stty
..
<experiment with settings>
..
$ stty $(cat save_stty)
```
You still may need to use Ctrl+J rather than Enter for the final stty command. You can use the same technique in a shell script:
```
save_stty=”$(stty -g)“
<alter stty settings>
stty $save_stty
```
#### Setting Terminal Modes from the Command Prompt

You can also use the stty command to set the terminal modes directly from the command prompt.
To set a mode in which your shell script could perform single character reads, you need to turn off canonical mode and set it to 1 and to 0. The command is as follows:
```
$ stty -icanon min 1 time 0
$ stty -echo
```
##### Remember to use stty echo to turn echoing back on after you try this!
### Terminal Speed

The final function served by the termios structure is manipulating the line speed. No members are defined for terminal speed; instead, it’s set by function calls. Input and output speeds are handled separately. The four call prototypes are
```
#include <termios.h>
speed_t cfgetispeed(const struct termios *);
speed_t cfgetospeed(const struct termios *);
int cfsetispeed(struct termios *, speed_t speed);
int cfsetospeed(struct termios *, speed_t speed);
```
Notice that these act on a termios structure, not directly on a port. This means that to set a new speed, you must read the current settings with tcgetattr , set the speed using one of the preceding calls, and then write the termios structure back using tcsetattr . Only after the call to tcsetattr will the line speed be changed.
Various values are allowed for speed in the preceding function calls, but the most important are
* B0 : Hang up the terminal
* B1200 : 1200 baud
* B2400 : 2400 baud
* B9600 : 9600 baud
* B19200 : 19200 baud
* B38400 : 38400 baud
There are no speeds greater than 38400 defined by the standard and no standard method of supporting serial ports at speeds greater than this.

### Additional Functions


There are a small number of additional functions for the control of terminals. These work directly on file descriptors, without needing to get and set termios structures. Their definitions are
```
#include <termios.h>
int tcdrain(int fd);
int tcflow(int fd, int flowtype);
int tcflush(int fd, int in_out_selector);
```
The functions have the following purposes:
* tcdrain causes the calling program to wait until all queued output has been sent.
* tcflow is used to suspend or restart output.
* tcflush can be used to flush input, output, or both.
Now that we’ve covered the rather large subject of the termios structure, let’s look at a few practical examples. Possibly the simplest is the disabling of echo to read a password. Do this by turning off the ECHO flag.

##### Try It Out
* password.c

Be careful to change only the flags you need to change, using the construct X &= ~FLAG (which clears the bit defined by FLAG in the variable X ). If needed, you could use X |= FLAG to set a single bit defined by FLAG , although this wasn’t necessary in the preceding example.

##### Try It Out
* menu4.c

## Terminal Output

Using the termios structure, you have control over keyboard input, but it would be good to have the same level of control over the way a program’s output is presented on the screen. You used printf at the beginning of the chapter to output characters to the screen, but with no way of placing the output at a particular position on the screen.

### Terminal Type

Not surprisingly, there is a solution in a package known as terminfo . Instead of each program having to cater for every sort of terminal, the program looks up a database of terminal types to get the correct information. In most modern UNIX systems, including Linux, this has been integrated with another package called curses , which you learn about in the next chapter.
To use terminfo functions you normally have include the curses header file, curses.h , and terminfo ’s own header file, term.h . On some Linux systems, you may have to use the implementation of curses known as ncurses , and include ncurses.h to provide prototypes for your terminfo functions.

### Identify Your Terminal Type

The Linux environment contains a variable, TERM , which is set to the type of terminal being used. It’s usually set automatically by the system at logon time.

```
$ echo $TERM
```
The terminfo package contains a database of capabilities and escape sequences for a large number of terminals and provides a uniform programming interface for using them. A single program can then be written that will take advantage of future terminals as the database is extended, rather than each application having to provide support for the many different terminals.
```
$ infocmp vt100
# print a readable version of a compiled terminfo entry
```
Each terminfo definition consists of three types of entry. Each entry is called a capname and defines a terminal capability.
Boolean capabilities simply indicate whether a terminal supports a particular feature.
Numeric capabilities define sizes, such as lines , the number of lines on the screen, and cols , the number of columns on the screen. The actual number is separated from the capability name by a # character. To define a terminal as having 80 columns and 24 lines, you would write cols#80, lines#24 .
String capabilities are slightly more complex. They are used for two distinct types of capability: defining output strings needed to access terminal features and defining the input strings that will be received when the user presses certain keys, normally function keys or special keys on the numeric keypad. Some string capabilities are quite simple, such as el , which is “erase to end of line.”

Things get slightly more complicated where the escape sequence needs some parameters. Most terminals can move the cursor to a specified row and column location. It’s clearly impractical to have a different capability for each possible cursor location, so a generic capability string is used, with parameters defining the values to be inserted when the stings are used. For example, a VT100 terminal uses the sequence Esc,[,<row>,;,<col>,H to move the cursor to a specified location. In terminfo source format, this is written with the rather intimidating cup=\E[%i%p1%d;%p2%dH$<5> .
This means
* \E : Send Escape
* [ : Send the [ character
* %i : Increment the arguments
* %p1 : Put the first argument on the stack
* %d : Output the number on the stack as a decimal number
* ; : Send the ; character
* %p2 : Put the second argument on the stack
* %d : Output the number on the stack as a decimal number
* H : Send the H character

### Using terminfo Capabilities

``` 
#include <term.h>
int setupterm(char *term, int fd, int *errret);
// set up the terminal type by calling setupterm - This will initialize a TERMINAL structure for the current terminal type.
```
* badterm.c
```
#include <term.h>
int tigetflag(char *capname);
int tigetnum(char *capname);
char *tigetstr(char *capname);
```
The functions tigetflag , tigetnum , and tigetstr return the value of Boolean, numeric, and string terminfo capabilities, respectively.
* sizeterm.c
If you use tigetstr to retrieve the cursor motion capability ( cup ) of the xterm terminal type, you get a parameterized answer: \E[%p1%d;%p2%dH .

You can substitute the parameters in a capability with actual values using the tparm function. Up to nine parameters can be substituted and a usable escape sequence is returned.
```
#include <term.h>
char *tparm(char *cap, long p1, long p2, ..., long p9);
```

```
#include <term.h>
int putp(char *const str);
int tputs(char *const str, int affcnt, int (*putfunc)(int));
```

##### Try It Out
* menu5.c

People who have programmed MS-DOS often look for the Linux equivalent of the kbhit function, which detects whether a key has been pressed without actually reading it. Unfortunately, they fail to find it, because there’s no direct equivalent. UNIX programmers don’t notice the omission because UNIX is normally programmed in such a way that programs should rarely, if ever, busy-wait on an event. Because this is the normal use for kbhit , it’s rarely missed on UNIX and Linux.
However, when you’re porting programs from MS-DOS, it’s often convenient to emulate kbhit , which you can do using the non-canonical input mode.

## Detecting Keystrokes

However, when you’re porting programs from MS-DOS, it’s often convenient to emulate kbhit , which you can do using the non-canonical input mode.

##### Try It Out
* kbhit.c

### Virtual Consoles

Using who and ps , you can see who is logged in and the shell and programs being executed on this virtual console:
```
$ who
$ ps -e
```
It is possible to run more than one X session on Linux. If you do this, for example, with
```
$ startx -- :1
```
Linux will start the X server on the next free virtual console, in this case, /dev/tty8 , and it is then possible to switch between them with Ctrl+Alt+F8 and Ctrl+Alt+F7.

### Pseudo-Terminals

These are devices that behave much like the terminals we have been using in this chapter, except that they have no associated hardware. They can be used to provide a terminal-like interface to other programs.

For example, using pseudo-terminals, it is possible to make two chess programs play each other, despite the fact that the programs themselves were designed to interact with a human player at a terminal. An application acting as an intermediary passes one program’s moves to the other and vice versa. It uses
pseudo-terminals to fool the programs into behaving normally without a terminal being present.

Pseudo-terminals were at one time implemented in a system-specific manner, if at all. They have now been incorporated into the Single UNIX Specification as UNIX98 Pseudo-Terminals or PTYs.