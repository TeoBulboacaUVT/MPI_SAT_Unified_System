#ifndef SATSOLVER_H
#define SATSOLVER_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

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

    virtual void addToAssignment(int literal);

    vector<vector<int>> clauses;
    vector<int> assignment;

protected:
    int numLiterals;
    int numClauses;
    
    // Watched literals data structures
    struct WatchedClause {
        int watch1;
        int watch2;
        vector<int> clause;
    };
    
    vector<WatchedClause> watchedClauses;
    unordered_map<int, vector<size_t>> literalToClauses; // Maps literals to clause indices
    
    void initializeWatchedLiterals();
    bool updateWatchedLiterals(int literal, vector<int>& assignment);
    void propagateWatchedLiterals(int literal, vector<int>& assignment);
};

#endif // SATSOLVER_H
