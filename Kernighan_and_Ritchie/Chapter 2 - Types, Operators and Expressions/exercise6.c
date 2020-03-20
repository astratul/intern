/* 
Exercise 2-6. Write a function setbits(x,p,n,y) that returns x with the n bits that begin at
position p set to the rightmost n bits of y , leaving the other bits unchanged.
*/

#include<stdio.h>

unsigned setbits(unsigned x,int p,int n,unsigned y);
unsigned getbits(unsigned x, int p, int n);
unsigned binary(int x);

int main(void)
{
    // printf("\ngetbits: get n bits from position p: %u\n",getbits((unsigned)12,4,3));
    printf("***************\nsetbits: %u\n***************\n",setbits((unsigned)140,7,3,(unsigned)23));
}
unsigned binary(int x){
    unsigned bin = 0;
    int k = 1;

    while (x)
    {
        bin += (x % 2) * k;
        k *= 10;
        x /= 2;
    }

    return bin;
}
unsigned setbits(unsigned x,int p,int n,unsigned y)
{
    unsigned da1 = x & ~(~(~0 << n) << (p+1-n));
    unsigned da2 = ( y & ~(~0<<n)) << (p+1-n);
    printf("x = %u, %u\n", x, binary(x));
    printf("y = %u, %u\n", y, binary(y));
    printf("p = %d, %d\n", p, binary(p));
    printf("n = %d, %d\n", n, binary(n));
    printf("(p+1-n) = %d, %u\n", (p+1-n), binary(p+1-n));
    printf("~(~0 << n) = %d, %u\n", ~(~0 << n) , binary(~(~0 << n) ));
    // printf("~(~(~0 << n) << (p+1-n)): %u, %u \n", ~(~(~0 << n) << (p+1-n)), binary(~(~(~0 << n) << (p+1-n))));
    printf("x & ~(~(~0 << n) << (p+1-n)) = %u , %u\n",da1, binary(da1));
    printf("( y & ~(~0<<n)) << (p+1-n) = %u, %u \n", da2, binary(da2));
    printf("x & ~(~(~0 << n) << (p+1-n)) | ( y & ~(~0<<n)) << (p+1-n) = %u, %u\n", x& da1 | da2, binary(x& da1 | da2));
    return x & ~(~(~0 << n) << (p+1-n)) | ( y & ~(~0<<n)) << (p+1-n);
    // << (p+1-n) gets n digits
    // ~(~0 << n) right most n 1s
}

unsigned getbits(unsigned x, int p, int n)
{   unsigned da1 = x >> (p+1-n);
    unsigned da2 = ~(~0 << n);
    printf("x este %u, %u\n", x, binary(x));
    printf("(p+1-n) este %d, %u\n", (p+1-n), binary(p+1-n));
    printf("x >> (p+1-n) este %u , %u\n",da1, binary(da1));
    printf("~(~0 << n) este %u, %u \n", da2, binary(da2));
    printf("tot este %u, %u\n", da1 & da2, binary(da1&da2));
    return (da1 & da2);
}
