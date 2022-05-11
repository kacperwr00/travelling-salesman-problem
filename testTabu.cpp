#include "TabuList.hpp"
#include "MatrixTSPInstance.hpp"
#include "EuclideanTSPInstance.hpp"
#include "MatrixTSPInstance.hpp"

int main()
{
    // std::cout << "EUCL" << std::endl;
    // EuclideanTSPInstance euclInstance;
    // euclInstance.randomInstance(clock(), 700);
    // euclInstance.solveTabuSearch(7, CLOCKS_PER_SEC * 30, true, false, true, &EuclideanTSPInstance::solve2Opt, &EuclideanTSPInstance::symmetricInvert, 
    //     &EuclideanTSPInstance::symmetricInvertNeighboorhood, &EuclideanTSPInstance::invertAcceleratedMeasurement);
    // euclInstance.randomInstance(clock(), 700);
    // euclInstance.solveTabuSearch(7, CLOCKS_PER_SEC * 30, true, false, false, &EuclideanTSPInstance::solve2Opt, &EuclideanTSPInstance::symmetricSwap, 
    //     &EuclideanTSPInstance::symmetricSwapNeighboorhood, &EuclideanTSPInstance::swapAcceleratedMeasurement);


    // euclInstance.randomInstance(clock(), 700);
    // euclInstance.solveTabuSearch(7, CLOCKS_PER_SEC * 30, true, false, true, &EuclideanTSPInstance::solve2Opt, &EuclideanTSPInstance::symmetricInsert, 
    //     &EuclideanTSPInstance::symmetricInsertNeighboorhood, &EuclideanTSPInstance::insertAcceleratedMeasurement);


    // euclInstance.deleteInstance();

    std::cout << "MATRIX" << std::endl;
    MatrixTSPInstance matrixTspInstance;
    matrixTspInstance.randomInstance(clock(), 90, false);
    matrixTspInstance.solveTabuSearch(7, CLOCKS_PER_SEC * 30, true, true, &MatrixTSPInstance::solveNearestNeighboor, &MatrixTSPInstance::invert,
                                 &MatrixTSPInstance::invertNeighboorhood, &MatrixTSPInstance::invertMeasurement);

    matrixTspInstance.randomInstance(clock(), 90, false);
    matrixTspInstance.solveTabuSearch(7, CLOCKS_PER_SEC * 30, true, true, &MatrixTSPInstance::solve2Opt, &MatrixTSPInstance::swap,
                                 &MatrixTSPInstance::swapNeighboorhood, &MatrixTSPInstance::swapMeasurement);


    matrixTspInstance.randomInstance(clock(), 90, false);
    matrixTspInstance.solveTabuSearch(7, CLOCKS_PER_SEC * 30, true, false, &MatrixTSPInstance::solve2Opt, &MatrixTSPInstance::insert,
                                 &MatrixTSPInstance::insertNeighboorhood, &MatrixTSPInstance::insertMeasurement);


    matrixTspInstance.deleteInstance();

    return 0;
}