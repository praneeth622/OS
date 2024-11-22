%{
#include <string.h>
#include "sql.tab.h"
%}

%%

[ \t]+                 ;  // Skip whitespace

"SELECT"               { yylval.str = strdup(yytext); return SELECT; }
"FROM"                 { yylval.str = strdup(yytext); return FROM; }
"WHERE"                { yylval.str = strdup(yytext); return WHERE; }
"ORDER"                { yylval.str = strdup(yytext); return ORDER; }
"BY"                   { yylval.str = strdup(yytext); return BY; }
"AND"                  { yylval.str = strdup(yytext); return AND; }
"OR"                   { yylval.str = strdup(yytext); return OR; }
"EXIT"                 { yylval.str = strdup(yytext); return EXIT; }

"*"                    { yylval.str = strdup(yytext); return ASTERISK; }
"="                    { yylval.str = strdup(yytext); return EQUALS; }
">="                   { yylval.str = strdup(yytext); return GREATER_EQUAL; }
"<="                   { yylval.str = strdup(yytext); return LESS_EQUAL; }
"<>"                   { yylval.str = strdup(yytext); return NOT_EQUAL; }
">"                    { yylval.str = strdup(yytext); return GREATER_THAN; }
"<"                    { yylval.str = strdup(yytext); return LESS_THAN; }
","                    { yylval.str = strdup(yytext); return COMMA; }
";"                    { yylval.str = strdup(yytext); return SEMICOLON; }

\'([^']|\'\')*\'       { 
    // Remove surrounding quotes and replace double quotes with single quote
    char *tmp = yytext + 1;
    tmp[strlen(tmp)-1] = '\0';
    char *processed = malloc(strlen(tmp) + 1);
    char *out = processed;
    while (*tmp) {
        if (*tmp == '\'' && *(tmp+1) == '\'') {
            *out++ = '\'';
            tmp += 2;
        } else {
            *out++ = *tmp++;
        }
    }
    *out = '\0';
    yylval.str = processed;
    return STRING; 
}

[0-9]+                 { yylval.str = strdup(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = strdup(yytext); return IDENTIFIER; }

.                      { printf("Unrecognized character: %s\n", yytext); }

%%

int yywrap() {
    return 1;
}