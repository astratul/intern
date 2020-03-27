/* 
Exercise 4-14. Define a macro swap(t,x,y) that interchanges two arguments of type t .
(Block structure will help.)
*/

#include<stdio.h>

#define swap(t, param1, param2) { t var; var = param1; param1 = param2; param2 = var; }

int main(void)
{
    int x, y;
    x = 98;
    y = 123;
    printf("Before swap: \n\tThe first argument is %d \t The second argument is %d\n",x,y);
    swap(int, x, y);
    printf("After swap: \n\tThe first argument is %d \t The second argument is %d\n",x,y);
}
