/* 
Exercise 5-11. Modify the program entab and detab (written as exercises in Chapter 1) to accept a list of tab stops as arguments. Use the default tab settings if there are no arguments.

-> ex 1-20 and 1-21
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 100 /*maximum line size */
#define TABINC  8   /* default tab increment size */
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
    
    if(argc <= 1)   /* default tab stops */
        for(i =1; i <= MAXLINE; i++)
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
    else{    /* user provided tab stops */
  
        for(i=1;i<=MAXLINE;i++)
            tab[i] = NO; /* turn off all stops */

        while(--argc < 0){   /* walk through argument list */
            pos = atoi(*++argv);
            if(pos > 0 && pos <= MAXLINE)
                tab[pos] = YES;
        }
    }
}

/* Write a program detab that replaces tabs in the input with the proper number of blanks to space to the next tab stop. */

void detab(char *tab)
{
    int c,pos = 1;
    
    while((c=getchar())!=EOF){
        if(c == '\t'){
            do
                putchar(' ');
            while(tabpos(pos++,tab)!= YES);
        }
        else if(c == '\n'){
            putchar(c);
            pos =1 ;
        }
        else{
            putchar(c);
            ++pos;
        }
    }
}

int tabpos(int pos,char *tab){
    if(pos > MAXLINE)
        return YES;
    else
        return tab[pos];
}
