#include "SATSolver.cpp"
#include "DIMACSParser.cpp"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    // Specify the path to a DIMACS CNF file
    string filename = "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\DIM\\satdim.cnf";

    // Create an instance of SATSolver
    SATSolver solver(filename);

    // Test: Print the initial clauses
    cout << "Initial Clauses:" << endl;
    solver.printClauses();

    // Test: Unit Propagation
    vector<int> assignment;
    solver.unitPropagation(assignment);
    cout << "\nClauses after Unit Propagation:" << endl;
    solver.printClauses();
    cout << "Assignment after Unit Propagation: ";
    for (int literal : assignment) {
        cout << literal << " ";
    }
    cout << endl; // Add this to properly close the output

    // Test: Eliminate Pure Literals
    solver.eliminatePureLiterals(assignment);
    cout << "\nClauses after Eliminating Pure Literals:" << endl;
    solver.printClauses();
    cout << "Assignment after Eliminating Pure Literals: ";
    for (int literal : assignment) {
        cout << literal << " ";
    }
    cout << endl;

    // Test: Check for Empty Clauses
    if (solver.checkEmptyClause()) {
        cout << "\nAn empty clause was found. The formula is unsatisfiable." << endl;
    } else if (solver.getClauses().empty()) {
        cout << "\nAll clauses are satisfied. The formula is satisfiable." << endl;
    } else {
        cout << "\nNo empty clauses found. The formula might be satisfiable." << endl;
    }

    return 0; // Properly close the main function
}


