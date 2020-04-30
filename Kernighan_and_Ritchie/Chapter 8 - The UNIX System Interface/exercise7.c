/*
Exercise 8-7. malloc accepts a size request without checking its plausibility; free believes that the block it is asked to free contains a valid size field. Improve these routines so they make more pains with error checking.
*/

#include<stdio.h>
#include<stdlib.h>

#define NALLOC      1024

typedef long Align;

union header{      /* block header */
    struct{
        union header *ptr; /* next block if on free list */
        unsigned size;
        /* size of this block */
    } s;
    Align x;
};
/* force alignment of blocks */

typedef union header Header;
static Header base;
static Header *freep = NULL;
void *malloc1(unsigned nbytes);
static Header *morecore(unsigned nu);
void free(void *ap);

void *malloc1(unsigned nbytes){
    Header *p,*prevp;
    Header *morecore(unsigned);
    unsigned nunits;
    if(nbytes > (unsigned)10240){
        fprintf(stderr,"alloc: can't allocate more than %u bytes\n",(unsigned)10240);
        return NULL;
    }
    // Improve these routines so they make more pains with error checking.
    nunits = (nbytes    + sizeof(Header) -1) / sizeof(Header) +1;
    if((prevp = freep) == NULL){
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }
    for(p = prevp->s.ptr ; ; prevp = p,p = p ->s.ptr){
        if(p->s.size >= nunits){
            if (p->s.size == nunits)
                prevp->s.ptr = p->s.ptr;
            else{
                p->s.size -= nunits;    
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
        return (void *)(p+1);
    }
    if(p == freep)  /* wrapped around free list */
        if((p = morecore(nunits)) == NULL)
            return NULL;
    }
}

static Header *morecore(unsigned nu){
    char *cp, *sbrk(int);
    Header *up;
    if (nu < NALLOC) 
        nu = NALLOC;
    cp = sbrk(nu * sizeof(Header));
    if (cp == (char *) -1) /* no space at all */
        return NULL;
    up = (Header *) cp;
    up->s.size = nu;
    free((void *)(up + 1));
    return freep;
}

void free(void *ap){
    Header *bp, *p;
    bp = (Header *)ap - 1; 
    /* point to block header */
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break;  /* freed block at start or end of arena */
    if (bp + bp->s.size == p->s.ptr) { /* join to upper nbr */
            bp->s.size += p->s.ptr->s.size;
            bp->s.ptr = p->s.ptr->s.ptr;
    } else
        bp->s.ptr = p->s.ptr;
    if (p + p->s.size == bp) { 
        /* join to lower nbr */
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    } else 
        p->s.ptr = bp; 
    freep = p;
}

int main(int argc, char *argv[]) {
    int *p = NULL;
    int i = 0;
    p = malloc1(1000);
    if (NULL == p)
        printf("returned NULL");
    else {
        for(i=0; i <= 100; i++) {
            printf("%08X", p[i]);
            if (i % 8 == 7) {
                printf("\n");
            }
        }
        printf("\n");
        free(p);
    }
    return 0;
}
    