grammar CProg  ;

prog: preproc? funcdef* ;
preproc: PREPROC_DIR+ ;
funcdef: type identifier '(' ')' block ;
type: INT ;
identifier: IDENTIFIER ;
block: '{' statement* '}' ;
statement: return_statement ';'
         | declaration ';'
         | assignment ';'
         | ';' ;
return_statement: RETURN (identifier | INT_LITTERAL);
declaration: type (identifier | assignment) (',' (identifier | assignment))* ;
assignment: identifier '=' (identifier | INT_LITTERAL) ;

PREPROC_DIR: '#' ~[\r\n]* '\r'? '\n' ;
ONELINE_COMMENT: '//' ~[\r\n]* -> skip ;
MULTILINE_COMMENT: '/*' .*? '*/' -> skip ;
INT: 'int' ;
RETURN: 'return' ;
SPACE: [ \t\r\n]+ -> skip ;
INT_LITTERAL: [0-9]+ ;
IDENTIFIER: [a-zA-Z_][0-9a-zA-Z_]* ;

