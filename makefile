OBJS_LSH = main_lsh.o ImageData.o ImageDataArray.o helpers.o ImageHashTable.o RandomVector.o
OBJS_HYPERCUBE = main_cube.o ImageData.o ImageDataArray.o helpers.o ImageHashTable.o RandomVector.o
OBJS_CLUSTERING = main_clustering.o ImageData.o ImageDataArray.o helpers.o ImageHashTable.o Clustering.o RandomVector.o
LSH_EXEC_NAME=lsh
HYPERCUBE_EXEC_NAME=cube
CLUSTERING_EXEC_NAME=cluster
CC = g++

all: lsh cube cluster

lsh: $(OBJS_LSH)
	$(CC) -std=c++14 $(OBJS_LSH) -o $(LSH_EXEC_NAME)

cube: $(OBJS_HYPERCUBE)
	$(CC) -std=c++14 $(OBJS_HYPERCUBE) -o $(HYPERCUBE_EXEC_NAME)

cluster: $(OBJS_CLUSTERING)
	$(CC) -std=c++14 $(OBJS_CLUSTERING) -o $(CLUSTERING_EXEC_NAME)

main_lsh.o: main_lsh.cpp
	$(CC) -std=c++14 -c main_lsh.cpp

main_cube.o: main_cube.cpp
	$(CC) -std=c++14 -c main_cube.cpp

main_clustering.o: main_clustering.cpp
	$(CC) -std=c++14 -c main_clustering.cpp

ImageData.o: ImageData.cpp ImageData.h
	$(CC) -std=c++14 -c ImageData.cpp

ImageDataArray.o: ImageDataArray.cpp ImageDataArray.h
	$(CC) -std=c++14 -c ImageDataArray.cpp

helpers.o: helpers.cpp helpers.h
	$(CC) -std=c++14 -c helpers.cpp

ImageHashTable.o: ImageHashTable.cpp ImageHashTable.h 
	$(CC) -std=c++14 -c ImageHashTable.cpp 

Clustering.o: Clustering.cpp Clustering.h
	$(CC) -std=c++14 -c Clustering.cpp

RandomVector.o: RandomVector.cpp RandomVector.h
	$(CC) -std=c++14 -c RandomVector.cpp

clean:
	rm *.o $(LSH_EXEC_NAME) $(HYPERCUBE_EXEC_NAME) $(CLUSTERING_EXEC_NAME) $(OBJS_LSH) $(OBJS_HYPERCUBE) $(OBJS_CLUSTERING)
