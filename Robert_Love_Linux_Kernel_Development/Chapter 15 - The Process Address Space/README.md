# The Process Address Space
This memory is called the process address space, which is the representation of memory given to each user-space process on the system. Linux is a virtual memory operating system, and thus the resource of memory is virtualized among the processes on the system.An individual process’s view of memory is as if it alone has full access to the system’s physical memory. More important, the address space of even a single process can be much larger than physical memory.This chapter discusses how the kernel manages the process address space.

## Address Spaces
The process address space consists of the virtual memory addressable by a process and the addresses within the virtual memory that the process is allowed to use. Each process is given a flat 32- or 64-bit address space, with the size depending on the architecture.The term flat denotes that the address space exists in a single range. (For example, a 32-bit address space extends from the address 0 to 4294967295.) Some operating systems provide a segmented address space, with addresses existing not in a single linear range, but instead in multiple segments. Modern virtual memory operating systems generally have a flat memory model and not a segmented one. Normally, this flat address space is unique to each process.A memory address in one process’s address space is completely unrelated to that same memory address in another process’s address space. Both processes can have different data at the same address in their respective address spaces.Alternatively, processes can elect to share their address space with other processes.We know these processes as threads.
A memory address is a given value within the address space, such as 4021f000 .This particular value identifies a specific byte in a process’s 32-bit address space.Although a process can address up to 4GB of memory (with a 32-bit address space), it doesn’t have permission to access all of it.The interesting part of the address space is the intervals of memory addresses, such as 08048000-0804c000 , that the process has permission to access.
These intervals of legal addresses are called memory areas.The process, through the kernel, can dynamically add and remove memory areas to its address space.
The process can access a memory address only in a valid memory area. Memory areas have associated permissions, such as readable, writable, and executable, that the associated process must respect. If a process accesses a memory address not in a valid memory area, or if it accesses a valid area in an invalid manner, the kernel kills the process with the dreaded “Segmentation Fault” message.
All valid addresses in the process address space exist in exactly one area; memory areas do not overlap.As you can see, there is a separate memory area for each different chunk of memory in a running process: the stack, object code, global variables, mapped file, and so on.

## The Memory Descriptor
The kernel represents a process’s address space with a data structure called the memory descriptor.This structure contains all the information related to the process address space. The memory descriptor is represented by struct mm_struct and defined in <linux/mm_types.h >.


### Allocating a Memory Descriptor
The memory descriptor associated with a given task is stored in the mm field of the task’s process descriptor. (The process descriptor is represented by the task_struct structure, defined in <linux/sched.h>. )
Processes may elect to share their address spaces with their children by means of the CLONE_VM flag to clone() .The process is then called a thread. Threads are regular processes to the kernel that merely share certain resources.
### Destroying a Memory Descriptor

When the process associated with a specific address space exits, the exit_mm() , defined in kernel/exit.c , function is invoked.This function performs some housekeeping and updates some statistics. It then calls mmput() , which decrements the memory descriptor’s mm_users user counter. If the user count reaches zero, mmdrop() is called to decrement the mm_count usage counter. If that counter is finally zero, the free_mm() macro is invoked to return the mm_struct to the mm_cachep slab cache via kmem_cache_free() , because the memory descriptor does not have any users.

### The mm_struct and Kernel Threads
Kernel threads do not have a process address space and therefore do not have an associated memory descriptor.Thus, the mm field of a kernel thread’s process descriptor is NULL .This is the definition of a kernel thread—processes that have no user context. This lack of an address space is fine because kernel threads do not ever access any userspace memory.Because kernel threads do not have any pages in user-space, they do not deserve their own memory descriptor and page tables.Despite this, kernel threads need some of the data, such as the page tables, even to access kernel memory.To provide kernel threads the needed data, without wasting memory on a memory descriptor and page tables, or wasting processor cycles to switch to a new address space whenever a kernel thread begins running, kernel threads use the memory descriptor of whatever task ran previously.

