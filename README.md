# Interactive Visual Simulation of Regular Expression to Minimal DFA

A C++ tool for visualizing the step-by-step transformation from a regular expression into a **minimal deterministic finite automaton (DFA)**. Designed for educational use, this project demonstrates key automata theory algorithms such as Thompson's construction, subset construction, and Hopcroft’s minimization.

---

## ✨ Features

- ✅ Regex ➔ NFA via **Thompson's construction**
- ✅ NFA ➔ DFA via **subset construction** (with ε-closures)
- ✅ DFA ➔ Minimal DFA via **Hopcroft’s algorithm**
- 🌟 **Simulation** of input strings with trace logging
- 📊 **Graphviz-based visualization** (NFA, DFA, Minimized DFA)
- ⚙️ Multiple CLI modes: simulation, batch testing, export, visualization
- 🌐 **FastAPI backend** for web-based interaction via RESTful API

---

## 📁 Project Structure

```bash
cpp/
├── CMakeLists.txt
├── input.txt
├── include/                 # Header files
│   ├── nfa.h
│   ├── dfa.h
│   └── nlohmann/json.hpp
├── src/                     # C++ source files
│   ├── main.cpp
│   ├── dfa.cpp
│   └── nfa.cpp
├── test/                    # Unit tests
│   └── test_all.cpp
├── build/                   # Build artifacts (CMake)
├── output/                  # JSON outputs
│   ├── nfa.json
│   ├── dfa.json
│   ├── min_dfa.json
│   └── result.txt
├── visualize/               # Python Graphviz scripts
│   ├── visualize_nfa.py
│   ├── visualize_dfa.py
│   ├── visualize_min_dfa.py
│   └── *.png (auto-generated visuals)
├── server/                  # FastAPI backend
│   ├── main.py
│   └── requirements.txt
└── README.md
```

---

## ⚙️ Build Instructions

### Requirements

- C++17-compatible compiler
- CMake ≥ 3.10
- Python 3
- `graphviz` (install via `pip install graphviz`)

### Build the C++ Project

```bash
cp cpp
mkdir build
cd build
cmake ..
cmake --build .
```

---

## 🚀 CLI Usage

### 1. Interactive Mode

```bash
./main
```

### 2. Simulate a Regex on a String

```bash
./main --simulate "(a|b)*abb" abb --trace
./main --simulate "(a|b)*abb" abb --trace --min   # using minimized DFA
```

### 3. Visualize Automata

```bash
./main --visualize "(a|b)*abb"         # generate NFA and DFA
./main --visualize-min "(a|b)*abb"     # generate Minimized DFA
```

### 4. Run from Input File

```bash
./main --file input.txt
```

### 5. Batch Test Mode

```bash
./main --test
```

---

## 🌐 FastAPI Server

Control the C++ backend through a Python-based REST API.

### 1. Setup Virtual Environment

```bash
cd server
python -m venv venv
```

### 2. Activate Environment

- **Windows**:

  ```bash
  venv\Scripts\activate
  ```

- **macOS/Linux**:
  ```bash
  source venv/bin/activate
  ```

### 3. Install Dependencies

```bash
pip install -r requirements.txt
```

### 4. Start the Server

```bash
uvicorn main:app --reload
```

### 5. API Endpoints

```bash
| Method | Endpoint                     | Description                           |
|--------|------------------------------|---------------------------------------|
| POST   | `/generate`                  | Generate NFA, DFA, Minimized DFA      |
| POST   | `/simulate`                  | Simulate DFA with trace output        |
| GET    | `/visuals/nfa|dfa|min_dfa`   | Return PNG visualization              |
| GET    | `/json/nfa|dfa|min_dfa`      | Return JSON structure                 |
```

---

## ✅ Test Suite

- Located in: `test/test_all.cpp`
- Covers:
  - String acceptance/rejection
  - State count limits
  - Dead state detection
  - Minimization effectiveness

### Run tests:

```bash
./test_all
```

---

## 📊 Example Output

```
Trace: 0 -> 1 -> 2 -> 3
Result: [OK] Accepted
Output: visualize/min_dfa_visual.png
```

---

## 📚 Algorithms Used

- **Thompson’s Construction**: Builds ε-NFA from regex
- **Subset Construction**: Converts ε-NFA to DFA
- **Hopcroft’s Algorithm**: Minimizes DFA via partition refinement

---

## 👨‍💻 Authors

- Quy An Le
- Bao Long Duong
- An Nguyen

---

## 📄 License

**Educational use only.**  
Developed for the COT 4210 Final Project.
