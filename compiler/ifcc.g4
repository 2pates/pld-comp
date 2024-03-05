grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{'  declare_stmt return_stmt '}' ;

declare_stmt: TYPE lvalue '=' rvalue ;
return_stmt: RETURN CONST ';' ;

rvalue : CONST | lvalue ;
TYPE: 'int' ;
lvalue: VAR ;
VAR: [_a-zA-Z][a-zA-Z0-9_-]* ;
RETURN : 'return' ;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
