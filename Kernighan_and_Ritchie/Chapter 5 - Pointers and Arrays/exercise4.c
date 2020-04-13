/*
Exercise 5-4. Write the function strend(s,t) , which returns 1 if the string t occurs at the
end of the string s , and zero otherwise.
*/

#include<stdio.h>
#define MAXLINE 1000

int getline2(char s[],int lim){
    int c,i;
    
    for(i=0;i<lim-1 && (c=getchar()) !=EOF && c!='\n';++i)
        s[i] = c;
    if(c == '\n'){
        s[i] = c;
        ++i;
    }
    s[i] = '\0';
    return i;
}
int strlen2(char *t){
    char *p;
    p=t;
    while(*p!='\0')
        ++p;
    return p-t;
}
int strend(char *s,char *t){
    int len;
    len=strlen2(t);
    while(*s!='\0')
        ++s;
    --s;
    while(*t!='\0')
        ++t;     
    --t;
    for(int i=0;i<len;i++){
        if(*t==*s){
            t--;
            s--;
            // printf("t: %s, s: %s, len: %d\n", t, s, i);
        }
        else
            return 0;
    }
    return 1;
}

int main(){
    char s[MAXLINE],t[MAXLINE];
    int res;
    getline2(s,MAXLINE);
    getline2(t,MAXLINE);
    res = strend(s,t);
    printf("\nResult: %d!\n",res);
    return 0;
}
