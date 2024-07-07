//C++ Program to implement SLR(1) Parser

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>

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
            grammar[(*it)[0]].insert(RHS.begin(), RHS.end());
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
            first[T].insert(T);
            return;
        }
        
        if (first[T].find(' ') != first[T].end())
            return;

        set<string> production = grammar[T];
        for (set<string>::iterator it = production.begin(); it != production.end(); it++)
        {
            int i;
            for (i = 0; i < it->size(); i++)
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
                    for (set<char>::iterator iter = first[(*it)[i]].begin(); iter != first[(*it)[i]].end(); iter++)
                        if (*iter == 'e')
                            has_null = true;
                        else if (*iter != ' ')
                            first[T].insert(*iter);
                    if (!has_null) break;    
                }
            }

            if (i == it->size())
                first[T].insert('e');
        }

        first[T].insert(' ');
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
                        for (set<char>::iterator iter1 = first[(*iter)[i+1]].begin(); iter1 != first[(*iter)[i+1]].end(); iter1++)
                            if (*iter1 == 'e')
                                has_null = true;
                            else if (*iter1 != ' ')
                                follow[T].insert(*iter1);
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
            first[*it].erase(' ');
            cout << "First (" << *it << ") -> ";
            for (set<char>::iterator  iter = first[*it].begin(); iter != first[*it].end(); iter++)
                cout << *iter << " ";
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
    map<char, set<char>> first, follow;

    FirstFollow() : Grammar()
    {
        computeFirstFollow();
        printFirstFollow();
    }
};


class ParsingTable : protected FirstFollow
{
    void getAugmentedGrammar()
    {
        augmented_grammar.push_back(make_pair('I', string(1, start_symbol)));
        for (set<char>::iterator iter = non_terminal.begin(); iter != non_terminal.end(); iter++)
            for (set<string>::iterator it = grammar[*iter].begin(); it != grammar[*iter].end(); it++)
                if (*it == "e")
                    augmented_grammar.push_back(make_pair(*iter, ""));
                else
                    augmented_grammar.push_back(make_pair(*iter, *it));


        cout << "Augmented Grammar\n";
        for (int i = 0; i < augmented_grammar.size(); i++)
            cout << i << "\t" << augmented_grammar[i].first << "->" << (augmented_grammar[i].second.empty() ? "e" : augmented_grammar[i].second) << endl;
    }

    void closure(vector<pair<char, string>> &state)
    {
        for (int i = 0; i < state.size(); i++)
            if (state[i].second.find('.')+1 != state[i].second.size() && isupper(state[i].second[state[i].second.find('.') + 1]))
                for (vector<pair<char, string>>::iterator iter = augmented_grammar.begin(); iter != augmented_grammar.end(); iter++)
                    if (iter->first == state[i].second[state[i].second.find('.') + 1])
                    {
                        string temp = "." + iter->second;
                        bool found = false;

                        for (vector<pair<char, string>>::iterator it = state.begin(); it != state.end(); it++)
                            if (*it == make_pair(iter->first, temp))
                            {
                                found = true;
                                break;
                            }
                        
                        if (!found)
                            state.push_back({iter->first, temp});
                    }
    }

    void computeLR0Items()
    {
        I.push_back(vector<pair<char, string>>(1, make_pair('I', "." + string(1, start_symbol))));

        closure(I[0]);

        for (int state = 0; state < I.size(); state++)
        {
            table.push_back(map<char, string>());

            for (int i = 0; i < I[state].size(); i++)       //Reduce
                if (I[state][i].second.find('.')+1 == I[state][i].second.size())
                {
                    string temp = I[state][i].second.substr(0, I[state][i].second.size()-1);
                    int production;

                    for (production = 0; production < augmented_grammar.size(); production++)
                        if (augmented_grammar[production].first == I[state][i].first && augmented_grammar[production].second == temp)
                            break;

                    if (production == 0)
                        table[state]['$'] = "Accept";
                    else
                        for (set<char>::iterator it = follow[I[state][i].first].begin(); it != follow[I[state][i].first].end(); it++)
                            table[state][*it] = "r" + to_string(production);
                }

            //Shift
            for (set<char>::iterator iter = terminal.begin(); iter != terminal.end(); iter++)
            {
                vector<pair<char, string>> temp;
                for (int i = 0; i < I[state].size(); i++)
                    if (I[state][i].second.find('.')+1 != I[state][i].second.size() && I[state][i].second[I[state][i].second.find('.')+1] == *iter)
                    {
                        pair<char, string> shift = I[state][i];
                        shift.second.insert(shift.second.find('.') + 2, ".");
                        shift.second.erase(shift.second.find('.'), 1);
                        temp.push_back(shift);
                    }

                if (temp.empty())   continue;

                closure(temp);

                bool found = false;
                int i;
                for (i = 0; i < I.size(); i++)
                    if (I[i] == temp)
                    {
                        found = true;
                        break;
                    }

                if (!found)
                    I.push_back(temp);
                
                table[state][*iter] = "s" + to_string(i);
            }

            //Goto
            for (set<char>::iterator iter = non_terminal.begin(); iter != non_terminal.end(); iter++)
            {
                vector<pair<char, string>> temp;
                for (int i = 0; i < I[state].size(); i++)
                    if (I[state][i].second.find('.')+1 != I[state][i].second.size() && I[state][i].second[I[state][i].second.find('.')+1] == *iter)
                    {
                        pair<char, string> shift = I[state][i];
                        shift.second.insert(shift.second.find('.') + 2, ".");
                        shift.second.erase(shift.second.find('.'), 1);
                        temp.push_back(shift);
                    }

                if (temp.empty())   continue;

                closure(temp);

                bool found = false;
                int i;
                for (i = 0; i < I.size(); i++)
                    if (I[i] == temp)
                    {
                        found = true;
                        break;
                    }

                if (!found)
                    I.push_back(temp);
                
                table[state][*iter] = to_string(i);
            }
        }
    }
    
