#include "dpllsolver.h"
#include <iostream>
using namespace std;

DPLLSolver::DPLLSolver(const string& filename) : SATSolver(filename) {}

bool DPLLSolver::solve() {
    cout << "Solving using DPLL..." << endl;
    vector<int> assignment;
    bool result = dpllRecursive(assignment);
    if (result) {
        finalAssignment = assignment;
        cout << "SATISFIABLE" << endl;
        printAssignment(finalAssignment);
    } else {
        cout << "UNSATISFIABLE" << endl;
    }
    return result;
}

std::vector<int> DPLLSolver::getAssignment() const {
    return finalAssignment;
}

bool DPLLSolver::dpllRecursive(vector<int>& assignment) {
    // Unit propagation
    unitPropagation(assignment);

    // Satisfied?
    if (clauses.empty()) return true;

    // Contradiction?
    if (checkEmptyClause()) return false;

    // Choose a literal to branch on
    int literal = chooseLiteral();
    if (literal == 0) return false; // Defensive

    // Save state
    auto savedClauses = clauses;
    assignment.push_back(literal);
    if (dpllRecursive(assignment)) return true;

    // Backtrack and try negation
    assignment.pop_back();
    clauses = savedClauses;
    assignment.push_back(-literal);
    if (dpllRecursive(assignment)) return true;

    // Backtrack
    assignment.pop_back();
    clauses = savedClauses;
    return false;
}

int DPLLSolver::chooseLiteral() const {
    for (const auto& clause : clauses) {
        if (!clause.empty()) return *clause.begin();
    }
    return 0;
}

void DPLLSolver::printAssignment(const vector<int>& assignment) const {
    cout << "Assignment: ";
    for (int lit : assignment) cout << lit << " ";
    cout << endl;
}