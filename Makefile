CC = g++
CFLAGS = -c -std=c++14
SHARED_OBJECTS = Vector.o Point.o Curve.o Hypercube.o HypercubeFunction.o HypercubeHashFunction.o RandomFunction.o Method.o CurveMethod.o VectorMethod.o VectorMetric.o LPNorm.o CurveMetric.o DiscreteFrechet.o Arithmetic.o File.o Parser.o Random.o String.o Grid.o CurveLSHTable.o CurveLSH.o LSHFunction.o LSHHashFunction.o VectorLSH.o VectorLSHTable.o

SEARCH_OUT = search
SEARCH_SPECIAL_OBJECTS = SearchArguments.o Frechet.o ContinuousFrechet.o
SEARCH_MAIN = AppliedSearch.o SearchMain.o

CLUSTER_OUT = cluster
CLUSTER_OBJECTS = CurveClusteringResults.o VectorClusteringResults.o ClusteringParameters.o ClusteringArguments.o VectorCluster.o CurveCluster.o VectorClustering.o CurveClustering.o CurveHashFunction.o VectorHashFunction.o BinaryTree.o
CLUSTER_MAIN = AppliedClustering.o ClusteringMain.o

UNIT_TEST_OUT = unitTest
UNIT_TEST_OBJECTS = Vector.o UnitTest.o VectorMetric.o Random.o Grid.o Point.o Curve.o ContinuousFrechet.o DiscreteFrechet.o CurveMetric.o Frechet.o Arithmetic.o String.o
UNIT_TEST_MAIN = unitTestMain.o

all:
	make search
	make cluster
	make unitTest

search: $(SEARCH_MAIN) $(SEARCH_OBJECTS) $(SEARCH_SPECIAL_OBJECTS) $(SHARED_OBJECTS)
	$(CC) $(SEARCH_MAIN) $(SEARCH_SPECIAL_OBJECTS) $(SHARED_OBJECTS) -o $(SEARCH_OUT)

cluster: $(CLUSTER_MAIN) $(CLUSTER_OBJECTS) $(LSH_OBJECTS) $(HYPERCUBE_OBJECTS) $(SHARED_OBJECTS)
	$(CC) $(CLUSTER_MAIN) $(CLUSTER_OBJECTS) $(SHARED_OBJECTS) -o $(CLUSTER_OUT)

unitTest: $(UNIT_TEST_MAIN) $(UNIT_TEST_OBJECTS)
	$(CC) $(UNIT_TEST_MAIN) $(UNIT_TEST_OBJECTS) -o  $(UNIT_TEST_OUT) -lcunit

LSHFunction.o: LSH-Modules/LSH-Function/LSHFunction.cpp LSH-Modules/LSH-Function/LSHFunction.h
	$(CC) $(CFLAGS) LSH-Modules/LSH-Function/LSHFunction.cpp

LSHHashFunction.o: LSH-Modules/LSH-Hash-Function/LSHHashFunction.cpp LSH-Modules/LSH-Hash-Function/LSHHashFunction.h
	$(CC) $(CFLAGS) LSH-Modules/LSH-Hash-Function/LSHHashFunction.cpp

Grid.o: LSH-Modules/Curve-LSH-Modules/Grid/Grid.cpp LSH-Modules/Curve-LSH-Modules/Grid/Grid.h
	$(CC) $(CFLAGS) LSH-Modules/Curve-LSH-Modules/Grid/Grid.cpp

CurveLSH.o: LSH-Modules/Curve-LSH-Modules/Curve-LSH/CurveLSH.cpp LSH-Modules/Curve-LSH-Modules/Curve-LSH/CurveLSH.h
	$(CC) $(CFLAGS) LSH-Modules/Curve-LSH-Modules/Curve-LSH/CurveLSH.cpp

