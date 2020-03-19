/*
Exercise 2-1. Write a program to determine the ranges of char , short , int , and long
variables, both signed and unsigned , by printing appropriate values from standard headers
and by direct computation. Harder if you compute them: determine the ranges of the various
floating-point types.
*/



#include<stdio.h>

int main(void)
{
    printf("\n-------------------signed-------------------\n");
    printf("Signed Char: [%d, %d]\n",-(char)((unsigned char) ~0 >> 1) - 1,(char) ((unsigned char) ~0 >> 1));
    printf("Signed Short: [%d, %d]\n",-(short)((unsigned short)~0 >>1) -1,(short)((unsigned short)~0 >> 1));
    printf("Signed Int: [%d, %d]\n",-(int)((unsigned int)~0 >> 1) -1, (int)((unsigned int)~0 >> 1));
    printf("Signed Long: [%ld, %ld]\n",-(long)((unsigned long)~0 >>1) -1,(long)((unsigned long)~0 >> 1));
    printf("\n-------------------unsigned-------------------\n");
    printf("Unsigned Char: [%d, %d]\n",(unsigned char) 0,(unsigned char)~0);
    printf("Unsigned Short: [%d, %d]\n",(unsigned short) 0, (unsigned short)~0);
    printf("Unsigned Int: [%u, %u]\n",(unsigned int) 0, (unsigned int)~0);
    printf("Unsigned Long: [%lu, %lu]\n",(unsigned long) 0, (unsigned long)~0);
    return 0;
}



/*  ~0 will give bits in 1s. ex:  (unsigned char) ~0 = 1111 1111
 (unsigned <type>) will cast it unsigned.
 >> 1 right shifts 1 bit to remove the sign bit -->    0111 1111
 (<type>) casting it the required type again    -->    127
*/
