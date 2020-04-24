// 7.2 Formatted Output - printf

#include <stdio.h>
#include <ctype.h>


int main(){
    char *s ="hello, world";
    printf("%10s\n", s);
    printf("%.10s\n", s);
    printf("%-10s\n", s);
    printf("%.15s\n", s);
    printf("%-15s\n", s);
    printf("%15.10s\n", s);
    printf("%-15.10s\n", s);
    printf("%s\n", s); /* SAFE */
    // printf(s); /* FAILS if s contains % */
    // int sprintf(char *string, char *format, arg1, arg2, ..);
}