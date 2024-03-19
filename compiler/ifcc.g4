grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' instruction* return_stmt '}' ;

function_def: TYPE FUNCNAME '(' declare_only_stmt? ')' '{' instruction* return_stmt? '}' ;
function_call: FUNCNAME '(' expr* ')' ';' ;

instruction: declare_stmt ';' | assignment_stmt ';' | selection_stmt ;

assignment_stmt: lvalue '=' rvalue ;

selection_stmt: 'if' '(' expr ')' instruction ('else' instruction)?
;

declare_stmt: TYPE declare;
declare: (lvalue | assignment_stmt) (',' declare)? ;

declare_only_stmt: TYPE lvalue (',' declare_only_stmt)? ;
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
| (CONST | VARNAME)					# expr_atom
;

TYPE: 'int' ;
RETURN : 'return' ;
VARNAME: [_a-zA-Z][_a-zA-Z0-9]*;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);



