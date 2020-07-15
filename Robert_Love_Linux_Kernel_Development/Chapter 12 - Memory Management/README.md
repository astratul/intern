# Memory Management
Unlike user-space, the kernel is not always afforded the capability to easily allocate memory. For example, the kernel cannot easily deal with memory allocation errors, and the kernel often cannot sleep. Because of these limitations, and the need for a lightweight memory allocation scheme, getting hold of memory in the kernel is more complicated than in user-space. This is not to say that, from a programmer’s point of view, kernel memory allocations are difficult—just different. This chapter discusses the methods used to obtain memory inside the kernel. Before you can delve into the actual allocation interfaces, however, you need to understand how the kernel handles memory.

## Pages

The kernel treats physical pages as the basic unit of memory management.Although the processor’s smallest addressable unit is a byte or a word, the memory management unit (MMU, the hardware that manages memory and performs virtual to physical address translations) typically deals in pages.Therefore, the MMU maintains the system’s page tables with page-sized granularity (hence their name). In terms of virtual memory, pages are the smallest unit that matters.
The kernel represents every physical page on the system with a struct page structure. This structure is defined in <linux/mm_types.h> . I’ve simplified the definition, removing two confusing unions that do not help color our discussion of the basics:
```
struct page {
unsigned long flags; // stores the status of the page. The flag values are defined in <linux/page-flags.h> .
atomic_t _count; // stores the usage count of the page—that is, how many references there are to this page.
atomic_t _mapcount; // 
unsigned long private; // 
struct address_space *mapping; // 
pgoff_t index; // 
struct list_head lru; // 
void *virtual; // the page’s virtual address.
};
```
The important point to understand is that the page structure is associated with physical pages, not virtual pages.Therefore, what the structure describes is transient at best. Even if the data contained in the page continues to exist, it might not always be associated with the same page structure because of swapping and so on.The kernel uses this data structure to describe the associated physical page.The data structure’s goal is to describe physical memory, not the data contained therein. The kernel uses this structure to keep track of all the pages in the system, because the kernel needs to know whether a page is free (that is, if the page is not allocated). If a page is not free, the kernel needs to know who owns the page. Possible owners include user-space processes, dynamically allocated kernel data, static kernel code, the page cache, and so on.
Developers are often surprised that an instance of this structure is allocated for each physical page in the system.

## Zones
DMA (direct memory access)
Because of hardware limitations, the kernel cannot treat all pages as identical. Some pages, because of their physical address in memory, cannot be used for certain tasks. Because of this limitation, the kernel divides pages into different zones.The kernel uses the zones to group pages of similar properties. In particular, Linux has to deal with two shortcomings of hardware with respect to memory addressing:
* Some hardware devices can perform DMA (direct memory access) to only certain memory addresses.
* Some architectures can physically addressing larger amounts of memory than they can virtually address. Consequently, some memory is not permanently mapped into the kernel address space.
Because of these constraints, Linux has four primary memory zones:
* ZONE_DMA —This zone contains pages that can undergo DMA.
* ZONE_DMA32 —Like ZOME_DMA , this zone contains pages that can undergo DMA.
Unlike ZONE_DMA , these pages are accessible only by 32-bit devices. On some architectures, this zone is a larger subset of memory.
* ZONE_NORMAL —This zone contains normal, regularly mapped, pages.
* ZONE_HIGHMEM —This zone contains “high memory,” which are pages not permanently mapped into the kernel’s address space.The rest of the system’s memory is called low memory.

These zones, and two other, less notable ones, are defined in <linux/mmzone.h> .
The actual use and layout of the memory zones is architecture-dependent.

Zone - Description - Physical Memory
* ZONE_DMA - DMA-able pages -  < 16MB
* ZONE_NORMAL - Normally addressable pages -  16–896MB
* ZONE_HIGHMEM - Dynamically mapped pages -  > 896MB
Each zone is represented by struct zone , which is defined in <linux/mmzone.h>.
The lock field is a spin lock that protects the structure from concurrent access.
The watermark array holds the minimum, low, and high watermarks for this zone.The kernel uses watermarks to set benchmarks for suitable per-zone memory consumption, varying its aggressiveness as the watermarks vary vis-à-vis free memory.
The name field is, unsurprisingly, a NULL -terminated string representing the name of this zone.The kernel initializes this value during boot in mm/page_alloc.c , and the three zones are given the names DMA, Normal, and HighMem.

