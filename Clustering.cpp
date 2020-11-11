
#include <cfloat>
#include <algorithm>

#include "ImageDataArray.h"
#include "Clustering.h"
#include "helpers.h"






// Kmeans++
// ImageDataArray *    :  for each cluster, it's center  (array of cluster centers)

ImageDataArray * Initialization(ImageDataArray * inputData, int clusters) {
    ImageDataArray * centroids = new ImageDataArray();

    centroids->array = new ImageData[clusters];
    centroids->size = clusters;

    int i = (int) my_random(inputData->size); // [0, N-1]

    // pick first center
    centroids->array[0] = inputData->array[i];

    i = 1;

    while (i < clusters) { // for each center we need to pick
        double * pr = new double [inputData->size];

        for (int j = 0; j < inputData->size; j++) {

            double mindist = DBL_MAX;

            for (int k = 0; k < i; k++) {
                mindist = min(mindist, manhattan(&inputData->array[j], &centroids->array[k]));
            }

            if (j == 0) {
                pr[0] = mindist*mindist;
            } else {
                pr[j] = mindist * mindist + pr[j - 1];
            }
        }

        double max = pr[inputData->size - 1];

        double r = my_random(max);

        for (int j = 0; j < inputData->size; j++) {
            if (r < pr[j]) {
                centroids->array[i] = inputData->array[j];
                break;
            }
        }

        delete [] pr;
        i++;
    }

    return centroids;
}

// Classic

// ImagePointerArray ** : 2D array: for each cluster, all points

ImagePointerArray ** AssignmentClassic(ImageDataArray * inputData, ImageDataArray * centroids) {
    ImagePointerArray ** clusters = new ImagePointerArray *[centroids->size];

    cout << " ==> Assignment classic with clusters " << centroids->size << " \n";
    for (int i = 0; i < centroids->size; i++) {
        clusters[i] = new ImagePointerArray();
    }

    for (int j = 0; j < inputData->size; j++) {
        double mindist = DBL_MAX;
        int mink = 0;

        for (int k = 0; k < centroids->size; k++) {
            double dist = manhattan(&inputData->array[j], &centroids->array[k]);


            if (dist < mindist) {
                mindist = dist;
                mink = k;
            }
        }

        clusters[mink]->points.push_back(&inputData->array[j]);

    }


    return clusters;
}

// LSH

ImagePointerArray ** AssignmentLSH(unordered_set<ImageHashTable * > & hashtables, ImageDataArray * inputData, ImageDataArray * centroids) {
    ImagePointerArray ** clusters = new ImagePointerArray *[centroids->size];

    cout << " ==> Assignment LSH ... \n";

    for (int i = 0; i < centroids->size; i++) {
        clusters[i] = new ImagePointerArray();
    }

    double R = 255;

    int threshold = 20;

    for (int j = 0; j < inputData->size; j++) {
        inputData->array[j].cluster_id = -1;
        inputData->array[j].cluster_R = -1;
    }

    // start increasting R ...
    for (int limit = 0; limit < 20; limit++) { // for each R

        int assigned = 0;

        for (int clusterID = 0; clusterID < centroids->size; clusterID++) { // for each cluster
            set<int> lsh_result3;

            // for each LSH tables
            for (unordered_set<ImageHashTable * >::iterator it = hashtables.begin(); it != hashtables.end(); ++it) {
                ImageHashTable * ht = *it;

                // search in radious R and add results to lsh_result3
                ht->search(&centroids->array[clusterID], &lsh_result3, R);
            }

            for (auto offset : lsh_result3) { // for each item
                // 1: unassigned item found 
                if (inputData->array[offset].cluster_R == -1) {
                    inputData->array[offset].cluster_id = clusterID;
                    inputData->array[offset].cluster_R = R;
                    assigned++;
                    continue;
                }

                // 2: self assigned item
                if (inputData->array[offset].cluster_R != -1 && inputData->array[offset].cluster_id == clusterID) {
                    continue;
                }

                // 3
                if (inputData->array[offset].cluster_R != -1 && inputData->array[offset].cluster_id != clusterID) {
                    if (inputData->array[offset].cluster_R < R) { // item found by other cluster in previous iteration
                        continue;
                    } else {
                        inputData->array[offset].cluster_id = clusterID;
                        inputData->array[offset].cluster_R = R;
                        assigned++;
                    }
                }
            }
            cout << endl;
        }

        if (assigned < threshold) {
            break;
        }

        R = R * 2;
    }

    for (int j = 0; j < inputData->size; j++) {
        if (inputData->array[j].cluster_id == -1) {
            double mindist = DBL_MAX;
            int mink = 0;

            for (int k = 0; k < centroids->size; k++) {
                double dist = manhattan(&inputData->array[j], &centroids->array[k]);

                if (dist < mindist) {
                    mindist = dist;
                    mink = k;
                }
            }

            clusters[mink]->points.push_back(&inputData->array[j]);
        }
    }


    return clusters;
}

