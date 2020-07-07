# Kernel Data Structures
Kernel developers should use these data structures whenever possible and not “roll your own”  solutions. In the following sections, we cover the most useful of these generic data structures, which are the following:
* Linked lists
* Queues
* Maps
* Binary trees
We conclude the chapter with a discussion on algorithmic complexity, the ease with which algorithms and data structures scale to support ever larger inputs.

## Linked Lists
A linked list is a data structure that allows the storage and manipulation of a variable number of elements, called the nodes of the list. Unlike in a static array, the elements in a linked list are dynamically created and inserted into the list.This enables the management of a varying number of elements unknown at compile time. Because the elements are created at different times, they do not necessarily occupy contiguous regions in memory.Therefore, the elements need to be linked together; thus each element in the list contains a pointer to the next element.As elements are added to or removed from the list, the pointer to the next node is simply adjusted.

### Singly and Doubly Linked Lists

```
/* an element in a linked list */
struct list_element {
    void *data; /* the payload */
    struct list_element *next; /* pointer to the next element */
};
```
In some linked lists, each element also contains a pointer to the previous element.These lists are called doubly linked lists because they are linked both forward and backward.
```
/* an element in a linked list */
struct list_element {
    void *data; /* the payload */
    struct list_element *next; /* pointer to the next element */
    struct list_element *prev; /* pointer to the previous element */
    };

```

### Circular Linked Lists
Circular linked lists can come in both doubly and singly linked versions. In a circular doubly linked list,the first node’s “previous” pointer points at the last node.
Although the Linux kernel’s linked list implementation is unique, it is fundamentally a circular doubly linked list. Using this type of linked list provides the greatest flexibility.

### Moving Through a Linked List

Movement through a linked list occurs linearly.You visit one element, follow the next pointer, and visit the next element. Rinse and repeat.This is the easiest method of moving through a linked list, and the one for which linked lists are best suited. Linked lists are illsuited for use cases where random access is an important operation. Instead, you use linked lists when iterating over the whole list is important and the dynamic addition and removal of elements is required.
In linked list implementations, the first element is often represented by a special pointer—called the head—that enables easy access to the “start” of the list. In a noncircular-linked list, the last element is delineated by its next pointer being NULL . In a circular linked list, the last element is delineated because it points to the head element.Traversing the list, therefore, occurs linearly through each element from the first to the last. In a doubly linked list, movement can also occur backward, linearly from the last element to the first. Of course, given a specific element in the list, you can iterate backward and forward any number of elements, too.You need not traverse the whole list.

### The Linux Kernel’s Implementation

### The Linked List Structure
The linked-list code is declared in the header file <linux/list.h> and the data structure is simple:
```
struct list_head {
    struct list_head *next
    struct list_head *prev;
};
```
The next pointer points to the next list node, and the prev pointer points to the previous list node.
```
struct fox {
    unsigned long tail_length;
    unsigned long weight;
    bool is_fantastic;
    struct list_head list;
};
```
Using the macro container_of() , we can easily find the parent structure containing any given member variable.This is because in C, the offset of a given variable into a structure is fixed by the ABI at compile time.
```
#define container_of(ptr, type, member) ({ const typeof( ((type *)0)->member ) *__mptr = (ptr); (type *)( (char *)__mptr - offsetof(type,member) );})
```
Using container_of() , we can define a simple function to return the parent structure containing any list_head :
```
#define list_entry(ptr, type, member) container_of(ptr, type, member)
```
Armed with list_entry() , the kernel provides routines to create, manipulate, and otherwise manage linked lists—all without knowing anything about the structures that the list_head resides within.
### Defining a Linked List
As shown, a list_head by itself is worthless; it is normally embedded inside your own structure:
```
struct fox {
unsigned long tail_length;  // length in centimeters of tail
unsigned long weight; // weight in kilograms
bool is_fantastic; // is this fox fantastic?
struct list_head list; // list of all fox structures
};
```
The list needs to be initialized before it can be used. Because most of the elements are created dynamically (probably why you need a linked list), the most common way of initializing the linked list is at runtime:
```
struct fox *red_fox;
red_fox = kmalloc(sizeof(*red_fox), GFP_KERNEL);
red_fox->tail_length = 40;
red_fox->weight = 6;
red_fox->is_fantastic = false;
INIT_LIST_HEAD(&red_fox->list);
```
If the structure is statically created at compile time, and you have a direct reference to it, you can simply do this:
```
struct fox red_fox = {
.tail_length = 40,
.weight = 6,
.list = LIST_HEAD_INIT(red_fox.list),
};
```
### List Heads
```
static LIST_HEAD(fox_list);
```
This defines and initializes a list_head named fox_list .The majority of the linked list routines accept one or two parameters: the head node or the head node plus an actual list node.

