#include "cdclsolver.h"
#include <algorithm>
#include <cmath>
#include <iostream>

CDCLSolver::CDCLSolver(const std::string& filename) : SATSolver(filename) {
    // Initialize solver parameters
    maxLearnedClauses = 10000;
    clauseActivityIncrement = 1.0;
    variableActivityIncrement = 1.0;
    currentDecisionLevel = 0;
    numLearnedClauses = 0;
    
    // Initialize variable states
    initializeVariableStates();
}

bool CDCLSolver::solve() {
    // Main solving loop
    while (true) {
        // Unit propagation
        if (!propagate()) {
            if (currentDecisionLevel == 0) {
                return false; // Conflict at root level - formula is unsatisfiable
            }
            
            // Conflict analysis and learning
            std::vector<int> learnedClause;
            int backtrackLevel;
            analyzeConflict(-1, learnedClause, backtrackLevel);
            addLearnedClause(learnedClause);
            backtrack(backtrackLevel);
            continue;
        }
        
        // Check if all variables are assigned
        if (std::all_of(variableStates.begin(), variableStates.end(),
                       [](const VariableState& vs) { return vs.assigned; })) {
            return true; // All variables assigned - formula is satisfiable
        }
        
        // Choose next variable and make a decision
        int nextVar = chooseNextVariable();
        if (nextVar == -1) {
            return true; // No more unassigned variables
        }
        
        // Make a decision
        currentDecisionLevel++;
        assign(nextVar * 2); // Assign true (positive literal)
    }
}

bool CDCLSolver::propagate() {
    // Unit propagation using a queue
    std::queue<int> propagationQueue;
    
    // Initialize queue with unit clauses
    for (const auto& clause : clauses) {
        if (clause.size == 1) {
            propagationQueue.push(clause.literals[0]);
        }
    }
    
    while (!propagationQueue.empty()) {
        int literal = propagationQueue.front();
        propagationQueue.pop();
        
        // Skip if literal is already assigned
        if (getLiteralValue(literal) != 0) {
            continue;
        }
        
        // Make the assignment
        assign(literal);
        
        // Check for conflicts and update other clauses
        for (auto& clause : clauses) {
            if (isClauseSatisfied(clause)) {
                continue;
            }
            
            if (isClauseConflicting(clause)) {
                return false; // Conflict detected
            }
            
            // Check if clause became unit
            int unassignedCount = 0;
            int lastUnassigned = 0;
            
            for (int lit : clause.literals) {
                if (getLiteralValue(lit) == 0) {
                    unassignedCount++;
                    lastUnassigned = lit;
                }
            }
            
            if (unassignedCount == 1) {
                propagationQueue.push(lastUnassigned);
            }
        }
    }
    
    return true;
}

int CDCLSolver::chooseNextVariable() const {
    // Variable selection using VSIDS (Variable State Independent Decaying Sum)
    int bestVar = -1;
    double bestActivity = -1.0;
    
    for (int var = 1; var <= numVariables; var++) {
        if (!variableStates[var].assigned) {
            double activity = variableStates[var].activity;
            if (activity > bestActivity) {
                bestActivity = activity;
                bestVar = var;
            }
        }
    }
    
    return bestVar;
}

void CDCLSolver::assign(int literal, int antecedent) {
    int var = std::abs(literal);
    bool value = literal > 0;
    
    variableStates[var].assigned = true;
    variableStates[var].value = value;
    variableStates[var].decisionLevel = currentDecisionLevel;
    variableStates[var].antecedent = antecedent;
    
    trail.push_back(literal);
    decisionLevels.push_back(currentDecisionLevel);
    
    // Update variable activity
    updateVariableActivity(var);
}

void CDCLSolver::unassign(int literal) {
    int var = std::abs(literal);
    variableStates[var].assigned = false;
}

void CDCLSolver::analyzeConflict(int conflictClause, std::vector<int>& learnedClause, int& backtrackLevel) {
    // Conflict analysis using 1-UIP (First Unique Implication Point)
    std::set<int> seen;
    std::vector<int> conflictClauseLiterals;
    
    // Get literals from the conflicting clause
    if (conflictClause != -1) {
        conflictClauseLiterals = clauses[conflictClause].literals;
    } else {
        // Find a conflicting clause
        for (const auto& clause : clauses) {
            if (isClauseConflicting(clause)) {
                conflictClauseLiterals = clause.literals;
                break;
            }
        }
    }
    
    // Process the conflict
    int currentLevel = currentDecisionLevel;
    int count = 0;
    
    for (int lit : conflictClauseLiterals) {
        int var = std::abs(lit);
        if (variableStates[var].decisionLevel == currentLevel) {
            count++;
        }
    }
    
    while (count > 1) {
        // Get the last assigned literal
        int lastAssigned = trail.back();
        trail.pop_back();
        decisionLevels.pop_back();
        
        int var = std::abs(lastAssigned);
        if (variableStates[var].decisionLevel == currentLevel) {
            count--;
            if (variableStates[var].antecedent != -1) {
                const auto& antecedent = clauses[variableStates[var].antecedent].literals;
                for (int lit : antecedent) {
                    int antVar = std::abs(lit);
                    if (variableStates[antVar].decisionLevel == currentLevel) {
                        count++;
                    }
                }
            }
        }
    }
    
    // Build the learned clause
    learnedClause.clear();
    for (int lit : conflictClauseLiterals) {
        int var = std::abs(lit);
        if (variableStates[var].decisionLevel < currentLevel) {
            learnedClause.push_back(lit);
        }
    }
    
    // Determine backtrack level
    backtrackLevel = 0;
    for (int lit : learnedClause) {
        int var = std::abs(lit);
        backtrackLevel = std::max(backtrackLevel, variableStates[var].decisionLevel);
    }
}

