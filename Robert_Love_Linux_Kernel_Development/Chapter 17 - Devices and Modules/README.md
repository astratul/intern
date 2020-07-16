# Devices and Modules

In this chapter, we discuss four kernel components related to device drivers and device management:
* Device types—Classifications used in all Unix systems to unify behavior of common devices
* Modules—The mechanism by which the Linux kernel can load and unload object code on demand
* Kernel objects—Support for adding simple object-oriented behavior and a parent/child relationship to kernel data structures
* Sysfs—A filesystem representation of the system’s device tree

## Device Types

In Linux, as with all Unix systems, devices are classified into one of three types:
* Block devices (e.g. hard drives, Blu-ray discs, and memory devices such as flash) -  abbreviated blkdevs, block devices are addressable in device-specified chunks called blocks and generally support seeking, the random access of data.
* Character devices (e.g. keyboards, mice, printers, and most pseudo-devices) - abbreviated cdevs, character devices are generally not addressable, providing access to data only as a stream, generally of characters (bytes).
* Network devices - Sometimes called Ethernet devices after the most common type of network devices, network devices provide access to a network (such as the Internet) via a physical adapter and a specific protocol (such as IP).
* Others like miscellaneous devices, often abbreviated miscdevs, which are actually a simplified form of character devices. Miscellaneous devices enable a device driver author to represent simple devices easily, trading functionality for common infrastructure.
Not all device drivers represent physical devices. Some device drivers are virtual, providing access to kernel functionality.We call these pseudo devices; some of the most common are the kernel random number generator (accessible at /dev/random and /dev/urandom ), the null device (accessible at /dev/null ), the zero device (accessible at /dev/zero ), the full device (accessible at /dev/full ), and the memory device (accessible at /dev/mem ). Most device drivers, however, represent physical hardware.

## Modules

Despite being “monolithic,” in the sense that the whole kernel runs in a single address space, the Linux kernel is modular, supporting the dynamic insertion and removal of code from itself at runtime. Related subroutines, data, and entry and exit points are grouped together in a single binary image, a loadable kernel object, called a module. Support for modules allows systems to have only a minimal base kernel image, with optional features and drivers supplied via loadable, separate objects. Modules also enable the removal and reloading of kernel code, facilitate debugging, and allow for the loading of new drivers on demand in response to the hot plugging of new devices.

### Hello, World!

hello.c - The Hello, World! Kernel Module

### Building Modules
The first decision in building modules is deciding where the module source is to live.You can add the module source to the kernel source proper, either as a patch or by eventually merging your code into the official tree.Alternatively, you can maintain and build your module source outside the source tree.

#### Living in the Source Tree

Ideally, your module is an official part of Linux and thus lives in the kernel source tree. Getting your work into the kernel proper might require more work at first, but it is the preferred path because when your code is in the Linux kernel, the entire kernel community can help maintain and debug it.
When you decide to place your module in the kernel source tree, the next step is deciding where in the tree your module is to live. Drivers are stored in subdirectories of the drivers/ directory in the root of the kernel source tree. Inside drivers/ , class, type, and specific device further organize drivers. For example, drivers for character devices live in drivers/char/ , block devices live in drivers/block/ , and USB devices live in drivers/usb/ .The rules are flexible because many devices belong in multiple cate- gories—for instance, many USB devices are character devices, but they reside in drivers/usb/ not drivers/char/ .
Let’s assume you have a character device and want to store it in drivers/char/ . Presume that you want to create your own subdirectory.Next, you need to add a line to the Makefile in drivers/char/ . So you edit drivers/char/Makefile and add
```
obj-m += fishing/
// or
obj-$(CONFIG_FISHING_POLE) += fishing/
// inside drivers/char/fishing/ , you add a new Makefile with the following line:
obj-m += fishing.o
```
The build system now descends into fishing/ and builds the module fishing.ko from fishing.c .Yes, confusingly, you write an extension of .o but the module is compiled as .ko .As before, more likely your fishing pole driver’s compilation is conditional on a configuration option. So you probably want to write the following:
```
obj-$(CONFIG_FISHING_POLE) += fishing.o
```
One day, your fishing pole driver might get so complicated—autodetection of fishing line test is just the latest “must have!”—that it grows to occupy more than one source file.
No problem, anglers! You simply make your Makefile read the following:
```
obj-$(CONFIG_FISHING_POLE) += fishing.o
fishing-objs := fishing-main.o fishing-line.o
```
Finally, you might need to pass to the C compiler additional compile flags during the build process solely for your file.To do so, simply add a line such as the following to your Makefile:
```
EXTRA_CFLAGS += -DTITANIUM_POLE
```

