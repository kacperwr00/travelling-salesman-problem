#include "TabuList.hpp"
#include "EuclideanTSPInstance.hpp"

int main()
{
    EuclideanTSPInstance euclInstance;
    euclInstance.randomInstance(clock(), 700);
    euclInstance.solveTabuSearch(7, CLOCKS_PER_SEC * 30, true, false, true, &EuclideanTSPInstance::solve2Opt, &EuclideanTSPInstance::symmetricInvert, 
        &EuclideanTSPInstance::symmetricInvertNeighboorhood, &EuclideanTSPInstance::invertAcceleratedMeasurement);

    euclInstance.randomInstance(clock(), 700);
    euclInstance.solveTabuSearch(7, CLOCKS_PER_SEC * 30, true, false, false, &EuclideanTSPInstance::solve2Opt, &EuclideanTSPInstance::symmetricSwap, 
        &EuclideanTSPInstance::symmetricSwapNeighboorhood, &EuclideanTSPInstance::swapAcceleratedMeasurement);


    euclInstance.randomInstance(clock(), 700);
    euclInstance.solveTabuSearch(7, CLOCKS_PER_SEC * 30, true, false, true, &EuclideanTSPInstance::solve2Opt, &EuclideanTSPInstance::symmetricInsert, 
        &EuclideanTSPInstance::symmetricInsertNeighboorhood, &EuclideanTSPInstance::insertAcceleratedMeasurement);


    euclInstance.deleteInstance();

    return 0;
}