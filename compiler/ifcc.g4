grammar ifcc;

axiom: prog EOF ;

prog: function_dec* 'int' 'main' '(' ')' block function_def*;
function_dec: type VARNAME '(' declare_only_stmt? ')' ';' ;
function_def: type VARNAME '(' declare_only_stmt? ')' block ;
function_call: VARNAME '(' ((expr',')*expr|) ')' ;

statement: instruction | block ;
instruction: declare_stmt ';' | assignment_stmt ';' | function_call ';'| return_stmt | selection_stmt | iterationStatement ;
block: '{' statement* '}' ;

assignment_stmt:
lvalue OP=('++' | '--')                     # post_incrementation
| OP=('++' | '--') lvalue                   # pre_incrementation
| lvalue '=' rvalue                         # assignment_equal
| lvalue OP=('+=' | '-=') rvalue            # assignment_add
| lvalue OP=('*=' | '/=' | '%=') rvalue     # assignment_mult
;

selection_stmt: 'if' '(' expr ')' statement ('else' statement)? #selection_if ;

iterationStatement: 'while' '(' expr ')' statement #iteration_while ;

declare_stmt: type (lvalue | assignment_stmt) (',' (lvalue | assignment_stmt))* ;
declare_only_stmt: type lvalue (',' declare_only_stmt)? ;

return_stmt: RETURN expr ';' ;
rvalue: expr ;
lvalue: pointer_type? VARNAME ;

expr: '(' expr ')'					        # expr_parenthesis
| OP=('-'|'~'|'!') expr				        # expr_unaire
| expr OP=('*'|'/'|'%') expr		        # expr_mult
| expr OP=('+'|'-') expr			        # expr_add
| expr OP=('<'|'<='|'>'|'>=') expr	        # expr_relational
| expr OP=('=='|'!=') expr			        # expr_equality
| expr '&' expr						        # expr_and
| expr '^' expr						        # expr_xor
| expr '|' expr						        # expr_or
| expr '&&' expr					        # expr_lazy_and
| expr '||' expr					        # expr_lazy_or
| assignment_stmt                           # expr_assignment
| function_call                             # expr_function
| (CONST_INT|CONST_CHAR|CONST_VOID|VARNAME) # expr_atom;

pointer_type: '*';
type: 'int'|'char'|'void';
RETURN: 'return' ;
VARNAME: [_a-zA-Z][_a-zA-Z0-9]*;
FUNCNAME: [_a-zA-Z][_a-zA-Z0-9]*;
CONST_INT: [0-9]+;
CONST_CHAR: '\'' . '\'';
CONST_VOID: 'NULL';
COMMENT: '/*' .*? '*/' -> skip;
ONE_LINE_COMMENT: '//' .*? '\n' -> skip;
DIRECTIVE: '#' .*? '\n' -> skip;
WS: [ \t\r\n] -> channel(HIDDEN);


