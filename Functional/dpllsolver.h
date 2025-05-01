
#include "SATSolver.h"
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

private:
    bool dpllRecursive(std::vector<int>& assignment);
    int chooseLiteral() const;
    void printAssignment(const std::vector<int>& assignment) const;

    std::vector<int> finalAssignment;
};