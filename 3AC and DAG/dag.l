%{
	#include <string.h>
	#include "y.tab.h"
%}


%%
[0-9]+	{
		yylval.text = strdup(yytext);
		return digit;
	}
\t	;
\n	return 0;
.	return yytext[0];
