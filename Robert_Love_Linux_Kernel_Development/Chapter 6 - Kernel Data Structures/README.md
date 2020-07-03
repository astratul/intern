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

