grammar CProg  ;

prog: funcdef ;
funcdef: type identifier '(' ')' block ;
type: INT ;
identifier: MAIN ;
block: '{' statement '}' ;
statement: return_statement;
return_statement: RETURN INT_LITTERAL ';' ;

INT: 'int' ;
MAIN: 'main' ;
RETURN: 'return' ;
SPACE: (' ' | '\t') -> skip ;
INT_LITTERAL: [0-9]+ ;
IDENTIFIER: [a-zA-Z][0-9a-zA-Z]* ;