void CDCLSolver::backtrack(int level) {
    // Non-chronological backtracking
    while (!trail.empty() && decisionLevels.back() > level) {
        int literal = trail.back();
        trail.pop_back();
        decisionLevels.pop_back();
        unassign(literal);
    }
    
    currentDecisionLevel = level;
}

void CDCLSolver::addLearnedClause(const std::vector<int>& clause) {
    // Add a new learned clause
    Clause newClause;
    newClause.literals = clause;
    newClause.isLearned = true;
    newClause.activity = 0;
    newClause.size = clause.size();
    
    clauses.push_back(newClause);
    numLearnedClauses++;
    
    // Update clause activities
    for (int lit : clause) {
        updateVariableActivity(std::abs(lit));
    }
    
    // Check if we need to reduce learned clauses
    if (numLearnedClauses > maxLearnedClauses) {
        reduceLearnedClauses();
    }
}

void CDCLSolver::decayActivities() {
    // Decay variable and clause activities
    clauseActivityIncrement *= 1.05;
    variableActivityIncrement *= 1.05;
    
    for (auto& clause : clauses) {
        clause.activity *= 0.95;
    }
    
    for (auto& state : variableStates) {
        state.activity *= 0.95;
    }
}

void CDCLSolver::reduceLearnedClauses() {
    // Remove less useful learned clauses
    std::sort(clauses.begin(), clauses.end(),
              [](const Clause& a, const Clause& b) {
                  return a.activity > b.activity;
              });
    
    int numToKeep = clauses.size() / 2;
    clauses.resize(numToKeep);
    numLearnedClauses = numToKeep;
}

bool CDCLSolver::isClauseSatisfied(const Clause& clause) const {
    for (int lit : clause.literals) {
        if (getLiteralValue(lit) == 1) {
            return true;
        }
    }
    return false;
}

bool CDCLSolver::isClauseConflicting(const Clause& clause) const {
    for (int lit : clause.literals) {
        if (getLiteralValue(lit) != -1) {
            return false;
        }
    }
    return true;
}

int CDCLSolver::getLiteralValue(int literal) const {
    int var = std::abs(literal);
    if (!variableStates[var].assigned) {
        return 0;
    }
    return (literal > 0) == variableStates[var].value ? 1 : -1;
}

void CDCLSolver::updateClauseActivity(Clause& clause) {
    clause.activity += clauseActivityIncrement;
    if (clause.activity > 1e100) {
        // Rescale activities to prevent overflow
        for (auto& c : clauses) {
            c.activity *= 1e-100;
        }
        clauseActivityIncrement *= 1e-100;
    }
}

void CDCLSolver::updateVariableActivity(int variable) {
    variableStates[variable].activity += variableActivityIncrement;
    if (variableStates[variable].activity > 1e100) {
        // Rescale activities to prevent overflow
        for (auto& state : variableStates) {
            state.activity *= 1e-100;
        }
        variableActivityIncrement *= 1e-100;
    }
}

void CDCLSolver::initializeVariableStates() {
    // Initialize variable states
    variableStates.resize(numVariables + 1); // Variables are 1-based
    for (int i = 1; i <= numVariables; i++) {
        variableStates[i].assigned = false;
        variableStates[i].value = false;
        variableStates[i].decisionLevel = -1;
        variableStates[i].antecedent = -1;
        variableStates[i].activity = 0.0;
    }
}

void CDCLSolver::printAssignment() const {
    std::cout << "Current assignment:" << std::endl;
    for (int i = 1; i <= numVariables; i++) {
        if (variableStates[i].assigned) {
            std::cout << i << "=" << (variableStates[i].value ? "true" : "false") << " ";
        }
    }
    std::cout << std::endl;
}

void CDCLSolver::printClause(const Clause& clause) const {
    std::cout << "Clause: ";
    for (int lit : clause.literals) {
        std::cout << lit << " ";
    }
    std::cout << std::endl;
}