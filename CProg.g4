grammar CProg ; // an ambitious grammar for the C programming language

// ----------------------------------------------------------------------- axiom

program: PREPROC_DIR* funcdef* ;

// -------------------------------------------------------- non-terminal symbols

funcdef: type_name IDENTIFIER '(' ')' block ;
type_name: VOID_TYPE_NAME
         | CHAR_TYPE_NAME
         | INT_TYPE_NAME
         | INT_16_TYPE_NAME
         | INT_32_TYPE_NAME
         | INT_64_TYPE_NAME
         | IDENTIFIER ;
block: '{' statement* '}' ;
statement: return_statement ';'
         | declaration ';'
         | int_expr ';'
         | ';' ;
return_statement: RETURN int_expr ;
declaration: type_name declarator (',' declarator)* ;
declarator: IDENTIFIER
          | assignment ;
assignment: IDENTIFIER '=' int_expr ;
int_expr: int_terms
        | assignment ;
int_terms: int_factors rhs_int_terms* ;
rhs_int_terms: OP_PLUS int_factors
             | OP_MINUS int_factors ;
int_factors: int_signed_atom rhs_int_factors* ;
rhs_int_factors: OP_MUL int_signed_atom
               | OP_DIV int_signed_atom
               | OP_MOD int_signed_atom ;
int_signed_atom: OP_MINUS int_signed_atom
               | OP_PLUS int_signed_atom
               | int_atom ;
int_atom: INT_LITERAL
        | CHAR_LITERAL
        | IDENTIFIER
        | '(' int_expr ')' ;

// -------------------------------------------------------------- skipped tokens

PREPROC_DIR: '#' ~[\r\n]* '\r'? '\n' ;
ONELINE_COMMENT: '//' ~[\r\n]* -> skip ;
MULTILINE_COMMENT: '/*' .*? '*/' -> skip ;
WHITESPACE: [ \t\r\n]+ -> skip ;

// ------------------------------------------------------------ terminal symbols

VOID_TYPE_NAME: 'void' ;
INT_TYPE_NAME: 'int' ;
INT_16_TYPE_NAME: 'int16_t' ;
INT_32_TYPE_NAME: 'int32_t' ;
INT_64_TYPE_NAME: 'int64_t' ;
CHAR_TYPE_NAME: 'char' ;
RETURN: 'return' ;
IF: 'if' ;
ELSE: 'else' ;
OP_PP: '++';
OP_MM: '--';
OP_PLUS: '+' ;
OP_MINUS: '-' ;
OP_NOT: '!';
OP_BNOT: '~';
OP_MUL: '*' ;
OP_DIV: '/' ;
OP_MOD: '%' ;
OP_LT: '<' ;
OP_GT: '>' ;
OP_LTE: '<=' ;
OP_GTE: '>=' ;
OP_EQ: '==' ;
OP_NE: '!=' ;
OP_ASGN: '=' ;
OP_AND: '&&' ;
OP_BAND: '&' ;
OP_OR: '||' ;
OP_BOR: '|' ;
OP_XOR: '^' ;
INT_LITERAL: [0-9]+ ;
CHAR_LITERAL: '\'' '\\'? . '\'' ;
IDENTIFIER: [a-zA-Z_][0-9a-zA-Z_]* ;
