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

By using the attron and attroff functions, you controlled the attributes of the text written out at each location. The program then demonstrated how characters can be inserted with insch , before closing the curses library and terminating.

## The Keyboard

### Keyboard Modes
```
#include <curses.h>
int echo(void);
int noecho(void);
// turn the echoing of typed characters on and off
int cbreak(void);
int nocbreak(void);
int raw(void);
int noraw(void)
// control how characters typed on the terminal are made available to the curses program
```

When a curses program starts by
calling initscr , the input mode is set to what is termed cooked mode. This means that all processing is done on a line-by-line basis; that is, input is only available after the user has pressed Enter (or the Return key on some keyboards). Keyboard special characters are enabled, so typing the appropriate key sequences can generate a signal in the program. Flow control, if the terminal is running from a terminal, is also enabled.
By calling cbreak , a program may set the input mode to cbreak mode whereby characters are available to the program immediately after they are typed, rather than being buffered up and only made available to the program when Enter is pressed. As in cooked mode, keyboard special characters are enabled, but
simple keys, such as Backspace, are passed directly to the program to be processed, so if you want the Backspace key to function as expected, you have to program it yourself.
A call to raw turns off special character processing, so it becomes impossible to generate signals or flow control by typing special character sequences.
Calling nocbreak sets the input mode back to cooked mode, but leaves special character processing unchanged; calling noraw restores both cooked mode and special character handling.

### Keyboard Input
```
#include <curses.h>
int getch(void);
int getstr(char *string); //provides no way of limiting the string length returned
int getnstr(char *string, int number_of_characters); // allows you to limit the number of characters read
int scanw(char *format, ...);
```

#### Try It Out
* ipmode.c

## Windows

### The WINDOW Structure
You can create and destroy windows using the newwin and delwin calls:
```
#include <curses.h>
WINDOW *newwin(int num_of_lines, int num_of_cols, int start_y, int start_x); // creates a new window with a screen location of (start_y,start_x) and with the specified number of lines and column. It returns a pointer to the new window, or null if the creation failed.
int delwin(WINDOW *window_to_delete); // deletes a window previously created by newwin
```
##### Take care never to try to delete curses ’ own windows, stdscr and curscr !

