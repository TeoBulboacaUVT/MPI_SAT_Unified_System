#include "./satsolver.h"
#include "../parser/DIMACSParser.h"
#include <algorithm> // For std::remove
#include <map>

// Constructor to initialize SATSolver with DIMACSParser
SATSolver::SATSolver(const string& filename) {
    cout << "[DEBUG] Initializing SATSolver with file: " << filename << endl;
    DIMACSParser parser(filename);
    numLiterals = parser.getNumLiterals();
    numClauses = parser.getNumClauses();
    clauses = parser.getClauses();
    cout << "[DEBUG] Number of literals: " << numLiterals << ", Number of clauses: " << numClauses << endl;
    printClauses();
}

void SATSolver::initializeWatchedLiterals() {
    watchedClauses.clear();
    literalToClauses.clear();
    
    for (size_t i = 0; i < clauses.size(); i++) {
        if (clauses[i].size() >= 2) {
            WatchedClause wc;
            wc.watch1 = clauses[i][0];
            wc.watch2 = clauses[i][1];
            wc.clause = clauses[i];
            watchedClauses.push_back(wc);
            
            literalToClauses[wc.watch1].push_back(i);
            literalToClauses[wc.watch2].push_back(i);
        }
    }
}

bool SATSolver::updateWatchedLiterals(int literal, vector<int>& assignment) {
    vector<size_t> clausesToUpdate = literalToClauses[-literal];
    bool conflict = false;
    
    for (size_t clauseIdx : clausesToUpdate) {
        WatchedClause& wc = watchedClauses[clauseIdx];
        
        // If the other watched literal is true, clause is satisfied
        if (find(assignment.begin(), assignment.end(), wc.watch1) != assignment.end() ||
            find(assignment.begin(), assignment.end(), wc.watch2) != assignment.end()) {
            continue;
        }
        
        // Try to find a new literal to watch
        bool foundNewWatch = false;
        for (int lit : wc.clause) {
            if (lit != wc.watch1 && lit != wc.watch2 &&
                find(assignment.begin(), assignment.end(), -lit) == assignment.end()) {
                // Update watched literals
                if (wc.watch1 == -literal) {
                    wc.watch1 = lit;
                } else {
                    wc.watch2 = lit;
                }
                literalToClauses[lit].push_back(clauseIdx);
                foundNewWatch = true;
                break;
            }
        }
        
        if (!foundNewWatch) {
            // No new watch found, check if clause is unit
            int unassignedLit = (wc.watch1 == -literal) ? wc.watch2 : wc.watch1;
            if (find(assignment.begin(), assignment.end(), -unassignedLit) == assignment.end()) {
                // Unit clause found
                assignment.push_back(unassignedLit);
                propagateWatchedLiterals(unassignedLit, assignment);
            } else {
                // Conflict found
                conflict = true;
                break;
            }
        }
    }
    
    return conflict;
}

void SATSolver::propagateWatchedLiterals(int literal, vector<int>& assignment) {
    vector<size_t> clausesToUpdate = literalToClauses[-literal];
    
    for (size_t clauseIdx : clausesToUpdate) {
        WatchedClause& wc = watchedClauses[clauseIdx];
        
        // If the other watched literal is true, clause is satisfied
        if (find(assignment.begin(), assignment.end(), wc.watch1) != assignment.end() ||
            find(assignment.begin(), assignment.end(), wc.watch2) != assignment.end()) {
            continue;
        }
        
        // Try to find a new literal to watch
        bool foundNewWatch = false;
        for (int lit : wc.clause) {
            if (lit != wc.watch1 && lit != wc.watch2 &&
                find(assignment.begin(), assignment.end(), -lit) == assignment.end()) {
                // Update watched literals
                if (wc.watch1 == -literal) {
                    wc.watch1 = lit;
                } else {
                    wc.watch2 = lit;
                }
                literalToClauses[lit].push_back(clauseIdx);
                foundNewWatch = true;
                break;
            }
        }
        
        if (!foundNewWatch) {
            // No new watch found, check if clause is unit
            int unassignedLit = (wc.watch1 == -literal) ? wc.watch2 : wc.watch1;
            if (find(assignment.begin(), assignment.end(), -unassignedLit) == assignment.end()) {
                // Unit clause found
                assignment.push_back(unassignedLit);
                propagateWatchedLiterals(unassignedLit, assignment);
            }
        }
    }
}

void SATSolver::unitPropagation(vector<int>& assignment) {
    cout << "[DEBUG] Starting unit propagation..." << endl;
    
    // Initialize watched literals if not already done
    if (watchedClauses.empty()) {
        initializeWatchedLiterals();
    }
    
    bool changed;
    do {
        changed = false;
        
        // Process unit clauses
        for (auto it = clauses.begin(); it != clauses.end();) {
            if (it->size() == 1) {
                int unit = (*it)[0];
                cout << "[DEBUG] Propagating unit: " << unit << endl;
                assignment.push_back(unit);
                
                // Update watched literals
                if (updateWatchedLiterals(unit, assignment)) {
                    clauses = {vector<int>()}; // Empty clause indicates UNSAT
                    return;
                }
                
                // Remove unit clause
                it = clauses.erase(it);
                changed = true;
            } else {
                ++it;
            }
        }
    } while (changed);
    
    cout << "[DEBUG] Unit propagation completed." << endl;
}