CurveLSHTable.o: LSH-Modules/Curve-LSH-Modules/Curve-LSH-Table/CurveLSHTable.cpp LSH-Modules/Curve-LSH-Modules/Curve-LSH-Table/CurveLSHTable.h
	$(CC) $(CFLAGS) LSH-Modules/Curve-LSH-Modules/Curve-LSH-Table/CurveLSHTable.cpp

VectorLSHTable.o: LSH-Modules/Vector-LSH-Modules/Vector-LSH-Table/VectorLSHTable.cpp LSH-Modules/Vector-LSH-Modules/Vector-LSH-Table/VectorLSHTable.h
	$(CC) $(CFLAGS) LSH-Modules/Vector-LSH-Modules/Vector-LSH-Table/VectorLSHTable.cpp

VectorLSH.o: LSH-Modules/Vector-LSH-Modules/Vector-LSH/VectorLSH.cpp LSH-Modules/Vector-LSH-Modules/Vector-LSH/VectorLSH.h
	$(CC) $(CFLAGS) LSH-Modules/Vector-LSH-Modules/Vector-LSH/VectorLSH.cpp

CurveCluster.o: Cluster-Modules/Curve-Clustering/Curve-Cluster/CurveCluster.cpp Cluster-Modules/Curve-Clustering/Curve-Cluster/CurveCluster.h
	$(CC) $(CFLAGS) Cluster-Modules/Curve-Clustering/Curve-Cluster/CurveCluster.cpp

CurveClustering.o: Cluster-Modules/Curve-Clustering/Curve-Clustering/CurveClustering.cpp Cluster-Modules/Curve-Clustering/Curve-Clustering/CurveClustering.h
	$(CC) $(CFLAGS) Cluster-Modules/Curve-Clustering/Curve-Clustering/CurveClustering.cpp

CurveClusteringResults.o: Cluster-Modules/Curve-Clustering/Curve-Clustering-Results/CurveClusteringResults.cpp Cluster-Modules/Curve-Clustering/Curve-Clustering-Results/CurveClusteringResults.h
	$(CC) $(CFLAGS) Cluster-Modules/Curve-Clustering/Curve-Clustering-Results/CurveClusteringResults.cpp

VectorCluster.o: Cluster-Modules/Vector-Clustering/Vector-Cluster/VectorCluster.cpp Cluster-Modules/Vector-Clustering/Vector-Cluster/VectorCluster.h
	$(CC) $(CFLAGS) Cluster-Modules/Vector-Clustering/Vector-Cluster/VectorCluster.cpp

VectorClusteringResults.o: Cluster-Modules/Vector-Clustering/Vector-Clustering-Results/VectorClusteringResults.cpp Cluster-Modules/Vector-Clustering/Vector-Clustering-Results/VectorClusteringResults.h
	$(CC) $(CFLAGS) Cluster-Modules/Vector-Clustering/Vector-Clustering-Results/VectorClusteringResults.cpp

VectorClustering.o: Cluster-Modules/Vector-Clustering/Vector-Clustering/VectorClustering.cpp Cluster-Modules/Vector-Clustering/Vector-Clustering/VectorClustering.h
	$(CC) $(CFLAGS) Cluster-Modules/Vector-Clustering/Vector-Clustering/VectorClustering.cpp

HashFunction.o: Cluster-Modules/Vector-Clustering/Vector-Clustering/VectorHashFunction.cpp LSH-Modules/LSH-Hash-Function/LSHHashFunction.h Cluster-Modules/Vector-Clustering/Vector-Clustering/VectorHashFunction.h Hypercube-Modules/Hypercube-Hash-Function/HypercubeHashFunction.h
	$(CC) $(CFLAGS) Cluster-Modules/Vector-Clustering/Vector-Clustering/HashFunction.cpp

ClusteringParameters.o: Cluster-Modules/Clustering-Parameters/ClusteringParameters.cpp Cluster-Modules/Clustering-Parameters/ClusteringParameters.h
	$(CC) $(CFLAGS) Cluster-Modules/Clustering-Parameters/ClusteringParameters.cpp

