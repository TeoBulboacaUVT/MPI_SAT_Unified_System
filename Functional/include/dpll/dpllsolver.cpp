#include "./dpllsolver.h"
#include <iostream>
#include <map>
#include <algorithm>
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
    int literal = chooseLiteral(assignment);
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

int DPLLSolver::chooseLiteral(const std::vector<int>& currentAssignment) const {
    // Create a map to store literal frequencies
    std::map<int, int> literalFreq;
    
    // Count frequencies of literals in active clauses
    for (const auto& clause : clauses) {
        for (int lit : clause) {
            // Skip if this literal is already assigned
            if (std::find(currentAssignment.begin(), currentAssignment.end(), lit) != currentAssignment.end() ||
                std::find(currentAssignment.begin(), currentAssignment.end(), -lit) != currentAssignment.end()) {
                continue;
            }
            literalFreq[lit]++;
        }
    }
    
    if (literalFreq.empty()) {
        return 0;  // No unassigned literals found
    }
    
    // Find the literal with maximum frequency
    int maxFreq = 0;
    int chosenLiteral = 0;
    
    for (const auto& pair : literalFreq) {
        int lit = pair.first;
        int freq = pair.second;
        // Consider both positive and negative occurrences
        int totalFreq = freq;
        if (literalFreq.find(-lit) != literalFreq.end()) {
            totalFreq += literalFreq.at(-lit);
        }
        
        if (totalFreq > maxFreq) {
            maxFreq = totalFreq;
            // Choose the form (positive/negative) that appears more frequently
            chosenLiteral = (freq >= literalFreq[-lit]) ? lit : -lit;
        }
    }
    
    return chosenLiteral;
}

void DPLLSolver::printAssignment(const vector<int>& assignment) const {
    cout << "Assignment: ";
    for (int lit : assignment) cout << lit << " ";
    cout << endl;
}