#### Living Externally
If you prefer to maintain and build your module outside the kernel source tree, to live the life of an outsider, simply create a Makefile in your own source directory with this single line:
```
obj-m := fishing.o
```
This compiles fishing.c into fishing.ko . If your source spans multiple files, two lines will suffice:
```
obj-m := fishing.o
fishing-objs := fishing-main.o fishing-line.o
```
This example compiles fishing-main.c and fishing-line.c into fishing.ko .
The main difference in living externally is the build process. Because your module lives outside the kernel tree, you need to instruct make on how to find the kernel source files and base Makefile.This is also easy:
```
make -C /kernel/source/location SUBDIRS=$PWD modules
```

### Installing Modules

Compiled modules are installed into /lib/modules/ version /kernel/ , where each directory under kernel/ corresponds to the module’s location in the kernel source tree.
To install compiled modules into the correct location:
```
$ sudo make modules_install
```

### Generating Module Dependencies

The Linux module utilities understand dependencies. Most Linux distributions generate the mapping automatically and keep it up to date on each boot.To build the module dependency information, as root simply run
```
depmod
// To perform a quick update, rebuilding only the information for modules newer than the dependency information run:
sudo depmod -A
```
The module dependency information is stored in the file /lib/modules/ version /modules.dep .

### Loading Modules

The simplest way to load a module is via insmod .This utility is basic. It simply asks the kernel to load the module you specify.The insmod program does not perform any dependency resolution or advanced error checking. Usage is trivial.As root, simply run this command:
```
sudo insmod module.ko // module.ko is the filename of the module that you want to load.
``` 
In a similar fashion, to remove a module, you use the rmmod utility. As root, simply run the following, where module is the name of an already-loaded module:
```
sudo rmmod module
```
The utility modprobe provides dependency resolution, error checking and reporting, configurable behavior, and more advanced features. Its use is highly encouraged.
To insert a module into the kernel via modprobe , run as root:
```
sudo modprobe module [ module parameters ]
```
The modprobe command attempts to load not only the requested module, but also any modules on which it depends. Consequently, it is the preferred mechanism for loading kernel modules.
The modprobe command can also be used to remove modules from the kernel.To remove a module, as root, run:
```
sudo modprobe –r modules
```

### Managing Configuration Options

If you created a new subdirectory and want a new Kconfig file to live there, you need to source it from an existing Kconfig .You do this by adding a line such as the following to an existing Kconfig file: 
```
source “drivers/char/fishing/Kconfig”
```

### Module Parameters

The Linux kernel provides a simple framework, enabling drivers to declare parameters that the user can specify on either boot or module load and then have these parameters exposed in your driver as global variables.These module parameters also show up in sysfs.
Consequently, creating and managing module parameters that can be specified in a myriad of convenient ways is trivial. Defining a module parameter is done via the macro module_param() :
```
module_param( name , type , perm );
```
* Here, name is the name of both the parameter exposed to the user and the variable holding the parameter inside your module.
* The type argument holds the parameter’s data type; it is one of byte , short , ushort , int , uint , long , ulong , charp , bool , or invbool .
* The perm argument specifies the permissions of the corresponding file in sysfs.The permissions can be specified in the usual octal format, for example 0644 (owner can read and write, group can read, everyone else can read), or by ORing together the usual S_Ifoo defines, for example S_IRUGO | S_IWUSR (everyone can read; user can also write).A value of zero disables the sysfs entry altogether.

### Exported Symbols

When modules are loaded, they are dynamically linked into the kernel.As with userspace, dynamically linked binaries can call only into external functions explicitly exported for use. In the kernel, this is handled via special directives called EXPORT_SYMBOL() and EXPORT_SYMBOL_GPL() .
Exported functions are available for use by modules. Functions not exported cannot be invoked from modules.The linking and invoking rules are much more stringent for modules than code in the core kernel image. Core code can call any nonstatic interface in the kernel because all core source files are linked into a single base image. Exported symbols, of course, must be nonstatic, too.The set of exported kernel symbols are known as the exported kernel interfaces.
Exporting a symbol is easy.After the function is declared, it is usually followed by an EXPORT_SYMBOL() .
Some developers want their interfaces accessible to only GPL-compliant modules.The kernel linker enforces this restriction through use of the MODULE_LICENSE() directive. If you want the previous function accessible to only modules that labeled themselves as GPL-licensed, use instead EXPORT_SYMBOL_GPL(get_pirate_beard_color);
If your code is configurable as a module, you must ensure that when compiled as a module all interfaces that it uses are exported. Otherwise linking errors (and a broken module) result.

