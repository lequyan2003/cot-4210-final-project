#include "nfa.h"
#include "dfa.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <filesystem>

void runInteractive()
{
    std::string regex;
    std::cout << "Enter regex: ";
    std::cin >> regex;

    NFA nfa = regexToNFA(regex);
    std::cout << "\nNFA transitions:\n";
    printNFA(nfa.start);

    std::ofstream f1("output/nfa.json");
    f1 << exportToJson(nfa.start).dump(4);
    std::cout << "[OK] NFA JSON saved to output/nfa.json\n";
    f1.close();

    DFA dfa = convertNFAtoDFA(nfa.start, nfa.accept->id);
    std::cout << "\nDFA transitions:\n";
    printDFA(dfa);

    std::ofstream f2("output/dfa.json");
    f2 << exportDFAtoJson(dfa).dump(4);
    std::cout << "[OK] DFA JSON saved to output/dfa.json\n";
    f2.close();

    std::string testStr;
    std::cout << "\nEnter test string: ";
    std::cin >> testStr;

    std::vector<int> trace;
    bool accepted = simulateDFA(dfa, testStr, trace);

    std::cout << "Trace: ";
    for (size_t i = 0; i < trace.size(); ++i)
    {
        std::cout << trace[i];
        if (i + 1 < trace.size())
            std::cout << " -> ";
    }
    std::cout << "\nResult: " << (accepted ? "[OK] Accepted" : "[X] Rejected") << "\n";
}

void runBatchTests()
{
    std::vector<std::string> testCases = {
        "a", "b", "a|b", "a*", "ab", "(a|b)*abb", "(a|(b|c)*)d"};
    std::vector<std::string> inputs = {"", "a", "b", "ab", "abb", "aaa", "abc", "bb"};

    for (const auto &regex : testCases)
    {
        std::cout << "===== Testing regex: " << regex << " =====\n";
        NFA nfa = regexToNFA(regex);
        DFA dfa = convertNFAtoDFA(nfa.start, nfa.accept->id);
        printDFA(dfa);

        for (const auto &input : inputs)
        {
            std::vector<int> trace;
            bool result = simulateDFA(dfa, input, trace);
            std::cout << "  Input: \"" << input << "\" -> ";
            for (size_t i = 0; i < trace.size(); ++i)
            {
                std::cout << trace[i];
                if (i + 1 < trace.size())
                    std::cout << " -> ";
            }
            std::cout << " => " << (result ? "[OK] Accepted" : "[X] Rejected") << "\n";
        }
        std::cout << "\n";
    }
}

void runDFAMode(const std::string &regex)
{
    std::cout << "Generating DFA for: " << regex << "\n";
    NFA nfa = regexToNFA(regex);
    DFA dfa = convertNFAtoDFA(nfa.start, nfa.accept->id);

    std::filesystem::create_directories("output");

    std::ofstream f("output/dfa.json");
    f << exportDFAtoJson(dfa).dump(4);
    std::cout << "[OK] DFA JSON saved to output/dfa.json\n";
    f.close();
}

void runSimulateMode(const std::string &regex, const std::string &input, bool verbose, bool minimized)
{
    std::cout << "Simulating " << (minimized ? "MINIMIZED " : "") << "DFA for regex: " << regex << " on input: " << input << "\n";
    NFA nfa = regexToNFA(regex);
    DFA dfa = convertNFAtoDFA(nfa.start, nfa.accept->id);
    if (minimized)
        dfa = minimizeDFA(dfa);

    std::vector<int> trace;
    printDFA(dfa);
    bool accepted = simulateDFA(dfa, input, trace, verbose);

    std::cout << "Trace: ";
    for (size_t i = 0; i < trace.size(); ++i)
    {
        std::cout << trace[i];
        if (i + 1 < trace.size())
            std::cout << " -> ";
    }
    std::cout << "\nResult: " << (accepted ? "[OK] Accepted" : "[X] Rejected") << "\n";
}

void runFileMode(const std::string &filename)
{
    std::ifstream infile(filename);
    if (!infile)
    {
        std::cerr << "[X] Cannot open file: " << filename << "\n";
        return;
    }

    std::string regex;
    if (!std::getline(infile, regex))
    {
        std::cerr << "[X] Empty file or missing regex.\n";
        return;
    }

    NFA nfa = regexToNFA(regex);
    DFA dfa = convertNFAtoDFA(nfa.start, nfa.accept->id);

    std::ofstream outfile("output/result.txt");
    if (!outfile)
    {
        std::cerr << "[X] Cannot write to output/result.txt\n";
        return;
    }

    outfile << "Regex: " << regex << "\n\n";
    std::string line;
    while (std::getline(infile, line))
    {
        std::vector<int> trace;
        bool accepted = simulateDFA(dfa, line, trace);

        outfile << "Input: \"" << line << "\" => ";
        for (size_t i = 0; i < trace.size(); ++i)
        {
            outfile << trace[i];
            if (i + 1 < trace.size())
                outfile << " -> ";
        }
        outfile << " => " << (accepted ? "[OK] Accepted" : "[X] Rejected") << "\n";
    }

    std::cout << "[OK] Results saved to output/result.txt\n";
}

