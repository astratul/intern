/* 
Exercise 2-7. Write a function invert(x,p,n) that returns x with the n bits that begin at
position p inverted (i.e., 1 changed into 0 and vice versa), leaving the others unchanged.
*/

#include<stdio.h>

unsigned invert(unsigned x,int p,int n);
unsigned binary(int x);
int main(void)
{
    printf("***************\ninvert: %u\n***************\n",(unsigned)invert((unsigned)87,(int)4,(int)3));
}
unsigned binary(int x){
    unsigned bin = 0;
    int k = 1;
    while (x)
    {   bin += (x % 2) * k;
        k *= 10;
        x /= 2;}
    return bin;
}

unsigned invert(unsigned x, int p, int n)
{
    printf("x = %u, %u\n", x, binary(x));
    printf("p = %d, %d\n", p, binary(p));
    printf("n = %d, %d\n", n, binary(n));
    printf("(p+1-n) = %d, %u\n", (p+1-n), binary(p+1-n));
    printf("~(~0 << n) = %d, %u\n", ~(~0 << n) , binary(~(~0 << n) ));
    printf("(~(~0 << n) << (p + 1 - n) = %u , %u\n",(~(~0 << n) << (p + 1 - n)), binary((~(~0 << n) << (p + 1 - n))));
    printf("x ^ (~(~0 << n) << (p + 1 - n)) = %u, %u\n", x ^ (~(~0 << n) << (p + 1 - n)), binary(x ^ (~(~0 << n) << (p + 1 - n))));
    return   x ^ (~(~0 << n) << (p + 1 - n));
}