## Virtual Memory Areas
The memory area structure, vm_area_struct, represents memory areas. It is defined in <linux/mm_types.h> . In the Linux kernel, memory areas are often called virtual memory areas (abbreviated VMAs).The vm_area_struct structure describes a single memory area over a contiguous interval in a given address space.The kernel treats each memory area as a unique memory object. Each memory area possesses certain properties, such as permissions and a set of associated operations.

### VMA Flags
The vm_flags field contains bit flags, defined in <linux/mm.h> , that specify the behavior of and provide information about the pages contained in the memory area. Unlike permissions associated with a specific physical page, the VMA flags specify behavior for which the kernel is responsible, not the hardware. Furthermore, vm_flags contains information that relates to each page in the memory area, or the memory area as a whole, and not specific individual pages.
##### vm_flags
Flag - Effect on the VMA and Its Pages
* VM_READ - Pages can be read from.
* VM_WRITE - Pages can be written to.
* VM_EXEC - Pages can be executed.
* VM_SHARED - Pages are shared.
* VM_MAYREAD - The VM_READ flag can be set.
* VM_MAYWRITE - The VM_WRITE flag can be set.
* VM_MAYEXEC - The VM_EXEC flag can be set.
* VM_MAYSHARE - The VM_SHARE flag can be set.
* VM_GROWSDOWN - The area can grow downward.
* VM_GROWSUP - The area can grow upward.
* VM_SHM - The area is used for shared memory.
* VM_DENYWRITE - The area maps an unwritable file.
* VM_EXECUTABLE - The area maps an executable file.
* VM_LOCKED - The pages in this area are locked.
* VM_IO - The area maps a device’s I/O space.
* VM_SEQ_READ - The pages seem to be accessed sequentially.
* VM_RAND_READ - The pages seem to be accessed randomly.
* VM_DONTCOPY - This area must not be copied on fork().
* VM_DONTEXPAND - This area cannot grow via mremap().
* VM_RESERVED - This area must not be swapped out.
* VM_ACCOUNT - This area is an accounted VM object.
* VM_HUGETLB - This area uses hugetlb pages.
* VM_NONLINEAR - This area is a nonlinear mapping.



### VMA Operations
The vm_ops field in the vm_area_struct structure points to the table of operations associated with a given memory area, which the kernel can invoke to manipulate the VMA.
The vm_area_struct acts as a generic object for representing any type of memory area, and the operations table describes the specific methods that can operate on this particular instance of the object.
The operations table is represented by struct vm_operations_struct and is defined in <linux/mm.h>
Here’s a description for each individual method:
* void open(struct vm_area_struct *area) - This function is invoked when the given memory area is added to an address space.
* void close(struct vm_area_struct *area) - This function is invoked when the given memory area is removed from an address space.
* int fault(struct vm_area_sruct *area, struct vm_fault *vmf) - This function is invoked by the page fault handler when a page that is not present in physical memory is accessed.
* int page_mkwrite(struct vm_area_sruct *area, struct vm_fault *vmf) - This function is invoked by the page fault handler when a page that was read-only is being made writable.
* int access(struct vm_area_struct *vma, unsigned long address, void *buf, int len, int write) - This function is invoked by access_process_vm() when get_user_pages() fails.

### Lists and Trees of Memory Areas

As discussed, memory areas are accessed via both the mmap and the mm_rb fields of the memory descriptor.These two data structures independently point to all the memory area objects associated with the memory descriptor. In fact, they both contain pointers to the same vm_area_struct structures, merely represented in different ways. The first field, mmap , links together all the memory area objects in a singly linked list.
Each vm_area_struct structure is linked into the list via its vm_next field.The areas are sorted by ascending address.The first memory area is the vm_area_struct structure to which mmap points.The last structure points to NULL .
The second field, mm_rb , links together all the memory area objects in a red-black tree.
The root of the red-black tree is mm_rb , and each vm_area_struct structure in this address space is linked to the tree via its vm_rb field.

