#include <stdio.h>
char data[4096];
typedef struct
{
    char *data;
    int key;
} item;

item array[] = {
    {"bill", 3},
    {"neil", 4},
    {"john", 2},
    {"rick", 5},
    {"alex", 1},
};

void sort(a, n)
    item *a;
{
    int i = 0, j = 0;
    int s = 1;
    for (; i < n && s != 0; i++)
    {
        s = 0;
        for (j = 0; j < n-1; j++)
        {
            if (a[j].key > a[j + 1].key)
            {
                item t = a[j];
                a[j] = a[j + 1];
                a[j + 1] = t;
                s++;
            }
        }
        n--;
    }
}


void main()
{
    int i;
    sort(array, 5);
    for (i = 0; i < 5; i++)
        printf("array[%d] = {%s, %d}\n", i, array[i].data, array[i].key);
}