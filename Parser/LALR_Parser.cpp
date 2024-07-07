//C++ Program to implement LALR(1) Parser

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


class ParsingTable : protected Grammar
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
                    for (set<char>::iterator iter = first[(*it)[i]].begin(); iter != first[(*it)[i]].end(); iter++)
                        if (*iter == 'e')
                            has_null = true;
                        else if (*iter != ' ')
                            first[T].insert(*iter);
                    if (!has_null) break;    
                }
                i++;
            }

            if (i == it->size())
                first[T].insert('e');
        }

        first[T].insert(' ');
    }

    void computeFirst()
    {
        for (set<char>::iterator it = terminal.begin(); it != terminal.end(); it++)
            First(*it);

        for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
            First(*it);

        for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
            first[*it].erase(' ');
    }

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

    set<char> computeLA(string alpha, set<char> beta)
    {
        set<char> LA;

        for (int i = 0; i < alpha.size(); i++)
        {
            LA.insert (first[alpha[i]].begin(), first[alpha[i]].end());

            bool has_null = false;
            for (set<char>::iterator it = first[alpha[i]].begin(); it != first[alpha[i]].end(); it++)
                if (*it == 'e')
                {
                    has_null = true;
                    break;
                }

            if (has_null)
                LA.erase('e');
            else
                return LA;
        }

        LA.insert (beta.begin(), beta.end());
        return LA;
    }

    void closure(vector<pair<char, pair<string, set<char>>>> &state)
    {
        for (int i = 0; i < state.size(); i++)
            if (state[i].second.first.find('.')+1 != state[i].second.first.size() && isupper(state[i].second.first[state[i].second.first.find('.') + 1]))
                for (vector<pair<char, string>>::iterator iter = augmented_grammar.begin(); iter != augmented_grammar.end(); iter++)
                    if (iter->first == state[i].second.first[state[i].second.first.find('.') + 1])
                    {
                        pair<char, pair<string, set<char>>> temp = {iter->first, {"." + iter->second, computeLA(state[i].second.first.substr(state[i].second.first.find('.')+2), state[i].second.second)}};
                        bool found = false;

                        for (vector<pair<char, pair<string, set<char>>>>::iterator it = state.begin(); it != state.end(); it++)
                            if (it->first == temp.first && it->second.first == temp.second.first)
                            {
                                found = true;
                                int k = it->second.second.size();
                                it->second.second.insert(temp.second.second.begin(), temp.second.second.end());

                                if (it->second.second.size() > k)
                                    i = -1;

                                break;
                            }
                        
                        if (i == -1)
                            break;

                        if (!found)
                            state.push_back(temp);
                    }
    }

    void computeLR1Items()
    {
        I.push_back(vector<pair<char, pair<string, set<char>>>>(1, make_pair('I', make_pair("." + string(1, start_symbol), set<char>({'$'})))));

        closure(I[0]);

        for (int state = 0; state < I.size(); state++)
        {
            table.push_back(map<char, string>());

            for (int i = 0; i < I[state].size(); i++)       //Reduce
                if (I[state][i].second.first.find('.')+1 == I[state][i].second.first.size())
                {
                    string temp = I[state][i].second.first.substr(0, I[state][i].second.first.size()-1);
                    int production;

                    for (production = 0; production < augmented_grammar.size(); production++)
                        if (augmented_grammar[production].first == I[state][i].first && augmented_grammar[production].second == temp)
                            break;

                    if (production == 0)
                        table[state]['$'] = "Accept";
                    else
                        for (set<char>::iterator it = I[state][i].second.second.begin(); it != I[state][i].second.second.end(); it++)
                            table[state][*it] = "r" + to_string(production);
                }

            //Shift
            for (set<char>::iterator iter = terminal.begin(); iter != terminal.end(); iter++)
            {
                vector<pair<char, pair<string, set<char>>>> temp;
                for (int i = 0; i < I[state].size(); i++)
                    if (I[state][i].second.first.find('.')+1 != I[state][i].second.first.size() && I[state][i].second.first[I[state][i].second.first.find('.')+1] == *iter)
                    {
                        pair<char, pair<string, set<char>>> shift = I[state][i];
                        shift.second.first.insert(shift.second.first.find('.') + 2, ".");
                        shift.second.first.erase(shift.second.first.find('.'), 1);
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
                vector<pair<char, pair<string, set<char>>>> temp;
                for (int i = 0; i < I[state].size(); i++)
                    if (I[state][i].second.first.find('.')+1 != I[state][i].second.first.size() && I[state][i].second.first[I[state][i].second.first.find('.')+1] == *iter)
                    {
                        pair<char, pair<string, set<char>>> shift = I[state][i];
                        shift.second.first.insert(shift.second.first.find('.') + 2, ".");
                        shift.second.first.erase(shift.second.first.find('.'), 1);
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
    
    bool compare(vector<pair<char, pair<string, set<char>>>> &a, vector<pair<char, pair<string, set<char>>>> &b)
    {
        if (a.size() != b.size())   return false;
        
        for (int i = 0; i < a.size(); i++)
            if (a[i].first != b[i].first || a[i].second.first != b[i].second.first)
                return false;
                
        return true;
    }

    void merge()
    {
        vector<int> index (I.size());
        for (int i = 0; i < index.size(); i++)
            index[i] = i;

        for (int i = 0; i < I.size() - 1; i++)
            for (int j  = i + 1; j < I.size(); j++)
                if (index[j] == j && compare(I[i], I[j]))
                    index[j] = i;
        
        for (int i = 0; i < I.size() - 1; i++)
            for (int j  = i + 1; j < I.size(); j++)
                if (compare(I[i], I[j]))
                {
                    for (int k = 0; k < I[i].size(); k++)
                        I[i][k].second.second.insert(I[j][k].second.second.begin(), I[j][k].second.second.end());

                    for (set<char>::iterator iter = non_terminal.begin(); iter != non_terminal.end(); iter++)
                        if (table[i][*iter].empty() && !table[j][*iter].empty())
                            table[i][*iter] = table[j][*iter];

                    for (set<char>::iterator iter = terminal.begin(); iter != terminal.end(); iter++)
                        if (table[i][*iter].empty() && !table[j][*iter].empty())
                            table[i][*iter] = table[j][*iter];

                    I.erase(I.begin() + j);
                    table.erase(table.begin() + j);
                    j--;
                }

        int ptr = 0;
        for (int i = 0; i < index.size(); i++)
            if (index[i] == i)
            {
                for (int j = i; j < index.size(); j++)
                    if (index[j] == i)
                        index[j] = ptr;
                ptr++;
            }

        for (int i = 0; i < table.size(); i++)
        {
            for (set<char>::iterator it = terminal.begin(); it != terminal.end(); it++)
                if (!table[i][*it].empty() && table[i][*it][0] == 's')
                    table[i][*it] = "s" + to_string(index[stoi(table[i][*it].substr(1, table[i][*it].size()))]);

            for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++)
                if (!table[i][*it].empty())
                    table[i][*it] = to_string(index[stoi(table[i][*it])]);
        }
    }

    void printLR1Items()
    {
        cout << "\n\nLR(1) Items:\n";

        for (int i = 0; i < I.size(); i++)
        {
            cout << "\nI" << i << ":\n";
            for (vector<pair<char, pair<string, set<char>>>>::iterator it = I[i].begin(); it != I[i].end(); it++)
            {
                cout << it->first << "->" << it->second.first << "\tLA:";
                for (set<char>::iterator iter = it->second.second.begin(); iter != it->second.second.end(); iter++)
                    cout << " " << *iter;
                cout << endl;
            }
        }
    }

    void printTable()
    {
        cout << "\nLALR(1) Parsing Table-\nState";
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
    map<char, set<char>> first;
    vector<pair<char, string>> augmented_grammar;
    vector<vector<pair<char, pair<string, set<char>>>>> I;
    vector<map<char, string>> table;    //CLR(1) Parsing Table

    ParsingTable() : Grammar()
    {
        computeFirst();
        getAugmentedGrammar();
        computeLR1Items();
        merge();
        printLR1Items();
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
    Parser LALR1;
    return 0;
}