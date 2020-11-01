
#include <iostream>
#include <cstdlib>

#include "helpers.h"

using namespace std;

double my_random(double w) {
    double r = w * (rand() / (RAND_MAX + 1.0));
    return r;
}

unsigned modulo(int a, int b) {
    if (a % b < 0) {
        return ((a % b) + b);
    } else {
        return a % b;
    }
}

unsigned exp_modulo(int base, int exponent, int modulus) {
    // https://en.wikipedia.org/wiki/Modular_exponentiation
    if (modulus == 1) {
        return 0;
    }
    int result = 1;

    base = base % modulus;

    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = (result * base) % modulus;
        }
        exponent = exponent >> 1;
        base = (base * base) % modulus;
    }

    return result;
}

// https://www.geeksforgeeks.org/hamming-distance-between-two-integers/
unsigned hammingDistance(int n1, int n2) { 
    unsigned x = n1 ^ n2; 
    unsigned setBits = 0; 
  
    while (x > 0) { 
        setBits += x & 1; 
        x >>= 1; 
    } 
  
    return setBits; 
} 
