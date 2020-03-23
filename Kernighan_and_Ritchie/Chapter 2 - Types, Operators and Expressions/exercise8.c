/* 
Exercise 2-8. Write a function rightrot(x,n) that returns the value of the integer x rotated to the right by n positions.
*/

#include<stdio.h>

unsigned rightrot(unsigned x,int n);
long long binary(int n);
int main(void)
{
    // for (int i=1;i<10;i++)
    //     for (int j=1;j<3;j++)
    //         printf("\nrightrot(%d,%d) = %u\n***************\n",i,j,(unsigned)rightrot((unsigned)i,(int)j));
    printf("\nrightrot(%d,%d) = %u\n",8,1,(unsigned)rightrot((unsigned)8,(int)1));
    return 0;
}

long long binary(int n) {
    long long bin = 0;
    int rem, i = 1, step = 1;
    while (n != 0) {
        rem = n % 2;
        n /= 2;
        bin += rem * i;
        i *= 10;
    }
    return bin;
}

unsigned rightrot(unsigned x,int n)
{
    // printf("x (1) = %u, %llu\n", x, binary(x));
    // printf("n = %d\n", n);
    int dim(void);
    unsigned rbit;
    rbit = x << (dim() - n);
    // printf("rbit = %d, %llu\n", rbit, binary(rbit));
    x = x >> n;
    // printf("x (2) = %u, %llu\n", x, binary(x));
    x = x | rbit;
    // printf("x (3) = %u, %llu\n", x, binary(x));
    return x;
}

int dim(void)
{
    int i;
    unsigned v = ((unsigned) ~0);
    for(i=1;(v=v>>1)>0;i++)
        ;
    return i;
}

