#include "../general/satsolver.h"
#include "./dpsolver.h"
#include "../res/resolutionsolver.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
using namespace std;


bool DPSolver::solve() {
        // Apply unit propagation and pure literal elimination
        unitPropagation(assignment);
        eliminatePureLiterals(assignment);

        // If all clauses are satisfied
        if (clauses.empty()) return true;

        // If any clause is empty, contradiction
        for (const auto& clause : clauses) {
            if (clause.empty()) return false;
        }

        // Pick a variable to branch on (e.g., first literal of first clause)
        int var = abs(clauses[0][0]);

        // Save state
        auto savedClauses = clauses;
        auto savedAssignment = assignment;

        // Try assigning var true
        addToAssignment(var);
        if (solve()) return true;

        // Restore state and try assigning var false
        clauses = savedClauses;
        assignment = savedAssignment;
        addToAssignment(-var);
        if (solve()) return true;

        // Restore state and return false
        clauses = savedClauses;
        assignment = savedAssignment;
        return false;
    }


vector<vector<int>> DPSolver::singleStep(const vector<vector<int>>& clauseSet) {
    vector<vector<int>> result = clauseSet;

    // Helper function to calculate clause score (lower is better)
    auto getClauseScore = [](const vector<int>& c1, const vector<int>& c2) -> int {
        return c1.size() + c2.size();  // Prefer shorter clauses
    };

    // Store potential resolvents with their scores
    vector<pair<int, pair<size_t, size_t>>> candidates;  // score, (clause1_idx, clause2_idx)

    // Find candidate pairs for resolution
    for (size_t i = 0; i < clauseSet.size(); i++) {
        for (size_t j = i + 1; j < clauseSet.size(); j++) {
            const auto& clause1 = clauseSet[i];
            const auto& clause2 = clauseSet[j];

            // Check for complementary literals
            for (int lit1 : clause1) {
                if (find(clause2.begin(), clause2.end(), -lit1) != clause2.end()) {
                    int score = getClauseScore(clause1, clause2);
                    candidates.push_back({score, {i, j}});
                    break;
                }
            }
        }
    }

    // Sort candidates by score
    sort(candidates.begin(), candidates.end());

    // Try candidates until we find a useful resolvent
    for (const auto& candidate : candidates) {
        const auto& clause1 = clauseSet[candidate.second.first];
        const auto& clause2 = clauseSet[candidate.second.second];

        // Generate resolvent
        set<int> resolvent;
        int resolutionLiteral = 0;

        // Find the resolution literal
        for (int lit1 : clause1) {
            if (find(clause2.begin(), clause2.end(), -lit1) != clause2.end()) {
                resolutionLiteral = lit1;
                break;
            }
        }

        // Add all literals except complementary pair
        for (int lit : clause1) {
            if (lit != resolutionLiteral) {
                resolvent.insert(lit);
            }
        }
        for (int lit : clause2) {
            if (lit != -resolutionLiteral) {
                resolvent.insert(lit);
            }
        }

        // Check if resolvent is useful
        bool isTautology = false;
        for (int lit : resolvent) {
            if (resolvent.find(-lit) != resolvent.end()) {
                isTautology = true;
                break;
            }
        }

        if (!isTautology) {
            // Check if resolvent is new
            vector<int> resolventVec(resolvent.begin(), resolvent.end());
            bool isNew = true;
            for (const auto& existing : result) {
                if (existing.size() == resolventVec.size() &&
                    is_permutation(existing.begin(), existing.end(), resolventVec.begin())) {
                    isNew = false;
                    break;
                }
            }

            // If resolvent is useful and new, add it and return
            if (isNew) {
                cout << "[DEBUG] New resolvent found: ";
                for (int lit : resolventVec) {
                    cout << lit << " ";
                }
                cout << endl;

                result.push_back(resolventVec);
                return result;
            }
        }
    }

    // No useful resolvent found
    return result;
}

    std::vector<int> DPSolver::getAssignment() const {
        return assignment;
    }

    bool DPSolver::solve_new() {
    cout << "[DEBUG] Starting Davis-Putnam solver..." << endl;

    // Main solving loop
    while (!clauses.empty()) {
        // Store initial state for comparison
        size_t clausesBefore = clauses.size();

        // Run unit propagation
        unitPropagation(assignment);
        if (checkEmptyClause()) {
            cout << "[DEBUG] Empty clause found after unit propagation. UNSAT." << endl;
            return false;
        }
        if (clauses.empty()) {
            cout << "[DEBUG] All clauses satisfied after unit propagation. SAT." << endl;
            return true;
        }

        // Run pure literal elimination
        eliminatePureLiterals(assignment);
        if (checkEmptyClause()) {
            cout << "[DEBUG] Empty clause found after pure literal elimination. UNSAT." << endl;
            return false;
        }
        if (clauses.empty()) {
            cout << "[DEBUG] All clauses satisfied after pure literal elimination. SAT." << endl;
            return true;
        }

        // If no changes from UP or PL, try resolution
        if (clauses.size() == clausesBefore) {
            cout << "[DEBUG] No changes from UP or PL, attempting resolution step..." << endl;
            //copy current clauses
            vector<vector<int>> f_clauses = clauses;

            // Perform single resolution step
            auto newClauses = singleStep(f_clauses);

            // If no new clauses were generated
            if (newClauses.size() == f_clauses.size()) {
                cout << "[DEBUG] No new clauses from resolution. Formula is SAT." << endl;
                return true;
            }

            // Update clauses with new resolvent
            clauses = newClauses;
            cout << "[DEBUG] Resolution step added new clause(s). Continuing..." << endl;
        }

        // Print current state
        cout << "[DEBUG] Current clause count: " << clauses.size() << endl;
        printClauses();
    }
        if (!clauses.empty()) {
            cout << "[DEBUG] Making assignments for remaining variables..." << endl;

            // Create a set of variables already assigned
            set<int> assignedVars;
            for (int lit : assignment) {
                assignedVars.insert(abs(lit));
            }

            // Handle remaining clauses
            for (const auto& clause : clauses) {
                // For each clause, assign its first literal if the variable isn't already assigned
                for (int lit : clause) {
                    int var = abs(lit);
                    if (assignedVars.find(var) == assignedVars.end()) {
                        assignment.push_back(lit);  // Add the literal (not just the variable)
                        assignedVars.insert(var);
                        break;
                    }
                }
            }

            cout << "[DEBUG] Final assignment after handling remaining clauses: ";
            for (int lit : assignment) {
                cout << lit << " ";
            }
            cout << endl;
        }

        cout << "[DEBUG] All clauses processed. Formula is SAT." << endl;
        return true;

}


