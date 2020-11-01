
#ifndef RANDOMVECTOR_H
#define RANDOMVECTOR_H

#include <vector>

using namespace std;

class RandomVector { // S
public:
    vector<float> components;
    
    RandomVector();
    virtual ~RandomVector();

    void fill(int d, double W);
};

#endif /* RANDOMVECTOR_H */

