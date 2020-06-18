# Summary - Programming GNOME Using GTK+

In this chapter, we cover
* The X Window System
* An introduction to GNOME/GTK+
* GTK+ widgets
* GNOME widgets and menus
* Dialogs
* CD Database GUI using GNOME/GTK+

## Introducing X
One of the most innovative, and ultimately frustrating, features of X is the
rigid adherence to the mantra of mechanism, not policy. That means X defines no user interface, but provides the means to make one. This means you’re free to create your own entire desktop environment, experimenting and innovating at will. But, it also hindered user interfaces on Linux and UNIX for a long time. Into this relative void, two desktop projects emerged as the favorites of Linux users: GNOME and KDE. The Linux desktop does not begin and end with X, however. In truth, the desktop in Linux is a rather nebulous thing, with no single project or group releasing a definitive version. A modern installation contains a myriad of libraries, utilities, and applications that collectively are called “the desktop.”

### X Server
The X server runs on the user’s local machine and is the part that performs the low-level operation of drawing the graphics onscreen. The server part of the name often confuses: the X server runs on your desktop PC. X clients may run on your desktop PC, or X clients can actually run on other systems on your network — including servers. The reversed terminology actually makes sense when you think about it, but it often seems backwards.
Because the X server talks directly to the graphics card, you must use an X server specific to your graphics card, and it must be configured with appropriate resolution, refresh rate, color depth, and so on. The configuration file is named xorg.conf or Xfree86Config .

### X Client

An X client is any program that uses the X Window System as a GUI. The X client need not be on the same machine as the X server.

### X Protocol

The X client and X server communicate using the X Protocol, which enables the client and server to be separated over a network. For instance, you can run an X client application from a remote computer on the Internet or over an encrypted Virtual Private Network (VPN).

### Xlib

Xlib is the library used indirectly by an X client to generate the X Protocol messages. It provides a very low-level API to allow the client to draw very basic elements on the X server and to respond to the simplest of inputs. We must emphasize that Xlib is very low level — to create something even as simple as a menu using Xlib is an incredibly laborious process that needs hundreds of lines of code.
A GUI programmer cannot sensibly program directly with Xlib. You need an API that makes GUI elements such as menus, buttons, and drop-down lists easy and simple to create. In a nutshell, this is the role of the toolkit.

### Toolkits
A toolkit is a GUI library that X clients utilize to greatly simplify the creation of windows, menus, buttons, and so on. Using a toolkit, you can create buttons, menus, frames, and the like with single function calls. The generic term for GUI elements such as these is widgets, a universal term you’ll find in all
modern GUI libraries.
There are dozens of toolkits for X to choose from, each with their defining strengths and weaknesses.
Which one you choose is an important design decision for your application, and some of the factors you should consider are
* Who are you targeting with your application?
* Will your users have the toolkit libraries installed?
* Does the toolkit have a port for other popular operating systems?
* What software license does the toolkit use, and is it compatible with your intended use?
* Does the toolkit support your programming language?
* Does the toolkit have a modern look and feel?

### Window Managers

### Other Ways to Create a GUI — Platform-Independent Windowing APIs

## Introducing GTK+

### GLib Type System

### GTK+ Object System

### Introducing GNOME

### Installing the GNOME/GTK+ Development Libraries

## Events, Signals, and Callbacks

## Packing Box Widgets

## GTK+ Widgets

### GtkWindow

### GtkEntry

### GtkSpinButton

### GtkButton

#### GtkToggleButton

#### GtkCheckButton

#### GtkRadioButton

### GtkTreeView
 
## GNOME Widgets

## GNOME Menus

## Dialogs

### GtkDialog

### Modal Dialog Box

### Nonmodal Dialogs

### GtkMessageDialog

## CD Database Application

