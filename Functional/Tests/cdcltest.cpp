#include "../include/cdcl/cdclsolver.h"
#include <iostream>
#include <cassert>

void testBasicFunctionality() {
    std::cout << "Testing basic CDCL solver functionality..." << std::endl;
    
    // Test 1: Simple satisfiable formula
    {
        std::cout << "\nTest 1: Simple satisfiable formula (x1 ∨ x2) ∧ (¬x1 ∨ x2) ∧ (x1 ∨ ¬x2)" << std::endl;
        CDCLSolver solver("test1.cnf");
        
        // Add clauses manually for testing
        solver.clauses.push_back({{1, 2}, false, 0.0, 2});  // (x1 ∨ x2)
        solver.clauses.push_back({{-1, 2}, false, 0.0, 2}); // (¬x1 ∨ x2)
        solver.clauses.push_back({{1, -2}, false, 0.0, 2}); // (x1 ∨ ¬x2)
        
        solver.numVariables = 2;
        solver.numClauses = 3;
        solver.initializeVariableStates();
        
        bool result = solver.solve();
        std::cout << "Result: " << (result ? "SATISFIABLE" : "UNSATISFIABLE") << std::endl;
        assert(result == true);
        solver.printAssignment();
    }
    
    // Test 2: Simple unsatisfiable formula
    {
        std::cout << "\nTest 2: Simple unsatisfiable formula (x1) ∧ (¬x1)" << std::endl;
        CDCLSolver solver("test2.cnf");
        
        solver.clauses.push_back({{1}, false, 0.0, 1});   // (x1)
        solver.clauses.push_back({{-1}, false, 0.0, 1});  // (¬x1)
        
        solver.numVariables = 1;
        solver.numClauses = 2;
        solver.initializeVariableStates();
        
        bool result = solver.solve();
        std::cout << "Result: " << (result ? "SATISFIABLE" : "UNSATISFIABLE") << std::endl;
        assert(result == false);
    }
    
    // Test 3: Unit propagation
    {
        std::cout << "\nTest 3: Unit propagation (x1) ∧ (¬x1 ∨ x2) ∧ (¬x2 ∨ x3)" << std::endl;
        CDCLSolver solver("test3.cnf");
        
        solver.clauses.push_back({{1}, false, 0.0, 1});      // (x1)
        solver.clauses.push_back({{-1, 2}, false, 0.0, 2});  // (¬x1 ∨ x2)
        solver.clauses.push_back({{-2, 3}, false, 0.0, 2});  // (¬x2 ∨ x3)
        
        solver.numVariables = 3;
        solver.numClauses = 3;
        solver.initializeVariableStates();
        
        bool result = solver.propagate();
        std::cout << "Propagation result: " << (result ? "SUCCESS" : "CONFLICT") << std::endl;
        assert(result == true);
        solver.printAssignment();
    }
    
    // Test 4: Conflict detection
    {
        std::cout << "\nTest 4: Conflict detection (x1) ∧ (¬x1 ∨ x2) ∧ (¬x2)" << std::endl;
        CDCLSolver solver("test4.cnf");
        
        solver.clauses.push_back({{1}, false, 0.0, 1});      // (x1)
        solver.clauses.push_back({{-1, 2}, false, 0.0, 2});  // (¬x1 ∨ x2)
        solver.clauses.push_back({{-2}, false, 0.0, 1});     // (¬x2)
        
        solver.numVariables = 2;
        solver.numClauses = 3;
        solver.initializeVariableStates();
        
        bool result = solver.propagate();
        std::cout << "Propagation result: " << (result ? "SUCCESS" : "CONFLICT") << std::endl;
        assert(result == false);
    }
    
    // Test 5: Variable selection
    {
        std::cout << "\nTest 5: Variable selection with activity" << std::endl;
        CDCLSolver solver("test5.cnf");
        
        solver.clauses.push_back({{1, 2, 3}, false, 0.0, 3});  // (x1 ∨ x2 ∨ x3)
        
        solver.numVariables = 3;
        solver.numClauses = 1;
        solver.initializeVariableStates();
        
        // Update activities
        solver.updateVariableActivity(1);
        solver.updateVariableActivity(1); // x1 has highest activity
        
        int selectedVar = solver.chooseNextVariable();
        std::cout << "Selected variable: " << selectedVar << std::endl;
        assert(selectedVar == 1);
    }
}

int main() {
    try {
        testBasicFunctionality();
        std::cout << "\nAll basic tests passed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}