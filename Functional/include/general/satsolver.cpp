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

void SATSolver::unitPropagation(vector<int>& assignment) {
    cout << "[DEBUG] Starting unit propagation..." << endl;
    bool changed;
    do {
        changed = false;

        // Check for direct contradictions in unit clauses
        for (size_t i = 0; i < clauses.size(); i++) {
            if (clauses[i].size() == 1) {
                int unit = clauses[i][0];
                for (size_t j = i + 1; j < clauses.size(); j++) {
                    if (clauses[j].size() == 1 && clauses[j][0] == -unit) {
                        cout << "[DEBUG] Direct contradiction found: " << unit << " and " << -unit << endl;
                        clauses = {vector<int>()};
                        return;
                    }
                }
            }
        }

        // Process unit clauses
        for (auto it = clauses.begin(); it != clauses.end();) {
            if (it->size() == 1) {
                int unit = (*it)[0];
                cout << "[DEBUG] Propagating unit: " << unit << endl;
                assignment.push_back(unit);

                // Remove unit clause and simplify remaining clauses
                it = clauses.erase(it);
                vector<vector<int>> newClauses;

                for (const auto& clause : clauses) {
                    // Skip if clause is satisfied
                    if (find(clause.begin(), clause.end(), unit) != clause.end()) {
                        continue;
                    }

                    // Remove negated literals
                    vector<int> newClause;
                    for (int lit : clause) {
                        if (lit != -unit) {
                            newClause.push_back(lit);
                        }
                    }

                    if (!newClause.empty()) {
                        newClauses.push_back(newClause);
                    }
                }

                clauses = newClauses;
                changed = true;
                cout << "[DEBUG] Updated clauses after propagation:" << endl;
                printClauses();
                break;
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
        for (const auto& [var, count] : literalCount) {
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