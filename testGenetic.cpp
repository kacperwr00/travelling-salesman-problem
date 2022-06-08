#include "TabuList.hpp"
#include "MatrixTSPInstance.hpp"
#include "EuclideanTSPInstance.hpp"

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        return -1;
    }

    int thread = atoi(argv[1]);

    if (thread <= 0 || thread > 4)
    {
        return -1;
    }
    
    long seed = 123;
    int instanceCountOfEachType = 8;
    unsigned baseCityCount = 70;


    // int totalVariantCount = 3 * 3 * 3 * 2 * 2;
    std::vector<std::array<unsigned, 5>> variants;

    for (unsigned i = 0, j = 0, k = 0, l = 0, m = 0; ; m++)
    {
        if (m == 3)
        {
            m = 0;
            l++;
        }
        if (l == 2)
        {
            l = 0;
            k++;
        }
        if (k == 3)
        {
            k = 0;
            j++;
        }
        if (j == 2)
        {
            j = 0;
            i++;
        }
        if (i == 3)
        {
            break;
        }
        variants.push_back({i, j, k, l, m});
        // variants.push_back({{0, 0, 0, 0, 0}});
    }
    // std::cout << "Total variants count: " << variants.size() << std::endl;
    
    srand(123);
    for (long unsigned i = variants.size() - 1, j; i > 0; i--)
    {
        j = rand() % (i + 1);
        auto tmp = variants[i];
        variants[i] = variants[j];
        variants[j] = tmp;
    }

    std::cout << "Thread: " << thread << " Total variants: " << variants.size() << std::endl;
    for (int i = 0; i < 100; i++)
    {
        std::cout << variants[i][0] << "," << variants[i][1] << "," << variants[i][2] << "," << variants[i][3] << "," << variants[i][4] << std::endl;
    }

    // {
        // std::cout << "EUCL" << std::endl;
    //     EuclideanTSPInstance euclInstance;
    
    // // EuclideanTSPInstance::getStartingPopulation startingPopulationsEucl[3];
    // // EuclideanTSPInstance::getCrossoverPairs getCrossoverPairsEucl[2];
    // // EuclideanTSPInstance::crossoverFunction crossoverFunctionsEucl[3];
    // // EuclideanTSPInstance::selectionFunction selectionFunctionsEucl[2];
    // // EuclideanTSPInstance::mutationFunction mutationsEucl[3];
    
    // // startingPopulationsEucl[0] = &EuclideanTSPInstance::startingPopulation;
    // // startingPopulationsEucl[1] = &EuclideanTSPInstance::startingPopulationTwo;
    // // startingPopulationsEucl[2] = &EuclideanTSPInstance::startingPopulationThree;

    // // getCrossoverPairsEucl[0] = &EuclideanTSPInstance::crossoverPairs;
    // // getCrossoverPairsEucl[1] = &EuclideanTSPInstance::crossoverPairsTwo;

    // // crossoverFunctionsEucl[0] = &EuclideanTSPInstance::crossover;
    // // crossoverFunctionsEucl[1] = &EuclideanTSPInstance::crossoverTwo;
    // // crossoverFunctionsEucl[2] = &EuclideanTSPInstance::crossoverThree;

    // // selectionFunctionsEucl[0] = &EuclideanTSPInstance::selection;
    // // selectionFunctionsEucl[1] = &EuclideanTSPInstance::selectionTwo;

    // // mutationsEucl[0] = &EuclideanTSPInstance::mutation;
    // // mutationsEucl[1] = &EuclideanTSPInstance::mutationTwo;
    // // mutationsEucl[2] = &EuclideanTSPInstance::mutationThree;
    //     // euclInstance.randomInstance(123, 70);
    //     // euclInstance.solveGenetic(CLOCKS_PER_SEC * 60, 123, true, 100, 15, 100, startingPopulationsEucl[2], getCrossoverPairsEucl[0],
    //     //         mutationsEucl[2], crossoverFunctionsEucl[2], selectionFunctionsEucl[0]);

    //     // std::cout << "Dla porównania 2-opt: \n";
    //     // euclInstance.solve2Opt(false);
    //     // std::cout << euclInstance.objectiveFunction() << std::endl;

    //     // std::cout << "Dla porównania Krandom(cityCount): \n";
    //     // euclInstance.solveKRandom(euclInstance.getCityCount(), 123, false);
    //     // std::cout << euclInstance.objectiveFunction() << std::endl;


    //     // std::cout << "MATRIX" << std::endl;

    //     //LOSOWE PRÓBKOWANIE


    //     for (int i = 0; i < instanceCountOfEachType; i++) 
    //     {
    //         switch(i)
    //         {
    //             case 0:
    //                 euclInstance.loadTSPLIB("../../ALL_tsp/a280.tsp");
    //                 break;
    //             case 1:
    //                 euclInstance.loadTSPLIB("../../ALL_tsp/berlin52.tsp");
    //                 break;
    //             case 2:
    //                 euclInstance.loadTSPLIB("../../ALL_tsp/bier127.tsp");
    //                 break;
    //             default:    
    //                 euclInstance.randomInstance(seed, (i - 2) * baseCityCount);
    //                 break;
    //         }

    //         EuclideanTSPInstance::getStartingPopulation startingPopulationsEucl;
    //         EuclideanTSPInstance::getCrossoverPairs getCrossoverPairsEucl;
    //         EuclideanTSPInstance::crossoverFunction crossoverFunctionsEucl;
    //         EuclideanTSPInstance::selectionFunction selectionFunctionsEucl;
    //         EuclideanTSPInstance::mutationFunction mutationsEucl;

    //         for (int i = (thread - 1) * 25; i < thread * 25; i++)
    //         {
    //             if (variants[i][0] == 0)
    //             {
    //                 startingPopulationsEucl = &EuclideanTSPInstance::startingPopulation;
    //             }
    //             else if (variants[i][0] == 1)
    //             {
    //                 startingPopulationsEucl = &EuclideanTSPInstance::startingPopulationTwo;
    //             }
    //             else //if (variants[i][0] == 2)
    //             {
    //                 startingPopulationsEucl = &EuclideanTSPInstance::startingPopulationThree;
    //             }

    //             if (variants[i][1] == 0)
    //             {
    //                 getCrossoverPairsEucl = &EuclideanTSPInstance::crossoverPairs;
    //             }
    //             else //if (variants[i][1] == 1)
    //             {
    //                 getCrossoverPairsEucl = &EuclideanTSPInstance::crossoverPairsTwo;
    //             }
                
    //             if (variants[i][2] == 0)
    //             {
    //                 mutationsEucl = &EuclideanTSPInstance::mutation;
    //             }
    //             else if (variants[i][2] == 1)
    //             {
    //                 mutationsEucl = &EuclideanTSPInstance::mutationTwo;
    //             }
    //             else //if (variants[i][2] == 2)
    //             {
    //                 mutationsEucl = &EuclideanTSPInstance::mutationThree;
    //             }

    //             if (variants[i][3] == 0)
    //             {
    //                 crossoverFunctionsEucl = &EuclideanTSPInstance::crossover;
    //             }
    //             else if (variants[i][3] == 1)
    //             {
    //                 crossoverFunctionsEucl = &EuclideanTSPInstance::crossoverTwo;
    //             }
    //             else //if (variants[i][3] == 2)
    //             {
    //                 crossoverFunctionsEucl = &EuclideanTSPInstance::crossoverThree;
    //             }

    //             if (variants[i][4] == 0)
    //             {
    //                 selectionFunctionsEucl = &EuclideanTSPInstance::selection;
    //             }
    //             else //if (variants[i][4] == 1)
    //             {
    //                 selectionFunctionsEucl = &EuclideanTSPInstance::selectionTwo;
    //             }

    //             // euclInstance.solveGenetic(CLOCKS_PER_SEC * 10, 123, true, 100, 15, 100, startingPopulationsEucl[variants[i][0]], getCrossoverPairsEucl[variants[i][1]],
    //             //     mutationsEucl[variants[i][2]], crossoverFunctionsEucl[variants[i][3]], selectionFunctionsEucl[variants[i][4]]);
    //             // euclInstance.solveGenetic(CLOCKS_PER_SEC * 10, 123, true, 100, 15, 100, &EuclideanTSPInstance::startingPopulationTwo, &EuclideanTSPInstance::crossoverPairs, &EuclideanTSPInstance::mutationThree,
    //             //     &EuclideanTSPInstance::crossover, &EuclideanTSPInstance::selection);
    //             euclInstance.solveGenetic(CLOCKS_PER_SEC * 60, 123, false, 40, 15, 40, startingPopulationsEucl, getCrossoverPairsEucl,
    //                 mutationsEucl, crossoverFunctionsEucl, selectionFunctionsEucl);
                
    //             std::cout << euclInstance.objectiveFunction() << ",";
    //             // std::cout << variants[i][0] << "," << variants[i][1] << "," << variants[i][2] << "," << variants[i][3] << "," << variants[i][4] << std::endl;
    //         }

    //         std::cout << std::endl;
    //         euclInstance.deleteInstance();
    //     }
    // }

    MatrixTSPInstance matrixInstance;
    // std::vector<morph::vVector<unsigned>> (MatrixTSPInstance::*startingPopulationsMatrix[3])(unsigned);
    // std::vector<std::pair<unsigned, unsigned>> (MatrixTSPInstance::*getCrossoverPairsMatrix[2])(const unsigned, const unsigned, const unsigned, const long, const int, std::set<std::pair<int, morph::vVector<unsigned>>>);
    // std::pair<int, morph::vVector<unsigned>> (MatrixTSPInstance::*crossoverFunctionsMatrix[3])(std::pair<int, morph::vVector<unsigned>>, std::pair<int, morph::vVector<unsigned>>);
    // std::set<std::pair<int, morph::vVector<unsigned>>> (MatrixTSPInstance::*selectionFunctionsMatrix[2])(std::set<std::pair<int, morph::vVector<unsigned>>>, unsigned);
    // std::pair<int, morph::vVector<unsigned>> (MatrixTSPInstance::*mutationsMatrix[3])(const std::pair<int, morph::vVector<unsigned>>);
    
    // startingPopulationsMatrix[0] = &MatrixTSPInstance::startingPopulation;
    // startingPopulationsMatrix[1] = &MatrixTSPInstance::startingPopulationTwo;
    // startingPopulationsMatrix[2] = &MatrixTSPInstance::startingPopulationThree;

    // getCrossoverPairsMatrix[0] = &MatrixTSPInstance::crossoverPairs;
    // getCrossoverPairsMatrix[1] = &MatrixTSPInstance::crossoverPairsTwo;

    // crossoverFunctionsMatrix[0] = &MatrixTSPInstance::crossover;
    // crossoverFunctionsMatrix[1] = &MatrixTSPInstance::crossoverTwo;
    // crossoverFunctionsMatrix[2] = &MatrixTSPInstance::crossoverThree;

    // selectionFunctionsMatrix[0] = &MatrixTSPInstance::selection;
    // selectionFunctionsMatrix[1] = &MatrixTSPInstance::selectionTwo;

    // mutationsMatrix[0] = &MatrixTSPInstance::mutation;
    // mutationsMatrix[1] = &MatrixTSPInstance::mutationTwo;
    // mutationsMatrix[2] = &MatrixTSPInstance::mutationThree;


    //matrix instances
    for (int i = 0; i < instanceCountOfEachType; i++) 
    {
        //use non-symmetric instances only so that they are actually different than the euclidean ones
        switch(i)
        {
            case 0:
                matrixInstance.loadTSPLIB("../../ALL_tsp/swiss42.tsp");
                break;
            case 1:
                matrixInstance.loadTSPLIB("../../ALL_tsp/bays29.tsp");
                break;
            case 2:
                matrixInstance.loadTSPLIB("../../ALL_tsp/dantzig42.tsp");
                break;
            default:    
                matrixInstance.randomInstance(seed, (i - 2) * baseCityCount, false);
                break;
        }

        MatrixTSPInstance::getStartingPopulation startingPopulationsMatrix;
        MatrixTSPInstance::getCrossoverPairs getCrossoverPairsMatrix;
        MatrixTSPInstance::crossoverFunction crossoverFunctionsMatrix;
        MatrixTSPInstance::selectionFunction selectionFunctionsMatrix;
        MatrixTSPInstance::mutationFunction mutationsMatrix;

        for (int i = (thread - 1) * 25; i < thread * 25; i++)
        {
            if (variants[i][0] == 0)
                {
                    startingPopulationsMatrix = &MatrixTSPInstance::startingPopulation;
                }
                else if (variants[i][0] == 1)
                {
                    startingPopulationsMatrix = &MatrixTSPInstance::startingPopulationTwo;
                }
                else //if (variants[i][0] == 2)
                {
                    startingPopulationsMatrix = &MatrixTSPInstance::startingPopulationThree;
                }

                if (variants[i][1] == 0)
                {
                    getCrossoverPairsMatrix = &MatrixTSPInstance::crossoverPairs;
                }
                else //if (variants[i][1] == 1)
                {
                    getCrossoverPairsMatrix = &MatrixTSPInstance::crossoverPairsTwo;
                }
                
                if (variants[i][2] == 0)
                {
                    mutationsMatrix = &MatrixTSPInstance::mutation;
                }
                else if (variants[i][2] == 1)
                {
                    mutationsMatrix = &MatrixTSPInstance::mutationTwo;
                }
                else //if (variants[i][2] == 2)
                {
                    mutationsMatrix = &MatrixTSPInstance::mutationThree;
                }

                if (variants[i][3] == 0)
                {
                    crossoverFunctionsMatrix = &MatrixTSPInstance::crossover;
                }
                else if (variants[i][3] == 1)
                {
                    crossoverFunctionsMatrix = &MatrixTSPInstance::crossoverTwo;
                }
                else //if (variants[i][3] == 2)
                {
                    crossoverFunctionsMatrix = &MatrixTSPInstance::crossoverThree;
                }

                if (variants[i][4] == 0)
                {
                    selectionFunctionsMatrix = &MatrixTSPInstance::selection;
                }
                else //if (variants[i][4] == 1)
                {
                    selectionFunctionsMatrix = &MatrixTSPInstance::selectionTwo;
                }

            // matrixInstance.solveGenetic(CLOCKS_PER_SEC * 60, 123, false, 100, 15, 100, startingPopulationsMatrix[variants[i][0]],  getCrossoverPairsMatrix[variants[i][1]],
            //     mutationsMatrix[variants[i][2]], crossoverFunctionsMatrix[variants[i][3]], selectionFunctionsMatrix[variants[i][4]]);
            matrixInstance.solveGenetic(CLOCKS_PER_SEC * 60, 123, false, 40, 15, 40, startingPopulationsMatrix, getCrossoverPairsMatrix,
                    mutationsMatrix, crossoverFunctionsMatrix, selectionFunctionsMatrix);
                
            std::cout << matrixInstance.objectiveFunction() << ",";
            // std::cout << variants[i][0] << "," << variants[i][1] << "," << variants[i][2] << "," << variants[i][3] << "," << variants[i][4] << std::endl;
        }

        std::cout << std::endl;
        matrixInstance.deleteInstance();
    }

    

    // matrixInstance.randomInstance(123, 70, false);
    // matrixInstance.solveGenetic(CLOCKS_PER_SEC * 60, 123, true, 100, 15, 100, startingPopulationsMatrix[2], getCrossoverPairsMatrix[0],
    //         mutationsMatrix[2], crossoverFunctionsMatrix[2], selectionFunctionsMatrix[0]);

    // std::cout << "Dla porównania 2-opt: \n";
    // matrixInstance.solve2Opt();
    // std::cout << matrixInstance.objectiveFunction() << std::endl;

    // std::cout << "Dla porównania Krandom(cityCount): \n";
    // matrixInstance.solveKRandom(matrixInstance.getCityCount(), 123);
    // std::cout << matrixInstance.objectiveFunction() << std::endl;


    return 0;
}