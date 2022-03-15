#include <morph/Visual.h>
#include <morph/GraphVisual.h>
#include <morph/vVector.h>
#include <morph/HdfData.h>

#include <vector>
#include <chrono>
#include <thread>

#include <time.h>
#include <stdlib.h>

#define CITY_LIMIT 150

class EuclideanTSPInstance 
{
    private:
        unsigned cityCount;
        std::pair<int, int>** cities;
        unsigned* solution;

    public:
        void visualize() 
        {
            std::cout << "Visualizing" << std::endl; 
        }

        void solve(bool withVisualization) 
        {
            std::cout << "Solving" << std::endl; 
        }

        void loadTSPLIB(char* fileName) 
        {
            std::cout << "Loading from TSPLIB" << std::endl; 
        }

        void loadSerialized(char* fileName)
        {

        }

        void saveSerialized(char* fileName)
        {
            
        }

        // Use cityCount = 0 to generate a random amount from 2 to CITY_LIMIT
        void randomInstance(long seed, unsigned cityCountarg, bool symmetric)
        {
            srand(seed);
            if (cityCountarg == 0)
            {
                cityCountarg = rand() % (CITY_LIMIT - 1) + 2;
            }

            cityCount = cityCountarg;

            cities = (std::pair<double, double>**)malloc(sizeof(*cities) * cityCount);
            for (unsigned i = 0; i < cityCount; i++)
            {
                cities[i] = (std::pair<double, double>*)malloc(sizeof(**cities) * cityCount);
            }

            for (unsigned i = 0; i < cityCount; i++)
            {
                for (unsigned j = 0; j < cityCount; j++)
                {
                    std::pair<int, int> tmpCoords = std::make_pair<int, int>(3, 4);
                }
            }
        }

        void deleteInstance()
        {
            if (solution)
            {
                free(solution);
            }
            if (cities)
            {
                while (*cities) 
                {
                    auto city = *cities++;
                    free(city);
                }
                free(cities);
            }
        }
};
