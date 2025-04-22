#include "nfa.h"
#include <stack>
#include <iostream>
#include <set>

static int stateId = 0;

State *createState()
{
    State *s = new State();
    s->id = stateId++;
    return s;
}

NFA singleCharNFA(char c)
{
    State *start = createState();
    State *accept = createState();
    start->transitions[c].push_back(accept);
    return {start, accept};
}

NFA concat(NFA a, NFA b)
{
    a.accept->transitions['\0'].push_back(b.start);
    return {a.start, b.accept};
}

NFA alternate(NFA a, NFA b)
{
    State *start = createState();
    State *accept = createState();
    start->transitions['\0'] = {a.start, b.start};
    a.accept->transitions['\0'].push_back(accept);
    b.accept->transitions['\0'].push_back(accept);
    return {start, accept};
}

NFA kleeneStar(NFA a)
{
    State *start = createState();
    State *accept = createState();
    start->transitions['\0'] = {a.start, accept};
    a.accept->transitions['\0'] = {a.start, accept};
    return {start, accept};
}

std::string addConcat(const std::string &regex)
{
    std::string result;
    for (size_t i = 0; i < regex.size(); ++i)
    {
        char c1 = regex[i];
        result += c1;
        if (i + 1 < regex.size())
        {
            char c2 = regex[i + 1];
            if ((isalnum(c1) || c1 == '*' || c1 == ')') &&
                (isalnum(c2) || c2 == '('))
            {
                result += '.';
            }
        }
    }
    return result;
}

int precedence(char op)
{
    switch (op)
    {
    case '*':
        return 3;
    case '.':
        return 2;
    case '|':
        return 1;
    default:
        return 0;
    }
}

std::string toPostfix(const std::string &regex)
{
    std::string postfix;
    std::stack<char> ops;
    for (char c : addConcat(regex))
    {
        if (isalnum(c))
        {
            postfix += c;
        }
        else if (c == '(')
        {
            ops.push(c);
        }
        else if (c == ')')
        {
            while (!ops.empty() && ops.top() != '(')
            {
                postfix += ops.top();
                ops.pop();
            }
            ops.pop(); // remove '('
        }
        else
        {
            while (!ops.empty() && precedence(ops.top()) >= precedence(c))
            {
                postfix += ops.top();
                ops.pop();
            }
            ops.push(c);
        }
    }
    while (!ops.empty())
    {
        postfix += ops.top();
        ops.pop();
    }
    return postfix;
}

NFA regexToNFA(const std::string &regex)
{
    stateId = 0; // OK Reset state IDs for each run

    std::stack<NFA> stack;
    std::string postfix = toPostfix(regex);
    for (char c : postfix)
    {
        if (isalnum(c))
        {
            stack.push(singleCharNFA(c));
        }
        else if (c == '*')
        {
            NFA a = stack.top();
            stack.pop();
            stack.push(kleeneStar(a));
        }
        else if (c == '.')
        {
            NFA b = stack.top();
            stack.pop();
            NFA a = stack.top();
            stack.pop();
            stack.push(concat(a, b));
        }
        else if (c == '|')
        {
            NFA b = stack.top();
            stack.pop();
            NFA a = stack.top();
            stack.pop();
            stack.push(alternate(a, b));
        }
    }
    return stack.top();
}

void printNFA(State *start)
{
    std::set<int> visited;
    std::stack<State *> stack;
    stack.push(start);
    while (!stack.empty())
    {
        State *curr = stack.top();
        stack.pop();
        if (visited.count(curr->id))
            continue;
        visited.insert(curr->id);
        for (auto &[c, nextStates] : curr->transitions)
        {
            for (State *next : nextStates)
            {
                std::cout << "State " << curr->id << " --"
                          << (c == '\0' ? "eps" : std::string(1, c))
                          << "--> State " << next->id << "\n";
                stack.push(next);
            }
        }
    }
}

#include <nlohmann/json.hpp> // use https://github.com/nlohmann/json
using json = nlohmann::json;

json exportToJson(State *start)
{
    std::set<int> visited;
    std::stack<State *> stack;
    json j;
    j["states"] = json::array();
    j["transitions"] = json::array();

    stack.push(start);
    while (!stack.empty())
    {
        State *curr = stack.top();
        stack.pop();
        if (visited.count(curr->id))
            continue;
        visited.insert(curr->id);
        j["states"].push_back(curr->id);

        for (auto &[c, nextStates] : curr->transitions)
        {
            for (State *next : nextStates)
            {
                j["transitions"].push_back({{"from", curr->id},
                                            {"to", next->id},
                                            {"symbol", c == '\0' ? "eps" : std::string(1, c)}});
                stack.push(next);
            }
        }
    }
    j["start"] = start->id;
    j["accept"] = stateId - 1; // last created state = accept
    return j;
}
