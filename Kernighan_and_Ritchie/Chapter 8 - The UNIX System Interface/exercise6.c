/*
Exercise 8-6. The standard library function calloc(n,size) returns a pointer to n objects of size size , with the storage initialized to zero. Write calloc , by calling malloc or by modifying it.
*/

#include<stdio.h>
#include<stdlib.h>

void *calloc1(unsigned, unsigned);

int main(int argc, char *argv[]){
    int *p = NULL;
    int i = 0;
    p = calloc1(100, sizeof *p);
    if (NULL == p)
        printf("returned NULL");
    else{
        for(i=0; i <= 100; i++){
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
void *calloc1(unsigned n,unsigned size){
    unsigned i,nb;
    char *p,*q;
    nb = n * size;
    if((p = q = malloc(nb)) != NULL)
        for(i = 0; i <nb; i++)
            *p++ = 0;
    return q;

}