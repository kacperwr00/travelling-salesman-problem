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

    std::vector<morph::vVector<unsigned>> (EuclideanTSPInstance::*startingPopulationsEucl[3])(unsigned);
    std::vector<std::pair<unsigned, unsigned>> (EuclideanTSPInstance::*getCrossoverPairsEucl[2])(const unsigned, const unsigned, const unsigned, const long, const int, std::set<std::pair<int, morph::vVector<unsigned>>>);
    std::pair<int, morph::vVector<unsigned>> (EuclideanTSPInstance::*crossoverFunctionsEucl[3])(std::pair<int, morph::vVector<unsigned>>, std::pair<int, morph::vVector<unsigned>>);
    std::set<std::pair<int, morph::vVector<unsigned>>> (EuclideanTSPInstance::*selectionFunctionsEucl[2])(std::set<std::pair<int, morph::vVector<unsigned>>>, unsigned);
    std::pair<int, morph::vVector<unsigned>> (EuclideanTSPInstance::*mutationsEucl[3])(const std::pair<int, morph::vVector<unsigned>>);
    
    startingPopulationsEucl[0] = &EuclideanTSPInstance::startingPopulation;
    startingPopulationsEucl[1] = &EuclideanTSPInstance::startingPopulationTwo;
    startingPopulationsEucl[2] = &EuclideanTSPInstance::startingPopulationThree;

    getCrossoverPairsEucl[0] = &EuclideanTSPInstance::crossoverPairs;
    getCrossoverPairsEucl[1] = &EuclideanTSPInstance::crossoverPairsTwo;

    crossoverFunctionsEucl[0] = &EuclideanTSPInstance::crossover;
    crossoverFunctionsEucl[1] = &EuclideanTSPInstance::crossoverTwo;
    crossoverFunctionsEucl[2] = &EuclideanTSPInstance::crossoverThree;

    selectionFunctionsEucl[0] = &EuclideanTSPInstance::selection;
    selectionFunctionsEucl[1] = &EuclideanTSPInstance::selectionTwo;

    mutationsEucl[0] = &EuclideanTSPInstance::mutation;
    mutationsEucl[1] = &EuclideanTSPInstance::mutationTwo;
    mutationsEucl[2] = &EuclideanTSPInstance::mutationThree;


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
    
    // sattolo_cycle shuffle
    // srand(123);
    // for (long unsigned i = variants.size(), j; i > 1; i--)
    // {
    //     j = rand() % i;
    //     std::swap(variants[i], variants[j]);
    // }

    std::cout << "Thread: " << thread << " Total variants: " << variants.size() << std::endl;
    for (int i = 0; i < 100; i++)
    {
        std::cout << variants[i][0] << "," << variants[i][1] << "," << variants[i][2] << "," << variants[i][3] << "," << variants[i][4] << std::endl;
    }


    // std::cout << "EUCL" << std::endl;
    EuclideanTSPInstance euclInstance;
    // euclInstance.randomInstance(123, 70);
    // euclInstance.solveGenetic(CLOCKS_PER_SEC * 60, 123, true, 100, 15, 100, startingPopulationsEucl[2], getCrossoverPairsEucl[0],
    //         mutationsEucl[2], crossoverFunctionsEucl[2], selectionFunctionsEucl[0]);

    // std::cout << "Dla porównania 2-opt: \n";
    // euclInstance.solve2Opt(false);
    // std::cout << euclInstance.objectiveFunction() << std::endl;

    // std::cout << "Dla porównania Krandom(cityCount): \n";
    // euclInstance.solveKRandom(euclInstance.getCityCount(), 123, false);
    // std::cout << euclInstance.objectiveFunction() << std::endl;


    // std::cout << "MATRIX" << std::endl;
    MatrixTSPInstance matrixInstance;

    //LOSOWE PRÓBKOWANIE

    std::vector<morph::vVector<unsigned>> (MatrixTSPInstance::*startingPopulationsMatrix[3])(unsigned);
    std::vector<std::pair<unsigned, unsigned>> (MatrixTSPInstance::*getCrossoverPairsMatrix[2])(const unsigned, const unsigned, const unsigned, const long, const int, std::set<std::pair<int, morph::vVector<unsigned>>>);
    std::pair<int, morph::vVector<unsigned>> (MatrixTSPInstance::*crossoverFunctionsMatrix[3])(std::pair<int, morph::vVector<unsigned>>, std::pair<int, morph::vVector<unsigned>>);
    std::set<std::pair<int, morph::vVector<unsigned>>> (MatrixTSPInstance::*selectionFunctionsMatrix[2])(std::set<std::pair<int, morph::vVector<unsigned>>>, unsigned);
    std::pair<int, morph::vVector<unsigned>> (MatrixTSPInstance::*mutationsMatrix[3])(const std::pair<int, morph::vVector<unsigned>>);
    
    startingPopulationsMatrix[0] = &MatrixTSPInstance::startingPopulation;
    startingPopulationsMatrix[1] = &MatrixTSPInstance::startingPopulationTwo;
    startingPopulationsMatrix[2] = &MatrixTSPInstance::startingPopulationThree;

    getCrossoverPairsMatrix[0] = &MatrixTSPInstance::crossoverPairs;
    getCrossoverPairsMatrix[1] = &MatrixTSPInstance::crossoverPairsTwo;

    crossoverFunctionsMatrix[0] = &MatrixTSPInstance::crossover;
    crossoverFunctionsMatrix[1] = &MatrixTSPInstance::crossoverTwo;
    crossoverFunctionsMatrix[2] = &MatrixTSPInstance::crossoverThree;

    selectionFunctionsMatrix[0] = &MatrixTSPInstance::selection;
    selectionFunctionsMatrix[1] = &MatrixTSPInstance::selectionTwo;

    mutationsMatrix[0] = &MatrixTSPInstance::mutation;
    mutationsMatrix[1] = &MatrixTSPInstance::mutationTwo;
    mutationsMatrix[2] = &MatrixTSPInstance::mutationThree;

    long seed = 123;
    int instanceCountOfEachType = 8;
    unsigned baseCityCount = 100;

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

        for (int i = (thread - 1) * 25; i < thread * 25; i++)
        {
            euclInstance.solveGenetic(CLOCKS_PER_SEC * 60, 123, false, 100, 15, 100, startingPopulationsEucl[variants[i][0]], getCrossoverPairsEucl[variants[i][1]],
                mutationsEucl[variants[i][2]], crossoverFunctionsEucl[variants[i][3]], selectionFunctionsEucl[variants[i][4]]);
            std::cout << euclInstance.objectiveFunction() << ",";
        // std::cout << variants[i][0] << "," << variants[i][1] << "," << variants[i][2] << "," << variants[i][3] << "," << variants[i][4] << std::endl;
        }

        std::cout << std::endl;
        euclInstance.deleteInstance();
    }

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

        for (int i = (thread - 1) * 25; i < thread * 25; i++)
        {
            matrixInstance.solveGenetic(CLOCKS_PER_SEC * 60, 123, false, 100, 15, 100, startingPopulationsMatrix[variants[i][0]],  getCrossoverPairsMatrix[variants[i][1]],
                mutationsMatrix[variants[i][2]], crossoverFunctionsMatrix[variants[i][3]], selectionFunctionsMatrix[variants[i][4]]);
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