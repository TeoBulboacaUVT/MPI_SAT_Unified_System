#include "DIMACSParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

DIMACSParser::DIMACSParser(const string& filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        throw runtime_error("Failed to open DIMACS file: " + filename);
    }

    string line;
    while (getline(inFile, line)) {
        if (line.empty() || line[0] == 'c') {
            continue; // Skip comments
        }
        if (line[0] == 'p') {
            // Parse the problem line
            stringstream ss(line);
            string tmp;
            ss >> tmp >> tmp >> numLiterals >> numClauses;
            cout << "Parsed problem line: " << numLiterals << " literals, " << numClauses << " clauses" << endl;
        } else {
            // Parse a clause
            stringstream ss(line);
            int literal;
            vector<int> clause;
            while (ss >> literal && literal != 0) {
                clause.push_back(literal); // Add literals in order
            }
            if (!clause.empty()) {
                clauses.push_back(clause);
                cout << "Parsed clause: ";
                for (int lit : clause) {
                    cout << lit << " ";
                }
                cout << "0" << endl;
            }
        }
    }
    inFile.close();
}

void DIMACSParser::printClauses() const {
    cout << "Parsed Clauses (" << clauses.size() << " clauses):" << endl;
    for (const auto& clause : clauses) {
        for (int literal : clause) {
            cout << literal << " ";
        }
        cout << "0" << endl;
    }
}