// Hypercube

ImagePointerArray ** AssignmentHypercube(unordered_set<ImageHashTable * > & hashtables, ImageDataArray * inputData, ImageDataArray * centroids, int limitnode, int limitchains) {
    ImagePointerArray ** clusters = new ImagePointerArray *[centroids->size];

    cout << " ==> Assignment Cube ... \n";

    for (int i = 0; i < centroids->size; i++) {
        clusters[i] = new ImagePointerArray();
    }

    double R = 255;

    int threshold = 20;

    for (int j = 0; j < inputData->size; j++) {
        inputData->array[j].cluster_id = -1;
        inputData->array[j].cluster_R = -1;
    }

    // start increasting R ...
    for (int limit = 0; limit < 20; limit++) { // for each R

        int assigned = 0;

        for (int clusterID = 0; clusterID < centroids->size; clusterID++) { // for each cluster
            set<int> lsh_result3;

            // for each LSH tables
            for (unordered_set<ImageHashTable * >::iterator it = hashtables.begin(); it != hashtables.end(); ++it) {
                ImageHashTable * ht = *it;

                // search in radious R and add results to lsh_result3
                ht->search(&centroids->array[clusterID], &lsh_result3, R, limitnode, limitchains);
            }

            for (auto offset : lsh_result3) { // for each item
                // 1: unassigned item found 
                if (inputData->array[offset].cluster_R == -1) {
                    inputData->array[offset].cluster_id = clusterID;
                    inputData->array[offset].cluster_R = R;
                    assigned++;
                    continue;
                }

                // 2: self assigned item
                if (inputData->array[offset].cluster_R != -1 && inputData->array[offset].cluster_id == clusterID) {
                    continue;
                }

                // 3
                if (inputData->array[offset].cluster_R != -1 && inputData->array[offset].cluster_id != clusterID) {
                    if (inputData->array[offset].cluster_R < R) { // item found by other cluster in previous iteration
                        continue;
                    } else {
                        inputData->array[offset].cluster_id = clusterID;
                        inputData->array[offset].cluster_R = R;
                        assigned++;
                    }
                }
            }
            cout << endl;
        }

        if (assigned < threshold) {
            break;
        }

        R = R * 2;
    }

    for (int j = 0; j < inputData->size; j++) {
        if (inputData->array[j].cluster_id == -1) {
            double mindist = DBL_MAX;
            int mink = 0;

            for (int k = 0; k < centroids->size; k++) {
                double dist = manhattan(&inputData->array[j], &centroids->array[k]);

                if (dist < mindist) {
                    mindist = dist;
                    mink = k;
                }
            }

            clusters[mink]->points.push_back(&inputData->array[j]);
        }
    }


    return clusters;
}

