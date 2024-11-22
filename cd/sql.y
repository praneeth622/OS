%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex();
%}

%union {
    char *str;
}

%token <str> SELECT FROM WHERE ORDER BY AND OR EXIT
%token <str> IDENTIFIER NUMBER STRING
%token <str> EQUALS GREATER_THAN LESS_THAN GREATER_EQUAL LESS_EQUAL NOT_EQUAL
%token <str> ASTERISK COMMA SEMICOLON

%type <str> select_stmt select_list from_clause where_clause_opt order_by_clause_opt condition expression value operator

%%

query:
    select_stmt SEMICOLON                { printf("SQL query parsed successfully.\n"); free($1); }
    | EXIT SEMICOLON                     { printf("Exiting.\n"); exit(0); }
    ;

select_stmt:
    SELECT select_list from_clause where_clause_opt order_by_clause_opt
    ;

select_list:
    ASTERISK                            { /* Handle '*' */ }
    | IDENTIFIER                        { /* Single column */ free($1); }
    | select_list COMMA IDENTIFIER      { /* Multiple columns */ free($3); }
    ;

from_clause:
    FROM IDENTIFIER                      { /* Table name */ free($2); }
    ;

where_clause_opt:
    /* empty */                          { /* Optional WHERE clause */ }
    | WHERE condition                    { /* WHERE clause */ }
    ;

order_by_clause_opt:
    /* empty */                          { /* Optional ORDER BY clause */ }
    | ORDER BY IDENTIFIER                { /* ORDER BY clause */ free($3); }
    ;

condition:
    expression                           { /* Single condition */ }
    | condition AND expression           { /* AND condition */ }
    | condition OR expression            { /* OR condition */ }
    ;

expression:
    IDENTIFIER operator value            { 
        /* Expression */ 
        free($1);  // Free identifier
        free($2);  // Free operator
        free($3);  // Free value
    }
    ;

operator:
    EQUALS                               { $$ = $1; }
    | GREATER_THAN                       { $$ = $1; }
    | LESS_THAN                          { $$ = $1; }
    | GREATER_EQUAL                      { $$ = $1; }
    | LESS_EQUAL                         { $$ = $1; }
    | NOT_EQUAL                          { $$ = $1; }
    ;

value:
    NUMBER                               { $$ = $1; }
    | STRING                             { $$ = $1; }
    | IDENTIFIER                         { $$ = $1; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main(void) {
    printf("Enter SQL queries (type 'exit;' to quit):\n");
    while (1) {
        yyparse();
    }
    return 0;
}