//C Program for Recursive Descent Parser - Arithmetic Expression Validator

/*
Grammer-
E->E+T|T
T->T*F|F
F->i|(E)

After Eliminating Left Recursion
E->TE'
E'->+TE'|ε
T->FT'
T'->*FT'|ε
F->i|(E)
*/

#include<stdio.h>

int E(), Edash(), T(), Tdash(), F();

char *ip;
char string[50];

int E()
{
	printf("%s\t\tE->TE'\n", ip);
	if (T())
	{
		if (Edash())    return 1;
		else    return 0;
	}
	else    return 0;
}

int Edash()
{
	if(*ip == '+')
	{
		printf("%s\t\tE'->+TE'\n", ip);
		ip++;
		if(T())
		{
			if  (Edash())   return 1;
			else    return 0;
		}
		else	return 0;
	}
	else
	{
        printf("%s\t\tE'-> epsilon\n", ip);
	    return 1;
	}
}

int T()
{
	printf("%s\t\tT->FT'\n", ip);
	if (F())
	{
		if (Tdash())    return 1;
		else    return 0;
	}
	else    return 0;
}

int Tdash()
{
	if(*ip == '*')
	{
		printf("%s\t\tT'->*FT'\n", ip);
		ip++;
		if(F())
		{
			if  (Tdash())   return 1;
			else    return 0;
		}
		else	return 0;
	}
	else
	{
        printf("%s\t\tT'-> epsilon\n", ip);
	    return 1;
	}
}

int F()
{
	if (*ip == '(')
	{
		printf("%s\t\tF->(E) \n", ip);
		ip++;
		if (E())
		{
			if (*ip == ')')
			{
				ip++;
				return 1;
			}
			else    return 0;
		}
		else    return 0;
	}
	else if (*ip == 'i')
	{
		ip++;
		printf("%s\t\tF->i\n", ip);
		return 1;
	}
	else    return 0;
}


int main()
{
	printf("Enter the string\n"); 
	scanf("%s", string);
	ip = string;

	printf("\n\nInput\t\tAction\n--------------------------\n");
	if (E() && *ip == '\0')
	{
		printf("\n-------------------------\n");
		printf("String is successfully parsed");
	}
	else
	{
		printf("\n-------------------------\n");
		printf("Error in parsing string\n");
	}

    return 0;
}