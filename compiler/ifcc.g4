grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' instruction* return_stmt '}' ;

instruction: declare_stmt | assignment_stmt ;

assignment_stmt: lvalue '=' rvalue ';' ;
declare_stmt: TYPE lvalue ';' ;
return_stmt: RETURN atomic_expr ';' ;

rvalue: expr ;
lvalue: VARNAME ;
atomic_expr: CONST | VARNAME ;

expr: OPU expr | expr OP expr | '(' expr ')' | atomic_expr ;

OPU: '-' | '~' ;
OP: '&' | '^' | '|' ; // AND > XOR > OR

TYPE: 'int' ;
RETURN : 'return' ;
VARNAME: [_a-zA-Z][_a-zA-Z0-9]*;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);



