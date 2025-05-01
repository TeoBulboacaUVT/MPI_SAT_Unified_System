#include "SATSolver.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
using namespace std;

class DPSolver : public SATSolver {
private:
    vector<int> assignment;

    void addToAssignment(int var) {
        assignment.push_back(var);
        // Remove clauses satisfied by this assignment
        clauses.erase(remove_if(clauses.begin(), clauses.end(),
                                [var](const vector<int>& clause) {
                                    return find(clause.begin(), clause.end(), var) != clause.end();
                                }),
                      clauses.end());
        // Remove the negation of the variable from remaining clauses
        for (auto& clause : clauses) {
            clause.erase(remove(clause.begin(), clause.end(), -var), clause.end());
        }
    }

public:
    DPSolver(const string& filename) : SATSolver(filename) {}

    bool solve() {
        // Apply unit propagation and pure literal elimination
        unitPropagation(assignment);
        eliminatePureLiterals(assignment);

        // If all clauses are satisfied
        if (clauses.empty()) return true;

        // If any clause is empty, contradiction
        for (const auto& clause : clauses)
            if (clause.empty()) return false;

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

    vector<int> getAssignment() const { return assignment; }
};