#include "../include/res/resolutionsolver.h"
#include "../include/general/satsolver.h"
#include "../include/parser/DIMACSParser.h"

#include <iostream>
#include <vector>
#include <chrono> // Include for timekeeping
#include <iomanip> // Include for setting precision
using namespace std;

int main() {
    // Specify the path to a DIMACS CNF file
    string filename = "C:\\Users\\Lenovo\\CLionProjects\\MPI_SAT_Unified\\Functional\\DIMACS\\extra\\ex.cnf";



    // Create an instance of ResolutionSolver
    ResolutionSolver solver(filename);

    // Test: Solve the SAT problem using resolution
    cout << "Testing Resolution Solver on file: " << filename << endl;

    // Start the clock
    auto start = chrono::high_resolution_clock::now();

    // Call the solver
    bool result = solver.solve();

    // Stop the clock
    auto end = chrono::high_resolution_clock::now();

    // Calculate and print the elapsed time
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Execution time: " << duration<< endl;

    // Output the result
    if (result) {
        cout << "The formula is SATISFIABLE." << endl;
    } else {
        cout << "The formula is UNSATISFIABLE." << endl;
    }

    return 0;
}