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
Window managers often support separate “workspaces” that divide the desktop, increasing the area with which you can interact. The window manager is also responsible for adding decoration around each window, which usually consists of a frame and title bar with maximize, minimize, and close icons. Window managers provide part of the look and feel of the desktop, such as the window title bars.

### Other Ways to Create a GUI — Platform-Independent Windowing APIs

## Introducing GTK+

Now that you’ve looked at the X Window System, it’s time to look at the GTK+ Toolkit. GTK+ started out life as part of the popular GNU Image Manipulation Program, The GIMP, which is how GTK derives its name (The Gimp ToolKit). The GIMP programmers clearly had great foresight in making GTK+ a project in its own right because it has grown and developed into one of the most powerful and popular toolkits around.

### GLib Type System
GLib and GObject aid cross-platform development by providing a standard set of replacement data types, functions, and macros to handle memory management and common tasks. These types, functions, and macros mean that as GTK+ programmers we can be confident that our code will port reliably to other platforms and architectures.
GLib also defines some convenient constants:
```
#include <glib/gmacros.h>
#define FALSE 0
#define TRUE !FALSE
```

### GTK+ Object System
Anybody who has experimented with GUI programming before will probably understand when we write that GUI libraries strongly lend themselves to the paradigm of object-oriented (OO) programming;
so much so that all modern toolkits are written in an object-oriented fashion, including GTK+. Despite GTK+ being written purely in C, it supports objects and OO programming through the GObject library. This library supports object inheritance and polymorphism using macros.
Let’s look at an example of inheritance and polymorphism by looking at the GtkWindow object hierarchy taken from the GTK+ API documentation:
GObject
```
    +----GInitiallyUnowned
        +----GtkObject
            +----GtkWidget
                +----GtkContainer
                    +----GtkBin
                        +----GtkWindow
```
This list of objects tells you that GtkWindow is a child of GtkBin , and therefore any function you can call with a GtkBin can be called with GtkWindow . Similarly, GtkWindow inherits from GtkContainer , which inherits from GtkWidget . As a matter of convenience, all widget creation functions return a GtkWidget type. For example:
```
GtkWidget* gtk_window_new (GtkWindowType type);
```
Suppose you create a GtkWindow , and want to pass the returned value to a function that expects a GtkContainer such as gtk_container_add :
```
void gtk_container_add (GtkContainer *container, GtkWidget *widget);
```
### Introducing GNOME
GNOME is the name given to a project started in 1997 by programmers working on the GNU Image Manipulation Program (The GIMP) to create a unified desktop for Linux.
### Installing the GNOME/GTK+ Development Libraries

```


```

#### Try It Out
* gtk1.c
```
$ gcc gtk1.c –o gtk1 `pkg-config --cflags --libs gtk+-2.0`
```

## Events, Signals, and Callbacks
As a GTK+ programmer, all you need to worry about is writing and connecting callback functions, because the signal-emitting code is internal to the particular widget. The callback function prototype is typically like this:
```
void a_callback_function ( GtkWidget *widget, gpointer user_data);
```
Connecting the callback is just as simple. You simply call g_signal_connect and pass in the widget, a signal name as a string, a callback function pointer, and your arbitrary pointer:
```
gulong g_signal_connect(gpointer *object, const gchar *name, GCallback func, gpointer user_data );
```
One point worthy of noting — there are no restrictions in connecting callbacks. You can have multiple signals to a single callback function, and multiple callback functions connected to a single signal.

#### Try It Out
* gtk2.c
```
$ gcc gtk2.c `pkg-config --cflags --libs gtk+-2.0`
```

## Packing Box Widgets
When the packing boxes are created, you should specify two parameters, homogeneous and spacing :
```
GtkWidget* gtk_vbox_new (gboolean homogeneous, gint spacing);
GtkWidget* gtk_hbox_new (gboolean homogeneous, gint spacing);
```
These parameters control layout for all of the widgets in that particular packing box. homogeneous is a boolean that, when set to TRUE, forces contained widgets to occupy equal space, regardless of individual size. spacing sets the gap between widgets in pixels.
Once you’ve created the packing box, add widgets using gtk_box_pack_start and
gtk_box_pack_end functions:
```
void gtk_box_pack_start (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
void gtk_box_pack_end (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
```
#### Try It Out
* container.c
```
$ gcc container.c `pkg-config --cflags --libs gtk+-2.0`
```
## GTK+ Widgets

