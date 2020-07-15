# The Virtual Filesystem
The Virtual Filesystem (sometimes called the Virtual File Switch or more commonly simply the VFS) is the subsystem of the kernel that implements the file and filesystem-related interfaces provided to user-space programs.All filesystems rely on the VFS to enable them not only to coexist, but also to interoperate.This enables programs to use standard Unix system calls to read and write to different filesystems, even on different media.

## Common Filesystem Interface
The VFS is the glue that enables system calls such as open() , read() , and write() to work regardless of the filesystem or underlying physical medium.These days, that might not sound novel—we have long been taking such a feature for granted—but it is a nontrivial feat for such generic system calls to work across many diverse filesystems and varying media. More so, the system calls work between these different filesystems and media— we can use standard system calls to copy or move files from one filesystem to another. In older operating systems, such as DOS, this would never have worked; any access to a nonnative filesystem required special tools. It is only because modern operating systems, such as Linux, abstract access to the filesystems via a virtual interface that such interoperation and generic access is possible.
New filesystems and new varieties of storage media can find their way into Linux, and programs need not be rewritten or even recompiled.

## Filesystem Abstraction Layer
Such a generic interface for any type of filesystem is feasible only because the kernelv implements an abstraction layer around its low-level filesystem interface.This abstraction layer enables Linux to support different filesystems, even if they differ in supported features or behavior.This is possible because the VFS provides a common file model that can represent any filesystem’s general feature set and behavior. Of course, it is biased toward Unix-style filesystems.
The actual filesystem code hides the implementation details.To the VFS layer and the rest of the kernel, however, each filesystem looks the same.They all support notions such as files and directories, and they all support operations such as creating and deleting files.
The result is a general abstraction layer that enables the kernel to support many types  of filesystems easily and cleanly.The filesystems are programmed to provide the abstracted interfaces and data structures the VFS expects; in turn, the kernel easily works with any filesystem and the exported user-space interface seamlessly works on any filesystem.
In fact, nothing in the kernel needs to understand the underlying details of the filesystems, except the filesystems themselves. For example, consider a simple user-space program that does
```
ret = write (fd, buf, len);
```
This system call writes the len bytes pointed to by buf into the current position in the file represented by the file descriptor fd .


## Unix Filesystems
Unix has provided four basic filesystem-related abstractions: files, directory entries, inodes, and mount points. A filesystem is a hierarchical storage of data adhering to a specific structure. Filesystems contain files, directories, and associated control information.
Typical operations performed on filesystems are creation, deletion, and mounting. In Unix, filesystems are mounted at a specific mount point in a global hierarchy known as a namespace.
A file is an ordered string of bytes.The first byte marks the beginning of the file, and the last byte marks the end of the file. Each file is assigned a human-readable name for identification by both the system and the user.Typical file operations are read, write,create, and delete.
Files are organized in directories.A directory is analogous to a folder and usually contains related files. Directories can also contain other directories, called subdirectories. In this fashion, directories may be nested to form paths. Each component of a path is called a directory entry.In
Unix, directories are actually normal files that simply list the files contained therein. Because a directory is a file to the VFS, the same operations performed on files can be performed on directories.
Unix systems separate the concept of a file from any associated information about it, such as access permissions, size, owner, creation time, and so on.This information is sometimes called file metadata (that is, data about the file’s data) and is stored in a separate data structure from the file, called the inode.This name is short for index node, although these days the term inode is much more ubiquitous.
All this information is tied together with the filesystem’s own control information, which is stored in the superblock.The superblock is a data structure containing information about the filesystem as a whole. Sometimes the collective data is referred to as filesystem metadata. Filesystem metadata includes information about both the individual files and the filesystem as a whole.

## VFS Objects and Their Data Structures

The VFS is object-oriented.A family of data structures represents the common file model.These data structures are akin to objects. Because the kernel is programmed strictly in C, without the benefit of a language directly supporting object-oriented paradigms, the data structures are represented as C structures.The structures contain both data and pointers to filesystem-implemented functions that operate on the data. The four primary object types of the VFS are
* The superblock object, which represents a specific mounted filesystem.
* The inode object, which represents a specific file.
* The dentry object, which represents a directory entry, which is a single component of a path.
* The file object, which represents an open file as associated with a process.


An operations object is contained within each of these primary objects.These objects describe the methods that the kernel invokes against the primary objects:
* The super_operations object, which contains the methods that the kernel can invoke on a specific filesystem, such as write_inode() and sync_fs()
* The inode_operations object, which contains the methods that the kernel can invoke on a specific file, such as create() and link()
* The dentry_operations object, which contains the methods that the kernel can invoke on a specific directory entry, such as d_compare() and d_delete()
* The file_operations object, which contains the methods that a process can invoke on an open file, such as read() and write()

