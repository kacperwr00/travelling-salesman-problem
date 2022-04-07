#include "EuclideanTSPInstance.hpp"
#include "MatrixTSPInstance.hpp"

namespace util
{
    const auto firstElement = [](auto const& pair) -> auto const& { return pair.first; };
    const auto secondElement = [](auto const& pair) -> auto const& { return pair.second; };
}

enum problemType {Euclidean, DistanceMatrix};

void runExperiment1() {
    EuclideanTSPInstance euclInstance;
    MatrixTSPInstance matInstance;

    // Exp 1
    printf("\n Experiment 1 - NN \n");
    int instances = 10;
    int seed = 123;
    int cityCount = 100;
    int values_nn[instances];
    int values_krandom[instances];
    int values_2opt[instances];
    for(int i=0; i<instances/2; i++) {
        if(i==0) {
            euclInstance.loadTSPLIB("../ALL_tsp/a280.tsp");
        } else if(i==1) {
            euclInstance.loadTSPLIB("../ALL_tsp/berlin52.tsp");
        } else if(i==2) {
            euclInstance.loadTSPLIB("../ALL_tsp/bier127.tsp");
        } else {
            euclInstance.randomInstance(seed, cityCount);
        }
        clock_t t = clock();
        euclInstance.solveNearestNeighboor(false);
        t = clock() -t;
        values_nn[i] = euclInstance.objectiveFunction();
        euclInstance.timedTestKRandom(t, seed);
        values_krandom[i] = euclInstance.objectiveFunction();
        euclInstance.timedTest2Opt(t);
        values_2opt[i] = euclInstance.objectiveFunction();
    }
    for(int i=instances/2; i<instances; i++) {
        if(i==instances/2) {
            matInstance.loadTSPLIB("../ALL_tsp/swiss42.tsp");
        } else if(i==(instances/2)+1) {
            matInstance.loadTSPLIB("../ALL_tsp/bays29.tsp");
        } else if(i==(instances/2)+2) {
            matInstance.loadTSPLIB("../ALL_tsp/dantzig42.tsp");
        } else {
            matInstance.randomInstance(seed, cityCount, false);
        }
        clock_t t = clock();
        matInstance.solveNearestNeighboor();
        t = clock() -t;
        values_nn[i] = matInstance.objectiveFunction();
        matInstance.timedTestKRandom(t, seed);
        values_krandom[i] = matInstance.objectiveFunction();
        matInstance.timedTest2Opt(t);
        values_2opt[i] = matInstance.objectiveFunction();
    }

    printf("\nInstances (random): %i, City count: %i, seed: %i\n", instances, cityCount, seed);
    printf("NN\tKrand\t2opt \n");
    for(int i=0; i<instances; i++) {
        printf("%i\t%i\t%i\n", values_nn[i], values_krandom[i], values_2opt[i]);
    }

}

int main()
{
    runExperiment1();

    return 0;
}