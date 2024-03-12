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



## Ressources

- see example of [c.g4](https://github.com/antlr/grammars-v4/blob/master/c/C.g4)
- 
- 

Precedence table
0. :=
1. lambda
2. if - else
3. or
4. and
5. not x
6. in, not in, is, is not, <, <=, >, >=, !=, ==
7. |
8. ^
9. &
10. <<, >>
11. +, -
12. *, @, /, //, %
13. +x, -x, ~x
14. **
14. await x
15. x[index], x[index:index], x(arguments...), x.attribute
16. (expressions...), [expressions...], {key: value...}, {expressions...}







