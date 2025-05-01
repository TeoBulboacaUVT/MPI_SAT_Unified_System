#include "./satsolver.h"
#include "../parser/DIMACSParser.h"
#include <algorithm> // For std::remove


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

// Utility: Eliminate pure literals
void SATSolver::eliminatePureLiterals(vector<int>& assignment) {
    cout << "[DEBUG] Starting pure literal elimination..." << endl;
    vector<int> literalCounts(numLiterals + 1, 0);

    // Count occurrences of each literal
    for (const auto& clause : clauses) {
        for (int literal : clause) {
            literalCounts[abs(literal)] += (literal > 0) ? 1 : -1;
        }
    }

    // Identify and eliminate pure literals
    for (int i = 1; i <= numLiterals; ++i) {
        if (literalCounts[i] > 0) {
            cout << "[DEBUG] Pure positive literal found: " << i << endl;
            assignment.push_back(i);
            clauses.erase(std::remove_if(clauses.begin(), clauses.end(),
                                         [i](const vector<int>& clause) {
                                             return std::find(clause.begin(), clause.end(), i) != clause.end();
                                         }),
                          clauses.end());
        } else if (literalCounts[i] < 0) {
            cout << "[DEBUG] Pure negative literal found: " << -i << endl;
            assignment.push_back(-i);
            clauses.erase(std::remove_if(clauses.begin(), clauses.end(),
                                         [i](const vector<int>& clause) {
                                             return std::find(clause.begin(), clause.end(), -i) != clause.end();
                                         }),
                          clauses.end());
        }
    }
    cout << "[DEBUG] Pure literal elimination completed. Updated clauses:" << endl;
    printClauses();
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