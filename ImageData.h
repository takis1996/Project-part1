#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class ImageData {
public:
    ImageData();
    virtual ~ImageData();
    
    vector<unsigned char> bytes;
    int rows, cols;
    int id;
    
    // R search:
    int cluster_id;
    double cluster_R;
};


class ProjectionData {
public:
    vector<double> si;
};

void print(ImageData * data);

double manhattan(ImageData *x, ImageData* y);

#endif /* IMAGEDATA_H */

