grammar CProg ; // an ambitious grammar for the C programming language

// ----------------------------------------------------------------------- axiom

program: PREPROC_DIR* funcdef* ;

// -------------------------------------------------------- non-terminal symbols

funcdef: type_name IDENTIFIER '(' ')' block ;
type_name: INT_TYPE_NAME ;
block: '{' statement* '}' ;
statement: return_statement ';'
         | declaration ';'
         | expression ';'
         | ';' ;
return_statement: RETURN expression ;
declaration: type_name (IDENTIFIER | assignment) (',' (IDENTIFIER | assignment))* ;
assignment: IDENTIFIER '=' expression ;
expression: expression '*' expression # expr_mul
          | expression '/' expression # expr_div
          | expression '%' expression # expr_mod
          | expression '+' expression # expr_add
          | expression '-' expression # expr_sub
          | '(' expression ')'        # expr_brk
          | assignment                # expr_ass
          | IDENTIFIER                # expr_id
          | INT_LITTERAL              # expr_lit
          ;

// -------------------------------------------------------------- skipped tokens

PREPROC_DIR: '#' ~[\r\n]* '\r'? '\n' ;
ONELINE_COMMENT: '//' ~[\r\n]* -> skip ;
MULTILINE_COMMENT: '/*' .*? '*/' -> skip ;
WHITESPACE: [ \t\r\n]+ -> skip ;

// ------------------------------------------------------------ terminal symbols

INT_TYPE_NAME: 'int' ;
RETURN: 'return' ;
INT_LITTERAL: [0-9]+ ;
IDENTIFIER: [a-zA-Z_][0-9a-zA-Z_]* ;
