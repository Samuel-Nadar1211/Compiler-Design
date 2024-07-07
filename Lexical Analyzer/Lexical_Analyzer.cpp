//C++ Program to implement Lexical Analyser

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>

using namespace std;

class GenerateTokens
{
    bool isOperator(char symbol)
    {
        return (symbol == 33 || symbol == 37 || symbol == 38 || (symbol >= 42 && symbol <= 47) || (symbol >= 60 && symbol <= 63) || symbol == 94 || symbol == 124 || symbol == 126);
    }

    bool isWhitespace(char symbol)
    {
        return (symbol == ' ' || symbol == '\n' ||symbol == '\t');
    }

    enum comment {NO_COMMENT, SINGLE_LINE_COMMENT, MULTI_LINE_COMMENT} mode;

protected:    
    vector <string> tokens;
    
    bool isNumber(char symbol)
    {
        return (symbol >= '0' && symbol <= '9');
    }

    bool isIdentifier(char symbol)
    {
        return (symbol == '_' || (symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z') || isNumber(symbol));
    }

    bool isPunctuator(char symbol)
    {
        return (symbol == ';' || symbol == ':' || symbol == '(' || symbol == ')' || symbol == '[' || symbol == ']' || symbol == '{' || symbol == '}');
    }

    GenerateTokens(string code)
    {
        mode = NO_COMMENT;

        string buffer;
        buffer.clear();
        for (int i = 0; i < code.size(); i++)
        {
            if (mode == SINGLE_LINE_COMMENT)
            {
                if (code[i] == '\n')    //End of Single Line Comment
                    mode = NO_COMMENT;
            }
            else if  (mode == MULTI_LINE_COMMENT)
            {
                if (code[i] == '*' && code[i+1] == '/')    //End of Multi Line Comment
                {
                    i++;
                    mode = NO_COMMENT;
                }
            }
            else if (buffer.empty())
            {
                if (!isWhitespace(code[i])) //Begin of New Token
                    buffer += code[i];
            }
            else if (buffer[0] == '#')
            {
                if (code[i] == 'i') //Directives
                {
                    tokens.push_back("#include");
                    buffer.clear();
                    i += 6;

                    while (code[i] != '<')  //Preprocessor Header Files
                        i++;
                    while (code[i] != '>')
                    {
                        buffer += code[i];
                        i++;
                    }
                    tokens.push_back(buffer + ">");
                    buffer.clear();
                }
                else if (code[i] == 'd')    //Macro Expansion
                {
                    tokens.push_back("#define");
                    buffer.clear();
                    i += 5;
                }
            }
            else if (buffer[0] == '"') //String Literal
            {
                buffer += code[i];
                if (code[i] == '\\')
                    buffer += code[++i];
                else if (code[i] == '"')     //End of String Literal
                {
                    tokens.push_back(buffer);
                    buffer.clear();
                }
            }
            else if (buffer[0] == '\'')    //Char Constant
            {
                buffer += code[i];
                if (code[i] == '\\')
                    buffer += code[++i];
                else if (code[i] == '\'')    //End of Char Constant
                {
                    tokens.push_back(buffer);
                    buffer.clear();
                }
            }
            else if (isWhitespace(code[i])) //End of Token
            {
                tokens.push_back(buffer);
                buffer.clear();
            }
            else if (isNumber(buffer[0])) //Integer/Float Constant
            {
                if (isNumber(code[i]) || code[i] == '.')
                    buffer += code[i];
                else                    //End of Integer/Float Constant
                {
                    tokens.push_back(buffer);
                    buffer.clear();
                    i--;
                }
            }
            else if (isIdentifier(buffer[0])) //Identifier
            {
                if (isIdentifier(code[i]) || isNumber(code[i]))
                    buffer += code[i];
                else                        //End of Identifier
                {
                    tokens.push_back(buffer);
                    buffer.clear();
                    i--;
                }
            }
            else if (isOperator(buffer[0]))   //Operator
            {
                if (buffer[0] == '/' && buffer.size() == 1)    //Comments
                {
                    if (code[i] == '/')
                    {
                        buffer.clear();
                        mode = SINGLE_LINE_COMMENT;
                        continue;
                    }
                    else if (code[i] == '*')
                    {
                        buffer.clear();
                        mode = MULTI_LINE_COMMENT;
                        continue;
                    }
                }
                if (isOperator(code[i]))
                    buffer += code[i];
                else    //End of Operators
                {
                    tokens.push_back(buffer);
                    buffer.clear();
                    i--;
                }
            }
            else if (isPunctuator(buffer[0]))
            {
                if (buffer[0] == ':' && code[i] == ':')
                    tokens.push_back("::");
                else
                {
                    tokens.push_back(buffer);
                    buffer.clear();
                    i--;
                }
            }
        }

        cout << "Number of tokens generated in the code is " << tokens.size() << "\n\n";
    }
};

class LexicalAnalyser : protected GenerateTokens
{
    unordered_set <string> keyword_list = {"alignas", "alignof", "asm", "auto", "bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t", "class", "compl", "concept", "const", "consteval", "constexpr", "constinit", "continue", "co_await", "co_return", "co_yield", "decltype", "default", "do", "double", "else", "enum", "explicit", "export", "extern", "float", "for", "friend", "goto", "if", "inline", "int", "long", "mutable", "namespace", "noexcept", "operator", "private", "protected", "public", "register", "requires", "return", "short", "signed", "static", "static_assert", "struct", "switch", "template", "this", "thread_local", "throw", "try", "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while"};
    unordered_set <string> operator_list = {"+", "-", "*", "%", "/", "+=", "-=", "*=", "%=", "/=", "++", "--", "=", "==", "<", ">", "<=", ">=", "<<", ">>", "<<=", ">>=", "&", "&&", "|", "||", "!", "^", "~", "&=", "&&=", "|=", "||=", "^=", ".*", ".", "->", "?", ",", "::", "new", "delete", "static_cast", "dynamic_cast", "const_cast", "reinterpret_cast", "xor", "xor_eq", "sizeof", "or", "or_eq", "not", "not_eq", "bitand", "bitor", "and", "and_eq"};
    unordered_set <string> constant_list = {"true", "false", "nullptr"};

    vector<string> directives, headers, macros, keywords, identifiers, operators, punctuators, constants, string_literals;
    
    void classifyToken()
    {
        for (vector<string>::iterator iter = tokens.begin(); iter != tokens.end(); iter++)
        {
            if (*iter == "#include")
                directives.push_back(*iter);
            else if (*iter == "#define")
                macros.push_back(*iter);
            else if ((*iter)[0] == '<' && (*iter)[(*iter).size()-1] == '>')
                headers.push_back(*iter);
            else if (keyword_list.find(*iter) != keyword_list.end())
                keywords.push_back(*iter);
            else if (operator_list.find(*iter) != operator_list.end())
                operators.push_back(*iter);
            else if (isIdentifier((*iter)[0]) && !isNumber((*iter)[0]))
                identifiers.push_back(*iter);
            else if (isPunctuator((*iter)[0]))
                punctuators.push_back(*iter);
            else if (isNumber((*iter)[0]) || (*iter)[0] == '\'')
                constants.push_back(*iter);
            else if ((*iter)[0] == '"')
                string_literals.push_back(*iter);
        }
    }

    void printToken()
    {
        if (directives.size())
        {
            cout << "Directives on the string are:\n" << directives[0];
            for (int i = 1; i < directives.size(); i++)
                cout << "\t" << directives[i];
        }
        else
            printf ("There exist no directives in the code");
        
        if (headers.size())
        {
            cout << "\n\nHeaders on the code are:\n" << headers[0];
            for (int i = 1; i < headers.size(); i++)
                cout << "\t" << headers[i];
        }
        else
            printf ("\n\nThere exist no headers in the code");

        if (macros.size())
        {
            cout << "\n\nMacros on the code are:\n" << macros[0];
            for (int i = 1; i < macros.size(); i++)
                cout << "\t" << macros[i];
        }
        else
            printf ("\n\nThere exist no macros in the code");

        if (keywords.size())
        {
            cout << "\n\nKeywords on the code are:\n" << keywords[0];
            for (int i = 1; i < keywords.size(); i++)
                cout << "\t" << keywords[i];
        }
        else
            printf ("\n\nThere exist no keywords in the code");

        if (identifiers.size())
        {
            cout << "\n\nIdentifiers on the code are:\n" << identifiers[0];
            for (int i = 1; i < identifiers.size(); i++)
                cout << "\t" << identifiers[i];
        }
        else
            printf ("\n\nThere exist no identifiers in the code");

        if (operators.size())
        {
            cout << "\n\nOperators on the code are:\n" << operators[0];
            for (int i = 1; i < operators.size(); i++)
                cout << "\t" << operators[i];
        }
        else
            printf ("\n\nThere exist no operators in the code");

        if (punctuators.size())
        {
            cout << "\n\nPunctuators on the code are:\n" << punctuators[0];
            for (int i = 1; i < punctuators.size(); i++)
                cout << "\t" << punctuators[i];
        }
        else
            printf ("\n\nThere exist no punctuators in the code");

        if (constants.size())
        {
            cout << "\n\nConstants on the code are:\n" << constants[0];
            for (int i = 1; i < constants.size(); i++)
                cout << "\t" << constants[i];
        }
        else
            printf ("\n\nThere exist no constants in the code");

        if (string_literals.size())
        {
            cout << "\n\nString literals on the code are:\n" << string_literals[0];
            for (int i = 1; i < string_literals.size(); i++)
                cout << "\t" << string_literals[i];
        }
        else
            printf ("\n\nThere exist no string literals in the code");
    }

public:
    LexicalAnalyser(string code) : GenerateTokens(code)
    {
        classifyToken();
        printToken();
    }
};

int main(int argc, char** argv)
{
    ifstream fin;
    fin.open(argv[1]);
    
    string code;
    stringstream ss;
    ss << fin.rdbuf(); // reading data
    code = ss.str();
        
    LexicalAnalyser obj(code);

    return 0;
}