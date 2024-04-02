# Projet compilateur

[doc x86](doc_x86.pdf)

## Choices

Here we put the non-programming choices we made

- variables can be only constitued of letters, `_` and numbers
- for the moment we can do multiple affectations
- we choose to change the memory_offset and the tmp_index before the actions
- 
- 
- 
- 
- 

## TODO
### Mandatory

- [ ] Data type char : *Roua & Gaspard*
- [x] Data type int
- [x] Variables
- [x] Integer constants
- [x] Basic arithmetic operations: +,-, *
- [x] Division and modulo (in IR) : *Bachir*
- [x] Bit-by-bit logical operations: |, &,ˆ
- [x] Comparison operations: ==, !=, <, >
- [x] Unary operations: ! and - and ~ (bonus)
- [x] Declaration of variables anywhere
- [x] Assignment (which, in C, also returns a value)
- [x] Possibility of initializing a variable when declaring it
- [x] Use of standard putchar and getchar functions for I/O
- [ ] Define functions with parameters, and int, char or void return type : *James*
- [x] Check consistency of function calls and parameters (reserved names)
- [x] Block structure using { and }
- [x] Support for variable scopes and shadowing
- [x] If, else, while control structures
- [x] Support for return expression anywhere
- [x] Check that a variable used in an expression has been declared
- [x] Check that a variable is not declared more than once
- [x] Check that a declared variable is used at least once

### Optional

- [ ] Retargeting to multiple architectures: x86, MSP430, ARM
- [ ] Double support with all implicit conversions
- [ ] Simple constant propagation
- [ ] Propagation of constant variables (with data-flow analysis)
- [ ] Arrays (one-dimensional)
- [ ] Pointers
- [ ] Break and continue
- [ ] Char data type width support (8-bit integer)
- [ ] Character strings represented by char arrays
- [ ] Switch...case
- [x] Lazy operators ||, && : *Myriam*
- [ ] Assignment operators +=, -= etc., increment ++ and decrement -- : *Myriam*





## Ressources

- see example of [c.g4](https://github.com/antlr/grammars-v4/blob/master/c/C.g4)
- 
- 

![operator priority](operator_priority.png "source - wikipedia")