### Manipulating Linked Lists
```
<linux/list.h>
```
#### Adding a Node to a Linked List
To add a node to a linked list:
```
list_add(struct list_head *new, struct list_head *head)
```
This function adds the new node to the given list immediately after the head node. Because the list is circular and generally has no concept of first or last nodes, you can pass any element for head . If you do pass the “last” element, however, this function can be used to implement a stack.
Returning to our fox example, assume we had a new struct fox that we wanted to add to the fox_list list.We’d do this:
```
list_add(&f->list, &fox_list);
```
To add a node to the end of a linked list:
```
list_add_tail(struct list_head *new, struct list_head *head)
```
This function adds the new node to the given list immediately before the head node.As with list_add() , because the lists are circular, you can generally pass any element for head .This function can be used to implement a queue, however, if you pass the “first” element.

#### Deleting a Node from a Linked List
After adding a node to a linked list, deleting a node from a list is the next most important operation.To delete a node from a linked list, use list_del() :
```
list_del(struct list_head *entry)
```
Fox example:
```
list_del(&f->list);
```
The implementation is instructive:
```
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}
static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
}
```
To delete a node from a linked list and reinitialize it, the kernel provides \
```
list_del_init() :
list_del_init(struct list_head *entry)
```
This function behaves the same as list_del() , except it also reinitializes the given list_head with the rationale that you no longer want the entry in the list, but you can reuse the data structure itself.

#### Moving and Splicing Linked List Nodes
```
list_move(struct list_head *list, struct list_head *head) // move a node from one list to another
list_move_tail(struct list_head *list, struct list_head *head) // move a node from one list to the end of another
list_empty(struct list_head *head) // check whether a list is empty
list_splice(struct list_head *list, struct list_head *head) // splice two unconnected lists together
list_splice_init(struct list_head *list, struct list_head *head)  // splice two unconnected lists together and reinitialize the old list
```
### Traversing Linked Lists
The linked lists are just containers that hold your important data; you need a way to use lists to move around and access the actual structures that contain the data.The kernel provides a nice set of interfaces for traversing linked lists and referencing the data structures that include them.
Unlike the list manipulation routines, iterating over a linked list in its entirety
is clearly an O(n) operation, for n entries in the list.
#### The Basic Approach
The most basic way to iterate over a list is with the list_for_each() macro.The macro takes two parameters, both list_head structures.The first is a pointer used to point to the current entry; it is a temporary variable that you must provide.The second is the list_head acting as the head node of the list you want to traverse (see the earlier section, “List Heads”). On each iteration of the loop, the first parameter points to the next entry in the list, until each entry has been visited. Usage is as follows:
```
struct list_head *p;
struct fox *f;
list_for_each(p, &fox_list) {
    /* p points to an entry in the list */
    /* f points to the structure in which the list is embedded */
    f = list_entry(p, struct fox, list);
}
```
#### The Usable Approach
```
list_for_each_entry(pos, head, member)
```
```
static struct inotify_watch *inode_find_handle(struct inode *inode, struct inotify_handle *ih)
{
    struct inotify_watch *watch;
    list_for_each_entry(watch, &inode->inotify_watches, i_list) {
        if (watch->ih == ih)
            return watch;
    }
    return NULL;
}

```
### Iterating Through a List Backward

### Iterating While Removing
```
void inotify_inode_is_dead(struct inode *inode)
{
    struct inotify_watch *watch, *next;
    mutex_lock(&inode->inotify_mutex);
    list_for_each_entry_safe(watch, next, &inode->inotify_watches, i_list) {
        struct inotify_handle *ih = watch->ih;
        mutex_lock(&ih->mutex);
        inotify_remove_watch_locked(ih, watch); /* deletes watch */
        mutex_unlock(&ih->mutex);
    }
    mutex_unlock(&inode->inotify_mutex);
}

```
## Queues( FIFO - first-in, first-out)
A common programming pattern in any operating system kernel is producer and consumer.
```
<linux/kfifo.h>
```
### KFIFO
### Creating a Queue
The most common method is dynamic:
```
int kfifo_alloc(struct kfifo *fifo, unsigned int size, gfp_t gfp_mask);
```
example:
```
struct kfifo fifo;
int ret;
ret = kfifo_alloc(&kifo, PAGE_SIZE, GFP_KERNEL);
if (ret)
    return ret;
/* ‘fifo’ now represents a PAGE_SIZE-sized queue ... */
```
If you want to allocate the buffer yourself, you can:
```
void kfifo_init(struct kfifo *fifo, void *buffer, unsigned int size);
```
Statically declaring a kfifo is simpler, but less common:
```
DECLARE_KFIFO(name, size);
INIT_KFIFO(name);
```
### Enqueuing Data
When your kfifo is created and initialized, enqueuing data into the queue is performed via the kfifo_in() function:
```
unsigned int kfifo_in(struct kfifo *fifo, const void *from, unsigned int len);
```

