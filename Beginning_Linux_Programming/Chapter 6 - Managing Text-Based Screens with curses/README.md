# Summary - Managing Text-Based Screens with curses

This chapter covers the following:
* Using the curses library
* The concepts of curses
* Basic input and output control
* Using multiple windows
* Using keypad mode
* Adding color

## Compiling with curses
 
The curses library takes its name from its ability to optimize the movement of the cursor and minimize the updates needed on a screen, and hence, reduce the number of characters that need to be sent to a text-based terminal. Although the number of characters output is much less important than it was in the days of dumb terminals and low-speed modems, the curses library survives as a useful addition to the programmer’s toolkit.

When you’re compiling curses programs, you must include the header file curses.h, and link against the curses library with -lcurses . On many Linux systems you can simply use curses but will find you are actually using the superior, and newer, ncurses implementation. You can check how your curses is set up by executing the command
```
ls –l /usr/include/*curses.h
```
to look at the header files, and
```
ls –l /usr/lib/lib*curses*
```
to check the library files. If you find that curses.h and ncurses.h are just linked files, and an ncurses library file is present, you should be able to compile the files in this chapter using a command such as the following:
```
$ gcc program.c -o program -lcurses
```
If, however, your curses setup is not automatically using ncurses , then you may have to explicitly force the use of ncurses by including ncurses.h , rather than curses.h , and by executing a compile command such as this:
```
$ gcc -I/usr/include/ncurses program.c -o program -lncurses
```
where the -I option specifies the directory in which to search for the header file.

## Curses Terminology and Concepts
The curses routines work on screens, windows, and subwindows. A screen is the device (usually a terminal screen, but it could also be an xterm screen) to which you are writing. It occupies all the available display on that device. Of course, if it’s a terminal window inside an X window, the screen is simply all the character positions available inside the terminal window. There is always at least one curses window, stdscr , that is the same size as the physical screen. You can create additional windows that are smaller than the screen. Windows can overlap each other and have many subwindows, but each subwindow must always be contained inside its parent window. 

The curses library maintains two data structures that act like a map of the terminal screen: stdscr , and curscr . stdscr , the more important of the two data structures, is updated when curses functions pro- duce output. The stdscr data structure is the “standard screen.” It acts in much the same way as stdout , the standard output, does for the stdio library. It’s the default output window in curses programs. The curscr structure is similar, but holds what the displayed screen actually looks like at the current moment. Output written to stdscr doesn’t appear on the screen until the program calls refresh , when the curses library compares the contents of stdscr (what the screen should look like) with the second structure curscr (what the screen currently looks like). curses then uses the differences between these two structures to update the screen. 
Some curses programs need to know that curses maintains a stdscr structure, which is required as a parameter to a few curses functions. However, the actual stdscr structure is implementation-dependent and should never be accessed directly. curses programs should never need to use the curscr structure.

Thus, the process for the output of characters in a curses program is as follows:
1. Use curses functions to update a logical screen.
2. Ask curses to update the physical screen with refresh .

The advantage of a two-level approach, in addition to being much easier to program, is that curses screen updates are very efficient. In addition, although this isn’t so important on a console screen, it can make a considerable difference if you’re running your program over a slow network link.

Because the curses library needs to create and destroy some temporary data structures, all curses programs must initialize the library before use and then allow curses to restore settings after use. This is done with a pair of function calls: initscr and endwin .

#### Try It Out
* screen1.c

## The Screen

As you’ve already seen, all curses programs must start with initscr and end with endwin . Here are their header file definitions:
```
#include <curses.h>
WINDOW *initscr(void);
int endwin(void);
```
The initscr function must only be called once in each program. The initscr function returns a pointer to the stdscr structure if it succeeds. If it fails, it simply prints a diagnostic error message and causes the program to exit.
The endwin function returns OK on success and ERR on failure. You can call endwin to leave curses and then later resume curses operation by calling clearok(stdscr, 1) and refresh . This effectively makes curses forget what the physical screen looks like and forces it to perform a complete redisplay.

### Output to the Screen

Several basic functions are provided for updating the screen:
```
#include <curses.h>
int addch(const chtype char_to_add);
int addchstr(chtype *const string_to_add);
int printw(char *format, ...);
int refresh(void);
int box(WINDOW *win_ptr, chtype vertical_char, chtype horizontal_char);
int insch(chtype char_to_insert);
int insertln(void);
int delch(void);
int deleteln(void);
int beep(void);
int flash(void);
```
curses has its own character type, chtype , which may have more bits than a standard char . In the standard Linux version of ncurses , chtype is actually a typedef for unsigned long .
The add... functions add the character or string specified at the current location. The printw function formats a string in the same way as printf and adds it to the current location. The refresh function causes the physical screen to be updated, returning OK on success and ERR if an error occurred. The box function allows you to draw a box around a window.

ACS_VLINE and ACS_HLINE to provide vertical and horizontal line characters, respectively, which enable you to draw a better-looking box. For this, your terminal needs to support line-drawing characters.

The insch function inserts a character, moving existing characters right, though what will happen at the end of a line isn’t specified and depends on the terminal you’re using. insertln inserts a blank line, moving existing lines down by one. The two delete functions are analogous to the two insert functions.

To make a sound, you can call beep .
flash causes the screen to flash, but if this isn’t possible, it tries to make a sound on the terminal instead.

### Reading from the Screen

You can read characters from the screen, although this facility isn’t commonly used because normally it’s easier to keep track of what was written. If you need it, it’s done with the following functions:
```
#include <curses.h>
chtype inch(void);
int instr(char *string);
int innstr(char *string, int number_of_characters);
```
The inch function returns a character and its attribute information from the current screen location of the cursor.

### Clearing the Screen

```
#include <curses.h>
int erase(void);
int clear(void);
int clrtobot(void);
int clrtoeol(void);
```
- erase function writes blanks to every screen location
- clear function, like erase , clears the screen, but forces a screen redisplay by internally calling a lower-level function
- clrtobot clears the screen from the cursor position onward to the end of the screen
- clrtoeol clears from the cursor position to the end of the line the cursor is on

### Moving the Cursor
A single function is provided for moving the cursor, with an additional command for controlling where curses leaves the cursor after screen updates:
```
#include <curses.h>
int move(int new_y, int new_x);
int leaveok(WINDOW *window_ptr, bool leave_flag);
```
The move function simply moves the logical cursor position to the specified location.
The leaveok function sets a flag that controls where curses leaves the physical cursor after a screen update. By default, the flag is false , and after a refresh the hardware cursor is left in the same position on the screen as the logical cursor. If the flag is set to true , the hardware cursor may be left randomly, anywhere on the screen.
### Character Attributes
Each curses character can have certain attributes that control how it’s displayed on the screen, assuming that the display hardware can support the requested attribute. The defined attributes are A_BLINK , A_BOLD ,A_DIM , A_REVERSE , A_STANDOUT , and A_UNDERLINE . You can use these functions to set attributes singly or collectively:
```
#include <curses.h>
int attron(chtype attribute);
int attroff(chtype attribute);
int attrset(chtype attribute);
int standout(void);
int standend(void);
```
#### Try It Out
* moveadd.c

## The Keyboard

### Keyboard Modes
```
#include <curses.h>
int echo(void);
int noecho(void);
int cbreak(void);
int nocbreak(void);
int raw(void);
int noraw(void)
```
### Keyboard Input


#### Try It Out
* 


## Windows

## Subwindows

## The Keypad

## Using Color

## Pads

## The CD Collection Application