    void printLR0Items()
    {
        cout << "\n\nLR(0) Items:\n";

        for (int i = 0; i < I.size(); i++)
        {
            cout << "\nI" << i << ":\n";
            for (vector<pair<char, string>>::iterator it = I[i].begin(); it != I[i].end(); it++)
                cout << it->first << "->" << it->second << endl; 
        }
    }

    void printTable()
    {
        cout << "\nSLR(1) Parsing Table-\nState";
        for (set<char>::iterator it = terminal.begin(); it != terminal.end(); it++)
            cout << "\t|   " << *it;

        for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
            cout << "\t|   " << *it;

        for (int i = 0; i < table.size(); i++)
        {
            cout << "\n  " << i;
            for (set<char>::iterator it = terminal.begin(); it != terminal.end(); it++)
            {
                if (table[i][*it] == "Accept")
                    cout << "\t|Accept";
                else
                    cout << "\t|   " << table[i][*it];
            }

            for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
                cout << "\t|   " << table[i][*it];
        }
        cout << endl << endl;
    }

protected:
    vector<pair<char, string>> augmented_grammar;
    vector<vector<pair<char, string>>> I;
    vector<map<char, string>> table;    //SLR(1) Parsing Table

    ParsingTable() : FirstFollow()
    {
        getAugmentedGrammar();
        computeLR0Items();
        printLR0Items();
        printTable();
    }
};


class Parser : protected ParsingTable
{
    vector<char> stack;
    vector<int> state;
    string input;

    void getInput()
    {
        cout << "\nEnter the Input String:\n";
        cin >> input;

        input += "$";
        stack.push_back('$');
        state.push_back(0);
    }

    void parseInput()
    {
        printf("\nStack\t\tInput\t\tAction\n-------------------------------\n");
        
        int LA = 0;
        while(stack.size() > 1 || LA < input.size() - 1)
        {
            if (stack.size() < 1 || LA > input.size() - 1)
                break;

            for (int i = 0 ; i < stack.size(); i++)
                cout << stack[i] << state[i];
            
            cout << "\t\t";
            for (int i = LA; i < input.size(); i++)
                cout << input[i];
            
            cout << "\t\t";
            string move = table[state[state.size()-1]][input[LA]];

            if (move == "")
                break;

            else if (move == "Accept")
            {
                cout << "Accept\n";
                break;
            }

            else if (move[0] == 's')    //Shift
            {
                cout << "Shift " << input[LA] << endl;

                stack.push_back(input[LA]);
                state.push_back(stoi(move.substr(1, move.size()-1)));
                LA++;
            }

            else if (move[0] == 'r')    //Reduce
            {
                pair<char, string> production = augmented_grammar[stoi(move.substr(1, move.size()-1))];
                cout << "Reduce by " << production.first << "->" << production.second << endl;

                bool is_valid = true;
                for (int i = production.second.size() - 1; i >= 0; i--)
                    if (stack[stack.size() - 1] == production.second[i])
                    {
                        stack.pop_back();
                        state.pop_back();
                    }
                    else
                    {
                        is_valid = false;
                        break;
                    }

                if (!is_valid || table[state[state.size()-1]][production.first] == "")
                    break;

                stack.push_back(production.first);
                state.push_back(stoi(table[state[state.size()-1]][production.first]));
            }
        }

        if (table[state[state.size()-1]]['$'] == "Accept" && LA == input.size() - 1)
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
    Parser SLR1;
    return 0;
}