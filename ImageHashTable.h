#ifndef IMAGEHASHTABLE_H
#define IMAGEHASHTABLE_H

#include <set>
#include <list>
#include <unordered_map>

#include "ImagePointerWithG.h"
#include "RandomVector.h"
#include "ResultNN.h"

using namespace std;

class ImageHashTable {
public:
    int numOfLists;
    int k;
    int M;
    int m;
    double W;
    int selector;
    list<ImagePointerWithG> * hashlists;
    list<RandomVector> randomVectors;
    unordered_map<int, int> * f_history;

    ImageHashTable(int selector, int numOfLists, int k, int d, double W);
    virtual ~ImageHashTable();

    void insert(ImageData * imagedata, int offset);
    ResultNN search(ImageData * query, int limitnode = -1, int limitchains = -1);
    double search(ImageData * query, set<int> * results, double R, int limitnode = -1, int limitchains = -1);
    double search(ImageData * query, vector<ResultNN> * results, int N, int limitnode = -1, int limitchains = -1);

    unsigned h(ImageData * x, RandomVector * s);
    unsigned f(int i, int value);
    
private:
    unsigned hash(ImageData * imagedata);
};


#endif /* IMAGEHASHTABLE_H */

