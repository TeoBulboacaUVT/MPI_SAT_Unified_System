#include "../parser/DIMACSParser.h"
#include "../general/satsolver.h"

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

    vector<int> getAssignment() const ;
    void addToAssignment(int literal);

    vector<vector<int>> singleStep(const vector<vector<int>>& clauseSet);

    bool verifySolution(const vector<int>& assignment, const vector<vector<int>>& originalClauses);

    bool solve();

    bool solve_new();
};