## The Device Model

The device model provides a single mechanism for representing devices and describing their topology in the system. Such a system provides several benefits:
* Minimization of code duplication
* A mechanism for providing common facilities, such as reference counting
* The capability to enumerate all the devices in the system, view their status, and see to what bus they attach
* The capability to generate a complete and valid tree of the entire device structure of the system, including all buses and interconnections
* The capability to link devices to their drivers and vice versa
* The capability to categorize devices by their class, such as input device, without the need to understand the physical device topology
* The capability to walk the tree of devices from the leaves up to the root, powering down devices in the correct order

The initial motivation for the device model was this final point: providing an accurate device tree to facilitate power management.To implement device-level power management in the kernel, you need to build a tree representing the device topology in the system.
When powering down, the kernel must power down the lower (leaf) nodes of the tree before the higher nodes.

### Kobjects

At the heart of the device model is the kobject, short for kernel object, which is represented by struct kobject and defined in <linux/kobject.h> .The kobject is similar to the Object class in object-oriented languages such as C# or Java. It provides basic facilities, such as reference counting, a name, and a parent pointer, enabling the creation of a hierarchy of objects.
Kobjects are usually embedded in other structures and are generally not interesting on their own. Instead, a more important structure, such as struct cdev , defined in <linux/cdev.h> , has a kobj member. When kobjects are embedded inside other structures, the structures receive the standardized functions that a kobject provides. Most important, the structure’s embedded kobject now enables the structure to become part of an object hierarchy.

### Ktypes

Kobjects are associated with a specific type, called a ktype, short for kernel object type. Ktypes are represented by struct kobj_type and defined in <linux/kobject.h>
Ktypes have the simple job of describing default behavior for a family of kobjects. Instead of each kobject defining its own behavior, the behavior is stored in a ktype, and kobjects of the same “type” point at the same ktype structure, thus sharing the same behavior.

### Ksets

Ksets, short for kernel object sets, are aggregate collections of kobjects. Ksets work as the base container class for a set of kernel objects, collecting related kobjects, such as “all block devices,” together in a single place.
The distinction is kept to allow kobjects of identical ktypes to be grouped into different ksets.That is, there are only a handful of ktypes, but many ksets, in the Linux kernel. The kset pointer points at a kobject’s associated kset. ksets are represented by the kset structure, which is declared in <linux/kobject.h>
Uevent, short for user events, is a mechanism for communicating with user-space information about the hotplugging and hot removal of devices from a system.

### Interrelation of Kobjects, Ktypes, and Ksets

The important key object is the kobject, represented by struct kobject .The kobject introduces basic object properties—such as reference counting, parent-child relationship, and naming—to kernel data structures.The kobject structure provides these features in a standard unified way. Kobjects, in and of themselves, are not particularly useful. Instead, kobjects are typically embedded in other data structures, giving those containing structures the features of the kobject.
Kobjects are associated with a specific ktype, which is represented by struct kobj_type and pointed at by the ktype variable inside of the kobject. ktypes define some default properties of related kobjects: destruction behavior, sysfs behavior, and default attributes.The ktype structure is not well named; think of ktypes not as a grouping but as a set of shared operations.
Kobjects are then grouped into sets, called ksets, which are represented by struct kset . Ksets provide two functions. First, their embedded kobject acts as a base class for a group of kobjects. Second, ksets aggregate together related kobjects. In sysfs, kobjects are the individual directories in the filesystem. Related directories—say, perhaps all subdirectories of a given directory—might be in the same kset.

### Managing and Manipulating Kobjects

