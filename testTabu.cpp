#include "TabuList.hpp"
#include "EuclideanTSPInstance.hpp"

int main()
{
    EuclideanTSPInstance euclInstance;
    euclInstance.randomInstance(clock(), 2000);
    euclInstance.solveTabuSearch(45, CLOCKS_PER_SEC * 30, false, &EuclideanTSPInstance::solveNearestNeighboor, &EuclideanTSPInstance::symmetricSwap, 
        &EuclideanTSPInstance::symmetricSwapNeighboorhood, &EuclideanTSPInstance::swapAcceleratedMeasurement);

    euclInstance.randomInstance(clock(), 2000);
    euclInstance.solveTabuSearch(45, CLOCKS_PER_SEC * 30, false, &EuclideanTSPInstance::solveNearestNeighboor, &EuclideanTSPInstance::symmetricInsert, 
        &EuclideanTSPInstance::symmetricInsertNeighboorhood, &EuclideanTSPInstance::insertAcceleratedMeasurement);


    euclInstance.deleteInstance();

    return 0;
}