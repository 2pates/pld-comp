# Compiler project

## Team members
- AL JAMOUS Myriam
- BOUSLIMI Roua
- CHEVALIER Mathieu
- GUEDDOUDA Bachir
- MAISON Benjamin 
- SERPINET Gaspard
- SUDLOW James

> Flat and agile management, we all contributed to the project.

## User documentation

### Prerequisites
Before building the compiler, you should install antlr using your distribution's package manager, or by  running the provided shell script: `install-antlr.sh`. 

`Makefile` contains the actual build logic. The Makefile includes a .mk file that defines several variables (ANTLR, ANTLRJAR, ANTLRINC and  ANTLRLIB) indicating the location of various parts of the Antlr suite. You can (should) change those values to suit your installation, either by editing the Makefile and/or adding another .mk file.

### Build the compiler
First, go in the `compiler` directory containing the Makefile : `cd compiler`

This Makefile has multiple targets, including a description of the available targets using : `make help`

To compile, simply run : `make`

To execute tests, run : `make test`

> Note : to compile without debug traces, remove the `DTRACE` flag in the `CCFLAGS` variable in the Makefile

### Compile a program
Once you have written a C program, you can compile it in assembly with `build/ifcc program.c > program.s` 

You can then compile the resulting assembly code with gcc : `gcc -o executable program.s`

If you run `build/ifcc program.c` without parameters, you'll get an output containing the debug and the assembly code.

If you wish to obtain both assembly and debug in separate files, simply run: `build/ifcc program.c 1>program.s 2>debug.txt`

### Supported C features
The IFCC compiler supports a subset of the C language. Among its features, it manages the following elements: 
- int and char types (int is a 32-bit signed int)
- declaration and definition of variables anywhere. A declared variable can only be used in its declaration block or in blocks contained within this block (the scope). Double declaration in blocks of the same scope will cause an error. Declaring a variable but not using it will also cause an error.
- basic arithmetic operations (+, -, *, /, %) 
- bitwise operations (|, &, ˆ, >>, <<)
- comparison operations (==, !=, <, >, <=, >=)
- unary operators (!, -, ~)
- logical operations && and || 
- variable assignment returns a value
- putchar and getchar functions (with an ASCII char or int between 0 and 127, IR does not require the stdio.h include).
- block structures
- if, else, if-else, while control structures
- break, continue
- functions : up to 6 int or char variables. Returns int, char or void but does not check that a function returns the correct type. Returns 0 for a non-void function without return. Functions are defined after main. You can declare functions before main, but this has no impact.


### Supported targets
In its current version, our compiler targets the x86 architecture, but our intermediate representation enables it to be retargeted to other architectures by adding the corresponding assembly instructions.

## Developper documention
To generate the developper documentation, you should have `doxygen` installed. See https://www.doxygen.nl/download.html for installation.

Then in the `compiler` directory run : `make doc`

This will open the doxygen documentation in the browser.