With the basic internals of kobjects and friends behind you, it’s time to look at the exported interfaces used for managing and manipulating kobjects. Most of the time, driver writers do not have to deal with kobjects directly. Instead, kobjects are embedded in some class-specific structure (as you saw with the character device structure) and managed “behind the scenes” by the associated driver subsystem. Nonetheless, kobjects are not intended to remain hidden and can seep through into driver code or you might be hacking on the driver subsystem itself.
The first step in using a kobject is declaring and initializing it. kobjects are initialized via the function kobject_init , which is declared in <linux/kobject.h>:
```
void kobject_init(struct kobject *kobj, struct kobj_type *ktype);
```
The function’s first parameter is the kobject to initialize. Before calling this function, the kobject must be zeroed.This might normally happen during the initialization of the larger function in which the kobject is embedded. If not, a simple call to memset() does the trick:
```
memset(kobj, 0, sizeof (*kobj));
```

### Reference Counts

One of the primary features provided by kobjects is a unified reference counting system. After initialization, the kobject’s reference count is set to one. So long as the reference count is nonzero, the object continues to exist in memory and is said to be pinned.Any code that holds a reference to the object first elevates the reference count.When the code is finished with the object, the reference count is decremented. Bumping the reference count is called getting a reference to the object, and decrementing the reference count is called putting a reference to the object.When the reference count reaches zero, the object can be destroyed and any associated memory freed.

#### Incrementing and Decrementing Reference Counts
Incrementing the reference count is done via kobject_get() , declared in <linux/kobject.h> :
```
struct kobject * kobject_get(struct kobject *kobj);
```
This function returns a pointer to the kobject or NULL on failure.
Decrementing the reference count is done via kobject_put() , also declared in <linux/kobject.h> :
```
void kobject_put(struct kobject *kobj);
```
If the provided kobject’s reference count reaches zero, the release function pointed at by the ktype associated with the kobject is invoked, any associated memory is freed, and the object is no longer valid.

#### Krefs
Internally, the kobject reference counting is provided by the kref structure, which is defined in <linux/kref.h >:
```
struct kref {
atomic_t refcount;
};
```
The lone member is an atomic variable used to hold the reference count.A structure is used simply to provide type checking. Before using a kref, you must initialize it via kref_init():
```
void kref_init(struct kref *kref)
{
atomic_set(&kref->refcount, 1);
}
```
To obtain a reference to a kref, use kref_get() , declared in <linux/kref.h> :
```
void kref_get(struct kref *kref)
{
WARN_ON(!atomic_read(&kref->refcount));
atomic_inc(&kref->refcount);
}
```
This function bumps the reference count. It has no return value.To drop a reference to a kref, use kref_put() , declared in <linux/kref.h> :
```
int kref_put(struct kref *kref, void (*release) (struct kref *kref))
{
WARN_ON(release == NULL);
WARN_ON(release == (void (*)(struct kref *))kfree);
if (atomic_dec_and_test(&kref->refcount)) {
release(kref);
return 1;
}
return 0;
}
```
This function drops the reference count by one and calls the provided release() function if the count is now zero.
* All these functions are defined in lib/kref.c and declared in <linux/kref.h> .

## sysfs

The sysfs filesystem is an in-memory virtual filesystem that provides a view of the kobject hierarchy. It enables users to view the device topology of their system as a simple filesystem. Using attributes, kobjects can export files that enable kernel variables to be read from and optionally written to.
By linking kobjects to dentries, kobjects trivially map to directories. Exporting the kobjects as a filesystem is now as easy as building a tree of the dentries in memory.
kobjects already form a tree, our beloved device model.With kobjects mapping to dentries and the object hierarchy already forming an in-memory tree, sysfs became trivial.

### Adding and Removing kobjects from sysfs

Initialized kobjects are not automatically exported to sysfs.To represent a kobject to sysfs, you use kobject_add() :
```
int kobject_add(struct kobject *kobj, struct kobject *parent, const char *fmt, ...);
```
A given kobject’s location in sysfs depends on the kobject’s location in the object hierarchy.
<linux/kobject.h>

### Adding Files to sysfs

#### Default Attributes

A default set of files is provided via the ktype field in kobjects and ksets. Consequently, all kobjects of the same type have the same default set of files populating their sysfs directories.The kobj_type structure contains a member, default_attrs , that is an array of attribute structures. Attributes map kernel data to files in sysfs.
The attribute structure is defined in <linux/sysfs.h>.
Because this single set of functions must handle file I/O requests on all attributes, they typically need to maintain some sort of generic mapping to invoke a handler specific to each attribute.

#### Creating New Attributes