void runVisualizeAll(const std::string &regex)
{
    std::cout << "Generating and visualizing NFA + DFA for: " << regex << "\n";

    NFA nfa = regexToNFA(regex);
    std::ofstream f1("output/nfa.json");
    f1 << exportToJson(nfa.start).dump(4);
    f1.close();

    DFA dfa = convertNFAtoDFA(nfa.start, nfa.accept->id);
    std::ofstream f2("output/dfa.json");
    f2 << exportDFAtoJson(dfa).dump(4);
    f2.close();

    std::cout << "[OK] JSONs exported.\nRendering images...\n";

    int res1 = system("python visualize/visualize_nfa.py");
    int res2 = system("python visualize/visualize_dfa.py");

    if (res1 == 0 && res2 == 0)
        std::cout << "[OK] Visuals rendered in visualize/nfa_visual.png and dfa_visual.png\n";
    else
        std::cerr << "[X] Visualization failed. Make sure Python & graphviz are installed.\n";
}

void runVisualizeMinimizedDFA(const std::string &regex)
{
    std::cout << "Generating and visualizing MINIMIZED DFA for: " << regex << "\n";

    NFA nfa = regexToNFA(regex);
    DFA dfa = convertNFAtoDFA(nfa.start, nfa.accept->id);
    DFA minDFA = minimizeDFA(dfa);

    std::filesystem::create_directories("output");

    std::ofstream f("output/min_dfa.json");
    f << exportDFAtoJson(minDFA).dump(4);
    f.close();

    std::cout << "[OK] Minimized DFA JSON exported.\nRendering minimized DFA image...\n";

    int res = system("python visualize/visualize_min_dfa.py");
    if (res == 0)
        std::cout << "[OK] Minimized DFA image rendered to visualize/min_dfa_visual.png\n";
    else
        std::cerr << "[X] Visualization failed. Ensure Python & graphviz are installed.\n";
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        std::string mode = argv[1];
        if (mode == "--test")
        {
            runBatchTests();
        }
        else if (mode == "--dfa" && argc > 2)
        {
            runDFAMode(argv[2]);
        }
        else if (mode == "--simulate" && argc > 3)
        {
            bool traceFlag = false;
            bool minimized = false;

            for (int i = 4; i < argc; ++i)
            {
                std::string flag = argv[i];
                if (flag == "--trace")
                    traceFlag = true;
                else if (flag == "--min")
                    minimized = true;
            }

            runSimulateMode(argv[2], argv[3], traceFlag, minimized);
        }
        else if (mode == "--file" && argc > 2)
        {
            runFileMode(argv[2]);
        }
        else if (mode == "--visualize" && argc > 2)
        {
            runVisualizeAll(argv[2]);
        }
        else if (mode == "--visualize-min" && argc > 2)
        {
            runVisualizeMinimizedDFA(argv[2]);
        }
        else if (mode == "--minimize" && argc > 2)
        {
            std::string regex = argv[2];
            NFA nfa = regexToNFA(regex);
            DFA dfa = convertNFAtoDFA(nfa.start, nfa.accept->id);
            DFA minDFA = minimizeDFA(dfa);

            std::filesystem::create_directories("output");
            std::ofstream f("output/min_dfa.json");
            f << exportDFAtoJson(minDFA).dump(4);
            f.close();
            std::cout << "[OK] Minimized DFA JSON saved to output/min_dfa.json\n";
        }
        else
        {
            std::cerr << "Usage:\n"
                      << "  ./main                             (interactive mode)\n"
                      << "  ./main --test                      (batch tests)\n"
                      << "  ./main --dfa REGEX                 (export DFA JSON to output/dfa.json)\n"
                      << "  ./main --simulate R S [--trace] [--min]   (run DFA on input S with optional trace and minimized mode)\n"
                      << "  ./main --file input.txt            (evaluate all strings in input.txt)\n"
                      << "  ./main --visualize REGEX           (generate NFA + DFA images)\n"
                      << "  ./main --visualize-min REGEX       (generate minimized DFA image)\n"
                      << "  ./main --minimize REGEX            (export minimized DFA JSON to output/min_dfa.json)\n";
        }
    }
    else
    {
        runInteractive();
    }
    return 0;
}
