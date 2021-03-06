#include "EuclideanTSPInstance.hpp"
#include "MatrixTSPInstance.hpp"


void runExperiment1(int instanceCountOfEachType, int seed, int baseCityCount, std::vector<unsigned> timeLimits) {
    EuclideanTSPInstance euclInstance;
    MatrixTSPInstance matInstance;

    for (unsigned timeLimit: timeLimits)
    {
        std::cout << "Time limit = " << timeLimit << " seconds" << std::endl << std::endl;

        //Euclidean instances
        for (int i = 0; i < instanceCountOfEachType; i++) 
        {
            switch(i)
            {
                case 0:
                    euclInstance.loadTSPLIB("../../ALL_tsp/a280.tsp");
                    break;
                case 1:
                    euclInstance.loadTSPLIB("../../ALL_tsp/berlin52.tsp");
                    break;
                case 2:
                    euclInstance.loadTSPLIB("../../ALL_tsp/bier127.tsp");
                    break;
                default:    
                    euclInstance.randomInstance(seed, ((i - 2) * 2 - 1) * baseCityCount);
                    break;
            }

            //output format: timeLimit, instanceNumber, instanceCityCount, firstVariantResult, secondVariantResult, ...
            std::cout << timeLimit << "," << i << "," << euclInstance.getCityCount() << ",";

            //aspiration, invertNeighboorhood, start z 2-opta, czas timeLimit, długość listy Tabu = 7
            euclInstance.solveTabuSearch(7, CLOCKS_PER_SEC * timeLimit, false, false, true, &EuclideanTSPInstance::solve2Opt, &EuclideanTSPInstance::symmetricInvert, 
            &EuclideanTSPInstance::symmetricInvertNeighboorhood, &EuclideanTSPInstance::invertAcceleratedMeasurement);
            std::cout << euclInstance.objectiveFunction() << ",";

            //aspiration, invertNeighboorhood, start z najbliższego sąsiada, czas timeLimit, długość listy Tabu = 7
            euclInstance.solveTabuSearch(7, CLOCKS_PER_SEC * timeLimit, false, false, true, &EuclideanTSPInstance::solveNearestNeighboor, &EuclideanTSPInstance::symmetricInvert, 
            &EuclideanTSPInstance::symmetricInvertNeighboorhood, &EuclideanTSPInstance::invertAcceleratedMeasurement);
            std::cout << euclInstance.objectiveFunction() << ",";

            //no aspiration, invertNeighboorhood, start z najbliższego sąsiada, czas timeLimit, długość listy Tabu = 7
            euclInstance.solveTabuSearch(7, CLOCKS_PER_SEC * timeLimit, false, false, false, &EuclideanTSPInstance::solveNearestNeighboor, &EuclideanTSPInstance::symmetricInvert, 
            &EuclideanTSPInstance::symmetricInvertNeighboorhood, &EuclideanTSPInstance::invertAcceleratedMeasurement);
            std::cout << euclInstance.objectiveFunction() << ",";

            //no aspiration, invertNeighboorhood, start z najbliższego sąsiada, czas timeLimit, długość listy Tabu = 35
            euclInstance.solveTabuSearch(35, CLOCKS_PER_SEC * timeLimit, false, false, false, &EuclideanTSPInstance::solveNearestNeighboor, &EuclideanTSPInstance::symmetricInvert, 
            &EuclideanTSPInstance::symmetricInvertNeighboorhood, &EuclideanTSPInstance::invertAcceleratedMeasurement);
            std::cout << euclInstance.objectiveFunction() << ",";

            //aspiration, invertNeighboorhood, start z 2-opta, czas timeLimit, długość listy Tabu = 13
            euclInstance.solveTabuSearch(13, CLOCKS_PER_SEC * timeLimit, false, false, true, &EuclideanTSPInstance::solve2Opt, &EuclideanTSPInstance::symmetricInvert, 
            &EuclideanTSPInstance::symmetricInvertNeighboorhood, &EuclideanTSPInstance::invertAcceleratedMeasurement);
            std::cout << euclInstance.objectiveFunction() << ",";

            //aspiration, swapNeighboorhood, start z 2-opta, czas timeLimit, długość listy Tabu = 7
            euclInstance.solveTabuSearch(7, CLOCKS_PER_SEC * timeLimit, false, false, true, &EuclideanTSPInstance::solve2Opt, &EuclideanTSPInstance::symmetricInsert, 
            &EuclideanTSPInstance::symmetricInsertNeighboorhood, &EuclideanTSPInstance::insertAcceleratedMeasurement);
            std::cout << euclInstance.objectiveFunction() << ",";

            //aspiration, insertNeighboorhood, start z 2-opta, czas timeLimit, długość listy Tabu = 7
            euclInstance.solveTabuSearch(7, CLOCKS_PER_SEC * timeLimit, false, false, true, &EuclideanTSPInstance::solve2Opt, &EuclideanTSPInstance::symmetricSwap, 
            &EuclideanTSPInstance::symmetricSwapNeighboorhood, &EuclideanTSPInstance::swapAcceleratedMeasurement);
            std::cout << euclInstance.objectiveFunction() << ",";

            std::cout << std::endl;
        }

        //matrix instances
        for (int i = 0; i < instanceCountOfEachType; i++) 
        {
            //use non-symmetric instances only so that they are actually different than the euclidean ones
            switch(i)
            {
                case 0:
                    matInstance.loadTSPLIB("../../ALL_tsp/swiss42.tsp");
                    break;
                case 1:
                    matInstance.loadTSPLIB("../../ALL_tsp/bays29.tsp");
                    break;
                case 2:
                    matInstance.loadTSPLIB("../../ALL_tsp/dantzig42.tsp");
                    break;
                default:    
                    matInstance.randomInstance(seed, (i - 2) * baseCityCount, false);
                    break;
            }

            //output format: timeLimit, instanceNumber, instanceCityCount, firstVariantResult, secondVariantResult, ...
            std::cout << timeLimit << "," << instanceCountOfEachType + i << "," << matInstance.getCityCount() << ",";
            
            // //aspiration, invertNeighboorhood, start z 2-opta, czas timeLimit, długość listy Tabu = 7
            matInstance.solveTabuSearch(7, CLOCKS_PER_SEC * timeLimit, false, true, &MatrixTSPInstance::solve2Opt, &MatrixTSPInstance::invert, 
            &MatrixTSPInstance::invertNeighboorhood, &MatrixTSPInstance::invertMeasurement);
            std::cout << matInstance.objectiveFunction() << ",";

            //aspiration, invertNeighboorhood, start z najbliższego sąsiada, czas timeLimit, długość listy Tabu = 7
            matInstance.solveTabuSearch(7, CLOCKS_PER_SEC * timeLimit, false, true, &MatrixTSPInstance::solveNearestNeighboor, &MatrixTSPInstance::invert, 
            &MatrixTSPInstance::invertNeighboorhood, &MatrixTSPInstance::invertMeasurement);
            std::cout << matInstance.objectiveFunction() << ",";

            //no aspiration, invertNeighboorhood, start z najbliższego sąsiada, czas timeLimit, długość listy Tabu = 7
            matInstance.solveTabuSearch(7, CLOCKS_PER_SEC * timeLimit, false, false, &MatrixTSPInstance::solveNearestNeighboor, &MatrixTSPInstance::invert, 
            &MatrixTSPInstance::invertNeighboorhood, &MatrixTSPInstance::invertMeasurement);
            std::cout << matInstance.objectiveFunction() << ",";

            //no aspiration, invertNeighboorhood, start z najbliższego sąsiada, czas timeLimit, długość listy Tabu = 35
            matInstance.solveTabuSearch(35, CLOCKS_PER_SEC * timeLimit, false, false, &MatrixTSPInstance::solveNearestNeighboor, &MatrixTSPInstance::invert, 
            &MatrixTSPInstance::invertNeighboorhood, &MatrixTSPInstance::invertMeasurement);
            std::cout << matInstance.objectiveFunction() << ",";

            //aspiration, invertNeighboorhood, start z 2-opta, czas timeLimit, długość listy Tabu = 13
            matInstance.solveTabuSearch(13, CLOCKS_PER_SEC * timeLimit, false, true, &MatrixTSPInstance::solve2Opt, &MatrixTSPInstance::invert, 
            &MatrixTSPInstance::invertNeighboorhood, &MatrixTSPInstance::invertMeasurement);
            std::cout << matInstance.objectiveFunction() << ",";

            // //aspiration, swapNeighboorhood, start z 2-opta, czas timeLimit, długość listy Tabu = 7
            matInstance.solveTabuSearch(7, CLOCKS_PER_SEC * timeLimit, false, true, &MatrixTSPInstance::solve2Opt, &MatrixTSPInstance::insert, 
            &MatrixTSPInstance::insertNeighboorhood, &MatrixTSPInstance::insertMeasurement);
            std::cout << matInstance.objectiveFunction() << ",";

            // //aspiration, insertNeighboorhood, start z 2-opta, czas timeLimit, długość listy Tabu = 7
            matInstance.solveTabuSearch(7, CLOCKS_PER_SEC * timeLimit, false, true, &MatrixTSPInstance::solve2Opt, &MatrixTSPInstance::swap, 
            &MatrixTSPInstance::swapNeighboorhood, &MatrixTSPInstance::swapMeasurement);
            std::cout << matInstance.objectiveFunction() << ",";

            std::cout << std::endl;
        }
    }

    euclInstance.deleteInstance();
    matInstance.deleteInstance();
}

int main()
{
    runExperiment1(8, 123, 75, {5});
    runExperiment1(4, 123, 75, {15, 30});


    return 0;
}