grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' instruction* return_stmt '}' ;

instruction: declare_stmt | assignment_stmt ;

assignment_stmt: lvalue '=' rvalue ';' ;
declare_stmt: TYPE lvalue ';' ;
return_stmt: RETURN CONST ';' ;

rvalue : CONST | VARNAME ;
lvalue: VARNAME ;

TYPE: 'int' ;
VARNAME: [_a-zA-Z][_a-zA-Z0-9]* ;
RETURN : 'return' ;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);

