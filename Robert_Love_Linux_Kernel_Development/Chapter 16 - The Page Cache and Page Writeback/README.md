# The Page Cache and Page Writeback

The Linux kernel implements a disk cache called the page cache. The goal of this cache is to minimize disk I/O by storing data in physical memory that would otherwise require disk access.This chapter deals with the page cache and the process by which changes to the page cache are propagated back to disk, which is called page writeback.
Two factors comingle to make disk caches a critical component of any modern operating system. First, disk access is several orders of magnitude slower than memory access— milliseconds versus nanoseconds.Accessing data from memory rather than the disk is much faster, and accessing data from the processor’s L1 or L2 cache is faster still. Second, data accessed once will, with a high likelihood, find itself accessed again in the near future.This principle—that access to a particular piece of data tends to be clustered in time—is called temporal locality, which ensures that if data is cached on its first access, there is a high probability of a cache hit (access to data in the cache) in the near future. Given that memory is so much faster than disk, coupled with the fact that once-used is likely twice-used data, an in-memory cache of the disk is a large performance win.

## Approaches to Caching

The page cache consists of physical pages in RAM, the contents of which correspond to physical blocks on a disk.The size of the page cache is dynamic; it can grow to consume any free memory and shrink to relieve memory pressure.We call the storage device being cached the backing store because the disk stands behind the cache as the source of the canonical version of any cached data.Whenever the kernel begins a read operation—for example, when a process issues the read() system call—it first checks if the requisite data is in the page cache. If it is, the kernel can forgo accessing the disk and read the data directly out of RAM.This is called a cache hit. If the data is not in the cache, called a cache miss, the kernel must schedule block I/O operations to read the data off the disk.After the data is read off the disk, the kernel populates the page cache with the data so that any subsequent reads can occur out of the cache. Entire files need not be cached; the page cache can hold some files in their entirety while storing only a page or two of other files. What is cached depends on what has been accessed.

### Write Caching

In the first strategy, called nowrite, the cache simply does not cache write operations.A write operation against a piece of data stored in the cache would be written directly to disk, invalidating the cached data and requiring it to be read from disk again on any subsequent read. Caches rarely employ this strategy because it not only fails to cache write operations, but it also makes them costly by invalidating the cache.
In the second strategy, a write operation would automatically update both the in-memory cache and the on-disk file.This approach is called a write-through cache because write operations immediately go through the cache to the disk.This approach has the benefit of keeping the cache coherent—synchronized and valid for the backing store—without needing to invalidate it. It is also simple.
The third strategy, employed by Linux, is called write-back.In a write-back cache, processes perform write operations directly into the page cache.The backing store is not immediately or directly updated. Instead, the written-to pages in the page cache are marked as dirty and are added to a dirty list. Periodically, pages in the dirty list are written back to disk in a process called writeback, bringing the on-disk copy in line with the in-memory cache.The pages are then marked as no longer dirty.The term “dirty” can be confusing because what is actually dirty is not the data in the page cache (which is up to date) but the data on disk (which is out of date).A better term would be unsynchronized. Nonetheless, we say the cache contents, not the invalid disk contents, are dirty.A write- back is generally considered superior to a write-through strategy because by deferring the writes to disk, they can be coalesced and performed in bulk at a later time.The downside is complexity.

### Cache Eviction

The final piece to caching is the process by which data is removed from the cache, either to make room for more relevant cache entries or to shrink the cache to make available more RAM for other uses.This process, and the strategy that decides what to remove, is called cache eviction. Linux’s cache eviction works by selecting clean (not dirty) pages and simply replacing them with something else. If insufficient clean pages are in the cache, the kernel forces a writeback to make more clean pages available.The hard part is deciding what to evict.The ideal eviction strategy evicts the pages least likely to be used in the future. Of course, knowing what pages are least likely to be accessed requires knowing the future, which is why this hopeful strategy is often referred to as the clairvoyant algorithm. Such a strategy is ideal, but impossible to implement.

#### Least Recently Used

Cache eviction strategies attempt to approximate the clairvoyant algorithm with what information they have access to. One of the more successful algorithms, particularly for general-purpose page caches, is called least recently used, or LRU.An LRU eviction strategy requires keeping track of when each page is accessed (or at least sorting a list of pages by access time) and evicting the pages with the oldest timestamp (or at the start of the sorted list).This strategy works well because the longer a piece of cached data sits idle, the less likely it is to be accessed in the near future. Least recently used is a great approximation of most likely to be used.