The operations objects are implemented as a structure of pointers to functions that operate on the parent object. For many methods, the objects can inherit a generic function if basic functionality is sufficient. Otherwise, the specific instance of the particular filesystem fills in the pointers with its own filesystem-specific methods.


## The Superblock Object
The superblock object is implemented by each filesystem and is used to store information describing that specific filesystem.This object usually corresponds to the filesystem superblock or the filesystem control block, which is stored in a special sector on disk (hence the object’s name). Filesystems that are not disk-based (a virtual memory–based filesystem, such as sysfs, for example) generate the superblock on-the-fly and store it in memory. The superblock object is represented by struct super_block and defined in <linux/fs.h> .
A superblock object is created and initialized via the alloc_super() function.When mounted, a filesystem invokes this function, reads its superblock off of the disk, and fills in its superblock object.

## Superblock Operations
The most important item in the superblock object is s_op , which is a pointer to the superblock operations table.The superblock operations table is represented by struct super_operations and is defined in <linux/fs.h> .
The superblock operations perform low-level operations on the filesystem and its inodes.
When a filesystem needs to perform an operation on its superblock, it follows the pointers from its superblock object to the desired method. For example, if a filesystem wanted to write to its superblock, it would invoke
```
sb->s_op->write_super(sb);
```
In this call, sb is a pointer to the filesystem’s superblock. Following that pointer into s_op yields the superblock operations table and ultimately the desired write_super() function, which is then invoked.

## The Inode Object
The inode object represents all the information needed by the kernel to manipulate a file or directory. For Unix-style filesystems, this information is simply read from the on-disk inode. If a filesystem does not have inodes, however, the filesystem must obtain the information from wherever it is stored on the disk. Filesystems without inodes generally store file-specific information as part of the file; unlike Unix-style filesystems, they do not separate file data from its control information.
The inode object is represented by struct inode and is defined in <linux/fs.h> .
An inode represents each file on a filesystem, but the inode object is constructed in memory only as files are accessed.This includes special files, such as device files or pipes.
Consequently, some of the entries in struct inode are related to these special files.

## Inode Operations

As with the superblock operations, the inode_operations member is important. It describes the filesystem’s implemented functions that the VFS can invoke on an inode.As with the superblock, inode operations are invoked via
```
i->i_op->truncate(i)
```

## The Dentry Object
The VFS often needs to perform directory-specific operations, such as path name lookup. Path name lookup involves translating each component of a path, ensuring it is valid, and following it to the next component.
To facilitate this, the VFS employs the concept of a directory entry (dentry).A dentry is a specific component in a path.
Dentry objects are all components in a path, including files. Resolving a path and walking its components is a nontrivial exercise, time-consuming and heavy on string operations, which are expensive to execute and cumbersome to code.The dentry object makes the whole process easier. Dentries might also include mount points. The VFS constructs dentry objects on the-fly, as needed, when performing directory operations. Dentry objects are represented by struct dentry and defined in <linux/dcache.h> .
Because the dentry object is not physically stored on the disk, no flag in struct dentry specifies whether the object is modified (that is, whether it is dirty and needs to be written back to disk).

### Dentry State

A valid dentry object can be in one of three states: used, unused, or negative.
* A used dentry corresponds to a valid inode ( d_inode points to an associated inode) and indicates that there are one or more users of the object ( d_count is positive).A used dentry is in use by the VFS and points to valid data and, thus, cannot be discarded.
* An unused dentry corresponds to a valid inode ( d_inode points to an inode), but the VFS is not currently using the dentry object ( d_count is zero). Because the dentry object still points to a valid object, the dentry is kept around—cached—in case it is needed again. Because the dentry has not been destroyed prematurely, the dentry need not be recreated if it is needed in the future, and path name lookups can complete quicker than if the dentry was not cached. If it is necessary to reclaim memory, however, the dentry can be discarded because it is not in active use.
* A negative dentry is not associated with a valid inode ( d_inode is NULL ) because either the inode was deleted or the path name was never correct to begin with.

### The Dentry Cache

