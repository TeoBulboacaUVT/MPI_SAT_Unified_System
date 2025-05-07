#include "../include/dpll/dpllsolver.h"
#include "../include/parser/DIMACSParser.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <set>

using namespace std;

bool isFileAccessible(const string& filename) {
    ifstream file(filename);
    return file.good();
}

string getFileNameFromPath(const string& path) {
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != string::npos) {
        return path.substr(lastSlash + 1);
    }
    return path;
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
        "C:\\Users\\Lenovo\\CLionProjects\\MPI_SAT_Unified\\Functional\\DIMACS\\extra\\ex.cnf",
    };


    // Open results file in append mode
    ofstream resultsFile("C:\\Users\\Lenovo\\CLionProjects\\MPI_SAT_Unified\\Functional\\Tests\\logs\\dpll_result.txt", ios::app);
    if (!resultsFile.is_open()) {
        cout << "Error: Could not open results file for writing" << endl;
        return 1;
    }

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
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

            // Write results to file
            resultsFile << getFileNameFromPath(filename) << endl;
            resultsFile << duration.count() << endl;

            cout << "Solving time: " << duration.count() << "ms" << endl;

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
            return 3;
        }
        catch (const exception& e) {
            cout << "Exception: " << e.what() << endl;
            return 1;
        }
        catch (...) {
            cout << "Unknown error occurred." << endl;
            return 2;
        }
        cout << "----------------------------------------\n" << endl;
    }

    resultsFile.close();
    return 0;
}