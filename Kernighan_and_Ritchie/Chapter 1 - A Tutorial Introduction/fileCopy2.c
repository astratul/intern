#include <stdio.h>
/* copy input to output; 2nd version
*/
int main()
{
int c;
while ((c = getchar() )!= EOF)
	{putchar(c);
	printf(" --> \tThe value of getchar() != EOF is %d \n", c != EOF);
	printf("\nThe value of EOF is: %d\t\n", EOF);
	}
printf("\nThe value of getchar() != EOF is %d \n", c != EOF);

// Exercise 1-7. Write a program to print the value of EOF .
printf("\nThe value of EOF is: %d\t", EOF);


return 0;
}
