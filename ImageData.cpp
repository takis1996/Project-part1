#include "ImageData.h"

ImageData::ImageData() {

}

ImageData::~ImageData() {

}

void print(ImageData * data) {
    int i =0;
    for (int y = 0;y<data->cols;y++) {
        for (int x = 0;x<data->rows;x++) {
            cout << data->bytes[i] <<"\t";
            i++;
        }
        cout << endl;
    }
}

// ------------------- functions ----------------------

double manhattan(ImageData * a, ImageData * b) {
    double distance = 0;
    int i =0;
    for (int y = 0;y<a->cols;y++) {
        for (int x = 0;x<a->rows;x++) {
            distance = distance + abs(a->bytes[i] - b->bytes[i]);
            i++;
        }
    }
    
    return distance;
}