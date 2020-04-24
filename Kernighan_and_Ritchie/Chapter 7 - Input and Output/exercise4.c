/*
Exercise 7-4. Write a private version of scanf analogous to minprintf from the previous section.
*/
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>


int minscanf(char *fmt,...);

int main(void){
    float v;
    while (minscanf("%f\n", &v))
        printf("\t%.2f\n", v);
    return 0;
}

/* minprintf: minimal printf with variable argument list */
int minscanf(char *fmt, ...){
    va_list ap; /* points to each unnamed arg in turn */
    char *p, *sval;
    int *ival;
    float *fval;
    unsigned int *uval;
    va_start(ap, fmt); /* make ap point to 1st unnamed arg */
    for (p = fmt; *p; p++){
        if (*p != '%'){
            putchar(*p);
            continue;
        }
        switch (*++p){
        case 'd': // decimal integer; int *
            ival = (int)va_arg(ap, int);
            scanf("%d", ival);
            return 1;
        case 'i': // integer; int * . The integer may be in octal (leading 0 ) or hexadecimal (leading 0x or 0X ).
            ival = va_arg(ap, int);
            scanf("%i", ival);
            return 1;
        case 'c': // characters; char * . The next input characters (default 1) are placed at the indicated spot. The normal skip-over white space is suppressed; to read the next non-white space character, use %1s
            ival = va_arg(ap, int);
            // scanf("%c", ival);
            putchar(ival);
            return 1;
        case 'u': // unsigned decimal integer; unsigned int *
            uval = va_arg(ap, unsigned int);
            scanf("%u", uval);
            return 1;
        case 'o': // octal integer (with or without leading zero); int *
            uval = va_arg(ap, unsigned int);
            scanf("0%o", uval);
            return 1;
        case 'x': // hexadecimal integer (with or without leading 0x or 0X ); int *
            uval = va_arg(ap, unsigned int);
            scanf("0x%x", uval);
            return 1;
        case 'e': // floating-point number with optional sign, optional decimal point and optional exponent; float *
            fval = va_arg(ap, float*);
            scanf("%e", fval);
            return 1;
        case 'f': // floating-point number with optional sign, optional decimal point and optional exponent; float *
            fval = va_arg(ap, float*);
            scanf("%f", fval);
            return 1;
        case 'g': // floating-point number with optional sign, optional decimal point and optional exponent; float *
            fval = va_arg(ap, float*);
            scanf("%g", fval);
            return 1;
        // case 's': // character string (not quoted); char * , pointing to an array of characters long enough for the string and a terminating '\0' that will be added.
        //     for ((char *)sval = va_arg(ap, char *); *sval; sval++)
        //         scanf(*sval);
        //     break;
        default:
            putchar(*p);
            return 1;
        }
    }
    va_end(ap); /* clean up when done */
    return 0;
}