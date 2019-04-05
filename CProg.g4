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
         | expr ';'
         | ';' ;
return_statement: RETURN expr ;
declaration: type_name declarator (',' declarator)* ;
declarator: IDENTIFIER
          | assignment ;

assignment: IDENTIFIER OP_ASGN expr ;

expr: asgn_expr ;

// binary operators r-t-l
asgn_expr: asgn_lhs* or_expr ;
asgn_lhs: IDENTIFIER asgn_op ;
asgn_op: OP_ASGN ;

// binary operators
or_expr: and_expr or_rhs* ;
or_rhs: or_op and_expr ;
or_op: OP_OR ;

// binary operators
and_expr: bor_expr and_rhs* ;
and_rhs: and_op bor_expr ;
and_op: OP_AND ;

// binary operators
bor_expr: xor_expr bor_rhs* ;
bor_rhs: bor_op xor_expr ;
bor_op: OP_BOR ;

// binary operators
xor_expr: band_expr xor_rhs* ;
xor_rhs: xor_op band_expr ;
xor_op: OP_XOR ;

// binary operators
band_expr: eq_expr band_rhs* ;
band_rhs: band_op eq_expr ;
band_op: OP_BAND ;

// binary operators
eq_expr: rel_expr eq_rhs* ;
eq_rhs: eq_op rel_expr ;
eq_op: OP_EQ | OP_NE ;

// binary operators
rel_expr: add_expr rel_rhs* ;
rel_rhs: rel_op add_expr ;
rel_op: OP_LT | OP_LTE | OP_GT | OP_GTE ;

// binary operators
add_expr: mult_expr add_rhs* ;
add_rhs: add_op mult_expr ;
add_op: OP_PLUS | OP_MINUS ;

// binary operators
mult_expr: unary_expr mult_rhs* ;
mult_rhs: mult_op unary_expr ;
mult_op: OP_MUL | OP_DIV | OP_MOD ;

// prefix unary operators
unary_expr: unary_lhs* postfix_expr ;
unary_lhs: unary_op ;
unary_op: OP_PP | OP_MM | OP_PLUS | OP_MINUS | OP_NOT | OP_BNOT ;

// suffix unary operators
postfix_expr: atom_expr postfix_rhs* ;
postfix_rhs: postfix_op ;
postfix_op: OP_PP | OP_MM | func_call ;

func_call: '(' arg_list? ')' ;

arg_list: expr (',' expr)* ;

atom_expr: INT_LITERAL
         | CHAR_LITERAL
         | IDENTIFIER
         | '(' expr ')' ;


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
