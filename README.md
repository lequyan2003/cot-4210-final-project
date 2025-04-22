# Interactive Visual Simulation of Regular Expression to Minimal DFA

This project is a C++ tool that visualizes the step-by-step transformation from a regular expression into a **minimal deterministic finite automaton (DFA)**. It provides educational insight into key automata theory algorithms: Thompson's construction, subset construction, and Hopcroft’s minimization.

## ✨ Features

- **Regex to NFA** using Thompson's construction
- **NFA to DFA** using subset construction (with ε-closures)
- **DFA Minimization** using Hopcroft’s algorithm
- **Input string simulation** with trace visualization
- **Graphviz-based visualization** for NFA, DFA, and Minimized DFA
- **Multiple CLI modes** for simulation, export, batch testing, and visualization

## 📁 Project Structure

```bash
Final_Project/
├── CMakeLists.txt
├── input.txt
├── output/                   # Output JSONs and result logs
│   ├── nfa.json
│   ├── dfa.json
│   ├── min_dfa.json
│   └── result.txt
├── include/                 # Header files
│   ├── dfa.h
│   ├── nfa.h
│   └── nlohmann/json.hpp
├── src/                     # Source code
│   ├── dfa.cpp
│   ├── nfa.cpp
│   └── main.cpp
├── test/                    # Unit testing
│   └── test_all.cpp
├── visualize/               # Python visualization
│   ├── visualize_nfa.py
│   ├── visualize_dfa.py
│   ├── visualize_min_dfa.py
│   ├── nfa_visual.png
│   ├── dfa_visual.png
│   └── min_dfa_visual.png
```

## ⚙️ Build Instructions

### Requirements

- C++17 compiler
- CMake >= 3.10
- Python 3 + `graphviz` (`pip install graphviz`)

### Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## 🚀 Usage

### Interactive Mode

```bash
./main
```

Enter regex and test strings interactively.

### Simulate a Regex on a String

```bash
./main --simulate "(a|b)*abb" abb --trace
./main --simulate "(a|b)*abb" abb --trace --min   # Minimized DFA
```

### Visualize Automata

```bash
./main --visualize "(a|b)*abb"          # NFA + DFA
./main --visualize-min "(a|b)*abb"      # Minimized DFA
```

### Run from File (e.g. input.txt)

```bash
./main --file input.txt
```

### Batch Test All Regexes

```bash
./main --test
```

## ✅ Test File (test_all.cpp)

Includes assertions for:

- Accepted and rejected strings
- DFA state count bounds
- Detection of dead states
- Minimized DFA state count

## 📊 Example Output

- Trace: `0 -> 1 -> 2 -> 3`
- Result: `[OK] Accepted`
- Output: `visualize/min_dfa_visual.png`

## 📚 Algorithms Used

- **Thompson’s Construction**: Build ε-NFA from regex
- **Subset Construction**: Convert ε-NFA to DFA
- **Hopcroft’s Algorithm**: Minimize DFA using partition refinement

## 👨‍💻 Authors

- Quy An Le
- Bao Long Duong
- An Nguyen

## 📄 License

Educational use only. Built for COT 4210 Final Project.
