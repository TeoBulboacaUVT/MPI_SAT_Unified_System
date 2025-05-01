#include "resolutionsolver.h"
#include "satsolver.h"
#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <unordered_set>

using namespace std;

   ResolutionSolver::ResolutionSolver(const string& filename) : SATSolver(filename) {}
    bool ResolutionSolver::solve() {
        cout << "[DEBUG] Solving using Resolution..." << endl;

        // Convert clauses to sets for easier resolution, skipping tautological clauses
        vector<set<int>> clauseSets;
        set<set<int>> uniqueClauses; // To track unique clauses
        for (const auto& clause : clauses) {
            set<int> clauseSet(clause.begin(), clause.end());
            if (isTautology(clauseSet)) {
                cout << "[DEBUG] Tautological clause removed: ";
                printClause(clauseSet);
                continue; // Skip tautological clauses
            }
            clauseSets.push_back(clauseSet);
            uniqueClauses.insert(clauseSet); // Add to unique clause tracker
        }

        cout << "[DEBUG] Initial clause sets after tautology elimination:" << endl;
        printClauseSets(clauseSets);

        const size_t MAX_CLAUSES = 10000; // Limit the number of clauses to prevent infinite loops
        size_t iterationCount = 0;       // Track the number of iterations

        while (true) {
            vector<set<int>> newClauses;

            // Sort clauses by size to prioritize smaller clauses
            sort(clauseSets.begin(), clauseSets.end(), [](const set<int>& a, const set<int>& b) {
                return a.size() < b.size();
            });

            // Generate all resolvents
            for (size_t i = 0; i < clauseSets.size(); i++) {
                for (size_t j = i + 1; j < clauseSets.size(); j++) {
                    set<int> resolvent;
                    if (resolve(clauseSets[i], clauseSets[j], resolvent)) {
                        cout << "[DEBUG] Resolvent generated from clauses " << i << " and " << j << ": ";
                        printClause(resolvent);

                        // If the resolvent is empty, UNSAT is detected
                        if (resolvent.empty()) {
                            cout << "[DEBUG] Empty resolvent found. UNSATISFIABLE." << endl;
                            return false;
                        }

                        // Avoid adding duplicate or tautological clauses
                        if (uniqueClauses.find(resolvent) == uniqueClauses.end()) {
                            newClauses.push_back(resolvent);
                            uniqueClauses.insert(resolvent); // Mark as unique
                        }
                    }
                }
            }

            // Check if new clauses are already in the set of clauses
            if (newClauses.empty()) {
                cout << "[DEBUG] No new clauses added. SATISFIABLE." << endl;
                return true;
            }

            // Add new clauses to the clause set
            for (const auto& clause : newClauses) {
                clauseSets.push_back(clause);
                cout << "[DEBUG] New clause added: ";
                printClause(clause);
            }

            // Break condition: Stop if the number of clauses exceeds the limit
            if (clauseSets.size() > MAX_CLAUSES) {
                cout << "[DEBUG] Clause limit exceeded (" << MAX_CLAUSES << "). Terminating resolution." << endl;
                return false;
            }

            // Optional: Add a limit on the number of iterations
            iterationCount++;
            if (iterationCount > 1000) { // Arbitrary iteration limit
                cout << "[DEBUG] Iteration limit exceeded. Terminating resolution." << endl;
                return false;
            }

            cout << "[DEBUG] Updated clause sets after iteration " << iterationCount << ":" << endl;
            printClauseSets(clauseSets);
        }
    }

    // Helper function to resolve two clauses
    bool ResolutionSolver::resolve(const set<int>& clause1, const set<int>& clause2, set<int>& resolvent) {
        for (int literal : clause1) {
            if (clause2.count(-literal)) {
                // Found a complementary pair
                resolvent = clause1;
                resolvent.insert(clause2.begin(), clause2.end());
                resolvent.erase(literal);
                resolvent.erase(-literal);

                // Check if the resolvent is a tautology
                if (isTautology(resolvent)) {
                    cout << "[DEBUG] Tautological resolvent discarded: ";
                    printClause(resolvent);
                    return false; // Skip tautological resolvents
                }

                // printClause(resolvent);
                return true;
            }
        }
        return false;
    }

    // Helper function to check if a clause is a tautology
    bool ResolutionSolver::isTautology(const set<int>& clause) const {
        for (int literal : clause) {
            if (clause.count(-literal)) {
                return true; // Clause contains both a literal and its negation
            }
        }
        return false;
    }

    // Helper function to print a single clause
    void ResolutionSolver::printClause(const set<int>& clause) const {
        cout << "{ ";
        for (int literal : clause) {
            cout << literal << " ";
        }
        cout << "}" << endl;
    }

    // Helper function to print all clause sets
    void ResolutionSolver::printClauseSets(const vector<set<int>>& clauseSets) const {
        for (size_t i = 0; i < clauseSets.size(); i++) {
            cout << "Clause " << i << ": ";
            printClause(clauseSets[i]);
        }
    }
