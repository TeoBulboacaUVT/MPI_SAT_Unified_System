#include "./dpllsolver.h"
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
    bool changed;
    do {
        changed = false;
        
        // Store size before unit propagation
        size_t beforeUP = assignment.size();
        unitPropagation(assignment);  // Using SATSolver's implementation
        if (checkEmptyClause()) {
            cout << "[DEBUG] Empty clause found after unit propagation" << endl;
            return false;
        }
        changed |= (assignment.size() > beforeUP);
        
        // Store size before pure literal elimination
        size_t beforePL = assignment.size();
        eliminatePureLiterals(assignment);  // Using SATSolver's implementation
        if (checkEmptyClause()) {
            cout << "[DEBUG] Empty clause found after pure literal elimination" << endl;
            return false;
        }
        changed |= (assignment.size() > beforePL);
        
    } while (changed);

    // Check if all clauses are satisfied
    if (clauses.empty()) {
        cout << "[DEBUG] All clauses satisfied" << endl;
        return true;
    }

    // Choose next literal (we can improve this later)
    int literal = chooseLiteral();
    if (literal == 0) {
        cout << "[DEBUG] No literal to choose" << endl;
        return false;
    }

    cout << "[DEBUG] Branching on literal: " << literal << endl;

    // Save current state
    auto savedClauses = clauses;
    
    // Try positive literal
    assignment.push_back(literal);
    if (dpllRecursive(assignment)) return true;

    // Restore state and try negative literal
    clauses = savedClauses;
    assignment.pop_back();
    assignment.push_back(-literal);
    if (dpllRecursive(assignment)) return true;

    // If both branches failed, backtrack
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