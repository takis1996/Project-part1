
#include "RandomVector.h"
#include "helpers.h"

RandomVector::RandomVector() {

}

RandomVector::~RandomVector() {

}

void RandomVector::fill(int d, double W) {
    for (int i=0;i<d;i++) {
        components.push_back(my_random(W));
    }
}