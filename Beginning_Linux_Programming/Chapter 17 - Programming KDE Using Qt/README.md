# Summary - Programming KDE Using Qt
In this chapter, we cover
* An introduction to Qt
* Installing Qt
* Getting started
* Signal/slot mechanism
* Qt widgets
* Dialogs
* Menus and toolbars with KDE
* Building your CD database application with KDE/Qt

## Introducing KDE and Qt
- KDE (K Desktop Environment) is an open source desktop environment based on the Qt GUI library.
- Qt is a mature, cross-platform GUI toolkit written in C++.
## Installing Qt
```
$ rpm –Uvh qt-3.3.8-4.i386.rpm
$ cd /usr/local
$ tar –xvzf qt-x11-free-3.3.8.tar.gz
$ ./configure
$ make
$ echo $QTDIR
```
#### Try It Out
* qt1.cpp

```
$ g++ -o qt1 qt1.cpp –I$QTDIR/include –L$QTDIR/lib –lqui
$ ./qt1
```
## Signals and Slots

Qt’s signal-handling mechanism consists of signals and slots, which are Qt’s names for signals and callback functions in GTK+ or Java events and event handlers.
Signals and slots must be member functions of a class derived from QObject .
If using multiple inheritance, QObject must appear first in the class list.
A Q_OBJECT statement must appear in the class declaration.
Signals cannot be used in templates.
Function pointers cannot be used as arguments to signals and slots.
Signals and slots cannot be overridden and upgraded to public status.
A typical class definition MyWindow.h for your GUI will look something like this:
```
class MyWindow : public QMainWindow
{
    Q_OBJECT
    public:
    MyWindow();
    virtual ~MyWindow();
    signals:
    void aSignal();
    private slots:
    void doSomething();
}
```
You have one signal and one slot, both with no arguments. To emit aSignal() , all you need to do is call emit somewhere in your code:
```
emit aSignal();
```
To use slots, you must connect them to a signal. You do this with the appropriately named static connect method in the QObject class:
```
bool QObject::connect (const QObject * sender, const char * signal,
const QObject * receiver, const char * member)
```
Simply pass in the object that owns the signal (the sender), the signal function, the object that owns the slot (the receiver), and finally the name of the slot.
In the MyWindow example, if you wanted to connect the clicked signal of a QPushButton widget to your doSomething slot, you’d write
```
connect (button, SIGNAL(clicked()), this, SLOT(doSomething()));
```
Note that you must use SIGNAL and SLOT macros to surround the signal and slot functions.

#### Try It Out
* ButtonWindow.h
* ButtonWindow.cpp
```
$ moc ButtonWindow.h –o ButtonWindow.moc
$ g++ -o button ButtonWindow.cpp –I$QTDIR/include –L$QTDIR/lib –lqui
```
* LayoutWindow.h
* LayoutWindow.cpp
```
$ moc LayoutWindow.h –o LayoutWindow.moc
$ g++ -o layout LayoutWindow.cpp –I$QTDIR/include –L$QTDIR/lib –lqui
```
## Qt Widgets
There are widgets for every occasion in Qt, and looking at them all would take up the rest of the book. In this section you take a look at the common Qt widgets, including data entry widgets, buttons, combo box, and list widgets.

### QLineEdit

QLineEdit is Qt’s single-line text entry widget. Use it for inputting brief amounts of text, such as a user’s first name. With a QLineEdit widget, you can limit text input using an input mask to fit a template or, for the ultimate control, you can apply a validator function, for example, to ensure the user enters a proper date, phone number, or other similar value. QLineEdit has editing features, enabling you to select parts of the text, cut and paste, undo, redo, and the like from both a user’s perspective and using the API.


#### Try It Out
* LineEdit.h
* LineEdit.cpp

### Qt Buttons

Button widgets are ubiquitous widgets, and they vary little in appearance, usage, and API from toolkit to toolkit. It’s no surprise that Qt offers standard PushButton , CheckBox , and RadioButton variants.

#### QButton: The Button Base Class

Button widgets in Qt all derive from the abstract QButton class. This class has methods to query and toggle the on/off state of the button and to set the button’s text or pixmap.

#### QPushButton
QPushButton is the standard generic button that contains text such as “OK” or “Cancel” and/or a pixmap icon. Like all QButton s, it emits the clicked signal when it’s activated, and is usually used to connect a slot and perform some action.

