#ifndef CDCLSOLVER_H
#define CDCLSOLVER_H

#include "../general/satsolver.h"
#include <vector>
#include <unordered_map>
#include <set>
#include <queue>

class CDCLSolver : public SATSolver {
public:
    CDCLSolver(const std::string& filename);

    // Main solving function
    bool solve();

    // Returns the satisfying assignment (if any)
    std::vector<int> getAssignment() const;

    // Data structures for CDCL
    struct VariableState {
        bool assigned;
        bool value;
        int decisionLevel;
        int antecedent;  // Index of the clause that forced this assignment (-1 for decisions)
        int polarity;    // Preferred polarity for this variable
        double activity; // Activity score for VSIDS heuristic
    };

    struct Clause {
        std::vector<int> literals;
        bool isLearned;
        double activity;    // For clause deletion
        int size;        // Number of unassigned literals
    };

    // Member variables
    std::vector<VariableState> variableStates;
    std::vector<Clause> clauses;
    std::vector<Clause> learnedClauses;
    std::vector<int> trail;          // Order of assignments
    std::vector<int> decisionLevels; // Decision levels of assignments
    int currentDecisionLevel;
    int numVariables;
    int numClauses;
    int numLearnedClauses;
    int maxLearnedClauses;
    double clauseActivityIncrement;   // Controls how much clause activity increases
    double variableActivityIncrement; // Controls how much variable activity increases

    // Core CDCL functions
    bool propagate();                // Unit propagation
    int chooseNextVariable() const;  // Variable selection
    void assign(int literal, int antecedent = -1);  // Make an assignment
    void unassign(int literal);      // Undo an assignment
    void analyzeConflict(int conflictClause, std::vector<int>& learnedClause, int& backtrackLevel);
    void backtrack(int level);       // Non-chronological backtracking
    void addLearnedClause(const std::vector<int>& clause);
    void decayActivities();          // Decay variable and clause activities
    void reduceLearnedClauses();     // Remove less useful learned clauses

    // Helper functions
    bool isClauseSatisfied(const Clause& clause) const;
    bool isClauseConflicting(const Clause& clause) const;
    int getLiteralValue(int literal) const;
    void updateClauseActivity(Clause& clause);
    void updateVariableActivity(int variable);
    void initializeVariableStates();
    void printAssignment() const;
    void printClause(const Clause& clause) const;
};

#endif // CDCLSOLVER_H 