AppliedSearch.o: Main-Modules/Applied-Search/AppliedSearch.cpp Main-Modules/Applied-Search/AppliedSearch.h
	$(CC) $(CFLAGS) Main-Modules/Applied-Search/AppliedSearch.cpp

SearchMain.o: Main-Modules/Applied-Search/SearchMain.cpp
	$(CC) $(CFLAGS) Main-Modules/Applied-Search/SearchMain.cpp

ClusteringMain.o: Main-Modules/Applied-Clustering/ClusteringMain.cpp
	$(CC) $(CFLAGS) Main-Modules/Applied-Clustering/ClusteringMain.cpp

AppliedClustering.o: Main-Modules/Applied-Clustering/AppliedClustering.cpp Main-Modules/Applied-Clustering/AppliedClustering.h
	$(CC) $(CFLAGS) Main-Modules/Applied-Clustering/AppliedClustering.cpp

ContinuousFrechet.o: Metric-Modules/Continuous-Frechet/ContinuousFrechet.cpp Metric-Modules/Continuous-Frechet/ContinuousFrechet.h
	$(CC) $(CFLAGS) Metric-Modules/Continuous-Frechet/ContinuousFrechet.cpp

Frechet.o: Metric-Modules/Continuous-Frechet/src/Frechet.cpp Metric-Modules/Continuous-Frechet/include/Frechet.hpp Metric-Modules/Continuous-Frechet/include/interval.hpp Metric-Modules/Continuous-Frechet/include/types.hpp
	$(CC) $(CFLAGS) Metric-Modules/Continuous-Frechet/src/Frechet.cpp

LPNorm.o: Metric-Modules/LP-Norm Metric-Modules/LP-Norm
	$(CC) $(CFLAGS) Metric-Modules/LP-Norm/LPNorm.cpp

VectorMetric.o: Metric-Modules/Vector-Metric/VectorMetric.cpp Metric-Modules/Vector-Metric/VectorMetric.h
	$(CC) $(CFLAGS) Metric-Modules/Vector-Metric/VectorMetric.cpp

DiscreteFrechet.o: Metric-Modules/Discrete-Frechet/DiscreteFrechet.cpp Metric-Modules/Discrete-Frechet/DiscreteFrechet.h
	$(CC) $(CFLAGS) Metric-Modules/Discrete-Frechet/DiscreteFrechet.cpp

CurveMetric.o: Metric-Modules/Curve-Metric/CurveMetric.cpp Metric-Modules/Curve-Metric/CurveMetric.h
	$(CC) $(CFLAGS) Metric-Modules/Curve-Metric/CurveMetric.cpp

Curve.o: Curve-Modules/Curve/Curve.cpp Curve-Modules/Curve/Curve.h
	$(CC) $(CFLAGS) Curve-Modules/Curve/Curve.cpp

Vector.o: Vector-Modules/Vector/Vector.cpp Vector-Modules/Vector/Vector.h
	$(CC) $(CFLAGS) Vector-Modules/Vector/Vector.cpp

Point.o: Vector-Modules/Point/Point.cpp Vector-Modules/Point/Point.h
	$(CC) $(CFLAGS) Vector-Modules/Point/Point.cpp

Hypercube.o: Hypercube-Modules/Hypercube/Hypercube.cpp Hypercube-Modules/Hypercube/Hypercube.h
	$(CC) $(CFLAGS) Hypercube-Modules/Hypercube/Hypercube.cpp

HypercubeHashFunction.o: Hypercube-Modules/Hypercube-Hash-Function/HypercubeHashFunction.cpp Hypercube-Modules/Hypercube-Hash-Function/HypercubeHashFunction.h
	$(CC) $(CFLAGS) Hypercube-Modules/Hypercube-Hash-Function/HypercubeHashFunction.cpp

HypercubeFunction.o: Hypercube-Modules/Hypercube-Function/HypercubeFunction.cpp Hypercube-Modules/Hypercube-Function/HypercubeFunction.h
	$(CC) $(CFLAGS) Hypercube-Modules/Hypercube-Function/HypercubeFunction.cpp

