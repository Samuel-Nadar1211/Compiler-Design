//C++ Program to implement Predictive Parser

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

        cout << "\nEnter the Grammar (without space):\nNote- Write e for epsilon production\n";
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
            grammar[(*it)[0]] = RHS;
        }

        terminal.insert('$');
    }

protected:
    char start_symbol;
    set<char> non_terminal, terminal;
    map<char, set<string>> grammar;

    Grammar()
    {
        storeGrammar(inputGrammar());
    }
};

class FirstFollow : protected Grammar
{
    void First(char T)
    {
        if (!isupper(T))        //Terminal
        {
            first[T].insert({T});
            return;
        }
        
        if (first[T].find({' '}) != first[T].end())
            return;

        set<string> production = grammar[T];
        for (set<string>::iterator it = production.begin(); it != production.end(); it++)
        {
            int i = 0;
            while (i < it->size())
            {
                if ((*it)[i] == T)
                {
                    bool has_null = false;
                    for (set<string>::iterator iter = production.begin(); iter != production.end(); iter++)
                        if ((*iter)[0] == 'e')
                        {
                            has_null = true;
                            break;
                        }
                    if (!has_null) break;
                }
                else
                {
                    First((*it)[i]);
                    bool has_null = false;
                    for (set<vector<char>>::iterator iter = first[(*it)[i]].begin(); iter != first[(*it)[i]].end(); iter++)
                        if ((*iter)[0] == 'e')
                            has_null = true;
                        else if ((*iter)[0] != ' ')
                            first[T].insert({(*iter)[0], (*it)[0]});
                    if (!has_null) break;    
                }
                i++;
            }

            if (i == it->size())
                first[T].insert({'e', (*it)[0]});
        }

        first[T].insert({' '});
    }

    void Follow(char T)
    {   
        if (follow[T].find(' ') != follow[T].end())
            return;
        
        if (T == start_symbol)
            follow[T].insert('$');

        for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
        {
            for (set<string>::iterator iter = grammar[*it].begin(); iter != grammar[*it].end(); iter++)
            {
                bool is_match = false;
                int i;
                if ((*iter)[0] == T)  is_match = true;

                for (i = 0; i < iter->size() - 1; i++)
                {
                    if (is_match)
                    {
                        bool has_null = false;
                        for (set<vector<char>>::iterator iter1 = first[(*iter)[i+1]].begin(); iter1 != first[(*iter)[i+1]].end(); iter1++)
                            if ((*iter1)[0] == 'e')
                                has_null = true;
                            else if ((*iter1)[0] != ' ')
                                follow[T].insert((*iter1)[0]);
                        if (!has_null)
                            is_match = false;
                    }

                    if ((*iter)[i+1] == T)
                        is_match = true;
                }
                if (is_match && *it != (*iter)[i])
                {
                    Follow(*it);
                    follow[T].insert(follow[*it].begin(), follow[*it].end());
                }
            }
        }
        follow[T].insert(' ');
    }

    void computeFirstFollow()
    {
        for (set<char>::iterator it = terminal.begin(); it != terminal.end(); it++)
            First(*it);

        for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
            First(*it);

        for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
            Follow(*it);
    }

    void printFirstFollow()
    {
        for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
        {
            first[*it].erase({' '});
            cout << "First (" << *it << ") -> ";
            for (set<vector<char>>::iterator  iter = first[*it].begin(); iter != first[*it].end(); iter++)
                cout << (*iter)[0] << " ";
            cout << endl;
        }
        cout << endl;
        
        for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
        {
            follow[*it].erase(' ');
            cout << "Follow (" << *it << ") -> ";
            for (set<char>::iterator  iter = follow[*it].begin(); iter != follow[*it].end(); iter++)
                cout << *iter << " ";
            cout << endl;
        }
        cout << endl;
    }

protected:
    map<char, set<vector<char>>> first;
    map<char, set<char>> follow;

    FirstFollow() : Grammar()
    {
        computeFirstFollow();
        printFirstFollow();
    }
};

class ParsingTable : protected FirstFollow
{
    void updateTable()
    {
        terminal.erase('e');

        for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
        {   
            for (set<vector<char>>::iterator iter = first[*it].begin(); iter != first[*it].end(); iter++)
                if ((*iter)[0] == 'e')
                    for (set<string>::iterator iter1 = grammar[*it].begin(); iter1 != grammar[*it].end(); iter1++)
                    {
                        if ((*iter1)[0] == (*iter)[1])
                        {
                            for (set<char>::iterator iter2 = follow[*it].begin(); iter2 != follow[*it].end(); iter2++)
                            {
                                table[*it][*iter2] = " ->";
                                table[*it][*iter2][0] = *it;
                                table[*it][*iter2] += *iter1;
                            }
                            break;
                        }
                    }
                else
                    for (set<string>::iterator iter1 = grammar[*it].begin(); iter1 != grammar[*it].end(); iter1++)
                        if ((*iter1)[0] == (*iter)[1])
                        {
                            table[*it][(*iter)[0]] = " ->";
                            table[*it][(*iter)[0]][0] = *it;
                            table[*it][(*iter)[0]] += *iter1;
                            break;
                        }
        }
    }

    void printTable()
    {
        cout << "Predictive Parsing Table -\n";
        for (set<char>::iterator it = terminal.begin(); it != terminal.end(); it++)
            cout << "\t|   " << *it;

        for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
        {
            cout << "\n   " << *it;
            for (set<char>::iterator iter = terminal.begin(); iter != terminal.end(); iter++)
                cout << "\t|" << table[*it][*iter];
        }
        cout << endl << endl;
    }

protected:
    map<char, map<char, string>> table; //Predictive Parsing Table

    ParsingTable() : FirstFollow()
    {
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
        stack.push_back(start_symbol);
    }

    void parseInput()
    {
        printf("\nStack\tInput\tAction\n-------------------------------\n");
        
        int LA = 0;
        while(stack.size() > 1 || LA < input.size() - 1)
        {
            if (stack.size() < 1 || LA > input.size() - 1)
                break;

            for (vector<char>::iterator it = stack.begin(); it != stack.end(); it++)
                cout << *it;
            
            cout << "\t";
            for (int i = LA; i < input.size(); i++)
                cout << input[i];
            
            cout << "\t";
            char top = stack[stack.size()-1];
            
            if (isupper(top))
            {
                cout << table[top][input[LA]];
                stack.pop_back();

                if (table[top][input[LA]].empty())  break;

                if (table[top][input[LA]][3] != 'e')
                    for (int i = table[top][input[LA]].size() - 1; i > 2; i--)
                        stack.push_back(table[top][input[LA]][i]);
            }
            else
            {
                cout << "Remove " << top;
                stack.pop_back();
                LA++;
            }
            cout << endl; 
        }

        if (stack.size() == 1 && LA == input.size() - 1)
        {
            printf("-------------------------------\n");
            printf("String is successfully parsed");
        }
        else
        {
            printf("-------------------------------\n");
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
    Parser predictive_parser;
    return 0;
}