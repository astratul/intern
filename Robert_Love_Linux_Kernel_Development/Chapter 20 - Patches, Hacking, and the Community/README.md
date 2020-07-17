# Patches, Hacking, and the Community

One of the greatest benefits of Linux is the large community of users and developers that surround it.The community provides eyes to check your code, experts to provide advice, and users to test and report issues. Most important, the community is the final arbiter of what code is accepted into Linus’ official kernel tree. Understanding how the system works is extremely important.

## The Community

If the Linux kernel community had to call somewhere home, it would be the Linux Kernel Mailing List.The Linux Kernel Mailing List (or as the regulars abbreviate it, just lkml) is the location of the majority of the announcements, discussions, debates, and flame wars over the kernel.
You can subscribe by sending the following message in plain text to majordomo@vger.kernel.org :

subscribe linux-kernel <your@email.address>

You can get more information at http://vger.kernel.org/ and a FAQ is available at http://www.tux.org/lkml/.

## Linux Coding Style

```consistency of coding style is crucial to productivity in coding```
Consistency implies familiarity, which leads to ease of reading, lack of confusion, and further expectations that code will continue to follow a given style.This increases the number of developers who can read your code, and the amount of code in which you can read. In an open-source project, the more eyes the better.

### Indention

The stylistic convention for indention is to use tabs that are eight characters in length.

### Switch Statements

Subordinate case labels should be indented to the same level as the parent switch statement, which helps alleviate the impact of eight character tabs.

### Spacing

This section covers the spacing around symbols and keywords, not the spacing used in indention, which we covered in the last two sections. Generally speaking, Linux coding style dictates spaces around most keywords and no spaces between functions and their parentheses. Conversely, functions, macros, and keywords that look like functions—such as sizeof .. have no space between the keyword and the parenthesis. Within parentheses, there is no space proceeding or preceding the argument, as previously shown.Around most binary and tertiary operators, put a space on either side of the operator.Conversely, around most unary operators, put no space between the operator and the operand.
```
if (foo)
while (foo)
for (i = 0; i < NR_CPUS; i++)
switch (foo)
wake_up_process(task);
size_t nlongs = BITS_TO_LONG(nbits);
int len = sizeof(struct task_struct);
typeof(*p)
__alignof__(struct sockaddr *)
__attribute__((packed))
int prio = task_prio( task ); /* BAD STYLE! */
int sum = a + b;
int product = a * b;
int mod = a % b;
int ret = (bar) ? bar : 0;
return (ret ? 0 : size);
if (!foo)
int len = foo.len;
struct work_struct *work = &dwork->work;
// Placing a space on either side of the dereference operator is incorrect style:
char * strcpy(char * dest, const char * src) /* BAD STYLE */
// Also incorrect is the C++ style of placing the dereference operator next to the type:
char* strcpy(char* dest, const char* src) /* BAD STYLE */
// The correct style is
char *strcpy(char *dest, const char *src)
```

### Braces

Brace placement is personal, and few technical reasons exist for one convention over the other, but we have to agree on something.The accepted kernel style is to put the opening brace on the first line, at the end of the statement.The closing brace goes on a new line as the first character. Following is an example:
``` 
if (strncmp(buf, “NO_”, 3) == 0) {
neg = 1;
cmp += 3;
}
```

### Line Length

Lines of source code should be kept to fewer than 80 characters in length.This allows code to fit lengthwise on a standard 80×24 terminal.
There is no accepted standard on what to do in cases where code absolutely must wrap 80 characters. Some developers just allow the line to wrap, letting their editor handle the chore of displaying the code in a readable fashion. Other developers break up the lines, manually inserting line breaks where appropriate, perhaps starting each new line a tab stop over from the original.

### Naming

No name should employ CamelCase, Studly Caps, or other mixed case schemes. Calling a local variable idx or even just i is perfectly fine if it is clear what it does.A cute name such as theLoopIndex is unacceptable.Hungarian notation (encoding the variable type in the variable name) is unnecessary and should never be used.
```This is C, not Java; Unix, not Windows.```
Nonetheless, global variables and functions should have descriptive names, in lowercase and delimited via an underscore as needed. Calling a global function atty() is confusing; a name such as get_active_tty() is much more acceptable.

### Functions

As a rule of thumb, functions should not exceed one or two screens of text and should have fewer than ten local variables.A function should do one thing and do it well.There is no harm in breaking a function into a series of smaller functions.

### Comments

Commenting your code is important, but the commenting must be done correctly. Generally, you want to describe what and why your code is doing what it is doing, not how it is doing it.The how should be apparent from the code itself. If not, you might need to rethink and refactor what you wrote.Additionally, comments should not include who wrote a function, the modification date, or other trivial nonsense. Such information is generally acceptable at the top of the source file, however.
The kernel uses C-style comments, even though gcc supports C++-style comments, too.The general style of a comment in the kernel resembles:
```
/*
* get_ship_speed() - return the current speed of the pirate ship
* We need this to calculate the ship coordinates. As this function can sleep,
* do not call while holding a spinlock.
*/
```
The kernel has a facility for self-generating documentation. It is based on GNOME-doc, but slightly modified and renamed Kernel-doc.To create the standalone documentation in HTML format, run
```
make htmldocs
//Or for postscript
make psdocs
```