### Memory Areas in Real Life
Let’s look at a particular process’s address space and the memory areas inside.This task uses the useful /proc filesystem and the pmap (1) utility.
The data is in the form:
```
start-end permission offset major:minor inode file
```
Note the memory areas without a mapped file on device 00:00 and inode zero.This is the zero page, which is a mapping that consists of all zeros. By mapping the zero page over a writable memory area, the area is in effect “initialized” to all zeros.This is important in that it provides a zeroed memory area, which is expected by the bss. Because the mapping is not shared, as soon as the process writes to this data, a copy is made (à la copy-on-write) and the value updated from zero. Each of the memory areas associated with the process corresponds to a vm_area_struct structure. Because the process was not a thread, it has a unique mm_struct structure referenced from its task_struct .


## Manipulating Memory Areas
The kernel often has to perform operations on a memory area, such as whether a given address exists in a given VMA.These operations are frequent and form the basis of the mmap() routine, which is covered in the next section.A handful of helper functions are defined to assist these jobs.
These functions are all declared in <linux/mm.h> .

### find_vma()

The kernel provides a function, find_vma() , for searching for the VMA in which a given memory address resides. It is defined in mm/mmap.c : 
```
struct vm_area_struct * find_vma(struct mm_struct *mm, unsigned long addr);
```
This function searches the given address space for the first memory area whose vm_end field is greater than addr . In other words, this function finds the first memory area that contains addr or begins at an address greater than addr . If no such memory area exists, the function returns NULL . Otherwise, a pointer to the vm_area_struct structure is returned.

### find_vma_prev()
The find_vma_prev() function works the same as find_vma() , but it also returns the last VMA before addr .The function is also defined in mm/mmap.c and declared in <linux/mm.h> :
```
struct vm_area_struct * find_vma_prev(struct mm_struct *mm, unsigned long addr, struct vm_area_struct **pprev)
```
The pprev argument stores a pointer to the VMA preceding addr .

### find_vma_intersection()
The find_vma_intersection() function returns the first VMA that overlaps a given address interval.The function is defined in <linux/mm.h> because it is inline:
```
static inline struct vm_area_struct *
find_vma_intersection(struct mm_struct *mm,
unsigned long start_addr,
unsigned long end_addr)
{
struct vm_area_struct *vma;
vma = find_vma(mm, start_addr);
if (vma && end_addr <= vma->vm_start)
vma = NULL;
return vma;
}
```
### mmap() and do_mmap(): Creating an Address Interval

The do_mmap() function is used by the kernel to create a new linear address interval. Saying that this function creates a new VMA is not technically correct, because if the created address interval is adjacent to an existing address interval, and if they share the same pemissions, the two intervals are merged into one. If this is not possible, a new VMA is created. In any case, do_mmap() is the function used to add an address interval to a process’s address space—whether that means expanding an existing memory area or creating a new one.
The do_mmap() function is declared in <linux/mm.h> :
```
unsigned long do_mmap(struct file *file, unsigned long addr,
unsigned long len, unsigned long prot,
unsigned long flag, unsigned long offset)
```
This function maps the file specified by file at offset offset for length len .The parameter can be NULL and offset can be zero, in which case the mapping will not be backed by a file. In that case, this is called an anonymous mapping. If a file and offset are provided, the mapping is called a file-backed mapping.
The addr function optionally specifies the initial address from which to start the search for a free interval.
The prot parameter specifies the access permissions for pages in the memory area.The possible permission flags are defined in <asm/mman.h> and are unique to each supported architecture

### munmap() and do_munmap(): Removing an Address Interval
The do_munmap() function removes an address interval from a specified process address space.The function is declared in <linux/mm.h> :
```
int do_munmap(struct mm_struct *mm, unsigned long start, size_t len)
```
The munmap() system call is exported to user-space as a means to enable processes to remove address intervals from their address space; it is the complement of the mmap() system call:
```
int munmap(void *start, size_t length)
```
The system call is defined in mm/mmap.c and acts as a simple wrapper to do_munmap().

## Page Tables

Although applications operate on virtual memory mapped to physical addresses, processors operate directly on those physical addresses. Consequently, when an application accesses a virtual memory address, it must first be converted to a physical address before the processor can resolve the request. Performing this lookup is done via page tables. Page tables work by splitting the virtual address into chunks. Each chunk is used as an index into a table.The table points to either another table or the associated physical page.