## Getting Pages
The kernel provides one low-level mechanism for requesting memory, along with several interfaces to access it.All these interfaces allocate memory with page-sized granularity and are declared in <linux/gfp.h> .The core function is 
```
struct page * alloc_pages(gfp_t gfp_mask, unsigned int order)
```
This allocates 2^order(that is, 1 << order) contiguous physical pages and returns a pointer to the first page’s page structure; on error it returns NULL .We look at the gfp_t type and gfp_mask parameter in a later section.You can convert a given page to its logical address with the function 
```
void * page_address(struct page *page)
```
This returns a pointer to the logical address where the given physical page currently resides. If you have no need for the actual struct page , you can call 
```
unsigned long __get_free_pages(gfp_t gfp_mask, unsigned int order)
```
This function works the same as alloc_pages() , except that it directly returns the logical address of the first requested page. Because the pages are contiguous, the other pages simply follow from the first.
If you need only one page, two functions are implemented as wrappers to save you a bit of typing:
```
struct page * alloc_page(gfp_t gfp_mask)
unsigned long __get_free_page(gfp_t gfp_mask)
```
These functions work the same as their brethren but pass zero for the order

### Getting Zeroed Pages
If you need the returned page filled with zeros, use the function 
```
unsigned long get_zeroed_page(unsigned int gfp_mask)
```
This function works the same as __get_free_page() , except that the allocated page is then zero-filled—every bit of every byte is unset.This is useful for pages given to userspace because the random garbage in an allocated page is not so random; it might contain sensitive data.All data must be zeroed or otherwise cleaned before it is returned to userspace to ensure system security is not compromised.

Flag - Description
* alloc_page(gfp_mask) - Allocates a single page and returns a pointer to its
* alloc_pages(gfp_mask, order) - Allocates 2 order pages and returns a pointer to the first page’s page structure
* __get_free_page(gfp_mask) - Allocates a single page and returns a pointer to its logical address
* __get_free_pages(gfp_mask) - Allocates 2 order pages and returns a pointer to the first page’s logical address
* get_zeroed_page(gfp_mask) - Allocates a single page, zero its contents and returns a pointer to its logical address

### Freeing Pages
A family of functions enables you to free allocated pages when you no longer need them:
```
void __free_pages(struct page *page, unsigned int order)
void free_pages(unsigned long addr, unsigned int order)
void free_page(unsigned long addr)
```

## kmalloc()
The kmalloc() function’s operation is similar to that of user-space’s familiar malloc() routine, with the exception of the additional flags parameter.The kmalloc() function is a simple interface for obtaining kernel memory in byte-sized chunks. If you need whole pages, the previously discussed interfaces might be a better choice. For most kernel allocations, however, kmalloc() is the preferred interface. The function is declared in <linux/slab.h> :
```
void * kmalloc(size_t size, gfp_t flags)
```
The function returns a pointer to a region of memory that is at least size bytes in length. 3 The region of memory allocated is physically contiguous. On error, it returns NULL . Kernel allocations always succeed, unless an insufficient amount of memory is available.Thus, you must check for NULL after all calls to kmalloc() and handle the error appropriately.
Let’s look at an example.Assume you need to dynamically allocate enough room for a fictional dog structure:
```
struct dog *p;
p = kmalloc(sizeof(struct dog), GFP_KERNEL);
if (!p)
/* handle error ... */
```
If the kmalloc() call succeeds, p now points to a block of memory that is at least the requested size.The GFP_KERNEL flag specifies the behavior of the memory allocator while trying to obtain the memory to return to the caller of kmalloc() .

### gfp_mask Flags
Flags are represented by the gfp_t type, which is defined in <linux/types.h> as an unsigned int . gfp stands for __get_free_pages() , one of the memory allocation routines we discussed earlier. The flags are broken up into three categories: action modifiers, zone modifiers, and types.Action modifiers specify how the kernel is supposed to allocate the requested memory.

The kernel divides physical memory into multiple zones, each of which serves a different purpose. Zone modifiers specify from which of these zones to allocate.Type flags specify a combination of action and zone modifiers as needed by a certain type of memory allocation.Type flags simplify the specification of multiple modifiers; instead of providing a combination of action and zone modifiers, you can specify just one type flag.The GFP_KERNEL is a type flag, which is used for code in process context inside the kernel. Let’s look at the flags.

