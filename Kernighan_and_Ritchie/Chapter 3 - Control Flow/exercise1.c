/*
Exercise 3-1. Our binary search makes two tests inside the loop, when one would suffice (at
the price of more tests outside.) Write a version with only one test inside the loop and
measure the difference in run-time.
*/

#include<stdio.h>
#include<time.h>

int binsearch1(int x,int v[],int n);
int binsearch2(int x,int v[],int n);
int main(void)
{   
    double time1=0.0, time2=0.0;
    int arr[]={1,8,11,22,33,44,55,66,77,88,99};
    clock_t start2=clock();
    printf("\nThe old binsearch = %d\n",binsearch1(44,arr,12));
    clock_t end2=clock();
    time2 += (double)(end2 - start2) / CLOCKS_PER_SEC;
    printf("The old function : %f.\n", time2);

    // int arr[]={1,8,11,22,33,44,55,66,77,88,99};
    clock_t start1=clock();
    printf("\nThe new binsearch = %d\n",binsearch2(44,arr,12));
    clock_t end1=clock();
    time1 += (double)(end1 - start1) / CLOCKS_PER_SEC;
    printf("The new function : %f.\n", time1);
    return 0;
}

int binsearch2(int x,int v[],int n)
{
    int low,high,mid; 
    low=0;
    high=n-1;  
    mid = ( low + high ) / 2;
    while(low < high && x != v[mid])
    {
        if( x > v[mid])
            low = mid+1;
        else
            high = mid -1;
        mid = ( low + high)/2;
    }
    if(x==v[mid])
        return mid;
    else
        return -1;
}
int binsearch1(int x,int v[],int n)
{
    int low, high, mid;
    low = 0;
    high = n - 1;
    while (low <= high) {
        mid = (low+high)/2;
        if (x < v[mid])
            high = mid + 1;
        else if (x > v[mid])
            low = mid + 1;
        else
        /* found match */
            return mid;
    }
    return -1;
/* no match */
}