### GtkWindow
GtkWindow is the basic element of all GTK+ applications.
There are dozens of GtkWindow API calls, but here are the functions worthy of special attention:
```
GtkWidget* gtk_window_new (GtkWindowType type);
void gtk_window_set_title (GtkWindow *window, const gchar *title);
void gtk_window_set_position (GtkWindow *window, GtkWindowPosition position);
void gtk_window_set_default_size (GtkWindow *window, gint width, gint height);
void gtk_window_resize (GtkWindow *window, gint width, gint height);
void gtk_window_set_resizable (GtkWindow *window, gboolean resizable);
void gtk_window_present (GtkWindow *window);
void gtk_window_maximize (GtkWindow *window);
void gtk_window_unmaximize (GtkWindow *window);
```
gtk_window_new creates a new, empty window in memory. The window title is unset, and the size and screen position of the window are undefined. You will normally populate the window with widgets and set up a menu and toolbar before making the window visible onscreen with a call to gtk_widget_show .
The gtk_window_set_title function changes the text of the title bar by informing the window manager of the request.
gtk_window_set_position controls the position of the initial placement onscreen.
gtk_window_set_default_size sets the size of the window onscreen in GTK+ drawing units.
To force a resize of the window once it’s onscreen, you can use gtk_window_resize.

### GtkEntry
The GtkEntry widget is a single-line text entry widget that is commonly used to enter simple textual information, for example an e-mail address, a username, or a hostname. GtkEntry can be set to display asterisks (or any other user-definable character) in place of the literal typed characters, which can be very useful for entering passwords when you don’t want anybody to lean over your shoulder and read the text.The most useful GtkEntry functions:
```
GtkWidget* gtk_entry_new (void);
GtkWidget* gtk_entry_new_with_max_length (gint max);
void gtk_entry_set_max_length (GtkEntry *entry, gint max);
G_CONST_RETURN gchar* gtk_entry_get_text (GtkEntry *entry);
void gtk_entry_set_text (GtkEntry *entry, const gchar *text);
void gtk_entry_append_text (GtkEntry *entry, const gchar *text);
void gtk_entry_prepend_text (GtkEntry *entry, const gchar *text);
void gtk_entry_set_visibility (GtkEntry *entry, gboolean visible);
void gtk_entry_set_invisible_char (GtkEntry *entry, gchar invch);
void gtk_entry_set_editable (GtkEntry *entry, gboolean editable);
```
You can create a GtkEntry with either gtk_entry_new , or with a fixed maximum length of input text with gtk_entry_new_with_max_length .
To get the contents of the GtkEntry , you call gtk_entry_get_text , which returns a const char pointer internal to the GtkEntry ( G_CONST_RETURN is a GLib-defined macro).
You can manually set and modify the contents of the GtkEntry using the _set_text , _append_text , and _modify_text functions. Note that these take const pointers.
To use GtkEntry as a password entry box that displays asterisks in place of characters, use gtk_entry_set_visibilility passing FALSE as the visible parameter. The invisible character can be changed using gtk_entry_set_invisible_char to suit your requirements.

#### Try It Out
* entry.c
```
$ gcc entry.c `pkg-config --cflags --libs gtk+-2.0`
```
### GtkSpinButton
Sometimes you’ll want the user to enter a numeric value such as a maximum speed or length of device, and in these situations, a GtkSpinButton is ideal. GtkSpinButton restricts a user to entering numeric characters only, and you can set the range for allowed values between a lower and upper bound. The widget also provides up and down arrows so the user can “spin” the value using only the mouse for convenience:
```
GtkWidget
    +----GtkEntry
        +----GtkSpinButton
```
Again, the API is straightforward, and we’ll list the most commonly used calls:
```
GtkWidget* gtk_spin_button_new (GtkAdjustment *adjustment, gdouble climb_rate,guint digits);
GtkWidget* gtk_spin_button_new_with_range (gdouble min, gdouble max, gdouble step);
void gtk_spin_button_set_digits (GtkSpinButton *spin_button, guint digits);
void gtk_spin_button_set_increments (GtkSpinButton *spin_button, gdouble step, gdouble page);
void gtk_spin_button_set_range (GtkSpinButton *spin_button, gdouble min, gdouble max);
gdouble gtk_spin_button_get_value (GtkSpinButton *spin_button);
gint gtk_spin_button_get_value_as_int (GtkSpinButton *spin_button);
void gtk_spin_button_set_value (GtkSpinButton *spin_button, gdouble value);
```
To create a GtkSpinButton using gtk_spin_button_new , you first need to create a GtkAdjustment object. A GtkAdjustment widget is an abstract object that contains logic to deal with controlling bounded values. GtdAdjustment is also used in other widgets, such as GtkHScale and GtkVScale .
To create a GtkAdjustment , pass in an initial value, lower and upper bounds, and increment sizes:
```
GtkObject* gtk_adjustment_new (gdouble value, gdouble lower, gdouble upper, gdouble step_increment, gdouble page_increment, gdouble page_size);
```
#### Try It Out
* spin.c
```
$ gcc spin.c `pkg-config --cflags --libs gtk+-2.0`
```

