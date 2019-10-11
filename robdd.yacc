%{
	#include "prop.h"
	#include <vector>
	#include <map>
	#include <string.h>
	#include <stdio.h>
	vector <prop *> prop_dump;
	map <string, int> id_map;
    extern int yylex();
    void yyerror(const char *s) { printf("ERROR: %s\n", s); }

%}

%union {
    prop *  p;
    std::vector<prop*> * vprop;
    std::string *string;
    int token;
}

%token <string> PROP_ID 
%token <token> PROP_AND PROP_NOT PROP_OR PROP_IMP LPAREN RPAREN

%left PROP_NOT PROP_IMP PROP_AND PROP_OR
%type <vprop> prop_list
%type <p> prp

%start prop_list

%%

prop_list : prp {$$ = new vector <prop*>; $$->push_back($1); prop_dump.push_back($1);} 
			| prop_list prp {$1->push_back($2);$$ = $1; prop_dump.push_back($2);} ;

prp : LPAREN prp RPAREN {$$ = $2;} 
	|PROP_NOT prp  {	
	$$ = negation($2);	}
	|  prp PROP_AND prp  {
		$$ = andop($1, $3);
	}
	| PROP_ID{
		$$ = new prop();
		if(id_map.count(*$1) == 0){
			$$->type = id_map.size() + 2;
			id_map[*$1] = $$->type;
		}
		else{
			$$->type = id_map[*$1];
		}
	}
	| prp PROP_IMP prp{
		$$ = implication($1,$3);
	}
	| prp PROP_OR prp{
		vector<prop*> temp;
		temp.push_back($1);
		temp.push_back($3);
		$$ = disjunction(temp);
	}
	;

%%