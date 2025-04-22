#pragma once
#include <vector>
#include <string>
#include <map>
#include <nlohmann/json.hpp>

struct State
{
    int id;
    std::map<char, std::vector<State *>> transitions; // char = '\0' means ε
};

struct NFA
{
    State *start;
    State *accept;
};

NFA regexToNFA(const std::string &regex);
void printNFA(State *start);
nlohmann::json exportToJson(State *start);
