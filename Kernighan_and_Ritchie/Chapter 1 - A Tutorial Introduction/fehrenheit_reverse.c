#include <stdio.h>
#define LOWER 0
#define UPPER 300
#define STEP 20

int main()
{
int fahr,c;
// printf("Introduceti temperatura in Fahrenheit: ");
// scanf("%3d", &c);
printf("Temperatura in Celsius : %6.1f\n", (5.0/9.0)*(c-32));
printf("Fahrenheit\tCelsius\n");
for (fahr = UPPER; fahr >= LOWER; fahr = fahr - STEP)
printf("%3d \t\t %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
return 0;
}