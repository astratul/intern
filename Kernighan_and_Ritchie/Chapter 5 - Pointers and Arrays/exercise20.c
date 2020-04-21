/* 
Exercise 5-20. Expand dcl to handle declarations with function argument types, qualifiers like const , and so on.
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAXTOKEN 100
#define BUFSIZE 100

enum
{
    NAME,
    PARENS,
    BRACKETS
};

void dcl(void);
void dirdcl(void);

enum
{
    NO,
    YES
};
void dcl(void);
void dirdcl(void);
void errmsgdcl(char *);
int gettoken(void);
int tokentype;
extern int prevtoken;
char token[MAXTOKEN];
char name[MAXTOKEN];
char datatype[MAXTOKEN];
char out[1000];
void dclspec(void);
int typespec(void);
int typequal(void);
int compare(char **, char **);
char buf[BUFSIZE]; /* buffer for ungetch   */
int bufp = 0;      /* next free position in buf   */
int prevtoken = NO;
int main()
{
    int type;
    char temp[MAXTOKEN];

    while (gettoken() != EOF)
    {
        strcpy(out, token);
        while ((type = gettoken()) != '\n')
            if (type == PARENS || type == BRACKETS)
                strcat(out, token);
            else if (type == '*')
            {
                sprintf(temp, "(*%s)", out);
                strcpy(out, temp);
            }
            else if (type == NAME)
            {
                sprintf(temp, "%s %s", token, out);
                strcpy(out, temp);
            }
            else
                printf("invalid input at %s\n", token);
    }
    return 0;
}

int gettoken(void) /* return next token */
{
    int c, getch(void);
    void ungetch(int);
    char *p = token;
    while ((c = getch()) == ' ' || c == '\t')
        ;
    if (c == '(')
    {
        if ((c = getch()) == ')')
        {
            strcpy(token, "()");
            return tokentype = PARENS;
        }
        else
        {
            ungetch(c);
            return tokentype = '(';
        }
    }
    else if (c == '[')
    {
        for (*p++ = c; (*p++ = getch()) != ']';)
            ;
        *p = '\0';
        return tokentype = BRACKETS;
    }
    else if (isalpha(c))
    {
        for (*p++ = c; isalnum(c = getch());)
            *p++ = c;
        *p = '\0';
        ungetch(c);
        return tokentype = NAME;
    }
    else
        return tokentype = c;
}

void dcl(void)
{
    int ns;
    for (ns = 0; gettoken() == '*';) /* count *'s */
        ns++;
    dirdcl();
    while (ns-- > 0)
        strcat(out, " pointer to");
}
/* dirdcl: parse a direct declarator */
void dirdcl(void)
{
    int type;
    void parmdcl(void);

    if (tokentype == '(')
    {/* ( dcl ) */
        
        dcl();
        if (tokentype != ')')
            errmsgdcl("error: missing )\n");
    }
    else if (tokentype == NAME) /* variable name */
        {
            if (name[0] == '\0')
                strcpy(name, token);
        }
    else
        prevtoken = YES;
        // errmsgdcl("error: expected name or (dcl)\n");
    while ((type = gettoken()) == PARENS || type == BRACKETS || type == '(')
        if (type == PARENS)
            strcat(out, " function returning");
        else if (type == '(')
        {
            strcat(out, "function expecting");
            parmdcl();
            strcat(out, "and returning");
        }
        else
        {
            strcat(out, " array");
            strcat(out, token);
            strcat(out, " of");
        }
}

void errmsgdcl(char *msg)
{
    printf("%s", msg);
    prevtoken = YES;
}

int getch(void) /* get a (possibly pushed back) character */
{
    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c)
{
    if (bufp >= BUFSIZE)
        printf("ungetch: too many characters \n");
    else
        buf[bufp++] = c;
}

/* compare: compare two strings for bsearch */
int compare(char **s, char **t)
{
    return strcmp(*s, *t);
}

/* typequal: return YES if token is a type-qualifier */
int typequal(void)
{
    static char *typeq[] =
        {
            "const",
            "volatile"};

    char *pt = token;

    if (bsearch(&pt, typeq, sizeof(typeq) / sizeof(char *), sizeof(char *), compare) == NULL)
        return NO;
    else
        return YES;
}

/* typespec: return YES if token is a type-specifier    */
int typespec(void)
{
    static char *type[] =
        {
            "char",
            "int",
            "void"};

    char *pt = token;

    if (bsearch(&pt, type, sizeof(type) / sizeof(char *), sizeof(char *), compare) == NULL)
        return NO;
    else
        return YES;
}
/* dclspec: declaration specification */
void dclspec(void)
{
    char temp[MAXTOKEN];

    temp[0] = '\0';
    gettoken();

    do
    {
        if (tokentype != NAME)
        {
            prevtoken = YES;
            dcl();
        }
        else if (typespec() == YES)
        {
            strcat(temp, " ");
            strcat(temp, token);
            gettoken();
        }
        else if (typequal() == YES)
        {
            strcat(temp, " ");
            strcat(temp, token);
            gettoken();
        }
        else
            errmsgdcl("unknown type in parameter list \n");
    } while (tokentype != ',' && tokentype != ')');

    strcat(out, temp);
    if (tokentype == ',')
        strcat(out, ",");
}

/* parmdcl: parse a parameter declarator */
void parmdcl(void)
{
    do{
        dclspec();
    } while (tokentype == ',');

    if (tokentype != ')')
        errmsgdcl("missing ) in parameter declaration \n");
}