After the VFS layer goes through the trouble of resolving each element in a path name into a dentry object and arriving at the end of the path, it would be quite wasteful to throw away all that work. Instead, the kernel caches dentry objects in the dentry cache or, simply, the dcache.
The dentry cache consists of three parts:
* Lists of “used” dentries linked off their associated inode via the i_dentry field of the inode object. Because a given inode can have multiple links, there might be multiple dentry objects; consequently, a list is used.
* A doubly linked “least recently used” list of unused and negative dentry objects.The list is inserted at the head, such that entries toward the head of the list are newer than entries toward the tail.When the kernel must remove entries to reclaim memory, the entries are removed from the tail; those are the oldest and presumably have the least chance of being used in the near future.
* A hash table and hashing function used to quickly resolve a given path into the associated dentry object.
Caching dentries and inodes is beneficial because file access exhibits both spatial and temporal locality. File access is temporal in that programs tend to access and reaccess the same files over and over.Thus when a file is accessed, there is a high probability that caching the associated dentries and inodes will result in a cache hit in the near future. File access is spatial in that programs tend to access multiple files in the same directory.Thus caching directories entries for one file have a high probability of a cache hit, as a related file is likely manipulated next.
## Dentry Operations
The dentry_operations structure specifies the methods that the VFS invokes on directory entries on a given filesystem.
The dentry_operations structure is defined in <linux/dcache.h>.

The methods are as follows:
* int d_revalidate(struct dentry *dentry, struct nameidata *) - Determines whether the given dentry object is valid.The VFS calls this function whenever it is preparing to use a dentry from the dcache. Most filesystems set this method to NULL because their dentry objects in the dcache are always valid.
* int d_hash(struct dentry *dentry, struct qstr *name) - Creates a hash value from the given dentry.The VFS calls this function whenever it adds a dentry to the hash table.
* int d_compare(struct dentry *dentry, struct qstr *name1, struct qstr *name2) - Called by the VFS to compare two filenames, name1 and name2 . Most filesystems leave this at the VFS default, which is a simple string compare. For some filesystems, such as FAT, a simple string compare is insufficient.The FAT filesystem is not casesensitive and therefore needs to implement a comparison function that disregards case.This function requires the dcache_lock .
* int d_delete (struct dentry *dentry) - Called by the VFS when the specified dentry object’s d_count reaches zero.This function requires the dcache_lock and the dentry’s d_lock .
* void d_release(struct dentry *dentry) - Called by the VFS when the specified dentry is going to be freed.The default function does nothing.
* void d_iput(struct dentry *dentry, struct inode *inode) - Called by the VFS when a dentry object loses its associated inode (say, because the entry was deleted from the disk). By default, the VFS simply calls the iput() function to release the inode. If a filesystem overrides this function, it must also call iput() in addition to performing whatever filesystem-specific work it requires.

## The File Object

The file object is used to represent a file opened by a process.When we think of the VFS from the perspective of user-space, the file object is what readily comes to mind. Processes deal directly with files, not superblocks, inodes, or dentries. It is not surprising that the information in the file object is the most familiar (data such as access mode and current offset) or that the file operations are familiar system calls such as read() and write() .
The file object is the in-memory representation of an open file.The object (but not the physical file) is created in response to the open() system call and destroyed in response to the close() system call.All these file-related calls are actually methods defined in the file operations table. Because multiple processes can open and manipulate a file at the same time, there can be multiple file objects in existence for the same file.The file object merely represents a process’s view of an open file.The object points back to the dentry (which in turn points back to the inode) that actually represents the open file.The inode and dentry objects, of course, are unique.
The file object is represented by struct file and is defined in <linux/fs.h>
## File Operations
The file object methods are specified in file_operations and defined in <linux/fs.h>.

