#include "TabuList.hpp"
#include "MatrixTSPInstance.hpp"
#include "EuclideanTSPInstance.hpp"

int main()
{
    // std::cout << "EUCL" << std::endl;
    EuclideanTSPInstance euclInstance;
    euclInstance.randomInstance(123, 70);
    euclInstance.solveGenetic(CLOCKS_PER_SEC * 30, 123, true, 10, 3, 10, &EuclideanTSPInstance::startingPopulation, &EuclideanTSPInstance::mutation, 
        &EuclideanTSPInstance::crossover, &EuclideanTSPInstance::selection);

    std::cout << "Dla porównania 2-opt: \n";
    euclInstance.solve2Opt(false);
    std::cout << euclInstance.objectiveFunction() << std::endl;

    std::cout << "Dla porównania Krandom(cityCount): \n";
    euclInstance.solveKRandom(euclInstance.getCityCount(), 123, false);
    std::cout << euclInstance.objectiveFunction() << std::endl;

    euclInstance.deleteInstance();

    // std::cout << "MATRIX" << std::endl;
    // MatrixTSPInstance matrixTspInstance;
    // //no aspiration, invertNeighboorhood, start z najbliższego sąsiada, czas timeLimit, długość listy Tabu = 35
    //         matrixTspInstance.loadTSPLIB("../../ALL_tsp/dantzig42.tsp");
    //         matrixTspInstance.solveTabuSearch(35, CLOCKS_PER_SEC * 5, false, false, &MatrixTSPInstance::solveNearestNeighboor, &MatrixTSPInstance::invert, 
    //         &MatrixTSPInstance::invertNeighboorhood, &MatrixTSPInstance::invertMeasurement);
    //         std::cout << matrixTspInstance.objectiveFunction() << ",";


    // matrixTspInstance.deleteInstance();

    return 0;
}