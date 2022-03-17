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
    // instance.randomInstance(4, 0);
    eucInstance.loadTSPLIB("/home/kacper/semestr6/metaheurystyczne/ALL_tsp/a280.tsp");
    eucInstance.printCities();
    matInstance.loadTSPLIB("/home/kacper/semestr6/metaheurystyczne/ALL_tsp/bays29.tsp");
    matInstance.printCities();

    eucInstance.saveSerialized("eucTest.h5", false);
    matInstance.saveSerialized("matTest.h5", false);

    eucInstance.deleteInstance();
    matInstance.deleteInstance();

    return 0;
}