#### Action Modifiers
All the flags, the action modifiers included, are declared in <linux/gfp.h> .The file <linux/slab.h> includes this header, however, so you often need not include it directly.
##### Action Modifiers
Flag - Description
* __GFP_WAIT - The allocator can sleep.
* __GFP_HIGH - The allocator can access emergency pools.
* __GFP_IO - The allocator can start disk I/O.
* __GFP_FS - The allocator can start filesystem I/O.
* __GFP_COLD - The allocator should use cache cold pages.
* __GFP_NOWARN - The allocator does not print failure warnings.
* __GFP_REPEAT - The allocator repeats the allocation if it fails, but the allocation can potentially fail.
* __GFP_NOFAIL - The allocator indefinitely repeats the allocation. The allocation cannot fail.
* __GFP_NORETRY - The allocator never retries if the allocation fails.
* __GFP_NOMEMALLOC - The allocator does not fall back on reserves.
* __GFP_HARDWALL - The allocator enforces “hardwall” cpuset boundaries.
* __GFP_RECLAIMABLE - The allocator marks the pages reclaimable.
* __GFP_COMP - The allocator adds compound page metadata (used internally by the hugetlb code).

These allocations can be specified together. For example
```
ptr = kmalloc(size, __GFP_WAIT | __GFP_IO | __GFP_FS);
```
This call instructs the page allocator (ultimately alloc_pages() ) that the allocation can block, perform I/O, and perform filesystem operations, if needed.This enables the kernel great freedom in how it can find the free memory to satisfy the allocation.
#### Zone Modifiers
Zone modifiers specify from which memory zone the allocation should originate. Normally, allocations can be fulfilled from any zone.The kernel prefers ZONE_NORMAL , however, to ensure that the other zones have free pages when they are needed. There are only three zone modifiers because there are only three zones other than ZONE_NORMAL (which is where, by default, allocations originate).

##### Zone Modifiers
Flag - Description
* __GFP_DMA - Allocates only from ZONE_DMA
* __GFP_DMA32 - Allocates only from ZONE_DMA32
* __GFP_HIGHMEM - Allocates from ZONE_HIGHMEM or ZONE_NORMAL
Only alloc_pages() can allocate high memory.The majority of your allocations, however, will not specify a zone modifier because ZONE_NORMAL is sufficient.

#### Type Flags
The type flags specify the required action and zone modifiers to fulfill a particular type of transaction.Therefore, kernel code tends to use the correct type flag and not specify the myriad of other flags it might need.This is both simpler and less error-prone.
##### Type Flags
Flag - Description
* GFP_ATOMIC - The allocation is high priority and must not sleep. This is the flag to use in interrupt handlers, in bottom halves, while holding a spinlock, and in other situations where you cannot sleep.
* GFP_NOWAIT  - Like GFP_ATOMIC , except that the call will not fallback on emergency memory pools. This increases the liklihood of the memory allocation failing.
* GFP_NOIO - This allocation can block, but must not initiate disk I/O. This is the flag to use in block I/O code when you cannot cause more disk I/O, which might lead to some unpleasant recursion.
* GFP_NOFS - This allocation can block and can initiate disk I/O, if it must, but it will not initiate a filesystem operation. This is the flag to use in filesystem code when you cannot start another filesystem operation.
* GFP_KERNEL - This is a normal allocation and might block. This is the flag to use in process context code when it is safe to sleep. The kernel will do whatever it has to do to obtain the memory requested by the caller. This flag should be your default choice.
* GFP_USER - This is a normal allocation and might block. This flag is used to allocate memory for user-space processes.
* GFP_HIGHUSER - This is an allocation from ZONE_HIGHMEM and might block. This flag is used to allocate memory for user-space processes.
* GFP_DMA - This is an allocation from ZONE_DMA . Device drivers that need DMA-able memory use this flag, usually in combination with one of the preceding flags.

##### Modifiers Behind Each Type Flag
Flag - Modifier Flags
* GFP_ATOMIC - __GFP_HIGH
* GFP_NOWAIT - 0
* GFP_NOIO - __GFP_WAIT
* GFP_NOFS - (__GFP_WAIT | __GFP_IO)
* GFP_KERNEL - (__GFP_WAIT | __GFP_IO | __GFP_FS)
* GFP_USER -  (__GFP_WAIT | __GFP_IO | __GFP_FS)
* GFP_HIGHUSER - (__GFP_WAIT | __GFP_IO | __GFP_FS | __GFP_HIGHMEM)
* GFP_DMA - __GFP_DMA
In the vast majority of the code that you write, you use either GFP_KERNEL or GFP_ATOMIC.

