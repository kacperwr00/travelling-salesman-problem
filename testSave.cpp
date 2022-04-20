#include "EuclideanTSPInstance.hpp"
#include "MatrixTSPInstance.hpp"

namespace util
{
    const auto firstElement = [](auto const& pair) -> auto const& { return pair.first; };
    const auto secondElement = [](auto const& pair) -> auto const& { return pair.second; };
}

enum problemType {Euclidean, DistanceMatrix};


int main()
{
    EuclideanTSPInstance eucInstance;
    MatrixTSPInstance matInstance;
    eucInstance.randomInstance(4, 0);
//    eucInstance.loadTSPLIB("../ALL_tsp/a280.tsp");
    eucInstance.printCities();
    matInstance.randomInstance(4, 0, false);
//    matInstance.loadTSPLIB("../ALL_tsp/bays29.tsp");
    matInstance.printCities();

    eucInstance.saveSerialized("eucTest.h5", false);
    matInstance.saveSerialized("matTest.h5", false);

    eucInstance.deleteInstance();
    matInstance.deleteInstance();

    return 0;
}