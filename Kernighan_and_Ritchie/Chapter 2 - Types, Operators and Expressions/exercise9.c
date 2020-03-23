/* Exercise 2-9. In a two's complement number system, x &= (x-1) deletes the rightmost 1-bit in x . Explain why. Use this observation to write a faster version of bitcount .*/
/* 
Explanation
x - 1 is any binary number subtrated by 0000 0001
x &= (x-1) <=> x = x&(x-1)
x & (x-1) sets the last bits to 0 until x = 0
*/
#include<stdio.h>

int bitcount1(unsigned x);
int bitcount2(unsigned x);
long long binary(int n);
int main(void)
{   unsigned k = bitcount2((unsigned)12);
    printf("\n bitcount = %d\n",k);
    unsigned k1 = bitcount2((unsigned)135);
    printf("\n bitcount = %d\n",k1);
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

int bitcount2(unsigned x)
{
    int b;
    
    for(b=0; x!=0; x &= x-1)

    {
    // {   printf("\n b = %u  \n", b);
    //     printf("\n x = %u  %llu \n", x, binary(x));
        printf("\nx = %u %llu \t x & x-1 = %u  %llu \n",x, binary(x), x & x-1, binary(x & x-1));
        ++b;
    }

    return b;
}


/* bitcount: count 1 bits in x */
int bitcount1(unsigned x)
{
int b;
for (b = 0; x != 0; x >>= 1)
    if (x & 01)
        b++;
return b;
}