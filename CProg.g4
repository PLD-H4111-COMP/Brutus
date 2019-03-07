grammar CProg  ;

prog: funcdef* ;
funcdef: type identifier '(' ')' block ;
type: INT ;
identifier: IDENTIFIER ;
block: '{' statement '}' ;
statement: return_statement;
return_statement: RETURN INT_LITTERAL ';' ;

INT: 'int' ;
RETURN: 'return' ;
SPACE: [ \t\r\n] -> skip ;
INT_LITTERAL: [0-9]+ ;
IDENTIFIER: [a-zA-Z_][0-9a-zA-Z_]* ;

