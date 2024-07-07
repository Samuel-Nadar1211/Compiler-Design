%{
	#include <stdio.h>
	
	void yyerror();
	int yylex(void);
	
	int i = 0;
	char identifier[20];
%}

%token digit id

%%

S : id '=' E	{ printf ("%s = t%d\n", identifier, i-1); }
;
E : E '+' T	{ $$=i++; printf ("t%d = t%d + t%d;\n", $$, $1, $3 );}
    | E '-' T	{ $$=i++; printf ("t%d = t%d - t%d;\n", $$, $1, $3 );}
    | T		{ $$=$1; }
;
T : T '*' F	{ $$=i++; printf ("t%d = t%d * t%d;\n", $$, $1, $3 );}
    | T '/' F	{ $$=i++; printf ("t%d = t%d / t%d;\n", $$, $1, $3 );}
    | F		{ $$=$1; }
;
F : P '^' F	{ $$=i++; printf ("t%d = t%d ^ t%d;\n", $$, $1, $3 );}
    | P		{ $$=$1; }
;
P : '(' E ')'	{ $$=$2; }
    | digit	{ $$=i++; printf ("t%d = %d;\n", $$, $1); }
;

%%


void yyerror()
{
	printf ("Error\n");
}

int main()
{
	printf ("Enter an expression : \n");
	yyparse();
	return 0;
}
