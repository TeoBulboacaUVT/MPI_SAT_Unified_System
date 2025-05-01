#ifndef DIMACSPARSER_H
#define DIMACSPARSER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

class DIMACSParser {
public:
    DIMACSParser(const string& filename);
    int getNumLiterals() const { return numLiterals; }
    int getNumClauses() const { return numClauses; }
    const vector<vector<int>>& getClauses() const { return clauses; }
    void printClauses() const;

private:
    int numLiterals;
    int numClauses;
    vector<vector<int>> clauses; // Use vector<int> to preserve order
};

#endif // DIMACSPARSER_H