#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <memory>
#include "include/dpll/dpllsolver.h"
#include "include/dp/dpsolver.h"
#include "include/res/resolutionsolver.h"

using namespace std;

bool fileExists(const string& filepath) {
    ifstream file(filepath);
    return file.good();
}

void printMenu() {
    cout << "\n=== SAT Solver Console ===\n";
    cout << "Available solvers:\n";
    cout << "1. DPLL algorithm\n";
    cout << "2. Davis-Putnam algorithm\n";
    cout << "3. Resolution-based solver\n";
    cout << "0. Exit\n";
    cout << "Choose a solver (0-3): ";
}

string getSolverType(int choice) {
    switch (choice) {
        case 1: return "dpll";
        case 2: return "dp";
        case 3: return "res";
        default: return "";
    }
}

int main() {
    while (true) {
        printMenu();

        int choice;
        cin >> choice;

        if (choice == 0) {
            cout << "Goodbye!\n";
            break;
        }

        if (choice < 1 || choice > 3) {
            cout << "Invalid choice. Please try again.\n";
            continue;
        }

        string solver_type = getSolverType(choice);

        cout << "\nEnter the path to your DIMACS CNF file: ";
        string filename;
        cin >> filename;

        // Check if file exists
        if (!fileExists(filename)) {
            cout << "Error: File does not exist: " << filename << endl;
            continue;
        }


        cout << "\nSolving with " << solver_type << " solver...\n";
        try {
            if (solver_type == "dpll") {
                DPLLSolver solver(filename);
                DIMACSParser parser(filename);
                auto originalClauses = parser.getClauses();

                auto start = chrono::high_resolution_clock::now();
                cout << "Parser initialized successfully" << endl;
                cout << "Formula statistics:" << endl;
                cout << "Variables: " << parser.getNumLiterals() << endl;
                cout << "Clauses: " << parser.getNumClauses() << endl;

                bool result = solver.solve();
                auto assignment = solver.getAssignment();

                auto end = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

                if (result) {
                    cout << "Final result: The formula is SATISFIABLE." << endl;
                } else {
                    cout << "Final result: The formula is UNSATISFIABLE." << endl;
                }

                cout << "Solving time: " << duration.count() << "ms" << endl;

            } else if (solver_type == "dp") {
                DPSolver solver(filename);
                DIMACSParser parser(filename);
                auto originalClauses = parser.getClauses();

                auto start = chrono::high_resolution_clock::now();
                cout << "Parser initialized successfully" << endl;
                cout << "Formula statistics:" << endl;
                cout << "Variables: " << parser.getNumLiterals() << endl;
                cout << "Clauses: " << parser.getNumClauses() << endl;

                bool result = solver.solve();
                auto assignment = solver.getAssignment();

                auto end = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

                if (result) {
                    cout << "Final result: The formula is SATISFIABLE." << endl;
                } else {
                    cout << "Final result: The formula is UNSATISFIABLE." << endl;
                }

                cout << "Solving time: " << duration.count() << "ms" << endl;
            } else if (solver_type == "res") {
                ResolutionSolver solver(filename);
                DIMACSParser parser(filename);
                auto originalClauses = parser.getClauses();

                auto start = chrono::high_resolution_clock::now();
                cout << "Parser initialized successfully" << endl;
                cout << "Formula statistics:" << endl;
                cout << "Variables: " << parser.getNumLiterals() << endl;
                cout << "Clauses: " << parser.getNumClauses() << endl;

                bool result = solver.solve();

                auto end = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

                if (result) {
                    cout << "Final result: The formula is SATISFIABLE." << endl;
                } else {
                    cout << "Final result: The formula is UNSATISFIABLE." << endl;
                }

                cout << "Solving time: " << duration.count() << "ms" << endl;
            }


        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }

        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }

    return 0;
}
