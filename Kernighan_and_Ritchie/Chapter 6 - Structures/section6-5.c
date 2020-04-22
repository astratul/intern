// 6.5 Self-referential Structures

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define MAXWORD 100
#define BUFSIZE  100

char buf[BUFSIZE];
int bufp = 0; 

struct tnode
{                        /* the tree node: */
    char *word;          /* points to the text */
    int count;           /* number of occurrences */
    struct tnode *left;  /* left child */
    struct tnode *right; /* right child */
};

// struct tnode *left; -> declares left to be a pointer to a tnode , not a tnode itself.

struct tnode *addtree1(struct tnode *, char *);
void treeprint(struct tnode *);
struct tnode *talloc(void);
int getword(char *, int);
char *strdup1(char *);

/* word frequency count */
int main()
{
    struct tnode *root;
    char word[MAXWORD];

    root = NULL;
    while (getword(word, MAXWORD) != EOF)
        if (isalpha(word[0]))
            root = addtree1(root, word);
    treeprint(root);
    return 0;
}

/* addtree1: add a node with w, at or below p */
struct tnode *addtree1(struct tnode *p, char *w)
{
    int cond;

    if (p == NULL)
    {
        /* a new word has arrived */
        p = talloc();
        /* make a new node */
        p->word = strdup1(w);
        p->count = 1;
        p->left = p->right = NULL;
    }
    else if ((cond = strcmp(w, p->word)) == 0)
        p->count++;
    /* repeated word */
    else if (cond < 0)
        /* less than into left subtree */
        p->left = addtree1(p->left, w);
    else
        /* greater than into right subtree */
        p->right = addtree1(p->right, w);
    return p;
}


/* treeprint: in-order print of tree p */
void treeprint(struct tnode *p)
{
    if (p != NULL)
    {
        treeprint(p->left);
        printf("%4d %s\n", p->count, p->word);
        treeprint(p->right);
    }
}
/* talloc: make a tnode */
struct tnode *talloc(void)
{
    return (struct tnode *)malloc(sizeof(struct tnode));
}

char *strdup1(char *s)
{
    char *p;
    /* make a duplicate of s */
    p = (char *)malloc(strlen(s) + 1); /* +1 for '\0' */
    if (p != NULL)
        strcpy(p, s);
    return p;
}


/* getword: get next word or character from input */
int getword(char *word, int lim)
{
    int c, getch(void);
    void ungetch(int);
    char *w = word;
    while (isspace(c = getch()))
        ;
    if (c != EOF)
        *w++ = c;
    if (!isalpha(c))
    {
        *w = '\0';
        return c;
    }
    for (; --lim > 0; w++)
        if (!isalnum(*w = getch()))
        {
            ungetch(*w);
            break;
        }
    *w = '\0';
    return word[0];
}

int getch(void)     /* get a (possibly pushed back) character */
{
    return ( bufp > 0)? buf[--bufp] : getchar();
}

void ungetch(int c)     /* push a character back on input */
{
    if(bufp >= BUFSIZE)
        printf("ungetch: too many characters \n");
    else
        buf[bufp++] = c;
}