##### Which Flag to Use When
* Situation  - Solution
* Process context, can sleep - Use GFP_KERNEL .
* Process context, cannot sleep - Use GFP_ATOMIC , or perform your allocations with GFP_KERNEL at an earlier or later point when you can sleep.
* Interrupt handler - Use GFP_ATOMIC .
* Softirq - Use GFP_ATOMIC .
* Tasklet - Use GFP_ATOMIC .
* Need DMA-able memory, can sleep - Use (GFP_DMA | GFP_KERNEL) .
* Need DMA-able memory, cannot sleep - Use (GFP_DMA | GFP_ATOMIC) , or perform your allocation at an earlier point when you can sleep.

### kfree()
The counterpart to kmalloc() is kfree() , which is declared in <linux/slab.h> :
```
void kfree(const void *ptr)
```
The kfree() method frees a block of memory previously allocated with kmalloc() . Do not call this function on memory not previously allocated with kmalloc(), or on memory that has already been freed. Doing so is a bug, resulting in bad behavior such as freeing memory belonging to another part of the kernel.As in user-space, be careful to balance your allocations with your deallocations to prevent memory leaks and other bugs. Note that calling kfree(NULL) is explicitly checked for and safe.

## vmalloc()

The vmalloc() function works in a similar fashion to kmalloc() , except it allocates memory that is only virtually contiguous and not necessarily physically contiguous.This is how a user-space allocation function works:The pages returned by malloc() are contiguous within the virtual address space of the processor, but there is no guarantee that they are actually contiguous in physical RAM.The kmalloc() function guarantees that the pages are physically contiguous (and virtually contiguous).The vmalloc() function ensures only that the pages are contiguous within the virtual address space. It does this by allocating potentially noncontiguous chunks of physical memory and “fixing up” the page tables to map the memory into a contiguous chunk of the logical address space.
The function returns a pointer to at least size bytes of virtually contiguous memory. On error, the function returns NULL .The function might sleep and thus cannot be called from interrupt context or other situations in which blocking is not permissible. To free an allocation obtained via vmalloc() , use
```
void vfree(const void *addr)
```
This function frees the block of memory beginning at addr that was previously allocated via vmalloc() .The function can also sleep and, thus, cannot be called from interrupt context. It has no return value.
Usage of these functions is simple:
```
char *buf;
buf = vmalloc(16 * PAGE_SIZE); /* get 16 pages */
if (!buf)
/* error! failed to allocate memory */
/*
* buf now points to at least a 16*PAGE_SIZE bytes
* of virtually contiguous block of memory
*/
// After you finish with the memory, make sure to free it by using
vfree(buf);
```

## Slab Layer
Allocating and freeing data structures is one of the most common operations inside any kernel.To facilitate frequent allocations and deallocations of data, programmers often introduce free lists.A free list contains a block of available, already allocated, data structures. When code requires a new instance of a data structure, it can grab one of the structures off the free list rather than allocate the sufficient amount of memory and set it up for the data structure. Later, when the data structure is no longer needed, it is returned to the free list instead of deallocated. In this sense, the free list acts as an object cache, caching a frequently used type of object.
One of the main problems with free lists in the kernel is that there exists no global control.When available memory is low, there is no way for the kernel to communicate to every free list that it should shrink the sizes of its cache to free up memory.The kernel has no understanding of the random free lists at all.To remedy this, and to consolidate code, the Linux kernel provides the slab layer (also called the slab allocator).The slab layer acts as a generic data structure-caching layer.

The slab layer attempts to leverage several basic tenets:
* Frequently used data structures tend to be allocated and freed often, so cache them.
* Frequent allocation and deallocation can result in memory fragmentation (the inability to find large contiguous chunks of available memory).To prevent this, the cached free lists are arranged contiguously. Because freed data structures return to the free list, there is no resulting fragmentation.
* The free list provides improved performance during frequent allocation and deallocation because a freed object can be immediately returned to the next allocation. 
* If the allocator is aware of concepts such as object size, page size, and total cache size, it can make more intelligent decisions.
* If part of the cache is made per-processor (separate and unique to each processor on the system), allocations and frees can be performed without an SMP lock.
* If the allocator is NUMA-aware, it can fulfill allocations from the same memory node as the requestor.
* Stored objects can be colored to prevent multiple objects from mapping to the same cache lines.

### Design of the Slab Layer

