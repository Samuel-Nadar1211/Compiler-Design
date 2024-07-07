// C++ Program to implement Operator Precedence Parser

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

class Grammar
{
    vector<string> inputGrammar()
    {
        int n;
        cout << "Enter no. of productions in the Grammar: ";
        cin >> n;

        cout << "\nEnter the Operator Grammar (without space):\n";
        vector<string> production(n);
        for (int i = 0; i < n; i++)
            cin >> production[i];
        cout << endl;

        return production;
    }

    void storeGrammar(vector<string> production)
    {
        start_symbol = production[0][0];

        for (vector<string>::iterator it = production.begin(); it != production.end(); it++)
        {
            non_terminal.insert((*it)[0]);

            set<string> RHS;
            string temp = "";
            for (int i = 3; i < it->size(); i++)
            {
                if ((*it)[i] == '|')
                {
                    RHS.insert(temp);
                    temp = "";
                }
                else
                {
                    temp += (*it)[i];
                    if (!isupper((*it)[i]))
                        terminal.insert((*it)[i]);
                }
            }
            RHS.insert(temp);
            grammar[(*it)[0]].insert(RHS.begin(), RHS.end());
        }

        terminal.insert('$');
    }

    bool checkOperatorGrammar()
    {
        for (set<char>::iterator iter = non_terminal.begin(); iter != non_terminal.end(); iter++)
            for (set<string>::iterator it = grammar[*iter].begin(); it != grammar[*iter].end(); it++)
            {
                bool is_NT = false;
                for (int i = 0; i < it->size(); i++)
                {
                    if (is_NT && isupper((*it)[i]))
                        return false;
                    else if (isupper((*it)[i]))
                        is_NT = true;
                    else
                        is_NT = false;
                }
            }
        return true;
    }

protected:
    char start_symbol;
    set<char> non_terminal, terminal;
    map<char, set<string>> grammar;

    Grammar()
    {
        storeGrammar(inputGrammar());
        if (!checkOperatorGrammar())
        {
            cout << "Given Grammar is not in Operator Grammar\nEnter the OPERATOR GRAMMAR again\n\n";
            non_terminal.clear();
            terminal.clear();
            grammar.clear();
            Grammar();
        }
    }
};

class LeadingTrailing : protected Grammar
{
    void Leading(char T)
    {
        if (leading[T].find(' ') != leading[T].end())
            return;

        for (set<string>::iterator it = grammar[T].begin(); it != grammar[T].end(); it++)
        {
            if (!isupper((*it)[0])) // terminal
                leading[T].insert((*it)[0]);
            else
            {
                if (it->size() > 1)
                    leading[T].insert((*it)[1]);
                if ((*it)[0] != T)
                {
                    Leading((*it)[0]);
                    leading[T].insert(leading[(*it)[0]].begin(), leading[(*it)[0]].end());
                }
            }
        }

        leading[T].insert(' ');
    }

    void Trailing(char T)
    {
        if (trailing[T].find(' ') != trailing[T].end())
            return;

        for (set<string>::iterator it = grammar[T].begin(); it != grammar[T].end(); it++)
        {
            if (!isupper((*it)[it->size() - 1])) // terminal
                trailing[T].insert((*it)[it->size() - 1]);
            else
            {
                if (it->size() > 1)
                    trailing[T].insert((*it)[it->size() - 2]);
                if ((*it)[it->size() - 1] != T)
                {
                    Trailing((*it)[it->size() - 1]);
                    trailing[T].insert(trailing[(*it)[it->size() - 1]].begin(), trailing[(*it)[it->size() - 1]].end());
                }
            }
        }

        trailing[T].insert(' ');
    }

    void computeLeadingTrailing()
    {
        for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
        {
            Leading(*it);
            Trailing(*it);
        }
    }

    void printLeadingTrailing()
    {
        for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
        {
            leading[*it].erase(' ');
            cout << "Leading (" << *it << ") -> ";
            for (set<char>::iterator iter = leading[*it].begin(); iter != leading[*it].end(); iter++)
                cout << *iter << " ";
            cout << endl;
        }
        cout << endl;

        for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
        {
            trailing[*it].erase(' ');
            cout << "Trailing (" << *it << ") -> ";
            for (set<char>::iterator iter = trailing[*it].begin(); iter != trailing[*it].end(); iter++)
                cout << *iter << " ";
            cout << endl;
        }
        cout << endl;
    }

protected:
    map<char, set<char>> leading, trailing;

    LeadingTrailing()
    {
        computeLeadingTrailing();
        printLeadingTrailing();
    }
};

class ParsingTable : protected LeadingTrailing
{
    void initaliseTable()
    {
        for (set<char>::iterator it1 = terminal.begin(); it1 != terminal.end(); it1++)
            for (set<char>::iterator it2 = terminal.begin(); it2 != terminal.end(); it2++)
                table[*it1][*it2] = ' ';

        table['$']['$'] = '*';
    }

