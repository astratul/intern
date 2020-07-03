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




## Queues

## Maps

## Binary Trees

## What Data Structure to Use, When

## Algorithmic Complexity