### GtkButton

GtkToggleButton is descended directly from GtkButton , GtkCheckButton from GtkToggleButton and similarly with GtkRadioButton , each child widget specializing in purpose.

#### GtkToggleButton
GtkToggleButton is identical to a GtkButton except in one important regard: GtkToggleButton possesses state. That is, it can be on or off. When the user clicks a GtkToggleButton , it emits the “clicked” signal in the usual manner and changes (or “toggles”) its state.
The API for GtkToggleButton is very straightforward:
```
GtkWidget* gtk_toggle_button_new (void);
GtkWidget* gtk_toggle_button_new_with_label (const gchar *label);
gboolean gtk_toggle_button_get_active (GtkToggleButton *toggle_button);
void gtk_toggle_button_set_active (GtkToggleButton *toggle_button, gboolean is_active);
```

#### GtkCheckButton
GtkCheckButton is a GtkToggleButton in disguise. Instead of the boring, rectangular blob of GtkToggleButton , GtkCheckButton appears as an exciting checkbox with text at the side. There are no functional differences.
```
GtkWidget* gtk_check_button_new (void);
GtkWidget* gtk_check_button_new_with_label (const gchar *label);
```

#### GtkRadioButton

GtkRadioButton is one of those buttons that allows you to select only one option at a time from a group of options. The name comes from old-style radios that have mechanical buttons that pop back out when you press in another button.
```
GtkWidget* gtk_radio_button_new (GSList *group);
GtkWidget* gtk_radio_button_new_from_widget (GtkRadioButton *group);
GtkWidget* gtk_radio_button_new_with_label (GSList *group, const gchar *label);
void gtk_radio_button_set_group (GtkRadioButton *radio_button, GSList *group);
GSList* gtk_radio_button_get_group (GtkRadioButton *radio_button);
```
#### Try It Out
* buttons.c
```
$ gcc buttons.c `pkg-config --cflags --libs gtk+-2.0`
```

### GtkTreeView
GtkTreeView is part of a family of widgets new to GTK+ 2 that creates list and tree views of data of the sort you find in a spreadsheet or file manager. Using GtkTreeView , you can create complex views of data, mix text, and bitmap graphics, and even input data using GtkEntry widgets, and so forth.
The GtkTreeView family is made up of four components:
* GtkTreeView : The Tree and List View (known as the view)
* GtkTreeViewColumn : Represents a list or tree column
* GtkCellRenderer : Controls drawing cells
* GtkTreeModel : Represents Tree and List data (known as the model)
The concept of separating the View from the Model (often called the Model/View/Controller design pattern, or MVC for short) is not particular to GTK+, but a design increasingly favored in all programming circles.
#### Try It Out
* tree.c
```
$ gcc tree.c `pkg-config --cflags --libs gtk+-2.0`
```

## GNOME Widgets

The GNOME libraries include GNOME widgets that extend GTK+ and replace parts of GTK+ with easierto-use widgets. In this section we look at how to program using GNOME widgets.
Before you use the GNOME libraries, you must initialize them at the start of your programs in the same way you did GTK+. You call gnome_program_init just as you do gtk_init for purely GTK+ programs.
This function takes an app_id and app_version that you use to describe your program to GNOME, a module_info that tells GNOME which library module to initialize, command-line parameters, and application properties set as a NULL -terminated list of name/value pairs.
```
GnomeProgram* gnome_program_init (const char *app_id, const char *app_version, const GnomeModuleInfo *module_info, int argc, char **argv, const char  first_property_name, ...);
```
#### Try It Out
* gnome1.c
```
$ gcc gnome1.c –o gnome1 `pkg-config --cflags --libs libgnome-2.0 libgnomeui-2.0`
```


## GNOME Menus

