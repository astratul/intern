// 7.1 Standard Input and Output
/* The function tolower is defined in <ctype.h> ; it converts an upper case letter to lower case, and returns other characters untouched. As we mentioned earlier, ``functions'' like getchar and putchar in <stdio.h> and tolower in <ctype.h> are often macros, thus avoiding the overhead of a function call per character. We will show how this is done in Section 8.5. Regardless of how the <ctype.h> functions are implemented on a given machine, programs that use them are shielded from knowledge of the character set. */

#include <stdio.h>
#include <ctype.h>
int main(){ /* lower: convert input to lower case*/
    int c;
    while ((c = getchar()) != EOF)
        putchar(tolower(c));
    return 0;
}