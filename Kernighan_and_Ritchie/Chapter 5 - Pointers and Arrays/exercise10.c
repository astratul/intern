/* 
Exercise 5-10. Write the program expr , which evaluates a reverse Polish expression from the command line, where each operator or operand is a separate argument. 

For example, 
expr 2 3 4 + *
evaluates 2 * (3+4).
*/

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#define MAXOP 100  /* maximum size of operand or operator */
#define NUMBER '0' /* signal that a number was found */
#define BUFSIZE 100
#define MAXVAL 100 /* maximum depth of value of stack */

int getop(char[]);
void ungets(char[]);
void push(double);
double pop(void);
int getch(void);
void ungetch(int);

char buf[BUFSIZE]; /* buffer for ungetch */
int bufp = 0;      /* next free position in buf */
int sp = 0;         /* next free stack position */
double val[MAXVAL]; /* value stack */

double atof(char s[])
{
    double val, power;
    int i, sign;
for (i = 0; isspace(s[i]); i++) /* skip white space */
    ;
    sign = (s[i] == '-') ? -1 : 1;
    if (s[i] == '+' || s[i] == '-')
        i++;
    for (val = 0.0; isdigit(s[i]); i++)
        val = 10.0 * val + (s[i] - '0');
    if (s[i] == '.')
        i++;
    for (power = 1.0; isdigit(s[i]); i++) {
        val = 10.0 * val + (s[i] - '0');
        power *= 10;
    }
    return sign * val / power;
}
int main(int argc, char *argv[]){ /* reverse polish calculator, uses command line */
    char s[MAXOP];
    double op2;

    while (--argc > 0){
        ungets(" "); /* push end of argument */
        ungets(*++argv);

        switch (getop(s))
        {
        case NUMBER:
            push(atof(s));
            break;
        case '+':
            push(pop() + pop());
            break;
        case '*':
            push(pop() * pop());
            break;
        case '-':
            op2 = pop();
            push(pop() - op2);
            break;
        case '/':
            op2 = pop();
            if (op2 != 0.0)
                push(pop() / op2);
            else
                printf("error: zero divisor \n");
            break;
        default:
            printf("error: unknown command %s \n", s);
            argc = 1;
            break;
        }
    }
    printf("\t %8g\n", pop());
    return 0;
}

int getop(char s[]){/* getop: get next operator or numeric operand */
    int i, c;
    while ((s[0] = c = getch()) == ' ' || c == '\t')
        ;
    s[1] = '\0';
    if (!isdigit(c) && c != '.')
        return c;
    i = 0;
    if (isdigit(c)) /* collect integer part */
        while (isdigit(s[++i] = c = getch()))
            ;
    if (c == '.') /* collect from fraction part */
        while (isdigit(s[++i] = c = getch()))
            ;
    s[i] = '\0';
    if (c != EOF)
        ungetch(c);
    return NUMBER;
}

int getch(void){ /* get a (possibly pushed back) character */

    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c) /* push character back on input */
{
    if (bufp >= BUFSIZE)
        printf("ungetch: too many characters \n");
    else
        buf[bufp++] = c;
}

void push(double f){/* push : push f onto value stack */
    if (sp < MAXVAL)
        val[sp++] = f;
    else
        printf("error: stack full, can't push %g \n", f);
}

double pop(void){/* pop: pop and return top value from the stack */
    if (sp > 0)
        return val[--sp];
    else
    {
        printf("error: stack empty \n");
        return 0.0;
    }
}

void ungets(char s[]){ /* ungets: push string back onto the input */
    int len = strlen(s);
    void ungetch(int);

    while (len > 0)
        ungetch(s[--len]);
}
