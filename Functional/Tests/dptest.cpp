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


int main() {
    vector<string> testFiles2 = {
        "C:\\Users\\Lenovo\\CLionProjects\\MPI_SAT_Unified\\Functional\\DIM\\aim-50-1_6-no-1.cnf"
    };

    for (const auto& filename : testFiles2) {
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

    return 0;
}