####  The Two-List Strategy

Linux, therefore, implements a modified version of LRU, called the two-list strategy. Instead of maintaining one list, the LRU list, Linux keeps two lists: the active list and the inactive list. Pages on the active list are considered “hot” and are not available for eviction. Pages on the inactive list are available for cache eviction. Pages are placed on the active list only when they are accessed while already residing on the inactive list. Both lists are maintained in a pseudo-LRU manner: Items are added to the tail and removed from the head, as with a queue.The lists are kept in balance: If the active list becomes much larger than the inactive list, items from the active list’s head are moved back to the inactive list, making them available for eviction.The two-list strategy solves the only-used-once failure in a classic LRU and also enables simpler, pseudo-LRU semantics to perform well.This two-list approach is also known as LRU/2; it can be generalized to n-lists, called LRU/n.

## The Linux Page Cache

The page cache, as its name suggests, is a cache of pages in RAM.The pages originate from reads and writes of regular filesystem files, block device files, and memory-mapped files. In this manner, the page cache contains chunks of recently accessed files. During a page I/O operation, such as read() , the kernel checks whether the data resides in the page cache. If the data is in the page cache, the kernel can quickly return the requested page from memory rather than read the data off the comparatively slow disk.

### The address_space Object

A page in the page cache can consist of multiple noncontiguous physical disk blocks. Checking the page cache to see whether certain data has been cached is made difficult because of this noncontiguous layout of the blocks that constitute each page.Therefore, it is not possible to index the data in the page cache using only a device name and block number, which would otherwise be the simplest solution.
Furthermore, the Linux page cache is quite general in what pages it can cache. Indeed, the original page cache introduced in System V Release 4 cached only filesystem data. Consequently, the SVR4 page cache used its equivalent of the inode object, called struct vnode , to manage the page cache.The Linux page cache aims to cache any page based object, which includes many forms of files and memory mappings.
To maintain a generic page cache—one not tied to physical files or the inode structure—the Linux page cache uses a new object to manage entries in the cache and page I/O operations.That object is the address_space structure.Think of address_space as the physical analogue to the virtual vm_area_struct.
The address_space structure is defined in <linux/fs.h>.The address_space is associated with some kernel object. Normally, this is an inode. If so, the host field points to the associated inode.The host field is NULL if the associated object is not an inode—for example, if the address_space is associated with the swapper.

### address_space Operations

The a_ops field points to the address space operations table, in the same manner as the VFS objects and their operations tables.The operations table is represented by struct address_space_operations and is also defined in <linux/fs.h>. 

### Radix Tree

Because the kernel must check for the existence of a page in the page cache before initiating any page I/O, such a check must be quick. Otherwise, the overhead of searching and checking the page cache could nullify any benefits the cache might provide.
Each address_space has a unique radix tree stored as page_tree .A radix tree is a type of binary tree.The radix tree enables quick searching for the desired page, given only the file offset. Page cache searching functions such as find_get_page() call radix_tree_lookup() , which performs a search on the given tree for the given object.
The core radix tree code is available in generic form in lib/radix-tree.c . Users of the radix tree need to include <linux/radix-tree.h> .

### The Old Page Hash Table

The global hash had four primary problems:
* A single global lock protected the hash. Lock contention was quite high on even moderately sized machines, and performance suffered as a result.
* The hash was larger than necessary because it contained all the pages in the page cache, whereas only pages pertaining to the current file were relevant.
* Performance when the hash lookup failed (that is, the given page was not in the page cache) was slower than desired, particularly because it was necessary to walk the chains off of a given hash value.
* The hash consumed more memory than other possible solutions.
The introduction of the radix tree-based page cache in 2.6 solved these issues.

## The Buffer Cache

Individual disk blocks also tie into the page cache, by way of block I/O buffers. A buffer is the in-memory representation of a single physical disk block. Buffers act as descriptors that map pages in memory to disk blocks; thus, the page cache also reduces disk access during block I/O operations by both caching disk blocks and buffering block I/O operations until later.This caching is often referred to as the buffer cache, although as implemented it is not a separate cache but is part of the page cache.
Block I/O operations manipulate a single disk block at a time.A common block I/O operation is reading and writing inodes.The kernel provides the bread() function to perform a low-level read of a single block from disk.Via buffers, disk blocks are mapped to their associated in-memory pages and cached in the page cache.
Today, we have one disk cache: the page cache.The kernel still needs to use buffers, however, to represent disk blocks in memory. Conveniently, the buffers describe the mapping of a block onto a page, which is in the page cache.

