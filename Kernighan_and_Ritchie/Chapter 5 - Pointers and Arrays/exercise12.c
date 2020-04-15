/* 

Exercise 5-12. Extend entab and detab to accept the shorthand entab -m +n to mean tab stops every n columns, starting at column m . Choose convenient (for the user) default behavior.

*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 100 /* maximum line size */
#define TABINC  8   /* default increment size */
#define YES 1
#define NO  0

void entab(int argc,char *argv[],char *tab);
void detab(char *tab);
int tabpos(int pos,char *tab);

int main(int argc,char *argv[]){
    char tab[MAXLINE+1];
    entab(argc,argv,tab);
    detab(tab);
    return 0;
}
/*  Write a program entab that replaces strings of blanks by the minimum number of abs and blanks to achieve the same spacing. Use the same tab stops as for detab. */

void entab(int argc,char *argv[],char *tab){
    int i,inc,pos;
    
    if(argc <= 1) /* default tab stops */
        for(i = 1; i <= MAXLINE; i++)
            if(i % TABINC == 0)
                tab[i] = YES;
            else
                tab[i] = NO;
    else if(argc == 3 && *argv[1] == '-' && *argv[2] == '+'){
        pos = atoi(&(*++argv)[1]);
        inc = atoi(&(*++argv)[1]);

        for(i=1;i<=MAXLINE;i++)
            if(i != pos)
                tab[i] = NO;
            else
            {
                tab[i] = YES; 
                pos += inc;
            }
    }
    else{
        for(i =1 ; i <= MAXLINE; i++)
            tab[i] = NO; /* turn off all tab stops */
        
        while(--argc > 0){ /* walk through argument list */
            pos = atoi(*++argv);
            if( pos > 0 && pos <= MAXLINE)
                tab[pos] = YES;
        }
    }
}

/* Write a program detab that replaces tabs in the input with the proper number of blanks to space to the next tab stop. */

void detab(char *tab)
{
    int c,pos;
    int nb = 0; /* # of blanks necessary */
    int nt = 0; /* # of tabs necessary */

    for(pos = 1; (c=getchar()) != EOF; pos++)
        if(c == ' '){
            if(tabpos(pos,tab) == NO)
                ++nb;
            else{
                nb = 0;  /* reset the number of blanks */
                ++nt;  /* one more tab */
            }
        }
        else{
            for(; nt > 0;nt--)
                putchar('\t'); /* output tabs */
            if(c == '\t')
                nb = 0;
            else
                for(;nb >0;nb--)
                    putchar(' ');
            putchar(c);
            
            if(c == '\n')
                pos = 0;
            else if(c == '\t')
                while(tabpos(pos,tab) != YES)
                    ++pos;
        }
}

int tabpos(int pos,char *tab)
{
    if ( pos > MAXLINE)
        return YES;
    else
        return tab[pos];
}
