// 6.8 Unions

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#define NSYM 100
struct {
    char *name;
    int flags;
    int utype;
    union {
        int ival;
        float fval;
        char *sval;
    } u;
} symtab[NSYM];

// symtab[i].u.ival
// *symtab[i].u.sval
// symtab[i].u.sval[0]

union u_tag {
    int ival;
    float fval;
    char *sval;
} u;

int main(){
    char *utype;
    u.ival=3;
    // u.fval=9.78;
    if(u.fval)
        utype="float";
    else if(u.ival)
        utype="int";
    else if(u.sval)
        utype="string";

    if (strcmp(utype, "int")==0)
        printf("%d\n", u.ival);
    else if (strcmp(utype, "float")==0)
        printf("%f\n", u.fval);
    else if (strcmp(utype, "string")==0)
        printf("%s\n", u.sval);
    else
        printf("bad type %s in utype\n", utype);
    return 0;
}

