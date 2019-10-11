%{
#include <string>
#include "prop.h"
#include "parser.hpp"
#include <stdio.h>
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
extern "C" int yywrap() { }
%}

%%

[ \t\n]						;
"and"						return PROP_AND;
"not"						return PROP_NOT;
"or"						return PROP_OR;
"->"						return PROP_IMP;
"("							return LPAREN;
")"	 						return RPAREN;
[a-zA-Z][a-zA-Z0-9]*		SAVE_TOKEN; return PROP_ID;
.							printf("Unknown token %s\n", yytext); yyterminate();

%%

