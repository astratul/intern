/*
Exercise 6-4. Write a program that prints the distinct words in its input sorted into decreasing order of frequency of occurrence. Precede each word by its count.
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define MAXWORD 100
#define BUFSIZE 100
#define MAXSTR 100

char buf[BUFSIZE];
int bufp = 0;
int pos1 = 0;
int pos2 = 0;

struct tnode{             /* the tree node: */
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
int maxcounttree(struct tnode *p);

int main(){
    struct tnode *root;
    char word[MAXWORD];
    int ln;
    int nlines = 0;
    root = NULL;
    while (getword(word, MAXWORD) != EOF)
        if (isalpha(word[0])){
            nlines++;
            root = addtree1(root, word, nlines);
        }
    treeprint(root);
    return 0;
}
struct tnode *addtree1(struct tnode *p, char *w, int nlines){
    int cond;
    int c = 1;
    if (p == NULL){
        /* a new word has arrived */
        p = talloc();
        /* make a new node */
        p->word = strdup1(w);
        p->count = c;
        p->left = p->right = NULL;
        p->occurs[1] = nlines;
        // p->occurs++;
    }
    else if ((cond = strcmp(w, p->word)) == 0){
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

void treeprint(struct tnode *p){
    if (p != NULL){        
        treeprint(p->left);
        treeprint(p->right);
        printf("%4d %s\n", p->count, p->word);
        
    }
}

struct tnode *talloc(void){
    return (struct tnode *)malloc(sizeof(struct tnode));
}

char *strdup1(char *s){
    char *p;
    /* make a duplicate of s */
    p = (char *)malloc(strlen(s) + 1); /* +1 for '\0' */
    if (p != NULL)
        strcpy(p, s);
    return p;
}

/* getword: get next word or character from input */
int getword(char *word, int lim){
    int c, getch(void);
    void ungetch(int);
    char *w = word;
    int t;

    while (isspace(c = getch()))
        ;
    if (c != EOF)
        *w++ = c;
    if (!isalpha(c)){
        if (c == '\"'){ /*string constant*/
            for (c = getch(); c != '\"'; c = getch())
                ;
        }
        else if (c == '#'){ /*preprocessor*/
            for (c = getch(); c != '\n'; c = getch())
                ;
        }
        else if (c == '\''){ /* '' */
            for (c = getch(); c != '\n'; c = getch())
                ;
        }
        else if (c == '`'){ /* '' */
            for (c = getch(); c != '\n'; c = getch())
                ;
        }
        else if (c == '\"'){ /* "" */
            for (c = getch(); c != '\n'; c = getch())
                ;
        }
        else if (c == '/') /*comment*/
            if ((c = getch()) == '/'){ /*single comment*/
                for (c = getch(); c != '\n'; c = getch())
                    ;
            }
            else if (c == '*'){ /*mutiline comment*/
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
        if (!isalnum(*w = getch())){
            if (!isspace(*w)){
                ungetch(*w);
                return (*w);
            }
            else{
                ungetch(*w);
                break;
            }
        }
    *w = '\0';
    return word[0];
}

int getch(void){ /* get a (possibly pushed back) character */
    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c) {/* push a character back on input */
    if (bufp >= BUFSIZE)
        printf("ungetch: too many characters \n");
    else
        buf[bufp++] = c;
}