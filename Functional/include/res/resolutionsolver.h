#ifndef RESOLUTIONSOLVER_H
#define RESOLUTIONSOLVER_H


#include <set>
#include "../general/satsolver.h"

class ResolutionSolver : public SATSolver {
    public:
        ResolutionSolver(const std::string& filename);
        bool solve();
        bool resolve(const std::set<int>& clause1, const std::set<int>& clause2, std::set<int>& resolvent); 
        bool isTautology(const std::set<int>& clause) const; // Check if a clause is a tautology

        void printClauseSets(const std::vector<std::set<int>>& clauseSets) const; // Print all clause sets
        void printClause(const std::set<int>& clause) const; // Print a single clause
};

#endif // RESOLUTIONSOLVER_H
