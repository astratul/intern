/*
Exercise 6-2. Write a program that reads a C program and prints in alphabetical order each group of variable names that are identical in the first 6 characters, but different somewhere thereafter. Don't count words within strings and comments. Make 6 a parameter that can be set from the command line.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define MAXWORD 100
#define BUFSIZE 100
#define DEF_CHAR_NUM 6
#define MAXSTR 100

char buf[BUFSIZE];
int bufp = 0;
int pos1 = 0;
int pos2 = 0;

struct tnode
{                        /* the tree node: */
    char *word;          /* points to the text */
    int count;           /* number of occurrences */
    struct tnode *left;  /* left child */
    struct tnode *right; /* right child */
    char *prefix;
};

struct tnode *addtree1(struct tnode *, char *);
void treeprint(struct tnode *);
struct tnode *talloc(void);
int getword(char *, int);
char *strdup1(char *);

int main(int argc, char *argv[])
{
    struct tnode *root;
    char word[MAXWORD];
    int ln;
    int nlines = 0;
    root = NULL;
    while (getword(word, MAXWORD) != EOF)
        if (isalpha(word[0]))
        {
            if (strlen(word) > DEF_CHAR_NUM)
            {
                root = addtree1(root, word);
            }
        }
    if (argc == 1)
        ln = 6;
    else if (argc == 2)
        ln = atoi(argv[1]);
    else
    {
        printf("Incorrect number of arguments.\n");
        return 1;
    }

    treeprint(root);
    return 0;
}

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
        p->prefix = strndup(w, DEF_CHAR_NUM);
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

void treeprint(struct tnode *p)
{

    if (p != NULL)
    {
        char *group = p->prefix;
        treeprint(p->left);
        printf("The group with the first %d characters '%s':\n", DEF_CHAR_NUM, p->prefix);
        if (strcmp(p->prefix, group) == 0)
        {
            printf("\t%4d x '%s'\n", p->count, p->word);
        }
        treeprint(p->right);
    }
}

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
    int t;
    char *idk = word;

    while (isspace(c = getch()))
        ;
    if (c != EOF)
        *w++ = c;
    if (!isalpha(c))
    {
        if (c == '\"')
        { /*string constant*/
            for (c = getch(); c != '\"'; c = getch())
                ;
        }
        else if (c == '#')
        { /*preprocessor*/
            for (c = getch(); c != '\n'; c = getch())
                ;
        }
        else if (c == '/') /*comment*/
            if ((c = getch()) == '/')
            { /*single comment*/
                for (c = getch(); c != '\n'; c = getch())
                    ;
            }
            else if (c == '*')
            { /*mutiline comment*/
                for (c = getch(), t = getch(); c != '*' && t != '/'; c = getch(), t = getch())
                    ungetch(t);
            }
            else
                ungetch(c);
        else if (c == '_') /*underscore*/
            for (; !isspace(c) && c != EOF; c = getch())
                ;
        if (c != '\"' && c != '\n' && c != '/')
            ungetch(c);
        *w = '\0';
        return c;
    }
    for (; --lim > 0; w++)
        if (!isalnum(*w = getch()))
        {
            if (!isspace(*w))
            {
                ungetch(*w);
                return (*w);
            }
            else
            {
                ungetch(*w);
                break;
            }
        }
    *w = '\0';
    return word[0];
}

int getch(void) /* get a (possibly pushed back) character */
{
    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c) /* push a character back on input */
{
    if (bufp >= BUFSIZE)
        printf("ungetch: too many characters \n");
    else
        buf[bufp++] = c;
}