#ifndef CLUSTERING_H
#define CLUSTERING_H

#include <unordered_set>

#include "ImageDataArray.h"
#include "ImageHashTable.h"

using namespace std;

ImageDataArray * Initialization(ImageDataArray * inputData, int clusters);

ImagePointerArray ** AssignmentClassic(ImageDataArray * inputData, ImageDataArray * centroids);

ImagePointerArray ** AssignmentLSH( unordered_set<ImageHashTable * > & hashtables, ImageDataArray * inputData, ImageDataArray * centroids) ;

ImagePointerArray ** AssignmentHypercube(unordered_set<ImageHashTable * > & hashtables, ImageDataArray * inputData, ImageDataArray * centroids, int limitnode, int limitchains);


ImageDataArray * Update(ImageDataArray * inputData, ImageDataArray * centroids, ImagePointerArray ** assignments) ;

double * Silhouette(ImageDataArray * inputData, ImageDataArray * centroids, ImagePointerArray ** assignments);

void Visualize(ImageDataArray * inputData, ImageDataArray * centroids, ImagePointerArray ** assignments, bool complete = false);


#endif /* CLUSTERING_H */