Creating a drop-down menu bar in GNOME is pleasantly simple. Each menu in the menu bar is represented by an array of GNOMEUIInfo structs with each element in the array corresponding to a single menu item. For example, if you have File, Edit, and View menus, you will have three arrays describing the contents of each menu.
Once you’ve defined each individual menu, you create the menu bar itself by referencing these arrays in another array of GNOMEUIInfo structs. The GNOMEUIInfo struct is a little complicated and needs to be explained:
```
typedef struct {
GnomeUIInfoType type;
gchar const *label;
gchar const *hint;
gpointer moreinfo;
gpointer user_data;
gpointer unused_data;
GnomeUIPixmapType pixmap_type;
gconstpointer pixmap_info;
guint accelerator_key;
GdkModifierType ac_mods;
GtkWidget *widget;
} GnomeUIInfo;

```
#### Try It Out
* menu1.c
* menu2.c
```
$ gcc menu1.c `pkg-config --cflags --libs libgnome-2.0 libgnomeui-2.0`
```
## Dialogs

An important part of any GUI application is interacting with and informing users of important events. Usually, you’ll create a temporary window with OK and Cancel buttons for this, and if the information is important enough to require an immediate response, such as deleting a file, you’ll want to block access to all other windows until the user has made a choice (such windows are called modal dialogs).
What we’ve described here is a dialog, and GTK+ provides special dialog widgets that are descended from GtkWindow to make your programming job that much easier.

### GtkDialog
GtkDialog is a child of GtkWindow and therefore inherits all its functions and properties
GtkDialog divides the window into two areas, one for widget content and one for buttons that run along the bottom. You can specify the buttons you want as well as other dialog settings when you create the dialog box.
```
GtkWidget* gtk_dialog_new_with_buttons (const gchar *title, GtkWindow *parent, GtkDialogFlags flags, const gchar *first_button_text, ...);
```
This function creates a dialog window complete with a title and buttons.


### Modal Dialog Box

A modal dialog box forces the user to respond before any other action can take place. It’s useful in situations where the user is about to do something with serious repercussions or to report errors and warning messages.
When the user presses a button (or the dialog is destroyed), gtk_dialog_run returns with an int resultvthat indicates which button the user pressed. GTK+ usefully defines an enum to describe the possible values:
```
typedef enum
{
    GTK_RESPONSE_NONE = -1,
    GTK_RESPONSE_REJECT = -2,
    GTK_RESPONSE_ACCEPT = -3,
    GTK_RESPONSE_DELETE_EVENT = -4
    GTK_RESPONSE_OK    = -5,
    GTK_RESPONSE_CANCEL= -6,
    GTK_RESPONSE_CLOSE = -7,
    GTK_RESPONSE_YES   = -8,
    GTK_RESPONSE_NO    = -9,
    GTK_RESPONSE_APPLY = -10,
    GTK_RESPONSE_HELP  = -11
} GtkResponseType;
```
The switch construction is ideal for calling the appropriate logic:
```
GtkWidget *dialog = create_dialog();
int result = gtk_dialog_run(GTK_DIALOG(dialog));
switch (result)
{
case GTK_RESPONSE_ACCEPT:
delete_file();
break;
case GTK_RESPONSE_REJECT:
do_nothing();
break;
default:
dialog_was_cancelled ();
break;
}
gtk_widget_destroy (dialog);

```
### Nonmodal Dialogs

Nonmodal dialogs work slightly differently, although you create them in the same way. Instead of calling gtk_dialog_run , you connect a callback function to the GtkDialog “response” signal that is emitted when a button is clicked or the window is destroyed.
```
void dialog_button_clicked (GtkWidget *dialog, gint response, gpointer user_data)
{
switch (response)
{
case GTK_RESPONSE_ACCEPT:
do_stuff();
break;
case GTK_RESPONSE_REJECT:
do_nothing();
break;
default:
dialog_was_cancelled ();
break;
}
gtk_widget_destroy(dialog);
}
int main()
{
...
GtkWidget *dialog = create_dialog();
g_signal_connect ( GTK_OBJECT (dialog), “response”,
GTK_SIGNAL_FUNC (dialog_button_clicked), user_data );
gtk_widget_show(dialog);
...
}


```
### GtkMessageDialog
For very simple dialog boxes, even GtkDialog is unnecessarily complicated.
```
GtkDialog
    +----GtkMessageDialog
```
With GtkMessageDialog , you can create a message dialog box in a single line of code.
```
GtkWidget* gtk_message_dialog_new (GtkWindow *parent, GtkDialogFlags flags, GtkMessageType type, GtkButtonsType buttons, const gchar *message_format, ...);
```