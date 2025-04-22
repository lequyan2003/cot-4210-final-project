#include "../include/nfa.h"
#include "../include/dfa.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <set>

// OK Check if accepted/rejected inputs match expectation
void checkAccepts(const std::string &regex, const std::vector<std::string> &accepted, const std::vector<std::string> &rejected)
{
    std::cout << "cTesting regex: " << regex << "\n";

    NFA nfa = regexToNFA(regex);
    DFA dfa = convertNFAtoDFA(nfa.start, nfa.accept->id);

    for (const auto &input : accepted)
    {
        std::vector<int> trace;
        bool result = simulateDFA(dfa, input, trace);
        std::cout << "  [OK] Accept test: \"" << input << "\" => " << (result ? "Passed" : "[X] Failed") << "\n";
        assert(result);
    }

    for (const auto &input : rejected)
    {
        std::vector<int> trace;
        bool result = simulateDFA(dfa, input, trace);
        std::cout << "  [X] Reject test: \"" << input << "\" => " << (!result ? "Passed" : "[X] Failed") << "\n";
        assert(!result);
    }
}

// OK Check number of DFA states
void checkDFAStateCount(const std::string &regex, int expectedMin, int expectedMax)
{
    NFA nfa = regexToNFA(regex);
    DFA dfa = convertNFAtoDFA(nfa.start, nfa.accept->id);

    int actual = dfa.states.size();
    std::cout << "  ## DFA state count: " << actual << " (expected between " << expectedMin << " and " << expectedMax << ")\n";
    assert(actual >= expectedMin && actual <= expectedMax);
}

void checkMinimizedStateCount(const std::string &regex, int expectedMax)
{
    NFA nfa = regexToNFA(regex);
    DFA dfa = convertNFAtoDFA(nfa.start, nfa.accept->id);
    DFA min = minimizeDFA(dfa);

    std::cout << "  >> Minimized DFA states: " << min.states.size() << " (should be ≤ " << expectedMax << ")\n";
    assert(min.states.size() <= expectedMax);
}

// OK Check for dead states
void checkDeadStates(const std::string &regex)
{
    NFA nfa = regexToNFA(regex);
    DFA dfa = convertNFAtoDFA(nfa.start, nfa.accept->id);

    std::set<int> acceptSet;
    for (const auto &[id, state] : dfa.states)
        if (state.isAccept)
            acceptSet.insert(id);

    for (const auto &[id, state] : dfa.states)
    {
        if (isDeadState(state, acceptSet))
        {
            std::cout << "  [X] Dead state detected: " << id << "\n";
            // No assert: dead states are valid, just log them
        }
    }
}

int main()
{
    std::cout << "===== [OK] Starting Enhanced Tests =====\n\n";

    // OK Accept/Reject logic
    checkAccepts("(a|b)*abb",
                 {"abb", "aabb", "aaabb", "babb"},
                 {"aab", "ab", "", "abc", "bcbc"});

    checkAccepts("a*",
                 {"", "a", "aa", "aaa"},
                 {"b", "ab", "ba"});

    checkAccepts("a|b",
                 {"a", "b"},
                 {"ab", "", "ba"});

    // OK DFA State Count Range Checks
    checkDFAStateCount("(a|b)*abb", 4, 15);
    checkDFAStateCount("a*", 2, 5);
    checkDFAStateCount("a|b", 2, 4);

    // OK Dead State Checks
    checkDeadStates("(a|b)*abb");
    checkDeadStates("a*");
    checkDeadStates("a|b");

    std::cout << "\n===== [OK] Testing Minimized DFA State Counts =====\n";
    checkMinimizedStateCount("(a|b)*abb", 5); // Depends on construction
    checkMinimizedStateCount("a*", 2);
    checkMinimizedStateCount("a|b", 2);

    std::cout << "\n[OK] All assertions passed.\n";
    return 0;
}