Functions:
```
loff_t llseek(struct file *file, loff_t offset, int origin)
// Updates the file pointer to the given offset. It is called via the llseek() system call.
ssize_t read(struct file *file, char *buf, size_t count, loff_t *offset)
// Reads count bytes from the given file at position offset into buf .The file pointer is then updated.This function is called by the read() system call.
ssize_t aio_read(struct kiocb *iocb, char *buf, size_t count, loff_t offset)
// Begins an asynchronous read of count bytes into buf of the file described in iocb . This function is called by the aio_read() system call.
ssize_t write(struct file *file, const char *buf, size_t count, loff_t *offset)
// Writes count bytes from buf into the given file at position offset .The file pointer is then updated.This function is called by the write() system call.
ssize_t aio_write(struct kiocb *iocb, const char *buf, size_t count, loff_t offset)
// Begins an asynchronous write of count bytes into buf of the file described in iocb . This function is called by the aio_write() system call.
int readdir(struct file *file, void *dirent, filldir_t filldir)
//Returns the next directory in a directory listing.This function is called by the readdir() system call.
unsigned int poll(struct file *file, struct poll_table_struct *poll_table)
// Sleeps, waiting for activity on the given file. It is called by the poll() system call.
int ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
// Sends a command and argument pair to a device. It is used when the file is an open device node.This function is called from the ioctl() system call. Callers must hold the BKL.
int unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
// Implements the same functionality as ioctl() but without needing to hold the BKL.The VFS calls unlocked_ioctl() if it exists in lieu of ioctl() when user- space invokes the ioctl() system call.Thus filesystems need implement only one, preferably unlocked_ioctl() .
int compat_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
// Implements a portable variant of ioctl() for use on 64-bit systems by 32-bit applications.This function is designed to be 32-bit safe even on 64-bit architectures, performing any necessary size conversions. New drivers should design their ioctl commands such that all are portable, and thus enable compat_ioctl() and unlocked_ioctl() to point to the same function. Like unlocked_ioctl() , compat_ioctl() does not hold the BKL.
int mmap(struct file *file, struct vm_area_struct *vma)
// Memory maps the given file onto the given address space and is called by the mmap() system call.
int open(struct inode *inode, struct file *file)
// Creates a new file object and links it to the corresponding inode object. It is called by the open() system call.
int flush(struct file *file)
// Called by the VFS whenever the reference count of an open file decreases. Its purpose is filesystem-dependent.
int release(struct inode *inode, struct file *file)
//Called by the VFS when the last remaining reference to the file is destroyed—for example, when the last process sharing a file descriptor calls close() or exits. Its purpose is filesystem-dependent.
int fsync(struct file *file, struct dentry *dentry, int datasync)
// Called by the fsync() system call to write all cached data for the file to disk.
int aio_fsync(struct kiocb *iocb, int datasync)
// Called by the aio_fsync() system call to write all cached data for the file associated with iocb to disk.
int fasync(int fd, struct file *file, int on)
// Enables or disables signal notification of asynchronous I/O.
int lock(struct file *file, int cmd, struct file_lock *lock)
//Manipulates a file lock on the given file.
ssize_t readv(struct file *file, const struct iovec *vector, unsigned long count, loff_t *offset)
//Called by the readv() system call to read from the given file and put the results into the count buffers described by vector .The file offset is then incremented.
ssize_t writev(struct file *file, const struct iovec *vector, unsigned long count, loff_t *offset)
// Called by the writev() system call to write from the count buffers described by vector into the file specified by file .The file offset is then incremented.
ssize_t sendfile(struct file *file, loff_t *offset,size_t size, read_actor_t actor, void *target)
//Called by the sendfile() system call to copy data from one file to another. It performs the copy entirely in the kernel and avoids an extraneous copy to user-space.
ssize_t sendpage(struct file *file, struct page *page, int offset, size_t size, loff_t *pos, int more)
// Used to send data from one file to another.
unsigned long get_unmapped_area(struct file *file, unsigned long addr, unsigned long len, unsigned long offset, unsigned long flags)
// Gets unused address space to map the given file.
int check_flags(int flags)
// Used to check the validity of the flags passed to the fcntl() system call when the SETFL command is given.As with many VFS operations, filesystems need not implement check_flags() ; currently, only NFS does so.This function enables filesystems to restrict invalid SETFL flags otherwise enabled by the generic fcntl() function. In the case of NFS, combining O_APPEND and O_DIRECT is not enabled.
int flock(struct file *filp, int cmd, struct file_lock *fl)
// Used to implement the flock() system call, which provides advisory locking.
```
## Data Structures Associated with Filesystems
Because Linux supports so many different filesystems, the kernel must have a special structure for describing the capabilities and behavior of each filesystem.The file_system_type structure is defined in <linux/fs.h>.

#### Standard Mount Flags
Flag - Description
* MNT_NOSUID - Forbids setuid and setgid flags on binaries on this filesystem
* MNT_NODEV - Forbids access to device files on this filesystem
* MNT_NOEXEC - Forbids execution of binaries on this filesystem
These flags are most useful on removable devices that the administrator does not trust. They are defined in <linux/mount.h> along with other, lesser used, flags.
## Data Structures Associated with a Process
Each process on the system has its own list of open files, root filesystem, current working directory, mount points, and so on.Three data structures tie together the VFS layer and the processes on the system: files_struct , fs_struct , and namespace .The files_struct is defined in <linux/fdtable.h> .This table’s address is pointed to by the files entry in the processor descriptor.All per-process information about open files and file descriptors is contained therein.
The third and final structure is the namespace structure, which is defined in <linux/mnt_namespace.h > and pointed at by the mnt_namespace field in the process descriptor.
By default, all processes share the same namespace. (That is, they all see the same filesystem hierarchy from the same mount table.) Only when the CLONE_NEWNS flag is specified during clone() is the process given a unique copy of the namespace structure. Because most processes do not provide this flag, all the processes inherit their parents’ namespaces. Consequently, on many systems there is only one namespace, although the functionality is but a single CLONE_NEWNS flag away.