### Dequeuing Data
When you add data to a queue with kfifo_in() , you can remove it with kfifo_out() :
```
unsigned int kfifo_out(struct kfifo *fifo, void *to, unsigned int len);
```
### Obtaining the Size of a Queue
To obtain the total size in bytes of the buffer used to store a kfifo’s queue, call kfifo_size() :
```
static inline unsigned int kfifo_size(struct kfifo *fifo);
```
### Resetting and Destroying the Queue
To reset a kfifo, jettisoning all the contents of the queue, call kfifo_reset() :
```
static inline void kfifo_reset(struct kfifo *fifo);
```
To destroy a kfifo allocated with kfifo_alloc() , call kfifo_free() :
```
void kfifo_free(struct kfifo *fifo);
```
### Example Queue Usage
In this example, we enqueue simple integers. In your own code, you will likely enqueue more complicated, task-specific structures. Using integers in this example, let’s see exactly how the kfifo works:

```
unsigned int i;
/* enqueue [0, 32) to the kfifo named ‘fifo’ */
for (i = 0; i < 32; i++)
kfifo_in(fifo, &i; sizeof(i));
```
We can take a peek at the first item in the queue and verify it is 0:
```
unsigned int val;
int ret;
ret = kfifo_out_peek(fifo, &val, sizeof(val), 0);
if (ret != sizeof(val))
return -EINVAL;
printk(KERN_INFO “%u\n”, val); /* should print 0 */
```
To dequeue and print all the items in the kfifo, we can use kfifo_out() :
```
/* while there is data in the queue ... */
while (kfifo_avail(fifo)) {
unsigned int val;
int ret;
/* ... read it, one integer at a time */
ret = kfifo_out(fifo, &val, sizeof(val));
if (ret != sizeof(val))
return -EINVAL;
printk(KERN_INFO “%u\n”, val);
}
```
This prints 0 through 31, inclusive, and in that order. (If this code snippet printed the numbers backward, from 31 to 0, we would have a stack, not a queue.)


## Maps
A map, also known as an associative array, is a collection of unique keys, where each key is associated with a specific value.The relationship between a key and its value is called a mapping. Maps support at least three operations:
* Add (key, value)
* Remove (key)
* value = Lookup (key)

### Initializing an idr
Setting up an idr is easy. First you statically define or dynamically allocate an idr structure.Then you call idr_init() :
```
void idr_init(struct idr *idp);
```
For example:
```
struct idr id_huh;
idr_init(&id_huh);
```
### Allocating a New UID
Once you have an idr set up, you can allocate a new UID, which is a two-step process.
First you tell the idr that you want to allocate a new UID, allowing it to resize the backing tree as necessary.Then, with a second call, you actually request the new UID.
The first function, to resize the backing tree, is idr_pre_get() :
```
int idr_pre_get(struct idr *idp, gfp_t gfp_mask);
```
The second function, to actually obtain a new UID and add it to the idr, is
idr_get_new() :
```
int idr_get_new(struct idr *idp, void *ptr, int *id);
```
Let’s look at a full example:
```
int id;
do {
if (!idr_pre_get(&idr_huh, GFP_KERNEL))
return -ENOSPC;
ret = idr_get_new(&idr_huh, ptr, &id);
} while (ret == -EAGAIN);
```
### Removing a UID
To remove a UID from an idr, use idr_remove() :
```
void idr_remove(struct idr *idp, int id);
```

### Destroying an idr
Destroying an idr is a simple affair, accomplished with the idr_destroy() function:
```
void idr_destroy(struct idr *idp);
```
## Binary Trees
A tree is a data structure that provides a hierarchical tree-like structure of data. Mathematically, it is an acyclic, connected, directed graph in which each vertex (called a node) has zero or more outgoing edges and zero or one incoming edges.A binary tree is a tree in which nodes have at most two outgoing edges—that is, a tree in which nodes have zero, one, or two children.

