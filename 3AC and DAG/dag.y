%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	
	void yyerror (const char *str);
	int yylex (void);
	
	struct node
	{
		int number;
		struct node *left, *right;
		int printed;
		const char *value;
	};
	
	struct node *nodelist[100];
	int node_count = 0;
%}


%start S
%union {
	char *text;
	struct node *n;
}
%token <text> digit
%type <n> S E T P F


%%

S : E	{ printTree($$); }
;
E : E '+' T	{ $$ = makeNode ($1, $3, "+"); }
    | E '-' T	{ $$ = makeNode ($1, $3, "-"); }
    | T		{ $$ = $1; }
;
T : T '*' F	{ $$ = makeNode ($1, $3, "*"); }
    | T '/' F	{ $$ = makeNode ($1, $3, "/"); }
    | F		{ $$ = $1; }
;
F : P '^' F	{ $$ = makeNode ($1, $3, "^"); }
    | P		{ $$ = $1; }
;
P : '(' E ')'	{ $$ = $2; }
    | digit	{ $$ = makeNode (NULL, NULL, $1); }
;

%%


void yyerror (const char *str)
{
	printf ("Error : %s\n", str);
}

struct node* makeNode (struct node *left, struct node *right, const char* value)
{
	for (int i = 0; i < node_count; i++)
		if (strcmp (nodelist[i]->value, value) == 0 && nodelist[i]->left == left && nodelist[i]->right == right)
			return nodelist[i];
	
	struct node *n = (struct node*) malloc(sizeof(struct node));

	n->left = left;
	n->value = value;
	n->right = right;
	n->number = node_count++;
	n->printed = 0;
	
	nodelist[node_count - 1] = n;
	return n;
}

void printTree (struct node *n)
{
	if (!n || n->printed)
		return;
	
	n->printed = 1;
	
	printf ("\nNode : %d\nValue : %s\n", n->number, n->value);
	
	if (n->left)
		printf ("Left child at : %d\n", n->left->number);
	if (n->right)
		printf ("Right child at : %d\n", n->right->number);
	
	printTree (n -> left);
	printTree (n -> right);
}

int main()
{
	printf ("Enter an expression:\n");
	yyparse();
	return 0;
}
