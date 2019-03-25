grammar CProg ; // an ambitious grammar for the C programming language

// ----------------------------------------------------------------------- axiom

program: PREPROC_DIR* funcdef* ;

// -------------------------------------------------------- non-terminal symbols

funcdef: type_name IDENTIFIER '(' ')' block ;
type_name: INT_TYPE_NAME ;
block: '{' statement* '}' ;
statement: return_statement ';'
         | declaration ';'
         | int_expr ';'
         | ';' ;
return_statement: RETURN int_expr ;
declaration: type_name (IDENTIFIER | assignment) (',' (IDENTIFIER | assignment))* ;
assignment: IDENTIFIER '=' int_expr ;
int_expr: int_terms | assignment ;
int_terms: int_factors rhs_int_terms* ;
rhs_int_terms: OP_ADD int_factors
             | OP_SUB int_factors ;
int_factors: int_signed_atom rhs_int_factors* ;
rhs_int_factors: OP_MUL int_signed_atom
               | OP_DIV int_signed_atom
               | OP_MOD int_signed_atom ;
int_signed_atom: OP_SUB int_signed_atom
               | OP_ADD int_signed_atom
               | int_atom ;
int_atom: INT_LITTERAL
        | IDENTIFIER
        | '(' int_expr ')' ;

// -------------------------------------------------------------- skipped tokens

PREPROC_DIR: '#' ~[\r\n]* '\r'? '\n' ;
ONELINE_COMMENT: '//' ~[\r\n]* -> skip ;
MULTILINE_COMMENT: '/*' .*? '*/' -> skip ;
WHITESPACE: [ \t\r\n]+ -> skip ;

// ------------------------------------------------------------ terminal symbols

INT_TYPE_NAME: 'int' ;
RETURN: 'return' ;
OP_ADD: '+' ;
OP_SUB: '-' ;
OP_MUL: '*' ;
OP_DIV: '/' ;
OP_MOD: '%' ;
INT_LITTERAL: [0-9]+ ;
IDENTIFIER: [a-zA-Z_][0-9a-zA-Z_]* ;