### Generalized Functions
You’ve already used the addch and printw functions for adding characters to the screen. Along with many other functions, these can be prefixed, either with a w for window, mv for move, or mvw for move and window. If you look in the curses header file for most implementations of curses , you’ll find that many of the functions used so far are simply macros ( #defines ) that call these more general functions.
When the w prefix is added, an additional WINDOW pointer must be pre-pended to the argument list. When the mv prefix is added, two additional parameters, a y and an x location, must be pre-pended. These specify the location where the operation will be performed. The y and x are relative to the window, rather than the screen, (0 , 0) being the top left of the window.
When the mvw prefix is added, three additional parameters, a WINDOW pointer and both y and x values, must be passed. Confusingly, the WINDOW pointer always comes before the screen coordinates, even though the prefix might suggest the y and x come first.
As an example, here is the full set of prototypes for just the addch and printw sets of functions:
```
#include <curses.h>
int addch(const chtype char);
int waddch(WINDOW *window_pointer, const chtype char)
int mvaddch(int y, int x, const chtype char);
int mvwaddch(WINDOW *window_pointer, int y, int x, const chtype char);
int printw(char *format, ...);
int wprintw(WINDOW *window_pointer, char *format, ...);
int mvprintw(int y, int x, char *format, ...);
int mvwprintw(WINDOW *window_pointer, int y, int x, char *format, ...);
```
Many other functions, such as inch , also have move and window variants available.

### Moving and Updating a Window

```
#include <curses.h>
// enable you to move and redraw windows
int mvwin(WINDOW *window_to_move, int new_y, int new_x); // moves a window on the screen
int wrefresh(WINDOW *window_ptr);
int wclear(WINDOW *window_ptr);
int werase(WINDOW *window_ptr); //all 3  - take a WINDOW pointer so that they can refer to a specific window, rather than stdscr

int touchwin(WINDOW *window_ptr); // informs the curses library that the contents of the window pointed to by its parameter have been changed
int scrollok(WINDOW *window_ptr, bool scroll_flag); // for true (usually nonzero) allows a window to scroll. By default, windows can’t scroll.
int scroll(WINDOW *window_ptr); // scrolls the window up one line
```

#### Try It Out
* multiw1.c

After the usual initialization, the program fills the standard screen with numbers, to make it easier to see the new curses windows being added on top. It then demonstrates how a new window can be added over the background, with text inside the new window wrapping. You then saw how to use touchwin to force curses to redraw the window, even if nothing has been changed.
A second window was then added which overlapped the first window, to demonstrate how curses can mange overlapping windows, before the program closes the curses library and exits.
### Optimizing Screen Refreshes
```
#include <curses.h>
int wnoutrefresh(WINDOW *window_ptr); // determines which characters would need sending to the screen, but doesn’t actually send them.
int doupdate(void); // sends the changes to the terminal.
```

## Subwindows

```
#include <curses.h>
WINDOW *subwin(WINDOW *parent, int num_of_lines, int num_of_cols,
int start_y, int start_x); //  make a subwindow
int delwin(WINDOW *window_to_delete); // subwindows are deleted
```

##### One restriction imposed by using subwindows is that the application should call touchwin on the parent window before refreshing the screen.

#### Try It Out
* subscl.c

## The Keypad
The translation between the sequences and logical keys is disabled when curses starts and has to be turned on by the keypad function. If the call succeeds, then it returns OK ; otherwise it returns ERR :
```
#include <curses.h>
int keypad(WINDOW *window_ptr, bool keypad_on);
```
Once keypad mode has been enabled by calling keypad with keypad_on set to true , curses takes over the processing of key sequences so that reading the keyboard may now not only return the key that was pressed, but also one of the KEY_ defines for logical keys.

#### Try It Out
* keypad.c

## Using Color
```
#include <curses.h>
bool has_colors(void); //returns true if color is supported
int start_color(void); //returns OK if color has been initialized successfully
```
Before you can use colors as attributes, you must initialize the color pairs that you wish to use. You do this with the init_pair function. Color attributes are accessed with the COLOR_PAIR function:
```
#include <curses.h>
int init_pair(short pair_number, short foreground, short background);
int COLOR_PAIR(int pair_number);
int pair_content(short pair_number, short *foreground, short *background);
```
curses.h usually defines some basic colors, starting with COLOR_ . An additional function, pair_content , allows previously defined color-pair information to be retrieved.

```
init_pair(1, COLOR_RED, COLOR_GREEN); // To define color pair number 1 to be red on green, you would use this
wattron(window_ptr, COLOR_PAIR(1)); // access this color pair as an attribute, using COLOR_PAIR
wattron(window_ptr, COLOR_PAIR(1) | A_BOLD);
```
#### Try It Out
* color.c

### Redefining Colors
```
#include <curses.h>
int init_color(short color_number, short red, short green, short blue);
```
This allows an existing color (in the range 0 to COLORS ) to be redefined with new intensity values in the range 0 to 1,000. This is a little like defining color values for GIF format image files.

## Pads
A pad structure is very similar to a WINDOW structure, and all the curses routines that write to windows can also be used on pads. However, pads do have their own routines for creation and refreshing.
You create pads in much the same way that you create normal windows:
```
#include <curses.h>
WINDOW *newpad(int number_of_lines, int number_of_columns);
```
Note that the return value is a pointer to a WINDOW structure, the same as newwin . Pads are deleted with delwin , just like windows.

Do this with the prefresh function:
```
#include <curses.h>
int prefresh(WINDOW *pad_ptr, int pad_row, int pad_column,
int screen_row_min, int screen_col_min,
int screen_row_max, int screen_col_max);
```
#### Try It Out
* pad.c

## The CD Collection Application

### Starting a New CD Collection Application
### Looking at main
main enables you to make selections from the menu until you select quit .
### Building the Menu
The getchoice function called by main is the principal function in this section. getchoice is passed greet , an introduction, and choices , which points either to the main or to the extended menu (depending on whether a CD has been selected). You can see how either main_menu or extended_menu is passed as a parameter in the preceding main function
### Database File Manipulation
#### Adding Records
#### Updating Records
#### Removing Records
### Querying the CD Database
