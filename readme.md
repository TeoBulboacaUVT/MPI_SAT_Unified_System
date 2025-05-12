# MPI SAT Unified System

## Overview
A unified SAT (Boolean Satisfiability) solving system that implements multiple solving algorithms including DPLL, Davis-Putnam, and Resolution-based approaches.

## Table of Contents
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Algorithms](#algorithms)
- [Input Format](#input-format)
- [Building from Source](#building-from-source)
- [Testing](#testing)
- [Contributing](#contributing)
- [License](#license)

## Features
- Multiple SAT solving algorithms:
    - DPLL algorithm
    - Davis-Putnam algorithm
    - Resolution-based solver
- Performance measurement
- DIMACS CNF file support
- Interactive console interface

## Prerequisites
- C++ compiler with C++11 support
- CMake (version 3.10 or higher)
- Make or equivalent build system

## Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/TeoBulboacaUVT/MPI_SAT_Unified_System.git
   ```

2. Open the project in your preferred IDE (e.g., CLion, Visual Studio, etc.)

3. Choose your testing approach:
    - **Single File Testing**:
        - Open and run `Functional/main.cpp`
        - This allows you to test individual DIMACS CNF files through the interactive console interface
        - Follow the on-screen prompts to select a solver and input file

    - **Batch Testing**:
        - Navigate to `Functional/Tests/[solver]test.cpp` (where [solver] is the specific algorithm you want to test)
        - Run the test file to process multiple DIMACS CNF files at once
        - This will provide comprehensive output for all test cases

## Usage

### Single File Testing (main.cpp)
1. Run the program
2. Select a solver (1-3):
    - 1: DPLL algorithm
    - 2: Davis-Putnam algorithm
    - 3: Resolution-based solver
3. Enter the absolute file path to your DIMACS CNF file
4. The program will output:
    - Result: SAT or UNSAT
    - Solving time (in milliseconds)

### Batch Testing ([solver]test.cpp)
1. Open the desired test file (e.g., `dplltest.cpp`, `dptest.cpp`, or `restest.cpp`)
2. Add your DIMACS file paths to the test vector
3. Run the test file
4. The program will:
    - Process all files in the vector
    - Display full solver output for each file
    - Show runtime statistics
    - Write results to `[solver]results.txt` containing:
        - Filename
        - Solving time (in milliseconds)

## Project Structure
```
.
├── Functional/           # Main source code directory
│   ├── include/         # Header files
│   ├── DIMACS/         # DIMACS format related files
│   ├── Tests/          # Test cases
│   └── main.cpp        # Main program entry point
├── build/              # Build directory
└── CMakeLists.txt     # CMake configuration
```

## Algorithms

### 1. DPLL (Davis-Putnam-Logemann-Loveland) Algorithm
The DPLL algorithm is a complete, backtracking-based search algorithm for determining the satisfiability of propositional logic formulae in conjunctive normal form (CNF).

Key Functions:
- `unitPropagation()`: Identifies and assigns values to unit clauses
- `pureLiteralElimination()`: Identifies and assigns values to pure literals
- `chooseLiteral()`: Selects the next literal for branching
- `backtrack()`: Handles backtracking when a conflict is found
- `solve()`: Main solving function that orchestrates the search process

### 2. Davis-Putnam Algorithm
The Davis-Putnam algorithm is a resolution-based algorithm that systematically eliminates variables from the formula.

Key Functions:
- `eliminateVariable()`: Removes a variable through resolution
- `resolve()`: Performs resolution between clauses
- `findPureLiteral()`: Identifies pure literals for elimination
- `simplify()`: Simplifies the formula after each elimination
- `solve()`: Main solving function that manages the elimination process

### 3. Resolution-based Solver
This solver uses the resolution inference rule to derive new clauses until either an empty clause (UNSAT) is found or no new clauses can be derived (SAT).

Key Functions:
- `resolveClauses()`: Performs resolution between two clauses
- `addNewClause()`: Adds newly derived clauses to the formula
- `checkContradiction()`: Checks for empty clause derivation
- `simplifyClauses()`: Removes redundant clauses
- `solve()`: Main solving function that manages the resolution process

Common Features Across All Solvers:
- DIMACS CNF file parsing
- Performance measurement
- Result verification
- Memory management for large formulas

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

