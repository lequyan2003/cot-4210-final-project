#pragma once
#include <set>
#include <map>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

struct DFAState
{
    int id;
    std::set<int> nfaStates;         // The NFA states this DFA state represents
    std::map<char, int> transitions; // input -> DFA state ID
    bool isAccept = false;
};

struct DFA
{
    int startState;
    std::map<int, DFAState> states; // id -> DFAState
};

DFA convertNFAtoDFA(State *nfaStart, int nfaAcceptId);
nlohmann::json exportDFAtoJson(const DFA &dfa);
void printDFA(const DFA &dfa);
bool isDeadState(const DFAState &state, const std::set<int> &acceptStates);
bool simulateDFA(const DFA &dfa, const std::string &input, std::vector<int> &trace, bool verbose = false);
DFA minimizeDFA(const DFA &dfa);
