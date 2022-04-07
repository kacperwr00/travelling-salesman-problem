#include "EuclideanTSPInstance.hpp"
#include "MatrixTSPInstance.hpp"

namespace util
{
    const auto firstElement = [](auto const& pair) -> auto const& { return pair.first; };
    const auto secondElement = [](auto const& pair) -> auto const& { return pair.second; };
}

enum problemType {Euclidean, DistanceMatrix};

void algorithmsTest(bool N)
{
    if (N)
    {
        std::cout << "test results with NNearest Neighboor as time limit: " << std::endl;
    }
    else
    {
        std::cout << "test results with Nearest Neighboor as time limit: " << std::endl;
    }
    
    EuclideanTSPInstance euclInstance;
    MatrixTSPInstance matInstance;

    euclInstance.loadTSPLIB("../ALL_tsp/a280.tsp");
    if (N)
    {
        euclInstance.testAlgorithmsNForSetInstance("a280.tsp");
    }
    else 
    {
        euclInstance.testAlgorithmsForSetInstance("a280.tsp");
    }

    euclInstance.randomInstance(clock(), 50);
    if (N)
    {
        euclInstance.testAlgorithmsNForSetInstance("randomEucl - 50 cities");
    }
    else 
    {
        euclInstance.testAlgorithmsForSetInstance("randomEucl - 50 cities");
    }

    euclInstance.randomInstance(clock(), 100);
    if (N)
    {
        euclInstance.testAlgorithmsNForSetInstance("randomEucl - 100 cities");
    }
    else 
    {
        euclInstance.testAlgorithmsForSetInstance("randomEucl - 100 cities");
    }
    
    euclInstance.randomInstance(clock(), 150);
    if (N)
    {
        euclInstance.testAlgorithmsNForSetInstance("randomEucl - 150 cities");
    }
    else 
    {
        euclInstance.testAlgorithmsForSetInstance("randomEucl - 150 cities");
    }
    
    euclInstance.randomInstance(clock(), 200);
    if (N)
    {
        euclInstance.testAlgorithmsNForSetInstance("randomEucl - 200 cities");
    }
    else 
    {
        euclInstance.testAlgorithmsForSetInstance("randomEucl - 200 cities");
    }
    
    //non-symmetric
    matInstance.loadTSPLIB("../ALL_tsp/bays29.tsp");
    if (N)
    {
        matInstance.testAlgorithmsNForSetInstance("bays29.tsp");
    }
    else 
    {
        matInstance.testAlgorithmsForSetInstance("bays29.tsp");
    }

    //symmetric
    matInstance.loadTSPLIB("../ALL_tsp/dantzig42.tsp");
    if (N)
    {
        matInstance.testAlgorithmsNForSetInstance("dantzig42.tsp");
    }
    else 
    {
        matInstance.testAlgorithmsForSetInstance("dantzig42.tsp");
    }

    matInstance.randomInstance(clock(), 50, true);
    if (N)
    {
        matInstance.testAlgorithmsNForSetInstance("randomMatSym - 50 cities");
    }
    else 
    {
        matInstance.testAlgorithmsForSetInstance("randomMatSym - 50 cities");
    }

    matInstance.randomInstance(clock(), 100, false);
    if (N)
    {
        matInstance.testAlgorithmsNForSetInstance("randomMatNonSym - 100 cities");
    }
    else 
    {
        matInstance.testAlgorithmsForSetInstance("randomMatNonSym - 100 cities");
    }

    matInstance.randomInstance(clock(), 150, true);
    if (N)
    {
        matInstance.testAlgorithmsNForSetInstance("randomMatSym - 150 cities");
    }
    else 
    {
        matInstance.testAlgorithmsForSetInstance("randomMatSym - 150 cities");
    }
    matInstance.deleteInstance();
    euclInstance.deleteInstance();
}

int main()
{
    algorithmsTest(false);
    algorithmsTest(true);

    return 0;
}