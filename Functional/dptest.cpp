#include "dpsolver.cpp"
#include "SATSolver.cpp"
#include "DIMACSParser.cpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
using namespace std;

bool isFileAccessible(const string& filename) {
    ifstream file(filename);
    return file.good();
}

// Add function to verify solution
bool verifySolution(const vector<int>& assignment, const vector<vector<int>>& originalClauses) {
    for (const auto& clause : originalClauses) {
        bool clauseSatisfied = false;
        for (int literal : clause) {
            if (find(assignment.begin(), assignment.end(), literal) != assignment.end()) {
                clauseSatisfied = true;
                break;
            }
        }
        if (!clauseSatisfied) return false;
    }
    return true;
}

int main() {
    vector<string> testFiles1 = {
        "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\satdim.cnf",
        "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\satdim2.cnf",
        "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\satdim3.cnf",
        "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\unsatdim.cnf",
        "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\unsatdim2.cnf",
        "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\unsatdim3.cnf"
    };
    vector<string> testFiles2 = {
        "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\aim-50-1_6-no-1.cnf"
    };

    for (const auto& filename : testFiles2) {
        try {
            if (!isFileAccessible(filename)) {
                cout << "Error: Cannot access file " << filename << endl;
                continue;
            }

            cout << "\nTesting DP Solver on file: " << filename << endl;
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
            
            bool result = solver.solve();
            auto assignment = solver.getAssignment();
            
            
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

            cout << "\nSolving time: " << duration.count() << "ms" << endl;
            
            if (result) {
                bool verified = verifySolution(assignment, originalClauses);
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