### Typedefs

The Linux kernel developer community employs a strong dislike of the typedef operator.Their rationale is
* typedef hides the real type of data structures.
* Because the type is hidden, code is more prone to do bad things, such as pass a
structure by value on the stack.
* typedef is just being lazy.
Therefore, to avoid ridicule, avoid typedef .

### Use Existing Routines

Do not reinvent the wheel.The kernel provides string manipulation functions, compression routines, and a linked list interface, so use them.
Do not wrap existing interfaces in generic interfaces.

### Minimize ifdefs in the Source

Putting ifdef preprocessor directives directly in the C source is frowned upon.
You should never do something like the following in your functions:
```
...
#ifdef CONFIG_FOO
foo();
#endif
...

// Instead, define foo() to nothing if CONFIG_FOO is not set:

#ifdef CONFIG_FOO
static int foo(void)
{
/* .. */
}
#else
static inline int foo(void) { }
#endif /* CONFIG_FOO */
// Then, you can unconditionally call foo() . Let the compiler do the work for you.
```

### Structure Initializers

Labeled identifiers need to be used to initialize structures.This is good because it prevents structure changes from resulting in incorrect initialization. It also enables values to be omitted. Unfortunately, C99 adopted quite an ugly format for labeled identifiers, and gcc is deprecating usage of the previous GNU-style labeled identifier, which was rather handsome. Consequently, kernel code needs to use the new C99 labeled identifier format, however ugly it is:
```
struct foo my_foo = {
    .a  = INITIAL_A,
    .b  = INITIAL_B,
};
```

### Fixing Up Code Ex Post Facto

If a pile of code falls into your lap that fails to even mildly resemble the Linux kernel coding style, do not fret.A little elbow grease and the indent utility can make everything perfect. indent , an excellent GNU utility found on most Linux systems, formats source according to given rules.The default settings are for the GNU coding style, which is not too pretty.To get the utility to follow the Linux kernel style, execute the following:
```
indent -kr -i8 -ts8 -sob -l80 -ss -bs -psl <file>
```
This instructs the utility to format the code according to the kernel coding style. Alternatively, the script scripts/Lindent automatically invokes indent with the desired options.

## Chain of Command

Most parts of the kernel have an associated maintainer.The maintainer is the individual (or individuals) who is in charge of specific parts of the kernel.

## Submitting Bug Reports

If you encounter a bug, the best course of action is to write a fix, create a patch, test it, and submit it as discussed in the following sections. Of course, you can also report the problem and get someone to fix it for you.

## Patches

All changes to the Linux kernel are distributed in the form of patches, which are the output of the GNU diff (1) program in a form that is readable by the patch (1) program.

### Generating Patches

The simplest way to generate a patch is to have two source trees, one that is the vanilla stock kernel and another that is the stock tree with your modifications.A common scheme is to name the stock tree linux-x.y.z (which is what the source tarball extracts to, initially) and to name your modified tree simply linux .Then, to generate a patch of the two trees, issue the following command from one directory below your trees:
```
diff -urN linux-x.y.z/ linux/ > my-patch
```
* The -u flag specifies that the unified diff format should be used.
* The -r flag instructs diff to recursively diff all directories.
* The -N flag specifies that new files in the modified tree should be included in the diff.
A useful utility is diffstat , which generates a histogram of a patch’s changes (line additions and removals).To generate the output on one of your patches, do
```
diffstat -p1 my-patch
```

### Generating Patches with Git

Generating patches with Git is an easy two-part process. 
* First, you
need to author and then locally commit your changes. Making changes to a Git tree is the same as a standard source tree.You do not need to do anything special to edit a file stored in Git.After you make your changes, you need to commit them to your Git repository:
```
git commit -a
```
The -a flag instructs Git to commit all your changes.
* When you have a commit (or two) in your tree, you can generate a patch for each commit, which you can treat as you do the patches described in the previous section:
```
git format-patch origin
```

### Submitting Patches

Patches should be generated as described in the previous section. If the patch touches a specific driver or subsystem, the patch should be sent to the maintainer listed in MAINTAINER .
Either way, the Linux Kernel Mailing List at linux-kernel@vger.kernel.org should be carbon copied.The patch should be sent to the kernel maintainer (for example, Linus) only after extensive discussion, or if the patch is trivial and clearly correct. Typically, the subject line of the email containing the patch is of the form “[PATCH] brief description.”The body of the email describes in technical detail the changes your patch makes and the rationale behind them. Be as specific as possible. Somewhere in the email, note the kernel version against which the patch was created.