    void checkLL1AndSet(char a, char b, char rel)
    {
        if (table[a][b] == ' ' || table[a][b] == rel)
            table[a][b] = rel;
        else
        {
            cout << "Given Grammar is not LL(1) Grammar\n";
            exit(0);
        }
    }

    void updateTable()
    {
        for (set<char>::iterator iter = leading[start_symbol].begin(); iter != leading[start_symbol].end(); iter++)
            table['$'][*iter] = '<';

        for (set<char>::iterator iter = trailing[start_symbol].begin(); iter != trailing[start_symbol].end(); iter++)
            table[*iter]['$'] = '>';

        for (set<char>::iterator iter = non_terminal.begin(); iter != non_terminal.end(); iter++)
            for (set<string>::iterator it = grammar[*iter].begin(); it != grammar[*iter].end(); it++)
                for (int i = 0; i < it->size() - 1; i++)
                {
                    if (i != it->size() - 2 && !isupper((*it)[i]) && !isupper((*it)[i + 2]) && isupper((*it)[i + 1]))
                        checkLL1AndSet((*it)[i], (*it)[i + 2], '=');

                    if (!isupper((*it)[i]) && !isupper((*it)[i + 1]))
                        checkLL1AndSet((*it)[i], (*it)[i + 1], '=');

                    else if (!isupper((*it)[i]) && isupper((*it)[i + 1]))
                        for (set<char>::iterator lead = leading[(*it)[i + 1]].begin(); lead != leading[(*it)[i + 1]].end(); lead++)
                            checkLL1AndSet((*it)[i], *lead, '<');

                    else if (isupper((*it)[i]) && !isupper((*it)[i + 1]))
                        for (set<char>::iterator trail = trailing[(*it)[i]].begin(); trail != trailing[(*it)[i]].end(); trail++)
                            checkLL1AndSet(*trail, (*it)[i + 1], '>');
                }
    }

    void printTable()
    {
        cout << "Operator Precedence Relation Table -\n";
        for (set<char>::iterator it = terminal.begin(); it != terminal.end(); it++)
            cout << "\t|   " << *it;

        for (set<char>::iterator it = terminal.begin(); it != terminal.end(); it++)
        {
            cout << "\n   " << *it;
            for (set<char>::iterator iter = terminal.begin(); iter != terminal.end(); iter++)
                cout << "\t|   " << table[*it][*iter];
        }
        cout << endl
             << endl;
    }

protected:
    map<char, map<char, char>> table; // Operation Precedence Relation Table

    ParsingTable() : LeadingTrailing()
    {
        initaliseTable();
        updateTable();
        printTable();
    }
};

class Parser : protected ParsingTable
{
    vector<char> stack;
    string input;

    void getInput()
    {
        cout << "Enter the Input String:\n";
        cin >> input;

        input += "$";
        stack.push_back('$');
    }

    char Top()
    {
        if (!isupper(stack[stack.size() - 1]))
            return stack[stack.size() - 1];
        else
            return stack[stack.size() - 2];
    }

