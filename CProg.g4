grammar CProg ; // an ambitious grammar for the C programming language

// ----------------------------------------------------------------------- axiom

program: PREPROC_DIR* funcdef* ;

// -------------------------------------------------------- non-terminal symbols

funcdef: type_name IDENTIFIER '(' arg_decl_list? ')' compound_statement ;
arg_decl_list: VOID_TYPE_NAME | type_name IDENTIFIER (',' type_name IDENTIFIER)* ;
type_name: VOID_TYPE_NAME
         | CHAR_TYPE_NAME
         | INT_TYPE_NAME
         | INT_16_TYPE_NAME
         | INT_32_TYPE_NAME
         | INT_64_TYPE_NAME
         | IDENTIFIER ;

statement: compound_statement
         | return_statement ';'
         | declaration ';'
         | if_condition
         | for_statement
         | while_statement
         | expr ';'
         | ';' ;

compound_statement: '{' statement* '}' ;
return_statement: RETURN expr ;
declaration: type_name declarator (',' declarator)* ;
declarator: IDENTIFIER
          | assignment ;
if_condition: IF '(' expr ')' statement (ELSE statement)? ;
for_statement: FOR '(' (declaration | expr)? ';' expr? ';' expr? ')' statement ;
while_statement: WHILE '(' expr ')' statement ;

assignment: IDENTIFIER OP_ASGN expr ;

expr: PAR_OP='(' expr ')'
    | expr POSTFIX_OP=(OP_PP | OP_MM)
    | IDENTIFIER ARG_OP='(' arg_list? ')'
    |<assoc=right> PREFIX_OP=(OP_PP | OP_MM | OP_PLUS | OP_MINUS | OP_NOT | OP_BNOT) expr
    | expr (OP_MUL | OP_DIV | OP_MOD) expr
    | expr (OP_PLUS | OP_MINUS) expr
    | expr (OP_LT | OP_LTE | OP_GT | OP_GTE) expr
    | expr (OP_EQ | OP_NE) expr
    | expr OP_BAND expr
    | expr OP_BXOR expr
    | expr OP_BOR expr
    | expr OP_AND expr
    | expr OP_OR expr
    |<assoc=right> IDENTIFIER OP_ASGN expr
    | INT_LITERAL
    | CHAR_LITERAL
    | IDENTIFIER ;

arg_list: expr (',' expr)* ;

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
OP_BXOR: '^' ;
INT_LITERAL: [0-9]+ ;
CHAR_LITERAL: '\'' '\\'? . '\'' ;
IDENTIFIER: [a-zA-Z_][0-9a-zA-Z_]* ;
