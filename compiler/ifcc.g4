grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' instruction* return_stmt '}' ;

instruction: declare_stmt | assignment_stmt ;

assignment_stmt: lvalue '=' rvalue ';' ;
declare_stmt: TYPE lvalue ';' ;
return_stmt: RETURN CONST ';' ;

rvalue : CONST | VARNAME | expr ;
lvalue: VARNAME ;

TYPE: 'int' ;
RETURN : 'return' ;
VARNAME: [_a-zA-Z][_a-zA-Z0-9]*;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);


