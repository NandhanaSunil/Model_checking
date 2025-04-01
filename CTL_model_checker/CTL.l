%{
#include "CTL.tab.h"
%}

%option noyywrap

%%
"|"         { return OR; }
"&"         { return AND; }
"~"         { return NOT; }
"->"        { return IMPLIES; }
"EX"        { return EX; }
"EG"        { return EG; }
"E"         { return E; }
"U"         { return U; }
"T"         { return T; }
"F"         { return F; }
[a-z]       { yylval.str = strdup(yytext); return PROP; }
"("         { return LPAREN; }
")"         { return RPAREN; }
"["         { return LBRACKET; }
"]"         { return RBRACKET; }
"AG"        { return AG; }
"AF"        { return AF; }
"AX"        { return AX; }
"A"        { return A; }
"EG"        { return EG; }
[ \t\n]     ;
.           { printf("Unexpected character: %c\n", yytext[0]); }
%%