The slab layer divides different objects into groups called caches, each of which stores a different type of object.There is one cache per object type.
Interestingly, the kmalloc() interface is built on top of the slab layer, using a family of general purpose caches. The caches are then divided into slabs (hence the name of this subsystem).The slabs are composed of one or more physically contiguous pages.Typically, slabs are composed of only a single page. Each cache may consist of multiple slabs. Each slab contains some number of objects, which are the data structures being cached. Each slab is in one of three states: full, partial, or empty.A full slab has no free objects. (All objects in the slab are allocated.) An empty slab has no allocated objects. (All objects in the slab are free.) A partial slab has some allocated objects and some free objects.When some part of the kernel requests a new object, the request is satisfied from a partial slab, if one exists. Otherwise, the request is satisfied from an empty slab.
When the kernel requests a new inode structure, the kernel returns a pointer to an already allocated, but unused structure from a partial slab or, if there is no partial slab, an empty slab.When the kernel is done using the inode object, the slab allocator marks the object as free.
Each cache is represented by a kmem_cache structure.This structure contains three lists— slabs_full , slabs_partial , and slabs_empty —stored inside a kmem_list3 structure, which is defined in mm/slab.c .These lists contain all the slabs associated with the cache.
Slab descriptors are allocated either outside the slab in a general cache or inside the slab itself, at the beginning.The descriptor is stored inside the slab if the total size of the slab is sufficiently small, or if internal slack space is sufficient to hold the descriptor.



### Slab Allocator Interface
A new cache is created via
```
struct kmem_cache * kmem_cache_create(const char *name,size_t size, size_t align, unsigned long flags, void (*ctor)(void *));
```
#### Allocating from the Cache
After a cache is created, an object is obtained from the cache via
```
void * kmem_cache_alloc(struct kmem_cache *cachep, gfp_t flags)
```
#### Example of Using the Slab Allocator
First, the kernel has a global variable that stores a pointer to the task_struct cache:
```
struct kmem_cache *task_struct_cachep;
```


## Statically Allocating on the Stack

User-space is afforded the luxury of a large, dynamically growing stack, whereas the kernel has no such luxury—the kernel’s stack is small and fixed.When each process is given a small, fixed stack, memory consumption is minimized, and the kernel need not burden itself with stack management code.
### Single-Page Kernel Stacks
### Playing Fair on the Stack
## High Memory Mappings
By definition, pages in high memory might not be permanently mapped into the kernel’s address space.Thus, pages obtained via alloc_pages() with the __GFP_HIGHMEM flag might not have a logical address.

### Permanent Mappings
To map a given page structure into the kernel’s address space, use this function, declared in <linux/highmem.h> :
```
void *kmap(struct page *page)
```
This function works on either high or low memory. If the page structure belongs to a page in low memory, the page’s virtual address is simply returned. If the page resides in high memory, a permanent mapping is created and the address is returned.The function may sleep, so kmap() works only in process context. Because the number of permanent mappings are limited (if not, we would not be in this mess and could just permanently map all memory), high memory should be unmapped when no longer needed.This is done via the following function, which unmaps the given page :
```
void kunmap(struct page *page)
```
### Temporary Mappings
For times when a mapping must be created but the current context cannot sleep, the kernel provides temporary mappings (which are also called atomic mappings).These are a set of reserved mappings that can hold a temporary mapping.The kernel can atomically map a high memory page into one of these reserved mappings. Consequently, a temporary mapping can be used in places that cannot sleep, such as interrupt handlers, because obtaining the mapping never blocks.
Setting up a temporary mapping is done via
```
void *kmap_atomic(struct page *page, enum km_type type)
```
The mapping is undone via
```
void kunmap_atomic(void *kvaddr, enum km_type type)
```
## Per-CPU Allocations
Modern SMP-capable operating systems use per-CPU data—data that is unique to a given processor—extensively.Typically, per-CPU data is stored in an array. Each item in the array corresponds to a possible processor on the system.
You declare the data as
```
unsigned long my_percpu[NR_CPUS];
```
Then you access it as
```
int cpu;
cpu = get_cpu();
/* get current processor and disable kernel preemption */
my_percpu[cpu]++; /* ... or whatever */
printk(“my_percpu on cpu=%d is %lu\n”, cpu, my_percpu[cpu]);
put_cpu();
/* enable kernel preemption */
```
Kernel preemption is the only concern with per-CPU data. Kernel preemption poses two problems, listed here:
* If your code is preempted and reschedules on another processor, the cpu variable is no longer valid because it points to the wrong processor. (In general, code cannot sleep after obtaining the current processor.)
* If another task preempts your code, it can concurrently access my_percpu on the same processor, which is a race condition.
Any fears are unwarranted, however, because the call get_cpu() , on top of returning the current processor number, also disables kernel preemption.The corresponding call to put_cpu() enables kernel preemption. Note that if you use a call to smp_processor_id() to get the current processor number, kernel preemption is not disabled; always use the aforementioned methods to remain safe.