// Add function to verify solution
bool DPSolver::verifySolution(const vector<int>& assignment, const vector<vector<int>>& originalClauses) {
    cout << "\nVerifying solution..." << endl;
    cout << "Assignment: ";
    for (int lit : assignment) {
        cout << lit << " ";
    }
    cout << endl;

    for (const auto& clause : originalClauses) {
        cout << "Checking clause: ";
        for (int lit : clause) {
            cout << lit << " ";
        }
        cout << endl;

        bool clauseSatisfied = false;
        for (int literal : clause) {
            if (find(assignment.begin(), assignment.end(), literal) != assignment.end()) {
                cout << "Clause satisfied by literal: " << literal << endl;
                clauseSatisfied = true;
                break;
            }
        }
        if (!clauseSatisfied) {
            cout << "Clause not satisfied!" << endl;
            return false;
        }
    }
    return true;
}

void DPSolver::addToAssignment(int literal) {
    assignment.push_back(literal);
    
    // Create a new vector for updated clauses
    vector<vector<int>> newClauses;
    
    for (const auto& clause : clauses) {
        // If clause contains the literal, it's satisfied
        if (find(clause.begin(), clause.end(), literal) != clause.end()) {
            continue;
        }
        
        // If clause contains the negation, remove just that literal
        vector<int> newClause;
        for (int lit : clause) {
            if (lit != -literal) {
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