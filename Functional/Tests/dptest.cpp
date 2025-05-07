#include "../include/dp/dpsolver.h"
#include "../include/general/satsolver.h"
#include "../include/parser/DIMACSParser.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
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

int main() {
    vector<string> testFiles = {
        "C:\\Users\\Lenovo\\CLionProjects\\MPI_SAT_Unified\\Functional\\DIMACS\\extra\\ex.cnf",
    };
    vector<string> testFiles2 = {
        "C:\\Users\\Lenovo\\CLionProjects\\MPI_SAT_Unified\\Functional\\DIMACS\\extra\\twsat.cnf",
    };

    // Open results file in append mode
    ofstream resultsFile("dpresults.txt", ios::app);
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

            cout << "\nTesting dp Solver on file: " << filename << endl;
            cout << "----------------------------------------" << endl;
            
            auto start = chrono::high_resolution_clock::now();
            
            DPSolver solver(filename);
            auto originalClauses = solver.getClauses();
            
            cout << "Parser initialized successfully" << endl;
            cout << "Formula statistics:" << endl;
            cout << "Variables: " << solver.getNumLiterals() << endl;
            cout << "Clauses: " << solver.getNumClauses() << endl;
            
            if (solver.getNumLiterals() > 1000 || solver.getNumClauses() > 1000) {
                cout << "Warning: Large formula detected. This might take a while..." << endl;
            }
            
            bool result = solver.solve_new();
            auto assignment = solver.getAssignment();
            
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

            // Write results to file
            resultsFile << getFileNameFromPath(filename) << endl;
            resultsFile << duration.count() << endl;

            cout << "\nSolving time: " << duration.count() << "ms" << endl;
            
            if (result) {
                bool verified = solver.verifySolution(assignment, originalClauses);
                cout << "Solution verification: " << (verified ? "VALID" : "INVALID") << endl;
                
                if (!verified) {
                    cout << "WARNING: Solver claims SAT but solution verification failed!" << endl;
                    result = false;
                }
            }

            cout << "Final result: ";
            if (result) {
                cout << "The formula is SATISFIABLE." << endl;
            } else {
                cout << "The formula is UNSATISFIABLE." << endl;
            }
        }
        catch (const std::bad_alloc& e) {
            cout << "Memory allocation failed: " << e.what() << endl;
            return 3;
        }
        catch (const exception& e) {
            cout << "Error processing file: " << e.what() << endl;
            return 1;
        }
        catch (...) {
            cout << "Unknown error occurred while processing file" << endl;
            return 2;
        }
        cout << "----------------------------------------\n" << endl;
    }

    resultsFile.close();
    return 0;
}