#ifndef RESOLUTIONSOLVER_H
#define RESOLUTIONSOLVER_H

#include <set>
#include <vector>
#include "../general/satsolver.h"

class ResolutionSolver : public SATSolver {

public:
    // Track which pairs of clauses we've already tried to resolve
    set<pair<size_t, size_t>> triedPairs;
    
    // Helper functions
    bool isTautology(const set<int>& clause) const;
    void printClause(const set<int>& clause) const;
    void printClauseSets(const vector<set<int>>& clauseSets) const;

    bool resolve(const set<int>& clause1, const set<int>& clause2, set<int>& resolvent);

    ResolutionSolver(const string& filename);
    bool solve();
};

#endif