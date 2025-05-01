#ifndef CSET_H
#define CSET_H

#include <iostream>
using namespace std;

class cset {
public:
    cset(); // Default constructor
    ~cset(); // Destructor to free memory
    void append(int element); // Append a single element
    void append(const cset& otherList); // Append another list
    int length() const; // Get the number of elements
    void reverse(); // Reverse the list
    friend void print(const cset& list); // Friend function to print the list

private:
    int *a; // Pointer to dynamically allocated array
    int len; // Number of elements in the list
    int capacity; // Current capacity of the array
    void resize(); // Resize the array when full
};

#endif // CSET_H