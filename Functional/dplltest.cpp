#include "dpllsolver.h"
#include "dpllsolver.cpp"
#include "DIMACSParser.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <set>
#include "SATSolver.cpp"
#include "DIMACSParser.cpp"
using namespace std;

bool isFileAccessible(const string& filename) {
    ifstream file(filename);
    return file.good();
}

bool verifySolution(const vector<int>& assignment, const vector<vector<int>>& originalClauses) {
    set<int> assigned;
    for (int lit : assignment) {
        if (assigned.count(-lit)) return false; // Contradiction!
        assigned.insert(lit);
    }
    for (const auto& clause : originalClauses) {
        bool clauseSatisfied = false;
        for (int literal : clause) {
            if (assigned.count(literal)) {
                clauseSatisfied = true;
                break;
            }
        }
        if (!clauseSatisfied) return false;
    }
    return true;
}

int main() {
    vector<string> testFiles = {
        "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\satdim.cnf",
        "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\satdim2.cnf",
        "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\satdim3.cnf",
        "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\unsatdim.cnf",
        "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\unsatdim2.cnf",
        "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\unsatdim3.cnf",
        "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\aim-50-1_6-no-1.cnf"
    };

    for (const auto& filename : testFiles) {
        try {
            if (!isFileAccessible(filename)) {
                cout << "Error: Cannot access file " << filename << endl;
                continue;
            }

            cout << "\nTesting DPLL Solver on file: " << filename << endl;
            cout << "----------------------------------------" << endl;

            auto start = chrono::high_resolution_clock::now();

            DPLLSolver solver(filename);
            DIMACSParser parser(filename);
            auto originalClauses = parser.getClauses();

            cout << "Parser initialized successfully" << endl;
            cout << "Formula statistics:" << endl;
            cout << "Variables: " << parser.getNumLiterals() << endl;
            cout << "Clauses: " << parser.getNumClauses() << endl;

            bool result = solver.solve();
            auto assignment = solver.getAssignment();

            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            cout << "Solving time: " << duration << "ms" << endl;

            if (result) {
                bool valid = verifySolution(assignment, originalClauses);
                cout << "Solution verification: " << (valid ? "VALID" : "INVALID") << endl;
                if (!valid) {
                    cout << "WARNING: Solver claims SAT but solution verification failed!" << endl;
                }
                cout << "Final result: The formula is SATISFIABLE." << endl;
            } else {
                cout << "Final result: The formula is UNSATISFIABLE." << endl;
            }
        }
        catch (const std::bad_alloc& e) {
            std::cerr << "Memory allocation failed: " << e.what() << std::endl;
        }
        catch (const exception& e) {
            cout << "Exception: " << e.what() << endl;
        }
        catch (...) {
            cout << "Unknown error occurred." << endl;
        }
    }
    return 0;
}