    void parseInput()
    {
        printf("\nStack\tInput\tAction\n-------------------------------\n");

        int LA = 0;
        while (LA < input.size())
        {
            if (stack.size() < 1 || LA > input.size() - 1)
                break;

            for (vector<char>::iterator it = stack.begin(); it != stack.end(); it++)
                cout << *it;

            cout << "\t";
            for (int i = LA; i < input.size(); i++)
                cout << input[i];

            cout << "\t";

            if (table[Top()][input[LA]] == '*')
            {
                if (stack[stack.size() - 1] != start_symbol)
                {
                    bool found;
                    char LHS;
                    string buffer;
                    buffer = LHS = stack[stack.size() - 1];

                    while (LHS != start_symbol)
                    {
                        found = false;

                        for (set<char>::iterator iter = non_terminal.begin(); iter != non_terminal.end(); iter++)
                            if (grammar[*iter].find(buffer) != grammar[*iter].end())
                            {
                                found = true;
                                LHS = *iter;
                                break;
                            }

                        cout << " Reduce by " << LHS << "->" << buffer << endl;
                        stack.pop_back();
                        stack.push_back(LHS);
                        buffer = LHS;

                        for (vector<char>::iterator it = stack.begin(); it != stack.end(); it++)
                            cout << *it;

                        cout << "\t";
                        for (int i = LA; i < input.size(); i++)
                            cout << input[i];

                        cout << "\t";

                        if (!found)
                            break;
                    }

                    if (!found)
                        break;
                }

                printf("Accept\n");
                break;
            }

            else if (table[Top()][input[LA]] == ' ')
            {
                printf("%c ? %c Reject\n", Top(), input[LA]);
                break;
            }

            else if (table[Top()][input[LA]] == '<' || table[Top()][input[LA]] == '=')
            {
                if (isupper(stack[stack.size() - 1]))
                {
                    bool found;
                    char LHS;
                    string buffer;
                    buffer = LHS = stack[stack.size() - 1];

                    while (true)
                    {
                        found = false;
                        for (set<char>::iterator iter = non_terminal.begin(); iter != non_terminal.end() && !found; iter++)
                            for (set<string>::iterator it = grammar[*iter].begin(); it != grammar[*iter].end() && !found; it++)
                                for (int i = 0; i < it->size() - 1; i++)
                                    if ((*it)[i] == LHS && (*it)[i + 1] == input[LA])
                                    {
                                        found = true;
                                        break;
                                    }

                        if (found)
                            break;

                        for (set<char>::iterator iter = non_terminal.begin(); iter != non_terminal.end(); iter++)
                            if (grammar[*iter].find(buffer) != grammar[*iter].end())
                            {
                                found = true;
                                LHS = *iter;
                                break;
                            }

                        if (!found)
                            break;

                        cout << " Reduce by " << LHS << "->" << buffer << endl;
                        stack.pop_back();
                        stack.push_back(LHS);
                        buffer = LHS;

                        for (vector<char>::iterator it = stack.begin(); it != stack.end(); it++)
                            cout << *it;

                        cout << "\t";
                        for (int i = LA; i < input.size(); i++)
                            cout << input[i];

                        cout << "\t";
                    }

                    if (!found)
                        break;
                }

                printf("%c %c %c Shift\n", Top(), table[Top()][input[LA]], input[LA]);

                stack.push_back(input[LA]);
                LA++;
            }

            else if (table[Top()][input[LA]] == '>')
            {
                string buffer = "";

                if (isupper(stack[stack.size() - 1]))
                {
                    bool found;
                    char LHS;
                    buffer = LHS = stack[stack.size() - 1];

                    while (true)
                    {
                        found = false;
                        for (set<char>::iterator iter = non_terminal.begin(); iter != non_terminal.end() && !found; iter++)
                            for (set<string>::iterator it = grammar[*iter].begin(); it != grammar[*iter].end() && !found; it++)
                                for (int i = 0; i < it->size() - 1; i++)
                                    if ((*it)[i] == stack[stack.size() - 2] && (*it)[i + 1] == LHS)
                                    {
                                        found = true;
                                        break;
                                    }

                        if (found)
                            break;

                        for (set<char>::iterator iter = non_terminal.begin(); iter != non_terminal.end(); iter++)
                            if (grammar[*iter].find(buffer) != grammar[*iter].end())
                            {
                                found = true;
                                LHS = *iter;
                                break;
                            }

                        if (!found)
                            break;

                        cout << " Reduce by " << LHS << "->" << buffer << endl;
                        stack.pop_back();
                        stack.push_back(LHS);
                        buffer = LHS;

                        for (vector<char>::iterator it = stack.begin(); it != stack.end(); it++)
                            cout << *it;

                        cout << "\t";
                        for (int i = LA; i < input.size(); i++)
                            cout << input[i];

                        cout << "\t";
                    }

                    if (!found)
                        break;

                    stack.pop_back();
                }

                printf("%c > %c Pop ", Top(), input[LA]);

                char temp = Top();

                buffer = Top() + buffer;
                stack.pop_back();

                while (true)
                {
                    if (isupper(stack[stack.size() - 1]))
                    {
                        buffer = stack[stack.size() - 1] + buffer;
                        stack.pop_back();
                    }
                    else if (table[Top()][temp] == '<')
                    {
                        cout << buffer;
                        break;
                    }
                    else
                    {
                        temp = Top();
                        buffer = Top() + buffer;
                        stack.pop_back();
                    }
                }

                bool found = false;
                char LHS;
                for (set<char>::iterator iter = non_terminal.begin(); iter != non_terminal.end(); iter++)
                    if (grammar[*iter].find(buffer) != grammar[*iter].end())
                    {
                        found = true;
                        LHS = *iter;
                        break;
                    }

                if (!found)
                    break;

                cout << " Reduce by " << LHS << "->" << buffer;

                stack.push_back(LHS);
                cout << endl;
            }
        }

        if (stack == vector<char>({'$', start_symbol}) && LA == input.size() - 1)
        {
            printf("\n-------------------------------\n");
            printf("String is successfully parsed");
        }
        else
        {
            printf("\n-------------------------------\n");
            printf("Error in parsing string\n");
        }
    }

public:
    Parser() : ParsingTable()
    {
        getInput();
        parseInput();
    }
};

int main()
{
    Parser operator_precedence_parser;
    return 0;
}