## The New percpu Interface
This new interface, however, grew out of the needs for a simpler and more powerful method for manipulating per-CPU data on large symmetrical multiprocessing computers.
The header <linux/percpu.h> declares all the routines.You can find the actual definitions there, in mm/slab.c , and in <asm/percpu.h> .

### Per-CPU Data at Compile-Time
Defining a per-CPU variable at compile time is quite easy:
```
DEFINE_PER_CPU(type, name);
```
This creates an instance of a variable of type type , named name , for each processor on the system. If you need a declaration of the variable elsewhere, to avoid compile warnings, the following macro is your friend:
```
DECLARE_PER_CPU(type, name);
```
### Per-CPU Data at Runtime
The kernel implements a dynamic allocator, similar to kmalloc() , for creating per-CPU data.This routine creates an instance of the requested memory for each processor on the systems.The prototypes are in <linux/percpu.h> :
```
void *alloc_percpu(type); /* a macro */
void *__alloc_percpu(size_t size, size_t align);
void free_percpu(const void *);
```
The alloc_percpu() macro allocates one instance of an object of the given type for every processor on the system. It is a wrapper around __alloc_percpu() , which takes the actual number of bytes to allocate as a parameter and the number of bytes on which to align the allocation.The alloc_percpu() macro aligns the allocation on a byte boundary that is the natural alignment of the given type.


## Reasons for Using Per-CPU Data
There are several benefits to using per-CPU data.The first is the reduction in locking requirements. Depending on the semantics by which processors access the per-CPU data, you might not need any locking at all. Keep in mind that the “only this processor accesses this data” rule is only a programming convention.You need to ensure that the local processor accesses only its unique data. Nothing stops you from cheating.
Second, per-CPU data greatly reduces cache invalidation.This occurs as processors try to keep their caches in sync. If one processor manipulates data held in another processor’s cache, that processor must flush or otherwise update its cache. Constant cache invalidation is called thrashing the cache and wreaks havoc on system performance.The use of per-CPU data keeps cache effects to a minimum because processors ideally access only their own data.The percpu interface cache-aligns all data to ensure that accessing one processor’s data does not bring in another processor’s data on the same cache line.
Consequently, the use of per-CPU data often removes (or at least minimizes) the need for locking.The only safety requirement for the use of per-CPU data is disabling kernel preemption, which is much cheaper than locking, and the interface does so automatically. Per-CPU data can safely be used from either interrupt or process context.

## Picking an Allocation Method
With myriad allocations methods and approaches, it is not always obvious how to get at memory in the kernel—but it sure is important! If you need contiguous physical pages, use one of the low-level page allocators or kmalloc() .This is the standard manner of allocating memory from within the kernel, and most likely, how you will allocate most of your memory. Recall that the two most common flags given to these functions are GFP_ATOMIC and GFP_KERNEL . Specify the GFP_ATOMIC flag to perform a high priority allocation that will not sleep.This is a requirement of interrupt handlers and other pieces of code that cannot sleep. Code that can sleep, such as process context code that does not hold a spin lock, should use GFP_KERNEL .This flag specifies an allocation that can sleep, if needed, to obtain the requested memory. If you want to allocate from high memory, use alloc_pages() .The alloc_pages() function returns a struct page and not a pointer to a logical address. Because high memory might not be mapped, the only way to access it might be via the corresponding struct page structure.To obtain an actual pointer, use kmap() to map the high memory into the kernel’s logical address space.
If you do not need physically contiguous pages—only virtually contiguous—use vmalloc() , although bear in mind the slight performance hit taken with vmalloc() over kmalloc() .The vmalloc() function allocates kernel memory that is virtually contiguous but not, per se, physically contiguous. It performs this feat much as user-space allocations do, by mapping chunks of physical memory into a contiguous logical address space.
If you are creating and destroying many large data structures, consider setting up a slab cache.The slab layer maintains a per-processor object cache (a free list), which might greatly enhance object allocation and deallocation performance. Rather than frequently allocate and free memory, the slab layer stores a cache of already allocated objects for you. When you need a new chunk of memory to hold your data structure, the slab layer often does not need to allocate more memory and instead simply can return an object from the cache.
