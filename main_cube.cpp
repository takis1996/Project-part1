
#include <cmath>
#include <iostream>
#include <utility>
#include <algorithm>
#include <cstdlib>
#include <unordered_set>
#include <cfloat>

#include "ImageData.h"
#include "ImageDataArray.h"
#include "helpers.h"
#include "ImageHashTable.h"
#include "comparator.h"

using namespace std;

// X = 0 : LSH
// X = 1 : HYPERCUBE

int main(int argc, char** argv) {
    int L = 1;
    int k = 4;
    int N = 1;
    double R = 1.0;
    double W = 40000;
    int d;
    int selector;
    int M_Cube, probes;
    string input_filename = "-";
    string query_filename = "-";
    string output_filename = "-";

    cout << " ************************************* " << endl;
    cout << "             HYPERCUBE" << endl;
    cout << " ************************************* " << endl;

    for (int i = 1; i < argc; i = i + 2) {
        string arg = argv[i];

        if (arg == "-selector") {
            int value = atoi(argv[i + 1]);
            selector = value;
        }
        if (arg == "-d") {
            string value = argv[i + 1];
            input_filename = value;
        }
        if (arg == "-probes") {
            probes = atoi(argv[i + 1]);
        }
        if (arg == "-q") {
            string value = argv[i + 1];
            query_filename = value;
        }
        if (arg == "-k") {
            int value = atoi(argv[i + 1]);
            k = value;
        }
        if (arg == "-M") {
            int value = atoi(argv[i + 1]);
            M_Cube = value;
        }
        if (arg == "-o") {
            string value = argv[i + 1];
            output_filename = value;
        }
        if (arg == "-N") {
            int value = atoi(argv[i + 1]);
            N = value;
        }
        if (arg == "-R") {
            double value = atof(argv[i + 1]);
            R = value;
        }
    }

    if (input_filename == "-") {
        cout << "required -d " << endl;
        return -1;
    }

    if (output_filename == "-") {
        cout << "output file set to: output.txt" << endl;
        output_filename = "output.txt";
    }

    if (query_filename == "-") {
        cout << "Type the query filename: ";
        cin >> query_filename;
    }

    if (k <= 0) {
        cout << "k cannot be zero or negative" << endl;
        exit(1);
    }

    if (L <= 0) {
        cout << "L cannot be zero or negative" << endl;
        exit(1);
    }

    if (N < 0) {
        cout << "N cannot be zero or negative" << endl;
        exit(1);
    }

    if (R <= 0) {
        cout << "R cannot be zero or negative" << endl;
        exit(1);
    }

    if (selector != 0 && selector != 1) {
        cout << "selector should be 0 (LSH) or 1 (Hypercube) " << endl;
        exit(1);
    }

    cout << "---------\n";
    cout << "Settings \n";
    cout << "---------\n";

    cout << "Selector     : " << selector << endl;
    cout << "Input        : " << input_filename << endl;
    cout << "Query        : " << query_filename << endl;
    cout << "Output       : " << output_filename << endl;
    cout << "K            : " << k << endl;
    cout << "L            : " << L << endl;
    cout << "N            : " << N << endl;
    cout << "R            : " << R << endl;

    srand(time(0));

    unordered_set<ImageHashTable * > hashtables;

    ImageDataArray * inputData = load(input_filename);
    ImageDataArray * queryData = load(query_filename);

    int numOfLists = (int) pow(2, k);
    d = inputData->array->cols * inputData->array->rows;

    for (int i = 0; i < L; i++) {
        ImageHashTable * p = new ImageHashTable(selector, numOfLists, k, d, W);
        cout << "Hashtable " << i << " Created at: " << p << endl;
        hashtables.insert(p);
    }

    cout << "Inserting to hashtables ... \n";
    for (int i = 0; i < inputData->size; i++) { // training
        for (unordered_set<ImageHashTable * >::iterator it = hashtables.begin(); it != hashtables.end(); ++it) {
            ImageHashTable * ht = *it;
            ht->insert(&inputData->array[i], i);
        }
    }


    for (int i = 0; i < queryData->size; i++) { // REM: remove i < 3
        ImageData * q = &queryData->array[i];

        cout << "*** Query:" << i << "***" << endl;

        // NN
        double t = 0;
        double t_lsh = 0;

        // ***************************** TRUE NN *****************************/ 
        ResultNN true_result1 = nearestNeighbor(q, inputData);

        // ***************************** CUBE NN *****************************/ 
        ResultNN cube_result1;
        cube_result1.distance = DBL_MAX;

        for (unordered_set<ImageHashTable * >::iterator it = hashtables.begin(); it != hashtables.end(); ++it) {
            ImageHashTable * ht = *it;
            ResultNN table_result = ht->search(q, M_Cube, probes);
            if (table_result.distance < cube_result1.distance) {
                cube_result1 = table_result;
            }
        }

        cout << "Nearest neighbor-1 (TRUE): " << true_result1.offset << endl;
        cout << "Nearest neighbor-1 (CUBE) : " << cube_result1.offset << endl;
        cout << "distanceCUBE      : " << cube_result1.distance << endl;
        cout << "distanceTrue      : " << true_result1.distance << endl;
        cout << "Time    True      : " << true_result1.t << "ms" << endl;
        cout << "Time    CUBE      : " << cube_result1.t << "ms" << endl;

        t += true_result1.t;
        t_lsh += cube_result1.t;

        // Top NN
        if (N != 0) {
            // ***************************** TRUE NN (Nth) *****************************/ 
            vector<ResultNN> true_result2 = nearestNeighbor(q, inputData, N);

            t += true_result2[true_result2.size() - 1].t;

            // ***************************** CUBE NN (Nth) *****************************/ 
            vector<ResultNN> cube_result2s;
            for (unordered_set<ImageHashTable * >::iterator it = hashtables.begin(); it != hashtables.end(); ++it) {
                ImageHashTable * ht = *it;
                t_lsh += ht->search(q, &cube_result2s, N, M_Cube, probes);
            }

            std::sort(cube_result2s.begin(), cube_result2s.end(), comparator());

            for (unsigned i = 1; i < (unsigned) N; i++) {
                if (i < true_result2.size()) {
                    cout << "True Nearest neighbor-" << i << ": " << true_result2[i].offset << endl;
                    cout << "distanceTrue      : " << true_result2[i].distance << endl;
//                    cout << "Time    True      : " << true_result2[i].t << "ms" << endl;
                }

                if (i < cube_result2s.size()) {
                    ResultNN cube_result2;
                    cube_result2.distance = cube_result2s[i].distance;
                    cube_result2.offset = cube_result2s[i].offset;
                    cube_result2.nn = cube_result2s[i].nn;
                    cube_result2.t = t_lsh;

                    cout << "LSH Nearest neighbor-" << i << ": " << cube_result2.offset << endl;
                    cout << "distanceLSH      : " << cube_result2.distance << endl;
//                    cout << "Time    LSH      : " << lsh_result2.t << "ms" << endl;
                }
            }

        }

        // R
        if (R != 0) {
            // ***************************** TRUE NN (R) *****************************/ 
            vector<ResultNN> true_result3 = nearestNeighbor(q, inputData, R);
            //            cout << "R        neighbors: " << true_result3.size() - 1 << endl;
            cout << "Time    True      : " << true_result3[0].t << "ms" << endl;

            for (unsigned i = 1; i < true_result3.size(); i++) {
                cout << true_result3[i].offset << " " << true_result3[i].distance << endl;
            }

            t += true_result1.t;

            // ***************************** CUBE NN (R) *****************************/ 
            set<int> cube_result3;

            for (unordered_set<ImageHashTable * >::iterator it = hashtables.begin(); it != hashtables.end(); ++it) {
                ImageHashTable * ht = *it;
                t_lsh += ht->search(q, &cube_result3, R);
            }

            cout << "Time     CUBE       : " << t_lsh << "ms" << endl;
            cout << "R        neighbors: " << cube_result3.size() << endl;

            for (auto offset : cube_result3) {
                cout << offset << endl;
            }
        }

        cout << "Total time true: " << t << "ms" << endl;
        cout << "Total time CUBE: " << t_lsh << "ms" << endl;
    }

    for (unordered_set<ImageHashTable * >::iterator it = hashtables.begin(); it != hashtables.end(); ++it) {
        ImageHashTable * p = *it;
        cout << "Destroying hashtable at " << p << endl;
        delete p;
    }

    delete inputData;

    delete queryData;

    return 0;
}

