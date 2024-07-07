%{
	#include <string.h>
	#include <stdlib.h>
	#include "y.tab.h"
	
	extern int yylval;
	extern char identifier[20];
%}


%%
[0-9]+	{
		yylval = atoi(yytext);
		return digit;
	}
[a-zA-Z]+	{
		strcpy (identifier, yytext);
		return id;
	}
\t	;
\n	return 0;
.	return yytext[0];
%%