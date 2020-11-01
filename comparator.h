#ifndef COMPARATOR_H
#define COMPARATOR_H

#include "ResultNN.h"


struct comparator {

    inline bool operator()(const ResultNN& struct1, const ResultNN& struct2) {
        return (struct1.distance > struct2.distance);
    }
};


#endif /* COMPARATOR_H */

