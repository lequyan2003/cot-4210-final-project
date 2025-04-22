#include "nfa.h"
#include "dfa.h"
#include <queue>
#include <set>
#include <map>
#include <iostream>

using namespace std;

// DFA conversion from NFA
DFA convertNFAtoDFA(State *start, int nfaAcceptId)
{
    DFA dfa;
    map<set<State *>, int> stateMap;
    map<int, set<State *>> reverseMap; // Track DFA state ID -> NFA state set
    int idCounter = 0;

    // Compute ε-closure of the start state
    set<State *> startClosure = {start};
    queue<State *> q;
    set<int> visited;
    q.push(start);
    visited.insert(start->id);

    while (!q.empty())
    {
        State *curr = q.front();
        q.pop();
        for (State *epsNext : curr->transitions['\0'])
        {
            if (!visited.count(epsNext->id))
            {
                startClosure.insert(epsNext);
                q.push(epsNext);
                visited.insert(epsNext->id);
            }
        }
    }

    int startId = idCounter++;
    stateMap[startClosure] = startId;
    reverseMap[startId] = startClosure;

    DFAState startDFA;
    startDFA.id = startId;
    for (State *s : startClosure)
        startDFA.nfaStates.insert(s->id);
    for (State *s : startClosure)
        if (s->id == nfaAcceptId)
            startDFA.isAccept = true;

    dfa.states[startId] = startDFA;

    queue<int> worklist;
    worklist.push(startId);

    while (!worklist.empty())
    {
        int currId = worklist.front();
        worklist.pop();
        DFAState &currDFA = dfa.states[currId];

        map<char, set<State *>> transitions;
        for (State *s : reverseMap[currId])
        {
            for (auto &[symbol, vec] : s->transitions)
            {
                if (symbol == '\0')
                    continue;
                for (State *dest : vec)
                    transitions[symbol].insert(dest);
            }
        }

        for (auto &[symbol, nextStates] : transitions)
        {
            set<State *> closureSet = nextStates;
            queue<State *> q;
            set<int> visited;
            for (State *s : nextStates)
            {
                q.push(s);
                visited.insert(s->id);
            }
            while (!q.empty())
            {
                State *curr = q.front();
                q.pop();
                for (State *epsNext : curr->transitions['\0'])
                {
                    if (!visited.count(epsNext->id))
                    {
                        closureSet.insert(epsNext);
                        q.push(epsNext);
                        visited.insert(epsNext->id);
                    }
                }
            }

            if (!stateMap.count(closureSet))
            {
                int newId = idCounter++;
                stateMap[closureSet] = newId;
                reverseMap[newId] = closureSet;

                DFAState newDFA;
                newDFA.id = newId;
                for (State *s : closureSet)
                    newDFA.nfaStates.insert(s->id);
                for (State *s : closureSet)
                    if (s->id == nfaAcceptId)
                        newDFA.isAccept = true;
                dfa.states[newId] = newDFA;
                worklist.push(newId);
            }

            currDFA.transitions[symbol] = stateMap[closureSet];
        }
    }

    dfa.startState = startId;
    return dfa;
}

#include <nlohmann/json.hpp>
using json = nlohmann::json;

json exportDFAtoJson(const DFA &dfa)
{
    json j;
    j["states"] = json::array();
    j["transitions"] = json::array();
    j["start"] = dfa.startState;
    j["accept"] = json::array();
    j["dead"] = json::array();

    std::set<int> stateIds;
    for (const auto &pair : dfa.states)
        stateIds.insert(pair.first);

    for (auto &[id, s] : dfa.states)
    {
        j["states"].push_back(id);
        if (s.isAccept)
            j["accept"].push_back(id);
        if (isDeadState(s, stateIds))
            j["dead"].push_back(id);
        for (auto &[c, dest] : s.transitions)
            j["transitions"].push_back({{"from", id}, {"to", dest}, {"symbol", std::string(1, c)}});
    }
    return j;
}

