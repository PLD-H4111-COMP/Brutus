grammar CProg  ;

prog: funcdef ;
funcdef: type identifier '(' ')' block ;
type: INT ;
identifier: MAIN ;
block: '{' statement '}' ;
statement: return_statement;
return_statement: RETURN INT_LITTERAL ';' ;

INT: 'int' ;
INT_LITTERAL: [0-9]+ ;
MAIN: 'main' ;
RETURN: 'return' ;
SPACE: (' ' | '\t') -> skip ;

