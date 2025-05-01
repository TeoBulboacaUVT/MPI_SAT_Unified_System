#include "DIMACSParser.h"
#include "SATSolver.h"

class DPSolver : public SATSolver {
private:
    DIMACSParser parser;
    vector<int> assignment;
    bool simplifyFormula(int literal);
    bool checkContradiction() const;
public:
    DPSolver(const string& filename) : SATSolver(filename), parser(filename) {}

    int getNumLiterals() const {
        return parser.getNumLiterals();
    }

    int getNumClauses() const {
        return parser.getNumClauses();
    }

    vector<int> getAssignment() const { return assignment; }
    void addToAssignment(int literal) { assignment.push_back(literal); }


    bool solve();
};