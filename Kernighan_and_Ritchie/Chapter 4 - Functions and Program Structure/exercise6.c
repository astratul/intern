/*
Exercise 4-6. Add commands for handling variables. (It's easy to provide twenty-six variables with single-letter names.) Add a variable for the most recently printed value.

Variables: [a-z]
*/

#include<stdio.h>
#include<stdlib.h>

#define MAXOP 100
#define NUMBER '0'

int getop(char []);
void push(double);
double pop(void);

int main(void)
{
    int type,var = 0;
    double op2,v;
    char s[MAXOP];
    double variable[26];

    while((type = getop(s)) != EOF)
    {
        switch(type)
        {
                case NUMBER:
                        push(atof(s));
                        break;
                case '+':
                        push(pop()+pop());
                        break;
                case '*':
                        push(pop()*pop());
                        break;
                case '-':
                        op2 = pop();
                        push(pop()-op2);
                        break;
                case '/':
                        op2 = pop();
                        if(op2 != 0.0)
                            push(pop()/op2);
                        else
                            printf("error: zero divisor\n");
                        break;
                case '=':
                        pop();
                        if(var>='a' && var <='z')
                            variable[var-'a']=pop();
                        else
                            printf("error: variables must be [a-z]\n");
                            break;             
                case '\n':
                        v = pop();
                        printf("\t%.8g\n",v);
                        break;
                default:
                        if(type>='a' && type<='z')
                            push(variable[type-'a']);
                        // else if(type == 'v')
                        //     push(v);
                        else
                            printf("error: unknown command %s\n",s);
                        break;
        }
        var = type;
    }
    return 0;
}


#define MAXVAL 100

int sp = 0;
double val[MAXVAL];

void push(double f)
{
    if(sp < MAXVAL)
        val[sp++]=f;
    else
        printf("error:stack full, cant push %g\n",f);
}


double pop(void)
{
    if(sp>0)
        return val[--sp];
    else
    {
        printf("error: stack empty\n");
        return 0.0;
    }
}

#include<ctype.h>

int getch(void);
void ungetch(int);

int getop(char s[])
{
    int i,c;

    while((s[0] = c = getch()) == ' ' || c =='\t')
        ;
    s[1] = '\0';
    if(!isdigit(c) && c!='.')
        return c;
    
    i = 0;
    if(isdigit(c))
        while(isdigit(s[++i] =c =getch()))
            ;

    if(c=='.')
        while(isdigit(s[++i] = c=getch()))
            ;
    
    s[i] = '\0';
    if(c!=EOF)
        ungetch(c);
    return NUMBER;
}

#define BUFSIZE 100

char buf[BUFSIZE];
int bufp = 0;

int getch(void)
{
    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c)
{
    if(bufp >= BUFSIZE)
        printf("ungetch: too many characters\n");
    else
        buf[bufp++] = c;
}

