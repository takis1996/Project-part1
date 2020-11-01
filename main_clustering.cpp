
#include <cmath>
#include <utility>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <unordered_set>
#include <cfloat>
#include <ratio>
#include <sys/time.h>

#include "ImageData.h"
#include "ImageDataArray.h"
#include "helpers.h"
#include "ImageHashTable.h"
#include "comparator.h"
#include "Clustering.h"

using namespace std;


int main(int argc, char** argv) {
    int L = 1;
    int k = 4;
    double W = 400;
    int d;
    int M_cube, probes;
    string input_filename = "-";
    string config_filename = "-";
    string output_filename = "-";
    bool complete = false;
    int selector = 0; // 0 : Lloyd,  1 : Reverse approach LSH  2: Reverse approach CUBE
    int clusters = 0;

    cout << " ************************************* " << endl;
    cout << "               Clustering" << endl;
    cout << " ************************************* " << endl;

    for (int i = 1; i < argc; i = i + 1) {
        string arg = argv[i];

        if (arg == "-i") {
            string value = argv[i + 1];
            input_filename = value;
        }
        if (arg == "-c") {
            string value = argv[i + 1];
            config_filename = value;
        }
        if (arg == "-o") {
            string value = argv[i + 1];
            output_filename = value;
        }
        if (arg == "-complete") {
            complete = true;
        }
        if (arg == "-m") {
            string value = argv[i + 1];
            if (value == "LSH") {
                selector = 0;
            }
            if (value == "Hypercube") {
                selector = 1;
            }
            if (value == "Classic") {
                selector = 2;
            }
        }
    }

    if (input_filename == "-") {
        cout << "required -i " << endl;
        return -1;
    }

    if (output_filename == "-") {
        cout << "output file set to: output.txt" << endl;
        output_filename = "output.txt";
    }

    if (config_filename == "-") {
        cout << "Type the config filename: ";
        cin >> config_filename;
    }
    // read config file

    ifstream fp(config_filename.c_str());

    while (true) {
        string token;
        int value;
        if (fp.eof()) {
            break;
        }

        if (!(fp >> token)) {
            break;
        }
        if (!(fp >> value)) {
            break;
        }

        if (token == "number_of_clusters:") {
            clusters = value;
        }
        if (token == "number_of_vector_hash_tables:") {
            L = value;
        }
        if (token == "number_of_vector_hash_functions:") {
            k = value;
        }
        if (token == "max_number_M_hypercube:") {
            M_cube = value;
        }
        if (token == "number_of_hypercube_dimensions:") {
            k = value;
        }
        if (token == "number_of_probes:") {
            probes = value;
        }
    }

    int limitnode = M_cube;
    int limitchains = probes;


    // 
    if (k <= 0) {
        cout << "k cannot be zero or negative" << endl;
        exit(1);
    }

    if (L <= 0) {
        cout << "L cannot be zero or negative" << endl;
        exit(1);
    }

    if (selector != 0 && selector != 1 && selector != 2) {
        cout << "selector should be 0 (Classic) or 1 (LSH) or 2 (Hypercube) " << endl;
        exit(1);
    }

    if (selector == 1) {
        L = 1;
    }

    cout << "---------\n";
    cout << "Settings \n";
    cout << "---------\n";

    cout << "Selector     : " << selector << endl;
    cout << "Input        : " << input_filename << endl;
    cout << "Config       : " << config_filename << endl;
    cout << "Output       : " << output_filename << endl;
    cout << "K            : " << k << endl;
    cout << "L            : " << L << endl;
    cout << "M_cube       : " << M_cube << endl;
    cout << "Probes       : " << probes << endl;


    srand(time(0));

    unordered_set<ImageHashTable * > hashtables;

    ImageDataArray * inputData = load(input_filename);

    int numOfLists = 0;


    if (selector == 0) { // LSH
        numOfLists = inputData->size / 2;
    }

    if (selector == 1) { // CUBE
        numOfLists = (int) pow(2, k);
    }

    d = inputData->array->cols * inputData->array->rows;

    if (selector == 0 || selector == 1) { // LSH or CUBE
        // create hashtables
        for (int i = 0; i < L; i++) {
            ImageHashTable * p = new ImageHashTable(selector, numOfLists, k, d, W);
            cout << "Hashtable " << i << "Created at: " << p << endl;
            hashtables.insert(p);
        }

        // fill hashtables with images
        for (int i = 0; i < inputData->size; i++) { // training
            for (unordered_set<ImageHashTable * >::iterator it = hashtables.begin(); it != hashtables.end(); ++it) {
                ImageHashTable * ht = *it;
                if ((i % 2000) == 0) {
                    cout << "i = " << i << endl;
                }
                ht->insert(&inputData->array[i], i);
            }
        }
    }


    struct timeval t1, t2;

    gettimeofday(&t1, NULL);


    ImageDataArray * initialCentroids = Initialization(inputData, clusters);
    ImagePointerArray ** assignments = NULL;

    cout << "----------------------- INITIAL --------------------------\n";
//    Visualize(NULL, initialCentroids, NULL);
    
    int loops = 1;
    while (loops < 15) {
        switch (selector) {
            case 0:
                assignments = AssignmentLSH(hashtables, inputData, initialCentroids);
                break;
            case 1:
                assignments = AssignmentHypercube(hashtables, inputData, initialCentroids, limitnode, limitchains);
                break;
            case 2:
                assignments = AssignmentClassic(inputData, initialCentroids);
                break;
        }

        ImageDataArray * modifiedCentroids = NULL;

        if (assignments == NULL) {
            cout << "Error during assignment \n";
            exit(1);
        }

        cout << "--------------------- MODIFIED (after assignment) --------------------------\n";

        modifiedCentroids = Update(inputData, initialCentroids, assignments);

        Visualize(inputData, modifiedCentroids, assignments);

        cout << "Calculating distance between steps ... : " << initialCentroids->size << " " << modifiedCentroids->size << endl;
        
        double dist = distance(initialCentroids, modifiedCentroids);

        cout << "Loop: " << loops++ << " - avg dist: " << dist << endl;

        if (dist < 10000) {
            break;
        } else {
            initialCentroids = modifiedCentroids;
        }
    }

    gettimeofday(&t2, NULL);

    double t = ((t2.tv_sec - t1.tv_sec) * 1000.0) + ((t2.tv_usec - t1.tv_usec) / 1000.0);

    cout << " ********************************************************* \n";
    cout << " ********************************************************* \n";
    cout << " ********************************************************* \n";

    if (selector == 0) {
        cout << "-----------------------------------\n";
        cout << "   Algorithm: LSH " << endl;
        cout << "-----------------------------------\n";
    }
    if (selector == 1) {
        cout << "-----------------------------------\n";
        cout << "   Algorithm: Hypercube " << endl;
        cout << "-----------------------------------\n";
    }
    if (selector == 2) {
        cout << "-----------------------------------\n";
        cout << "   Algorithm: Classic " << endl;
        cout << "-----------------------------------\n";
    }

    Visualize(inputData, initialCentroids, assignments, false);

    cout << "Calculating silhouette ... " << endl;


//    double * s = Silhouette(inputData, initialCentroids, assignments);
    double * s = NULL;

    if (s) {
        cout << "Clustering time: " << t << endl;
        cout << "Silhouette:    : { ";

        for (int i = 0; i <= clusters; i++) {
            cout << s[i] << " ";
        }

        cout << " } " << endl;
    }
    
    cout << "Clean up \n";

    delete inputData;


    for (unordered_set<ImageHashTable * >::iterator it = hashtables.begin(); it != hashtables.end(); ++it) {
        ImageHashTable * p = *it;
        cout << "Destroying hashtable at " << p << endl;
        delete p;
    }

    cout << "Finished." << endl;
    
    return 0;
}