RandomFunction.o: Hypercube-Modules/Random-Function/RandomFunction.cpp Hypercube-Modules/Random-Function/RandomFunction.h
	$(CC) $(CFLAGS) Hypercube-Modules/Random-Function/RandomFunction.cpp

CurveMethod.o: Method-Modules/Curve-Method/CurveMethod.cpp Method-Modules/Curve-Method/CurveMethod.h
	$(CC) $(CFLAGS) Method-Modules/Curve-Method/CurveMethod.cpp

Method.o: Method-Modules/Method/Method.cpp Method-Modules/Curve-Method/CurveMethod.h Method-Modules/Method/Method.h Method-Modules/Vector-Method/VectorMethod.h
	$(CC) $(CFLAGS) Method-Modules/Method/Method.cpp

VectorMethod.o: Method-Modules/Vector-Method/VectorMethod.cpp Method-Modules/Vector-Method/VectorMethod.h
	$(CC) $(CFLAGS) Method-Modules/Vector-Method/VectorMethod.cpp

SearchArguments.o: CMD-Modules/Search-Arguments/SearchArguments.cpp CMD-Modules/Search-Arguments/SearchArguments.h
	$(CC) $(CFLAGS) CMD-Modules/Search-Arguments/SearchArguments.cpp

ClusteringArguments.o: CMD-Modules/Clustering-Arguments/ClusteringArguments.cpp CMD-Modules/Clustering-Arguments/ClusteringArguments.h
	$(CC) $(CFLAGS) CMD-Modules/Clustering-Arguments/ClusteringArguments.cpp

String.o: Utilities/String/String.cpp Utilities/String/String.h
	$(CC) $(CFLAGS) Utilities/String/String.cpp

Arithmetic.o: Utilities/Arithmetic/Arithmetic.cpp Utilities/Arithmetic/Arithmetic.h
	$(CC) $(CFLAGS) Utilities/Arithmetic/Arithmetic.cpp

Parser.o: Utilities/Parser/Parser.cpp Utilities/Parser/Parser.h
	$(CC) $(CFLAGS) Utilities/Parser/Parser.cpp

Random.o: Utilities/Random/Random.cpp Utilities/Random/Random.h
	$(CC) $(CFLAGS) Utilities/Random/Random.cpp

File.o: Utilities/File/File.cpp Utilities/File/File.h
	$(CC) $(CFLAGS) Utilities/File/File.cpp

VectorHashFunction.o: Cluster-Modules/Vector-Clustering/Vector-Clustering/VectorHashFunction.cpp Cluster-Modules/Vector-Clustering/Vector-Clustering/VectorHashFunction.h
	$(CC) $(CFLAGS) Cluster-Modules/Vector-Clustering/Vector-Clustering/VectorHashFunction.cpp

CurveHashFunction.o: Cluster-Modules/Curve-Clustering/Curve-Clustering/CurveHashFunction.cpp Cluster-Modules/Curve-Clustering/Curve-Clustering/CurveHashFunction.h
	$(CC) $(CFLAGS) Cluster-Modules/Curve-Clustering/Curve-Clustering/CurveHashFunction.cpp

BinaryTree.o: Cluster-Modules/Curve-Clustering/Binary-Tree/BinaryTree.cpp Cluster-Modules/Curve-Clustering/Binary-Tree/BinaryTree.h
	$(CC) $(CFLAGS) Cluster-Modules/Curve-Clustering/Binary-Tree/BinaryTree.cpp

unitTestMain.o: Unit-Test/unitTestMain.cpp
	$(CC) $(CFLAGS) -lcunit Unit-Test/unitTestMain.cpp

UnitTest.o: Unit-Test/UnitTest.h Unit-Test/UnitTest.cpp
	$(CC) $(CFLAGS) -lcunit Unit-Test/UnitTest.cpp

clean:
	rm -f *.o

wipe:
	make clean
	rm -f search
	rm -f cluster
	rm -f unitTest
