#include <unistd.h>
/* Example of an argument list */
/* Note that we need a program name for argv[0] */
char *const ps_argv[] =
    {"ps", "ax", 0};
/* Example environment, not terribly useful */
char *const ps_envp[] =
    {"PATH=/bin:/usr/bin", "TERM=console", 0};
/* Possible calls to exec functions */
execl("/bin/ps", "ps", "ax", 0);
execlp("ps", "ps", "ax", 0);
execle("/bin/ps", "ps", "ax", 0, ps_envp);
/* assumes ps is in /bin */
/* assumes /bin is in PATH */
/* passes own environment */
execv("/bin/ps", ps_argv);
execvp("ps", ps_argv);
execve("/bin/ps", ps_argv, ps_envp);