### Binary Search Trees
A binary search tree (often abbreviated BST) is a binary tree with a specific ordering imposed on its nodes.The ordering is often defined via the following induction:
* The left subtree of the root contains only nodes with values less than the root.
* The right subtree of the root contains only nodes with values greater than the root.
* All subtrees are also binary search trees.
A binary search tree is thus a binary tree in which all nodes are ordered such that left children are less than their parent in value and right children are greater than their parent.
Consequently, both searching for a given node and in-order traversal are efficient (logarithmic and linear, respectively).


### Self-Balancing Binary Search Trees
The depth of a node is measured by how many parent nodes it is from the root.Nodes at the “bottom”of the tree—those with no children—are called leaves.The height of a tree is the depth of the deepest node in the tree.A balanced binary search tree is a binary search tree in which the depth of all leaves differs by at most one
#### Red-Black Trees
A red-black tree is a type of self-balancing binary search tree. Linux’s primary binary tree data structure is the red-black tree. Red-black trees have a special color attribute, which is either red or black. Red-black trees remain semi-balanced by enforcing that the following six properties remain true:
1. All nodes are either red or black.
2. Leaf nodes are black.
3. Leaf nodes do not contain data.
4. All non-leaf nodes have two children.
5. If a node is red, both of its children are black.
6. The path from a node to one of its leaves contains the same number of black nodes as the shortest path to any of its other leaves.
Taken together, these properties ensure that the deepest leaf has a depth of no more than double that of the shallowest leaf. Consequently, the tree is always semi-balanced.

##### rbtrees
The Linux implementation of red-black trees is called rbtrees.They are defined in lib/rbtree.c and declared in <linux/rbtree.h> .Aside from optimizations, Linux’s rbtrees resemble the “classic” red-black tree as described in the previous section.They remain balanced such that inserts are always logarithmic with respect to the number of nodes in the tree.
The root of an rbtree is represented by the rb_root structure.To create a new tree, we allocate a new rb_root and initialize it to the special value RB_ROOT :
```
struct rb_root root = RB_ROOT;
```
This function thus searches the given inode’s rbtree for a matching offset value:
```
struct page * rb_search_page_cache(struct inode *inode, unsigned long offset)
{
    struct rb_node *n = inode->i_rb_page_cache.rb_node;
    while (n) {
      struct page *page = rb_entry(n, struct page, rb_page_cache);
        if (offset < page->offset)
            n = n->rb_left;
        else if (offset > page->offset)
            n = n->rb_right;
        else
            return page;
        }
        return NULL;
}

```
## What Data Structure to Use, When
* If your primary access method is iterating over all your data, use a linked list.
* If your code follows the producer/consumer pattern, use a queue, particularly if you want (or can cope with) a fixed-size buffer.
* If you need to map a UID to an object, use a map.
* If you need to store a large amount of data and look it up efficiently, consider a red-black tree. Red-black trees enable the searching in logarithmic time, while still providing an efficient linear time in-order traversal.

## Algorithmic Complexity
Asymptotic behavior shows how well an algorithm scales as its input grows larger and larger. Studying an algorithm’s scalability—how it performs as the size of its input increases—enables us to model the algorithm against a benchmark and better understand its behavior.
### Algorithms
An algorithm is a series of instructions, possibly one or more inputs, and ultimately a result or output.
### Big-O Notation
One useful asymptotic notation is the upper bound, which is a function whose value, after an initial point, is always greater than the value of the function that you are studying. It is said that the upper bound grows as fast or faster than the function in question.
### Big Theta Notation
When most people talk about big-o notation, they are more accurately referring to what Donald Knuth describes as big-theta notation.Technically, big-o notation refers to an upper bound.Subsequently, when most people discuss function growth, they talk about the least upper bound, or a function that models both the upper and lower bounds. 2 Professor Knuth, the father of the field of algorithmic analysis, describes this as big-theta notation and gives the following definition:
```
If f(x) is big-theta of g(x), then
g(x) is both an upper bound and a
lower bound for f(x).
```
### Time Complexity

O(g(x)) - Name
* 1 - Constant (perfect scalability)
* log n - Logarithmic
* n - Linear
* n ^ 2 - Quadratic
* n ^ 3 - Cubic
* 2 ^ n - Exponential
* n! - Factorial