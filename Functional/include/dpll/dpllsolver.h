#include "../general/satsolver.h"
#include <vector>
#include <set>
#include <string>

class DPLLSolver : public SATSolver {
public:
    DPLLSolver(const std::string& filename);

    // Returns true if SAT, false if UNSAT
    bool solve();

    // Returns the satisfying assignment (if any)
    std::vector<int> getAssignment() const;

    // Override base class method
    void addToAssignment(int literal) override;

private:
    bool dpllRecursive(std::vector<int>& assignment);
    int chooseLiteral(const std::vector<int>& currentAssignment) const;
    void printAssignment(const std::vector<int>& assignment) const;
    bool isClauseSatisfied(const std::vector<int>& clause, const std::vector<int>& currentAssignment) const;
    void updateClauses(const std::vector<int>& currentAssignment);

    std::vector<int> finalAssignment;
    std::vector<std::vector<int>> originalClauses;  // Store original clauses for verification
};