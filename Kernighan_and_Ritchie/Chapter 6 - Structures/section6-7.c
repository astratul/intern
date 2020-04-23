// 6.7 Typedef
// can be used in contexts in the sort program of Chapter 5.
#include <stdlib.h>
# define MAXLINES 100

typedef int Length;
Length len, maxlen;
Length *lengths[MAX];
typedef char *String;
String p, lineptr[MAXLINES], alloc(int);

typedef struct tnode *Treeptr;
typedef struct tnode { /*the tree node: */
    char *word;/* points to the text */
    int count;/* number of occurrences */
    struct tnode *left;/* left child */
    struct tnode *right;/* right child */
} Treenode;

Treeptr talloc(void)
{
    return (Treeptr) malloc(sizeof(Treenode));
}

int main(){
    // int strcmp(String, String);
    p = (String) malloc(100);
    typedef int (*PFI)(char *, char *);
    // PFI strcmp, numcmp;
    return 0;
}


