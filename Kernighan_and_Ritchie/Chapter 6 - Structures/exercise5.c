/*
Exercise 6-5. Write a function undef that will remove a name and definition from the table maintained by lookup and install .
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#define HASHSIZE 101
// #define IN 1
// state = IN;
struct nlist { /* table entry: */
    struct nlist *next; /* next entry in chain */
    char *name; /* defined name */
    char *defn; /* replacement text */
};

static struct nlist *hashtab[HASHSIZE];
unsigned hash(char *s);
struct nlist *lookup(char *s);
struct nlist *install(char *name, char *defn);
struct nlist *undef(char *name);

int main(int argc, char *argv[]){
    struct nlist *table[4] = {
        (install("k1", "v1")),
        (install("k2", "v2")),
        (install("k3", "v3")),
        (install("k4", "v4"))};
    int i;
    for (i = 0; i < 4; i++){
        printf(" %s->%s\n", table[i]->name, table[i]->defn);
    }
    printf("********\n");
    undef("k2");
    undef("k3");
    struct nlist *result;
    char *keys[4] = {
        "k1",
        "k2",
        "k3",
        "k4"};
    for (i = 0; i < 4; i++){
        if ((result = lookup(keys[i])) == NULL){
            printf("Not found\n");
        } else {
            printf(" %s->%s\n", result->name, result->defn);
        }
    }
    return 0;
}

/* hash: form hash value for string s */
unsigned hash(char *s){
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
    hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}
/* lookup: look for s in hashtab */
struct nlist *lookup(char *s){
    struct nlist *np;

    for (np = hashtab[hash(s)]; np != NULL; np = np->next)
        if (strcmp(s, np->name) == 0)
            return np; /* found */
    return NULL; /* not found */
}
/* install: put (name, defn) in hashtab */
struct nlist *install(char *name, char *defn) {
    struct nlist *np;
    unsigned hashval;

    if ((np = lookup(name)) == NULL) { /* not found */
        np = (struct nlist *) malloc(sizeof(*np));
        if (np == NULL || (np->name = strdup(name)) == NULL)
            return NULL;
        hashval = hash(name);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    } else
        /* already there */
        free((void *) np->defn);
    /*free previous defn */
    if ((np->defn = strdup(defn)) == NULL)
        return NULL;
    return np;
}
struct nlist *undef(char *name){
    struct nlist *found;
    found = lookup(name);
    if (found == NULL)
        return NULL;
    else {
        if (found->next != NULL){
            found->next = found->next->next;
            found = found->next;
        } else {
            hashtab[hash(name)] = NULL;
            free((void *)found);
        }
    }
    return found;
}
