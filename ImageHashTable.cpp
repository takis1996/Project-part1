#include "ImageHashTable.h"
#include "helpers.h"

#include <iostream>
#include <cmath>
#include <map>
#include <unordered_map>
#include <cfloat>
#include <sys/time.h>

using namespace std;

// TODO:

//vector<int> findnumbers(int x, int hammingdistance, int bits) {
//    //
//}

// findnumbers(010, 0, 3) = { 010 }
// findnumbers(010, 1, 3) = { 110 000 011 }
// findnumbers(0000, 1, 4) = { 1000 0100 0010 0001 }

ImageHashTable::ImageHashTable(int selector, int numOfLists, int k, int d, double W) {
    this->numOfLists = numOfLists;
    this->k = k;
    this->M = (int) (pow(2, 32 / k));
    this->m = (int) ((M / 2) - 1);
    this->W = 40000;
    this->selector = selector;

    hashlists = new list<ImagePointerWithG>[numOfLists];

    for (int i = 0; i < k; i++) {
        RandomVector rv;
        rv.fill(d, W);
        randomVectors.push_back(rv);
    }

    if (selector == 0) {
        f_history = NULL;
    } else {
        f_history = new unordered_map<int, int>[k]();
    }
}

ImageHashTable::~ImageHashTable() {
    delete [] hashlists;

    if (selector == 0) {
    } else {
        delete [] f_history;
    }
}

unsigned ImageHashTable::hash(ImageData * imagedata) {
    unsigned g = 0;

    int i = 0;
    for (list<RandomVector>::iterator it = randomVectors.begin(); it != randomVectors.end(); ++it) {
        unsigned hi;

        if (selector == 0) { // LSH
            hi = h(imagedata, &(*it));
            g = (g << (32 / k)) + hi;
        } else {
            hi = f(i, h(imagedata, &(*it)));
            g = (g << 1) + hi;
            i++;
        }
    }
    return g;
}

void ImageHashTable::insert(ImageData * imagedata, int offset) {
    unsigned g = hash(imagedata);

    ImagePointerWithG ipwg;
    ipwg.pointer = imagedata;
    ipwg.g = g;
    ipwg.offset = offset;

    //    cout << g % numOfLists << endl;
    //        cout << g  % numOfLists << endl;
    hashlists[g % numOfLists].push_back(ipwg);
}

unsigned ImageHashTable::h(ImageData * x, RandomVector * s) {
    static bool firstrun = false;
    static unsigned * mi_mod = nullptr;
    unsigned sum = 0;

    if (firstrun == false) {
        //        cout << "Initialize: mi_mod " << endl;
        firstrun = true;
        mi_mod = new unsigned[ s->components.size()];

        for (unsigned i = 0; i < s->components.size(); i++) { // i = 0 ... 254
            mi_mod[i] = exp_modulo(m, i, M);
        }
    }

    for (unsigned i = 0; i < s->components.size(); i++) { // i = 0 ... 254
        unsigned ai = (int) floor((x->bytes[i] - s->components[i]) / W);
        unsigned ai_mod = moduloPowerOf2(ai, M);

        unsigned prod = moduloPowerOf2((ai_mod * mi_mod[i]), M);

        sum = sum + prod;
    }

    unsigned res = moduloPowerOf2(sum, M);

    return res;
}

ResultNN ImageHashTable::search(ImageData * q, int limitnode, int limitchains) {
    // Nearest neighbor
    unsigned g = 0;

    ResultNN result;
    double dist_nn = DBL_MAX;

    struct timeval t1, t2;

    result.offset = -1;

    gettimeofday(&t1, NULL);

    g = hash(q);

    if (selector == 0) { // LSH
        int chain = g % numOfLists;
        for (list<ImagePointerWithG>::iterator it = hashlists[chain].begin(); it != hashlists[chain].end(); ++it) {
            ImagePointerWithG & ipwg = *it;

            if (ipwg.g == g) {
                // compare: NN + NN top + NN- R
                double dist = manhattan(ipwg.pointer, q);
                if (dist < dist_nn) {
                    dist_nn = dist;
                    result.nn = ipwg.pointer;
                    result.offset = ipwg.offset;
                }

            }
        }
    } else if (selector == 1) { // CUBE
        unsigned hd = 0;

        while (limitnode > 0 && limitchains > 0 && hd < (unsigned) k) {
            for (int i = 0; i < numOfLists; i++) {
                int chain = i;

                if (hammingDistance(chain, g % numOfLists) == hd) {
                    for (list<ImagePointerWithG>::iterator it = hashlists[chain].begin(); it != hashlists[chain].end(); ++it) {
                        ImagePointerWithG & ipwg = *it;

                        if (ipwg.g == g) {
                            // compare: NN + NN top + NN- R
                            double dist = manhattan(ipwg.pointer, q);
                            if (dist < dist_nn) {
                                dist_nn = dist;
                                result.nn = ipwg.pointer;
                                result.offset = ipwg.offset;
                            }
                        }
                        limitnode--;
                    }
                    limitchains--;
                }
                if (limitnode <= 0 || limitchains <= 0) {
                    break;
                }
            }

            hd++;
        }
    }

    gettimeofday(&t2, NULL);

    result.t = ((t2.tv_sec - t1.tv_sec) * 1000.0) + ((t2.tv_usec - t1.tv_usec) / 1000.0);

    result.distance = dist_nn;

    return result;
}

