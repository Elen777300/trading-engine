//
// Created by Elen Soghomonyan on 10.03.25.
//
#include <iostream>

void runUnitTests();

int main() {
    try {
        runUnitTests();
        return 0;
    } catch (...) {
        std::cerr << "Unhandled exception in tests!\n";
        return 1;
    }
}