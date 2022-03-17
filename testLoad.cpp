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
    eucInstance.loadSerialized("eucTest.h5", false);
    eucInstance.printCities();
    matInstance.loadSerialized("matTest.h5", false);
    matInstance.printCities();

    eucInstance.deleteInstance();
    matInstance.deleteInstance();

    return 0;
}