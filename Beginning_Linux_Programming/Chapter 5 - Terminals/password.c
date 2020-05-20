#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#define PASSWORD_LEN 8
int main()
{
    struct termios initialrsettings, newrsettings;
    char password[PASSWORD_LEN + 1];
// get the current settings from the standard input and copy them into the termios structure that you created previously
    tcgetattr(fileno(stdin), &initialrsettings);
// Make a copy of the original settings to replace them at the end.Turn off the ECHO flag on the newrsettings and ask the user for his password:
    newrsettings = initialrsettings;
    newrsettings.c_lflag &= ~ECHO;
    printf("Enter password: ");
// set the terminal attributes to newrsettings and read in the password, reset the terminal attributes to their original setting and print the password to render all the previous security effort useless
    if (tcsetattr(fileno(stdin), TCSAFLUSH, &newrsettings) != 0)
    {
        fprintf(stderr,"Could not set attributes\n");
    }
    else
    {
        fgets(password, PASSWORD_LEN, stdin);
        tcsetattr(fileno(stdin), TCSANOW, &initialrsettings);
        fprintf(stdout, "\nYou entered % s\n", password);
    }
    exit(0);
}