void SATSolver::eliminatePureLiterals(vector<int>& assignment) {
    bool changed;
    do {
        changed = false;
        map<int, int> literalCount;  // tracks positive and negative occurrences
        
        // Count occurrences
        for (const auto& clause : clauses) {
            for (int lit : clause) {
                int var = abs(lit);
                if (lit > 0) literalCount[var]++;
                else literalCount[-var]--;
            }
        }
        
        vector<vector<int>> newClauses;
        bool clauseRemoved = false;
        
        // Find and process pure literals
        for (const auto& pair : literalCount) {
            int var = pair.first;
            int count = pair.second;
            if (var > 0) { // Only process positive variables
                // Check if literal appears pure positive
                if (literalCount.find(-var) == literalCount.end()) {
                    cout << "[DEBUG] Pure positive literal found: " << var << endl;
                    assignment.push_back(var);
                    changed = true;
                    clauseRemoved = true;
                }
                // Check if literal appears pure negative
                else if (literalCount.find(var) == literalCount.end()) {
                    cout << "[DEBUG] Pure negative literal found: " << -var << endl;
                    assignment.push_back(-var);
                    changed = true;
                    clauseRemoved = true;
                }
            }
        }
        
        // Update clauses if we found pure literals
        if (clauseRemoved) {
            // Keep only clauses that don't contain any pure literals
            for (const auto& clause : clauses) {
                bool keepClause = true;
                for (int lit : clause) {
                    int var = abs(lit);
                    if ((lit > 0 && literalCount.find(-var) == literalCount.end()) ||
                        (lit < 0 && literalCount.find(var) == literalCount.end())) {
                        keepClause = false;
                        break;
                    }
                }
                if (keepClause) {
                    newClauses.push_back(clause);
                }
            }
            clauses = newClauses;
        }
        
        cout << "[DEBUG] Current clauses:" << endl;
        for (const auto& clause : clauses) {
            for (int lit : clause) {
                cout << lit << " ";
            }
            cout << "0" << endl;
        }
        
    } while (changed && !clauses.empty());
}

// Utility: Check if there is an empty clause
bool SATSolver::checkEmptyClause() const {
    cout << "[DEBUG] Checking for empty clauses..." << endl;
    if (clauses.empty()) {
        cout << "[DEBUG] All clauses are satisfied." << endl;
        return false;
    }
    for (const auto& clause : clauses) {
        if (clause.empty()) {
            cout << "[DEBUG] Empty clause found!" << endl;
            return true;
        }
    }
    cout << "[DEBUG] No empty clauses found." << endl;
    return false;
}

bool SATSolver::checkContradictions() {
    cout << "[DEBUG] Checking for contradictions in binary clauses..." << endl;
    for (size_t i = 0; i < clauses.size(); i++) {
        if (clauses[i].size() == 2) {
            int lit1 = clauses[i][0];
            int lit2 = clauses[i][1];

            // Look for complementary pair
            for (size_t j = i + 1; j < clauses.size(); j++) {
                if (clauses[j].size() == 2) {
                    if ((clauses[j][0] == -lit1 && clauses[j][1] == -lit2) ||
                        (clauses[j][0] == -lit2 && clauses[j][1] == -lit1)) {
                        cout << "[DEBUG] Contradiction found in binary clauses: (" << lit1 << ", " << lit2
                             << ") and (" << -lit1 << ", " << -lit2 << ")" << endl;
                        return true;
                    }
                }
            }
        }
    }
    cout << "[DEBUG] No contradictions found in binary clauses." << endl;
    return false;
}

void SATSolver::printClauses() const {
    cout << "[DEBUG] Current clauses:" << endl;
    for (const auto& clause : clauses) {
        for (int literal : clause) {
            cout << literal << " ";
        }
        cout << "0" << endl;
    }
}

vector<vector<int>> SATSolver::getClauses() const {
    return clauses;
}

void SATSolver::addToAssignment(int var) {
    assignment.push_back(var);

    // Create a new vector for updated clauses
    vector<vector<int>> newClauses;

    for (const auto& clause : clauses) {
        // If clause contains the literal, it's satisfied
        if (find(clause.begin(), clause.end(), var) != clause.end()) {
            continue;
        }

        // If clause contains the negation, remove just that literal
        vector<int> newClause;
        for (int lit : clause) {
            if (lit != -var) {
                newClause.push_back(lit);
            }
        }

        // Only add non-empty clauses
        if (!newClause.empty()) {
            newClauses.push_back(newClause);
        }
    }

    clauses = newClauses;
}