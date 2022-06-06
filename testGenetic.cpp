#include "TabuList.hpp"
#include "MatrixTSPInstance.hpp"
#include "EuclideanTSPInstance.hpp"

int main()
{
    std::cout << "EUCL" << std::endl;
    EuclideanTSPInstance euclInstance;
    euclInstance.randomInstance(123, 70);
    euclInstance.solveGenetic(CLOCKS_PER_SEC * 30, 123, true, 10, 3, 10, &EuclideanTSPInstance::startingPopulation, &EuclideanTSPInstance::crossoverPairs, &EuclideanTSPInstance::mutation, 
        &EuclideanTSPInstance::crossover, &EuclideanTSPInstance::selection);

    std::cout << "Dla porównania 2-opt: \n";
    euclInstance.solve2Opt(false);
    std::cout << euclInstance.objectiveFunction() << std::endl;

    std::cout << "Dla porównania Krandom(cityCount): \n";
    euclInstance.solveKRandom(euclInstance.getCityCount(), 123, false);
    std::cout << euclInstance.objectiveFunction() << std::endl;

    euclInstance.deleteInstance();

    std::cout << "MATRIX" << std::endl;
    MatrixTSPInstance matrixInstance;

    //LOSOWE PRÓBKOWANIE

    std::vector<morph::vVector<unsigned>> (MatrixTSPInstance::*startingPopulationsMatrix[3])(unsigned);
    std::vector<std::pair<unsigned, unsigned>> (MatrixTSPInstance::*getCrossoverPairsMatrix[2])(const unsigned, const unsigned, const unsigned, const long, const int, std::set<std::pair<int, morph::vVector<unsigned>>>);
    std::pair<int, morph::vVector<unsigned>> (MatrixTSPInstance::*crossoverFunctionsMatrix[3])(std::pair<int, morph::vVector<unsigned>>, std::pair<int, morph::vVector<unsigned>>);
    std::set<std::pair<int, morph::vVector<unsigned>>> (MatrixTSPInstance::*selectionFunctionsMatrix[2])(std::set<std::pair<int, morph::vVector<unsigned>>>, unsigned);
    std::pair<int, morph::vVector<unsigned>> (MatrixTSPInstance::*mutationsMatrix[3])(const std::pair<int, morph::vVector<unsigned>>);
    
    startingPopulationsMatrix[0] = &MatrixTSPInstance::startingPopulation;
    startingPopulationsMatrix[1] = &MatrixTSPInstance::startingPopulationTwo;
    // startingPopulationsMatrix[2] = &MatrixTSPInstance::startingPopulationThree;

    getCrossoverPairsMatrix[0] = &MatrixTSPInstance::crossoverPairs;
    // crossoverSelectionFunctionsMatrix[1] = &MatrixTSPInstance::crossoverSelectionTwo;

    crossoverFunctionsMatrix[0] = &MatrixTSPInstance::crossover;
    // crossoverFunctionsMatrix[1] = &MatrixTSPInstance::crossoverTwo;
    // crossoverFunctionssMatrix[2] = &MatrixTSPInstance::crossoverThree;

    selectionFunctionsMatrix[0] = &MatrixTSPInstance::selection;
    // selectionFunctionsMatrix[1] = &MatrixTSPInstance::selectionTwo;

    mutationsMatrix[0] = &MatrixTSPInstance::mutation;
    // mutationsMatrix[1] = &MatrixTSPInstance::mutationTwo;
    // mutationsMatrix[2] = &MatrixTSPInstance::mutationThree;

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
    }
    std::cout << "Total variants count: " << variants.size() << std::endl;
    
    // sattolo_cycle shuffle
    srand(123);
    for (long unsigned i = variants.size(), j; i > 1; i--)
    {
        j = rand() % i;
        std::swap(variants[i], variants[j]);
    }

    for (long unsigned i = 0; i < variants.size(); i++)
    {
        // std::cout << "Variant number " << i << ": " << variants[i][0] << " " << variants[i][1] << " " << variants[i][2] << " "  << variants[i][3] << " " << variants[i][4] << std::endl;
    }

    // for (int i = 0; i < 100; i++)
    // {
    //     matrixInstance.solveGenetic(CLOCKS_PER_SEC * 60, 123, false, 100, 15, 100, startingPopulationsMatrix[variants[i][0]],  getCrossoverPairsMatrix[0],
    //         mutationsMatrix[variants[i][2]], crossoverFunctionsMatrix[variants[i][3]], selectionFunctionsMatrix[variants[i][4]]);
    // }

    matrixInstance.randomInstance(123, 70, false);
    matrixInstance.solveGenetic(CLOCKS_PER_SEC * 60, 123, true, 100, 15, 100, startingPopulationsMatrix[0], getCrossoverPairsMatrix[0],
            mutationsMatrix[0], crossoverFunctionsMatrix[0], selectionFunctionsMatrix[0]);

    std::cout << "Dla porównania 2-opt: \n";
    matrixInstance.solve2Opt();
    std::cout << matrixInstance.objectiveFunction() << std::endl;

    std::cout << "Dla porównania Krandom(cityCount): \n";
    matrixInstance.solveKRandom(matrixInstance.getCityCount(), 123);
    std::cout << matrixInstance.objectiveFunction() << std::endl;

    matrixInstance.deleteInstance();

    return 0;
}