/*
Exercise 6-3. Write a cross-referencer that prints a list of all words in a document, and for each word, a list of the line numbers on which it occurs. Remove noise words like ``the,'' ``and,'' and so on.
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
    int occurs[];
};
struct tnode *addtree1(struct tnode *, char *, int);
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
            nlines++;
            // printf("Random print1: %s\n", word);
            if (strcmp(word, "this") != 0 )
                // if(strcmp(word, "and") != 0)
                root = addtree1(root, word, nlines);
        }
    // qsort2((void **)root, 0, nlines - 1, (int (*)(void *, void *))numcmp);
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
struct tnode *addtree1(struct tnode *p, char *w, int nlines)
{
    int cond;
    int c = 1;
    if (p == NULL)
    {
        /* a new word has arrived */
        p = talloc();
        /* make a new node */
        p->word = strdup1(w);
        p->count = c;
        p->left = p->right = NULL;
        p->occurs[1] = nlines;
        // p->occurs++;
    }
    else if ((cond = strcmp(w, p->word)) == 0)
    {
        c++;
        p->count = c;
        p->occurs[c] = nlines;
    }
    /* repeated word */
    else if (cond < 0)
        /* less than into left subtree */
        p->left = addtree1(p->left, w, nlines);
    else
        /* greater than into right subtree */
        p->right = addtree1(p->right, w, nlines);
    return p;
}

void treeprint(struct tnode *p)
{
    int x = 0;
    if (p != NULL)
    {
        treeprint(p->left);
        x += p->count;
        int size = sizeof p->occurs[x];
        if (p->count > 1)
        {
            for (int i = 1; i < size - 1; i++)
            {
                printf("%4d x '%s', %d occurs on line %d.\n", p->count, p->word, i, p->occurs[i]);
            }
        }
        else
        {
            printf("%4d x '%s' occurs on line %d.\n", p->count, p->word, p->occurs[1]);
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
        else if (c == '\'')
        { /* '' */
            for (c = getch(); c != '\n'; c = getch())
                ;
        }
        else if (c == '`')
        { /* '' */
            for (c = getch(); c != '\n'; c = getch())
                ;
        }
        else if (c == '\"')
        { /* "" */
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