Generally, the default attributes provided by the ktype associated with a kobject are sufficient. Indeed, the purpose of ktype is to provide common operations to kobjects. Sharing ktypes between kobjects not only simplifies programming, but also provides code consoli- dation and a uniform look and feel to sysfs directories of related objects.
The Kernel provides the sysfs_create_file() interface for adding new attributes on top of the default set:
```
int sysfs_create_file(struct kobject *kobj, const struct attribute *attr);
```
This function associates the attribute structure pointed at by attr with the kobject pointed at by kobj . Before it is invoked, the given attribute should be filled out.This function returns zero on success and a negative error code otherwise.
In addition to creating actual files, it is possible to create symbolic links. Creating a symlink in sysfs is easy:
```
int sysfs_create_link(struct kobject *kobj, struct kobject *target, char *name);
```
This function creates a link named name in the directory mapped from kobj to the directory mapped from target .This function returns zero on success and a negative error code otherwise.

#### Destroying Attributes

Removing an attribute is handled via sysfs_remove_file() :
```
void sysfs_remove_file(struct kobject *kobj, const struct attribute *attr);
```
Upon call return, the given attribute no longer appears in the given kobject’s directory. Symbolic links created with sysfs_create_link() can be removed with sysfs_remove_link() :
```
void sysfs_remove_link(struct kobject *kobj, char *name);
```
Upon return, the symbolic link name in the directory mapped from kobj is removed.
All four of these functions are declared in <linux/kobject.h> in file.c and in symlink.c .

#### sysfs Conventions

The sysfs filesystem is currently the place for implementing functionality previously reserved for ioctl() calls on device nodes or the procfs filesystem. Instead of these deprecated kernel interfaces, today kernel developers implement such functionality as sysfs attributes in the appropriate directory.
1. First, sysfs attributes should export one value per file.Values should be text-based and map to simple C types.The goal is to avoid the highly structured or highly messy representation of data we have today in /proc .
2. Second, organize data in sysfs in a clean hierarchy. Correctly parent kobjects so that they map intuitively into the sysfs tree.Associate attributes with the correct kobject and keep in mind that the kobject hierarchy exists not only in the kernel, but also as an exported tree to user-space. Keep the sysfs tree organized and hierarchical.
3. Finally, remember that sysfs provides a kernel-to-user service and is thus a sort of user-space ABI. User programs can rely on the existence, location, value, and behavior of sysfs directories and files. Changing existing files in any way is discouraged, and modifying the behavior of a given attribute but keeping its name and location is surely begging for trouble.

### The Kernel Events Layer
The Kernel Event Layer implements a kernel-to-user notification system on top of kobjects.After the release of 2.6.0, it became clear that a mechanism for pushing events out of the kernel and up into user-space was needed, particularly for desktop systems that needed a more integrated and asynchronous system.The idea was to have the kernel push events up the stack: Hard drive full! Processor is overheating! Partition mounted!
Early revisions of the event layer came and went, and it was not long before the whole thing was tied intimately to kobjects and sysfs.The result, it turns out, is pretty neat.The Kernel Event Layer models events as signals emitting from objects—specifically, kobjects.
Because kobjects map to sysfs paths, the source of each event is a sysfs path. If the event in question has to do with your first hard drive, /sys/block/hda is the source address.
Each event is given a verb or action string representing the signal.The strings are terms such as modified or unmounted that describe what happened.
Finally, each event has an optional payload. Rather than pass an arbitrary string to user-space that provides the payload, the kernel event layer represents payloads as sysfs attributes.
Internally, the kernel events go from kernel-space out to user-space via netlink. Netlink is a high-speed multicast socket that transmits networking information. Using netlink means that obtaining kernel events from user-space is as simple as blocking on a socket.The intention is for user-space to implement a system daemon that listens on the socket, processes any read events, and transmits the events up the system stack.
To send events out to user-space from your kernel code, use kobject_uevent() :
```
int kobject_uevent(struct kobject *kobj, enum kobject_action action);
```
The first parameter specifies the kobject emitting this signal.The actual kernel event contains the sysfs path to which this kobject maps.
The second parameter specifies the action or verb describing this signal.The actual kernel event contains a string that maps to the provided enum kobject_action value. Rather than directly provide the string, the function uses an enumeration to encourage value reuse, provide type safety, and prevent typos and other mistakes.The enumerations are defined in <linux/kobject.h >