void printDFA(const DFA &dfa)
{
    for (auto &[id, s] : dfa.states)
    {
        std::cout << "State " << id << (s.isAccept ? " (accept)" : "") << ":";
        for (auto &[c, dest] : s.transitions)
            std::cout
                << "  --" << c << "--> " << dest << "\n";
    }
}

bool isDeadState(const DFAState &state, const std::set<int> &acceptStates)
{
    if (state.isAccept)
        return false;
    for (auto &[_, dest] : state.transitions)
        if (acceptStates.count(dest))
            return false;
    return true;
}

bool simulateDFA(const DFA &dfa, const std::string &input, std::vector<int> &trace, bool verbose)
{
    int current = dfa.startState;
    trace.push_back(current);

    for (char c : input)
    {
        if (verbose)
            std::cout << "Current state: " << current << ", reading '" << c << "'\n";

        if (!dfa.states.at(current).transitions.count(c))
        {
            if (verbose)
                std::cout << "[X] No transition for '" << c << "'\n";
            return false;
        }
        current = dfa.states.at(current).transitions.at(c);
        trace.push_back(current);
    }

    if (verbose)
        std::cout << "Final state: " << current << (dfa.states.at(current).isAccept ? " (accepting)\n" : " (not accepting)\n");

    return dfa.states.at(current).isAccept;
}

DFA minimizeDFA(const DFA &dfa)
{
    using Partition = std::set<int>;
    using PartitionSet = std::set<Partition>;

    // Step 1: Initial partition — accept vs non-accept
    Partition accept, nonAccept;
    for (const auto &[id, state] : dfa.states)
    {
        if (state.isAccept)
            accept.insert(id);
        else
            nonAccept.insert(id);
    }

    std::vector<char> alphabet;
    std::set<char> alphaSet;
    for (const auto &[id, state] : dfa.states)
    {
        for (const auto &[sym, _] : state.transitions)
            alphaSet.insert(sym);
    }
    for (char c : alphaSet)
        alphabet.push_back(c);

    PartitionSet P;
    if (!accept.empty())
        P.insert(accept);
    if (!nonAccept.empty())
        P.insert(nonAccept);

    std::queue<std::pair<Partition, char>> workQueue;
    for (char c : alphabet)
    {
        if (!accept.empty())
            workQueue.push({accept, c});
        if (!nonAccept.empty())
            workQueue.push({nonAccept, c});
    }

    while (!workQueue.empty())
    {
        auto [A, c] = workQueue.front();
        workQueue.pop();

        std::map<Partition, Partition> newPartitions;
        for (const auto &Y : P)
        {
            Partition Y1, Y2;
            for (int q : Y)
            {
                const auto &trans = dfa.states.at(q).transitions;
                if (trans.count(c) && A.count(trans.at(c)))
                    Y1.insert(q);
                else
                    Y2.insert(q);
            }

            if (!Y1.empty() && !Y2.empty())
            {
                P.erase(Y);
                P.insert(Y1);
                P.insert(Y2);
                for (char a : alphabet)
                {
                    workQueue.push({Y1, a});
                    workQueue.push({Y2, a});
                }
                break;
            }
        }
    }

    // Step 2: Build new minimized DFA
    std::map<int, int> stateToGroup;
    int groupId = 0;
    std::map<Partition, int> groupMap;

    for (const auto &block : P)
    {
        for (int id : block)
            stateToGroup[id] = groupId;
        groupMap[block] = groupId++;
    }

    DFA minDFA;
    for (const auto &[block, gid] : groupMap)
    {
        DFAState newState;
        newState.id = gid;

        // Representative state (any) for transitions and accept
        int rep = *block.begin();
        newState.isAccept = dfa.states.at(rep).isAccept;

        for (auto [sym, dest] : dfa.states.at(rep).transitions)
            newState.transitions[sym] = stateToGroup[dest];

        minDFA.states[gid] = newState;
    }

    minDFA.startState = stateToGroup[dfa.startState];
    return minDFA;
}
