#ifndef SATSOLVER_H
#define SATSOLVER_H

#include <vector>
#include <string>
#include <iostream>

#pragma once

using namespace std;

class SATSolver {
public:
    SATSolver(const string& filename);
    void unitPropagation(vector<int>& assignment);
    void eliminatePureLiterals(vector<int>& assignment);
    bool checkEmptyClause() const;
    void printClauses() const;
    vector<vector<int>> getClauses() const; // Added getter for clauses
    void debugPrint() const {
        cout << "Current formula state:" << endl;
        printClauses();
        cout << "Number of clauses: " << clauses.size() << endl;
    }
    bool checkContradictions(); // Added function to check for contradictions

    // Fix the getter functions
    int getNumLiterals() const { return numLiterals; }
    int getNumClauses() const { return numClauses; }

    vector<vector<int>> clauses;

protected:
    int numLiterals;
    int numClauses;
};

#endif // SATSOLVER_H
