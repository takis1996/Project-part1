#include <algorithm>
#include <cstring>
#include <utility>
#include <cfloat>
#include <sys/time.h>

#include "ImageDataArray.h"
#include "comparator.h"

ImageDataArray * load(string filename) {
    char bytes[4];
    FILE * fp = fopen(filename.c_str(), "rb");

    int magic_number, number_of_images, number_of_rows, number_of_cols;

    fread(&magic_number, 4, 1, fp);
    memcpy(bytes, &magic_number, 4);
    swap(bytes[0], bytes[3]);
    swap(bytes[1], bytes[2]);
    memcpy(&magic_number, bytes, 4);

    fread(&number_of_images, 4, 1, fp);
    memcpy(bytes, &number_of_images, 4);
    swap(bytes[0], bytes[3]);
    swap(bytes[1], bytes[2]);
    memcpy(&number_of_images, bytes, 4);


    fread(&number_of_rows, 4, 1, fp);
    memcpy(bytes, &number_of_rows, 4);
    swap(bytes[0], bytes[3]);
    swap(bytes[1], bytes[2]);
    memcpy(&number_of_rows, bytes, 4);


    fread(&number_of_cols, 4, 1, fp);
    memcpy(bytes, &number_of_cols, 4);
    swap(bytes[0], bytes[3]);
    swap(bytes[1], bytes[2]);
    memcpy(&number_of_cols, bytes, 4);

    cout << filename << " => Magic number:     " << magic_number << endl;
    cout << filename << " => Images      :     " << number_of_images << endl;
    cout << filename << " => Rows        :     " << number_of_rows << endl;
    cout << filename << " => Cols        :     " << number_of_cols << endl;


    ImageDataArray * array = new ImageDataArray();

    array->array = new ImageData[number_of_images]();
    array->size = number_of_images;

    unsigned char byte;

    for (int id = 0; id < number_of_images; id++) {
        array->array[id].cols = number_of_cols;
        array->array[id].rows = number_of_rows;
        array->array[id].id = id;
        for (int x = 0; x < number_of_rows; x++) {
            for (int y = 0; y < number_of_cols; y++) {
                fread(&byte, 1, 1, fp);
                array->array[id].bytes.push_back(byte);
            }
        }
    }

    fclose(fp);

    return array;
}

ResultNN nearestNeighbor(ImageData * q, ImageDataArray * array) {
    ResultNN result;
    int N = array->size;
    double dist_nn = DBL_MAX;

    struct timeval t1, t2;

    result.offset = -1;

    gettimeofday(&t1, NULL);

    for (int i = 0; i < N; i++) {
        double dist = manhattan(&array->array[i], q);
        if (dist < dist_nn) {
            dist_nn = dist;
            result.nn = &array->array[i];
            result.offset = i;
        }
    }

    gettimeofday(&t2, NULL);

    result.t = ((t2.tv_sec - t1.tv_sec) * 1000.0) + ((t2.tv_usec - t1.tv_usec) / 1000.0);

    result.distance = dist_nn;

    return result;
}

vector<ResultNN> nearestNeighbor(ImageData * q, ImageDataArray * array, int N) {
    vector<ResultNN> results;
    ResultNN timeresult;

    struct timeval t1, t2;

    gettimeofday(&t1, NULL);

    for (int i = 0; i < array->size; i++) {
        double dist = manhattan(&array->array[i], q);
        ResultNN result;
        result.nn = &array->array[i];
        result.offset = i;
        result.distance = dist;
        result.t = -1;
        results.push_back(result);
    }
    
    std::sort(results.begin(), results.end(), comparator());
    
    gettimeofday(&t2, NULL);

    timeresult.distance = results[N-1].distance;
    timeresult.offset = results[N-1].offset;
    timeresult.nn = results[N-1].nn;
    timeresult.t = ((t2.tv_sec - t1.tv_sec) * 1000.0) + ((t2.tv_usec - t1.tv_usec) / 1000.0);

    results.push_back(timeresult);
    
    return results;
}

vector<ResultNN> nearestNeighbor(ImageData * q, ImageDataArray * array, double R) {
    vector<ResultNN> results;
    ResultNN timeresult;
    int N = array->size;

    struct timeval t1, t2;

    results.push_back(timeresult);

    gettimeofday(&t1, NULL);

    for (int i = 0; i < N; i++) {
        double dist = manhattan(&array->array[i], q);
        if (dist < R) {
            ResultNN result;
            result.nn = &array->array[i];
            result.offset = i;
            result.distance = dist;
            result.t = -1;
            results.push_back(result);
        }
    }

    gettimeofday(&t2, NULL);

    results[0].distance = 0;
    results[0].nn = nullptr;
    results[0].t = ((t2.tv_sec - t1.tv_sec) * 1000.0) + ((t2.tv_usec - t1.tv_usec) / 1000.0);

    return results;
}

double calculateW(ImageDataArray * array) {
    int N = array->size;
    double sum = 0;

    for (int i = 0; i < N; i++) {
        double dist_nn = DBL_MAX;

        for (int j = 0; j < N; j++) {
            if (i != j) {
                double dist = manhattan(&array->array[i], &array->array[j]);
                if (dist < dist_nn) {
                    dist_nn = dist;
                }
            }
        }

        sum += dist_nn;
    }

    sum /= N;

    return sum;
}

double distance(ImageDataArray * x, ImageDataArray * y) {
    
    int N = x->size;
    
    double mean = 0;
    
    if (x->size != y->size) {
        cout << "Centroids corrupted: " << endl;
        cout << "x->size = " << x->size << endl;
        cout << "y->size = " << y->size << endl;
        exit(3);
    }
    
    for (int i=0;i<N;i++) {
        mean += manhattan(&x->array[i], &y->array[i]);
    }
    
    mean = mean/ N;
    
    return mean;
}