## The Flusher Threads

Write operations are deferred in the page cache.When data in the page cache is newer than the data on the backing store, we call that data dirty. Dirty pages that accumulate in memory eventually need to be written back to disk. Dirty page writeback occurs in three situations:
* When free memory shrinks below a specified threshold, the kernel writes dirty data back to disk to free memory because only clean (nondirty) memory is available for eviction.When clean, the kernel can evict the data from the cache and then shrink the cache, freeing up more memory.
* When dirty data grows older than a specific threshold, sufficiently old data is written back to disk to ensure that dirty data does not remain dirty indefinitely.
* When a user process invokes the sync() and fsync() system calls, the kernel performs writeback on demand.

First, the flusher threads need to flush dirty data back to disk when the amount of free memory in the system shrinks below a specified level.The goal of this background write- back is to regain memory consumed by dirty pages when available physical memory is low. The memory level at which this process begins is configured by the dirty_background_ratio sysctl.When free memory drops below this threshold, the kernel invokes the wakeup_flusher_threads() call to wake up one or more flusher threads and have them run the bdi_writeback_all () function to begin writeback of dirty pages.This function takes as a parameter the number of pages to attempt to write back.
The function continues writing out data until two conditions are true:
* The specified minimum number of pages has been written out.
* The amount of free memory is above the dirty_background_ratio threshold.
These conditions ensure that the flusher threads do their part to relieve low-memory conditions.Writeback stops prior to these conditions only if the flusher threads write back all the dirty pages and there is nothing left to do.
For its second goal, a flusher thread periodically wakes up (unrelated to low-memory conditions) and writes out old dirty pages.This is performed to ensure that no dirty pages remain in memory indefinitely. During a system failure, because memory is volatile, dirty pages in memory that have not been written to disk are lost

### Laptop Mode

Laptop mode is a special page writeback strategy intended to optimize battery life by minimizing hard disk activity and enabling hard drives to remain spun down as long as possible. It is configurable via /proc/sys/vm/laptop_mode . By default, this file contains a zero and laptop mode is disabled.Writing a one to this file enables laptop mode.
Laptop mode makes a single change to page writeback behavior. In addition to performing writeback of dirty pages when they grow too old, the flusher threads also piggyback off any other physical disk I/O, flushing all dirty buffers to disk. In this manner, page writeback takes advantage that the disk was just spun up, ensuring that it will not cause the disk to spin up later. 

### History: bdflush, kupdated, and pdflush

Prior to the 2.6 kernel, the job of the flusher threads was met by two other kernel threads, bdflush and kupdated.
The bdflush kernel thread performed background writeback of dirty pages when available memory was low.A set of thresholds was maintained, similar to the flusher threads’, and bdflush was awakened via wakeup_bdflush() whenever free memory dropped below those thresholds.
Two main differences distinguish bdflush and the current flusher threads.The first, which is discussed in the next section, is that there was always only one bdflush daemon, whereas the number of flusher threads is a function of the number of disk spindles.The second difference is that bdflush was buffer-based; it wrote back dirty buffers. Conversely, the flusher threads are page-based; they write back whole pages. Of course, the pages can  correspond to buffers, but the actual I/O unit is a full page and not a single buffer.This is beneficial as managing pages is easier than managing buffers because pages are a more general and common unit.
Because bdflush flushes buffers only when memory is low or the number of buffers is too large, the kupdated thread was introduced to periodically write back dirty pages. It served an identical purpose to the wb_writeback() function.

### Avoiding Congestion with Multiple Threads

One of the major flaws in the bdflush solution was that bdflush consisted of one thread. This led to possible congestion during heavy page writeback where the single bdflush thread would block on a single congested device queue (the list of I/O requests waiting to submit to disk), whereas other device queues would sit relatively idle. If the system has multiple disks and the associated processing power, the kernel should keep each disk busy.
Unfortunately, even with plenty of data needing writeback, bdflush can become stuck handling a single queue and fail to keep all disks saturated.This occurs because the throughput of disks is a finite—and unfortunately comparatively small—number. If only a single thread is performing page writeback, that single thread can easily spend a long time waiting for a single disk because disk throughput is such a limiting quantity.To mitigate this, the kernel needs to multithread page writeback. In this manner, no single device queue can become a bottleneck.
In the face of heavy activity, the flusher threads can maintain high throughput across multiple disks.
