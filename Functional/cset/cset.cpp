#include "cset.h"
#include <algorithm>

// Default constructor
cset::cset() {
    len = 0; 
    capacity = 10; // Initial capacity
    a = new int[capacity]; 
}

cset::~cset() {
    delete[] a;
}

// Resize the array when it's full
void cset::resize() {
    capacity *= 2; // Double the capacity
    int *newArray = new int[capacity]; // Allocate a new array
    for (int i = 0; i < len; i++) {
        newArray[i] = a[i]; // Copy elements to the new array
    }
    delete[] a; // Free the old array
    a = newArray; // Update the pointer to the new array
}

void cset::append(int element) {
    if (len == capacity) { // Check if the array is full
        resize(); // Resize the array
    }
    a[len] = element;
    len++;
}

void cset::append(const cset& otherList) {
    for (int i = 0; i < otherList.len; i++) {
        append(otherList.a[i]);
    }
}

int cset::length() const {
    return len;
}

void cset::reverse() {
    for (int i = 0; i < len / 2; i++) {
        swap(a[i], a[len - i - 1]);
    }
}

void print(const cset& list) {
    cout << "[";
    for (int i = 0; i < list.len; i++) {
        cout << list.a[i];
        if (i < list.len - 1) {
            cout << ", ";
        }
    }
    cout << "]" << endl;
}