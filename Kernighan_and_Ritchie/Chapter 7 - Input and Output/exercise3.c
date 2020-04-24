/*
Exercise 7-3. Revise minprintf to handle more of the other facilities of printf .
*/
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>


void minprintf(char *fmt,...);

int main(){
    minprintf("%d\n",123);   
    minprintf("%c\n",'b');   
    minprintf("%u\n",10987);   
    minprintf("%o\n","Bonjour!");    
    minprintf("%x\n",1298);   
    minprintf("%X\n","Bonjour!");
    minprintf("%f\n",89.98);
    minprintf("%E\n",8);
    minprintf("%s\n","Bonjour!");
    minprintf("Bonjour!\n");
    return 0;
}

/* minprintf: minimal printf with variable argument list */
void minprintf(char *fmt, ...){
    va_list ap; /* points to each unnamed arg in turn */
    char *p, *sval;
    int ival;
    double dval;
    unsigned uval;
    va_start(ap, fmt); /* make ap point to 1st unnamed arg */
    for (p = fmt; *p; p++){
        if (*p != '%'){
            putchar(*p);
            continue;
        }
        switch (*++p){
        case 'd': // int ; decimal number
            ival = va_arg(ap, int);
            printf("%d", ival);
            break;
        case 'i': // int ; decimal number
            ival = va_arg(ap, int);
            printf("%i", ival);
            break;
        case 'c': // int ; single character
            ival = va_arg(ap, int);
            putchar(ival);
            break;
        case 'u': // int ; unsigned decimal number
            uval = va_arg(ap, unsigned int);
            printf("%u", uval);
            break;
        case 'o': // int ; unsigned octal number (without a leading zero)
            uval = va_arg(ap, unsigned int);
            printf("0%o", uval);
            break;
        case 'x': // int ; unsigned hexadecimal number (without a leading 0x or 0X ), using abcdef or ABCDEF for 10, ...,15.
            uval = va_arg(ap, unsigned int);
            printf("0x%x", uval);
            break;
        case 'X': // int ; unsigned hexadecimal number (without a leading 0x or 0X ), using abcdef or ABCDEF for 10, ...,15.
            uval = va_arg(ap, unsigned int);
            printf("0X%X", uval);
            break;
        case 'e': // double ; [-] m.dddddd e+/- xx or [-] m.dddddd E+/- xx , where the number of d 's is given by the precision (default 6).
            dval = va_arg(ap, double);
            printf("%e", dval);
            break;
        case 'E': // double ; [-] m.dddddd e+/- xx or [-] m.dddddd E+/- xx , where the number of d 's is given by the precision (default 6).
            dval = va_arg(ap, double);
            printf("%E", dval);
            break;
        case 'f': // double ; [-] m.dddddd , where the number of d 's is given by the precision (default 6).
            dval = va_arg(ap, double);
            printf("%f", dval);
            break;
        case 's': // char * ; print characters from the string until a '\0' or the number of characters given by the precision.
            for (sval = va_arg(ap, char *); *sval; sval++)
                putchar(*sval);
            break;
        default:
            putchar(*p);
            break;
        }
    }
    va_end(ap); /* clean up when done */
}