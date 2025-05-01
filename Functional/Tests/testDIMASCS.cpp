#include "DIMACSParser.cpp"
#include <iostream>

int main() {
    string filename = "C:\\Users\\Lenovo\\Desktop\\UVT\\Sem 2\\MPI Project\\New folder\\Functional\\aim-50-1_6-no-1.cnf";

    DIMACSParser parser(filename);

    cout << "Number of literals: " << parser.getNumLiterals() << endl;
    cout << "Number of clauses: " << parser.getNumClauses() << endl;

    parser.printClauses();

    return 0;
}