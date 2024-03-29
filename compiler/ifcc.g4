grammar ifcc;

axiom: prog EOF ;

prog: 'int' 'main' '(' ')' '{' statement* return_stmt '}' function_def*;

function_def: type VARNAME '(' declare_only_stmt? ')' '{' statement* return_stmt? '}' ;
function_call: VARNAME '(' expr* ')' ;

statement: instruction | block ;
instruction: declare_stmt ';' | assignment_stmt ';' | function_call ';'| selection_stmt | iterationStatement ;
block: '{' statement* '}' ;

assignment_stmt: lvalue '=' rvalue ;

selection_stmt: 'if' '(' expr ')' statement ('else' statement)? #selection_if ;

iterationStatement: 'while' '(' expr ')' statement #iteration_while ;

declare_stmt: type declare;
declare: (lvalue | assignment_stmt) (',' declare)? ;

declare_only_stmt: type lvalue (',' declare_only_stmt)? ;
return_stmt: RETURN expr ';' ;

rvalue: expr ;
lvalue: VARNAME ;

expr: '(' expr ')'					# expr_parenthesis
| OP=('-'|'~'|'!') expr				# expr_unaire
| expr OP=('*'|'/'|'%') expr		# expr_mult
| expr OP=('+'|'-') expr			# expr_add
| expr OP=('<'|'<='|'>'|'>=') expr	# expr_relational
| expr OP=('=='|'!=') expr			# expr_equality
| expr '&' expr						# expr_and
| expr '^' expr						# expr_xor
| expr '|' expr						# expr_or
| expr '&&' expr					# expr_lazy_and
| expr '||' expr					# expr_lazy_or
| assignment_stmt                   # expr_assignment
| (CONST | VARNAME)					# expr_atom
| function_call                     # expr_function
;

type: 'int'|'char';
RETURN : 'return' ;
VARNAME: [_a-zA-Z][_a-zA-Z0-9]*;
FUNCNAME: [_a-zA-Z][_a-zA-Z0-9]*;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
ONE_LINE_COMMENT : '//' .*? '\n' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);