double ImageHashTable::search(ImageData * q, set<int> * results, double R, int limitnode, int limitchains) {
    // Nearest neighbor at R
    unsigned g = 0;

    struct timeval t1, t2;

    gettimeofday(&t1, NULL);

    g = hash(q);

    if (selector == 0) { // LSH
        for (list<ImagePointerWithG>::iterator it = hashlists[g % numOfLists].begin(); it != hashlists[g % numOfLists].end(); ++it) {
            ImagePointerWithG & ipwg = *it;

            if (ipwg.g == g) {
                double dist = manhattan(ipwg.pointer, q);
                if (dist < R) {
                    results->insert(ipwg.offset);
                }
            }
        }
    } else { // CUBE
        map<int, vector<int>> cache;

        for (int chain = 0; chain < numOfLists; chain++) {
            int h = hammingDistance(chain, g % numOfLists);
            cache[h].push_back(chain);
        }

        unsigned hd = 0;

        while (limitnode > 0 && limitchains > 0 && hd < (unsigned) k) {
            for (auto chain : cache[hd]) {
                for (list<ImagePointerWithG>::iterator it = hashlists[chain].begin(); it != hashlists[chain].end(); ++it) {
                    ImagePointerWithG & ipwg = *it;

                    if (ipwg.g == g) {
                        double dist = manhattan(ipwg.pointer, q);
                        if (dist < R) {
                            results->insert(ipwg.offset);
                        }
                    }
                    limitnode--;
                }
                limitchains--;
                if (limitnode <= 0 || limitchains <= 0) {
                    break;
                }
            }

            hd++;
        }
    }

    gettimeofday(&t2, NULL);

    double t = ((t2.tv_sec - t1.tv_sec) * 1000.0) + ((t2.tv_usec - t1.tv_usec) / 1000.0);

    return t;
}

double ImageHashTable::search(ImageData * q, vector<ResultNN> * results, int N, int limitnode, int limitchains) {
    // Nearest neighbor top N
    unsigned g = 0;

    struct timeval t1, t2;

    gettimeofday(&t1, NULL);

    g = hash(q);

    if (selector == 0) {
        for (list<ImagePointerWithG>::iterator it = hashlists[g % numOfLists].begin(); it != hashlists[g % numOfLists].end(); ++it) {
            ImagePointerWithG & ipwg = *it;

            if (ipwg.g == g) {
                double dist = manhattan(ipwg.pointer, q);
                ResultNN result;
                result.nn = ipwg.pointer;
                result.offset = ipwg.offset;
                result.distance = dist;
                result.t = -1;

                results->push_back(result);
            }
        }
    } else {
        map<int, vector<int>> cache;

        for (int chain = 0; chain < numOfLists; chain++) {
            int h = hammingDistance(chain, g % numOfLists);
            cache[h].push_back(chain);
        }

        unsigned hd = 0;

        while (limitnode > 0 && limitchains > 0 && hd < (unsigned) k) {
            for (auto chain : cache[hd]) {
                for (list<ImagePointerWithG>::iterator it = hashlists[chain].begin(); it != hashlists[chain].end(); ++it) {
                    ImagePointerWithG & ipwg = *it;

                    if (ipwg.g == g) {
                        double dist = manhattan(ipwg.pointer, q);
                        ResultNN result;
                        result.nn = ipwg.pointer;
                        result.offset = ipwg.offset;
                        result.distance = dist;
                        result.t = -1;

                        results->push_back(result);
                    }

                    limitnode--;
                }
                limitchains--;

                if (limitnode <= 0 || limitchains <= 0) {
                    break;
                }
            }

            hd++;
        }
    }

    gettimeofday(&t2, NULL);

    double t = ((t2.tv_sec - t1.tv_sec) * 1000.0) + ((t2.tv_usec - t1.tv_usec) / 1000.0);

    return t;
}

unsigned ImageHashTable::f(int i, int value) {
    unordered_map<int, int> * active = &f_history[i];

    if (active->find(value) == active->end()) {
        (*active)[value] = rand() % 2;
        return (*active)[value];
    } else {
        return (*active)[value];
    }
}