#### QCheckBox
QCheckBox is a button that has state; that is, it can be turned on or off. The appearance of QCheckBox depends on the current windowing style (Motif, Windows, and so on) but is usually drawn as a ticked box with text to the right.

#### QRadioButton
Radio buttons are toggle buttons used to represent exclusive choices when you can select only one option out of a group of options presented (think back to those old car radios, where only one station button could be pressed in at a time). QRadioButton s themselves are hardly any different from QCheckBox es, because the grouping and exclusivity are all handled by the QButtonGroup class, the main difference being that radio buttons appear as round buttons rather than ticked boxes.

#### Try It Out
* Buttons.h
* Buttons.cpp

#### QComboBox
Radio buttons are an excellent way of enabling the user to select from a small number of options, say six or fewer. When you have more than six, things start to get out of hand and it becomes increasingly so as the number of options increases to keep the window to a sensible size.
QComboBox combines the functionality of a QLineEdit , QPushButton , and drop-down menus, enabling the user to pick a single option from an unlimited choice of options.
A QComboBox can be either read/write or read-only. If it is read/write the user can type an alternative to the options offered; otherwise the user is limited to selecting from the drop-down list.

#### Try It Out
* ComboBox.h
* ComboBox.cpp

#### QListView
Lists and trees in Qt are provided by the QListView widget. QListView displays both plain lists and hierarchical data divided into rows and columns. It’s perfect for displaying things like directory structures,because the child elements can be expanded and contracted by clicking the plus and minus boxes, just like a file viewer.
#### Try It Out
* ListView.cpp

## Dialogs
### QDialog

QDialog is the base dialog class in Qt that provides exec and show methods for handling modal and nonmodal dialogs, has an integral QLayout you can use, and has several signals and slots useful for responding to button presses.

### Modal Dialogs

### Nonmodal Dialogs
Nonmodal dialogs are little different from ordinary main windows, the key difference being that they position themselves over their parent window, share their taskbar entry, and hide automatically when the accept or reject slot is called.

### Semimodal Dialog
To create a semimodal dialog you must set the modal flag in the QDialog constructor and use the show method.

### QMessageBox
A QMessageBox is a modal dialog that displays a simple message with an icon and buttons. The icon depends on the severity of the message, which can be regular information or warnings and other critical
information.

### QInputDialog
QInputDialog is useful for inputting single values from the user, which can either be text, an option from a drop-down list, an integer, or a floating-point value. The QInputDialog class has static methods like QMessageBox that are a bit of a handful because they have many parameters, but fortunately most have default values.

### Using qmake to Simplify Writing Makefiles
Compiling applications with both the KDE and Qt libraries becomes quite a chore because your makefile gets ever more complicated with the need to use moc and having libraries here, there, and everywhere.
Fortunately, Qt comes with a utility called qmake to create makefiles for you.
If you’ve used Qt before, you may be familiar with tmake — an earlier (and now deprecated) incarnation of qmake that shipped with previous versions of Qt.
qmake takes a .pro file as input. This file contains the very basic information that the compilation requires, such as the sources, headers, target binary, and KDE/Qt library locations.
A typical KDE .pro file looks like this:
```
TARGET = app
MOC_DIR = moc
OBJECTS_DIR = obj
INCLUDEPATH = /usr/include/kde
QMAKE_LIBDIR_X11 += /usr/lib
QMAKE_LIBS_X11 += -lkdeui -lkdecore
SOURCES = main.cpp window.cpp
HEADERS = window.h
```
You specify the target binary, temporary moc and object directories, the KDE library path, and the sources and headers to build from. Note that the location of the KDE header and library files depends on your distribution. SUSE users should set INCLUDEPATH to /opt/kde3/include and QMAKE_LIBS_X11 to /opt/kde3/lib .
```
$ qmake file.pro –o Makefile
```
Then you can run make as normal; it’s that straightforward. For a KDE/Qt program of any complexity, you should use qmake to simplify the build routine.

## Menus and Toolbars with KDE
#### Try It Out
* KDEMenu.h
* KDEMenu.cpp
* menu.pro
```
$ qmake menu.pro –o Makefile
$ make
$ ./kdemenu
```