ImageDataArray * Update(ImageDataArray * inputData, ImageDataArray * previousCentroids, ImagePointerArray ** assignments) {
    ImageDataArray * centroids = new ImageDataArray();

    int clusters = previousCentroids->size;
    centroids->array = new ImageData[clusters];
    centroids->size = clusters;

    for (int idCluster = 0; idCluster < centroids->size; idCluster++) {

        int N = assignments[idCluster]->points.size();
        unsigned d = previousCentroids->array[idCluster].bytes.size();

        vector<int> temp[d];

        centroids->array[idCluster].rows = previousCentroids->array[0].rows;
        centroids->array[idCluster].cols = previousCentroids->array[0].cols;
        centroids->array[idCluster].cluster_id = idCluster;
        centroids->array[idCluster].id = idCluster;

        for (unsigned idDimension = 0; idDimension < previousCentroids->array[idCluster].bytes.size(); idDimension++) {
            for (int idPoint = 0; idPoint < N; idPoint++) {
                std::vector<int> &myvector = temp[idDimension];
                int myvalue = assignments[idCluster]->points[idPoint]->bytes[idDimension];

                myvector.push_back(myvalue);
            }
        }

        for (unsigned idDimension = 0; idDimension < previousCentroids->array[idCluster].bytes.size(); idDimension++) {
            std::vector<int> &myvector = temp[idDimension];
            sort(myvector.begin(), myvector.end());
        }


        for (unsigned idDimension = 0; idDimension < previousCentroids->array[idCluster].bytes.size(); idDimension++) {
            int unique = temp[idDimension].size();
            if (unique != 0) {
                int median = temp[idDimension][unique / 2];
                centroids->array[idCluster].bytes.push_back(median);
            } else {
                centroids->array[idCluster].bytes.push_back(0);
            }
        }

        if (centroids->array[idCluster].bytes.size() != d) {
            cout << "corrupted dimensions \n";
            exit(2);
        }
    }

    return centroids;
}

double * Silhouette(ImageDataArray * inputData, ImageDataArray * centroids, ImagePointerArray ** assignments) {
    double value = 0.0;

    double * silhouettes = new double[centroids->size + 1];


    for (int c = 0; c < centroids->size; c++) {
        silhouettes[c] = 0.0;

        for (unsigned j1 = 0; j1 < assignments[c]->points.size(); j1++) {
            // nearest
            int nearest_k = c;

            // second nearest
            double mindist_2 = DBL_MAX;
            int second_nearest_k = 0;

            for (int k = 0; k < centroids->size; k++) {
                if (k != nearest_k) {
                    double dist = manhattan(&inputData->array[j1], &centroids->array[k]);

                    if (dist < mindist_2) {
                        mindist_2 = dist;
                        second_nearest_k = k;
                    }
                }
            }

            double bi = 0, ai = 0;

            for (unsigned ja = 0; ja < assignments[nearest_k]->points.size(); ja++) {
                if (j1 != ja) {
                    ai = ai + manhattan(assignments[nearest_k]->points[ja], assignments[nearest_k]->points[j1]);
                }
            }

            ai = ai / (assignments[nearest_k]->points.size() - 1);


            for (unsigned jb = 0; jb < assignments[second_nearest_k]->points.size(); jb++) {
                bi = bi + manhattan(assignments[second_nearest_k]->points[jb], assignments[nearest_k]->points[j1]);
            }

            bi = bi / assignments[second_nearest_k]->points.size();            

            silhouettes[c] = silhouettes[c] + (bi - ai) / max(bi, ai);
        }

        if ( assignments[c]->points.size() > 0) {
            silhouettes[c] = silhouettes[c] / assignments[c]->points.size();
        }
    }

    for (int c = 0; c < centroids->size; c++) {
        value += silhouettes[c];
    }
    
    value = value / centroids->size;

    silhouettes[centroids->size] = value;

    return silhouettes;
}

void Visualize(ImageDataArray * inputData, ImageDataArray * centroids, ImagePointerArray ** assignments, bool complete) {
    int C = centroids->size;

    for (int i = 0; i < C; i++) {
        if (assignments) {
            cout << "Cluster: #" << i << ": { size: " << assignments[i]->points.size() << ", centroid: " << endl;
        } else {
            cout << "Cluster: #" << i << ": { size: unassigned, centroid: " << endl;
        }

        for (unsigned j = 0; j < centroids->array[i].bytes.size(); j++) {
            cout << (int) centroids->array[i].bytes[j] << ",";
        }
        cout << "} " << endl << endl;

    }

    if (complete && assignments != NULL) {
        for (int i = 0; i < C; i++) {
            cout << "Cluster: #" << i << ": { size: " << centroids->array[i].bytes.size() << ", items: " << endl;

            int N = assignments[i]->points.size();

            for (int j = 0; j < N; j++) {
                cout << assignments[i]->points[j]->id;
                cout << "   ";
            }
            cout << endl;
        }

        cout << endl;
    }
}

