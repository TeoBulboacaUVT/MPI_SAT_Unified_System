#include "./dpllsolver.h"
#include <iostream>
#include <map>
#include <algorithm>
using namespace std;

DPLLSolver::DPLLSolver(const string& filename) : SATSolver(filename) {
    // Store original clauses for verification
    originalClauses = clauses;
}

bool DPLLSolver::isClauseSatisfied(const vector<int>& clause, const vector<int>& currentAssignment) const {
    for (int lit : clause) {
        // Check if any literal in the clause is satisfied by the current assignment
        if (find(currentAssignment.begin(), currentAssignment.end(), lit) != currentAssignment.end()) {
            return true;
        }
    }
    return false;
}

void DPLLSolver::updateClauses(const vector<int>& currentAssignment) {
    vector<vector<int>> newClauses;
    
    for (const auto& clause : clauses) {
        // Skip satisfied clauses
        if (isClauseSatisfied(clause, currentAssignment)) {
            continue;
        }
        
        // Create new clause without negations of assigned literals
        vector<int> newClause;
        bool hasUnassignedLiterals = false;
        
        for (int lit : clause) {
            // Skip if the negation of this literal is in the assignment
            if (find(currentAssignment.begin(), currentAssignment.end(), -lit) != currentAssignment.end()) {
                continue;
            }
            // If this literal is not assigned, keep it
            if (find(currentAssignment.begin(), currentAssignment.end(), lit) == currentAssignment.end()) {
                newClause.push_back(lit);
                hasUnassignedLiterals = true;
            }
        }
        
        // Only add non-empty clauses with unassigned literals
        if (!newClause.empty() && hasUnassignedLiterals) {
            newClauses.push_back(newClause);
        }
    }
    
    clauses = newClauses;
}

void DPLLSolver::addToAssignment(int literal) {
    // Add the literal to the assignment
    assignment.push_back(literal);
    
    // Update clauses based on the current assignment
    updateClauses(assignment);
    
    // Check for unit clauses after the update
    for (const auto& clause : clauses) {
        if (clause.size() == 1) {
            // Found a unit clause, propagate it
            int unitLiteral = clause[0];
            if (find(assignment.begin(), assignment.end(), -unitLiteral) == assignment.end()) {
                addToAssignment(unitLiteral);
            }
        }
    }
}

bool DPLLSolver::solve() {
    cout << "Solving using DPLL..." << endl;
    vector<int> assignment;
    bool result = dpllRecursive(assignment);
    if (result) {
        // Sort and remove duplicates from the final assignment
        sort(assignment.begin(), assignment.end());
        assignment.erase(unique(assignment.begin(), assignment.end()), assignment.end());
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
        unitPropagation(assignment);
        if (checkEmptyClause()) {
            cout << "[DEBUG] Empty clause found after unit propagation" << endl;
            return false;
        }
        changed |= (assignment.size() > beforeUP);
        
        // Store size before pure literal elimination
        size_t beforePL = assignment.size();
        eliminatePureLiterals(assignment);
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

    // Choose next literal
    int literal = chooseLiteral(assignment);
    if (literal == 0) {
        cout << "[DEBUG] No literal to choose" << endl;
        return false;
    }

    cout << "[DEBUG] Branching on literal: " << literal << endl;

    // Save current state
    auto savedClauses = clauses;
    auto savedAssignment = assignment;
    
    // Try positive literal
    addToAssignment(literal);
    if (dpllRecursive(assignment)) {
        // If successful, keep the current assignment
        return true;
    }

    // Restore state and try negative literal
    clauses = savedClauses;
    assignment = savedAssignment;
    addToAssignment(-literal);
    if (dpllRecursive(assignment)) {
        // If successful, keep the current assignment
        return true;
    }

    // If both branches failed, backtrack
    assignment = savedAssignment;
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