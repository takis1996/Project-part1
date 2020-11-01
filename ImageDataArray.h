
#ifndef IMAGECOLLECTION_H
#define IMAGECOLLECTION_H

#include "ImageData.h"
#include "ResultNN.h"

#include <string>
#include <vector>

using namespace std;

class ImageDataArray {
public:
    ImageData * array;
    int size;
};

ImageDataArray * load(string filename);



class ImagePointerArray {
public:
    vector<ImageData *> points;
};



ResultNN nearestNeighbor(ImageData * q, ImageDataArray * array);

vector<ResultNN> nearestNeighbor(ImageData * q, ImageDataArray * array, int N);

vector<ResultNN> nearestNeighbor(ImageData * q, ImageDataArray * array, double R);

double calculateW(ImageDataArray * inputData);

double distance(ImageDataArray * x, ImageDataArray * y);

#endif /* IMAGECOLLECTION_H */

