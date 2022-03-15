#include <morph/Visual.h>
#include <morph/GraphVisual.h>
#include <morph/vVector.h>
#include <morph/HdfData.h>

#include <vector>
#include <chrono>
#include <thread>

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define CITY_LIMIT 150
#define BUFSIZE 2048

class EuclideanTSPInstance 
{
    private:
        unsigned cityCount;
        std::pair<int, int>* cities;
        morph::vVector<unsigned> solution;

    public:
        void visualizeInstance() 
        {
            if (cities)
            {
                morph::Visual v(600, 400, "TSP euclidean instance visualized (press \"x\" to exit)");
    
                auto gv = new morph::GraphVisual<double> (v.shaderprog, v.tshaderprog, {-1.6,-1,0});

                double xMax = 0.0, xMin = 0.0, yMax = 0.0, yMin = 0.0;
                morph::vVector<double> cordFirsts, cordSeconds; 

                for (unsigned i = 0; i < cityCount; i++)
                {
                    if (cities[i].first > xMax)
                    {
                        xMax = cities[i].first;
                    }
                    else if (cities[i].first < xMin)
                    {
                        xMin = cities[i].first;
                    } 
                    if (cities[i].second > yMax)
                    {
                        yMax = cities[i].second;
                    }
                    else if (cities[i].second < yMin)
                    {
                        yMin = cities[i].second;
                    }
                    cordFirsts.push_back(cities[i].first);
                    cordSeconds.push_back(cities[i].second);
                }

                gv->setsize(3.5, 2.2);
                gv->setlimits(1.1 * xMin, 1.1 * xMax, 1.1 * yMin, 1.1 * yMax);
                gv->policy = morph::stylepolicy::markers; // markers, lines, both, allcolour

                gv->setdata(cordFirsts, cordSeconds);
                gv->finalize();

                v.addVisualModel(gv);
                v.keepOpen();

                return;
            }
            std::cout << "Error: Can't Visualize - Instance still hasn't been instantiated" << std::endl; 
        }

        int objectiveFunction()
        {
            return -1;
        }

        void solve(bool withVisualization) 
        {
            std::cout << "Solving" << std::endl; 
        }

        void loadTSPLIB(const char* fileName) 
        {
            FILE* file = fopen(fileName, "r");

            if (file == NULL)
            {
                std::cout << "Error loading file" << std::endl;
                return;
            }

            if (feof(file))
            {
                std::cout << "File finished" << std::endl;
                return;
            }

            char buffer[BUFSIZE];
            
            while(fgets(buffer, BUFSIZE, file))
            {
                char* token = strtok(buffer, " :");
                if (strcmp(token, "EDGE_WEIGHT_TYPE") == 0)
                {
                    char* tmp = strtok(NULL, " :");
                    if (!strstr(tmp, "EUC_2D"))
                    {
                        std::cout << "Wrong EDGE_WEIGHT_TYPE: " << tmp << std::endl;
                        return;
                    }
                }
                else if (strcmp(token, "DIMENSION") == 0)
                {
                    char* tmp;
                    if ((tmp = strtok(NULL, " :")) != NULL)
                    {
                        cityCount = atoi(tmp);
                    }
                    else
                    {
                        std::cout << "PANIC: null ptr" << std::endl;
                        return;
                    }

                    cities = (std::pair<int, int>*)malloc(sizeof(*cities) * cityCount);
                }
                else if (strstr(token, "NODE_COORD_SECTION"))
                {
                    if (cities)
                    {
                        for (unsigned i = 0; i < cityCount; i++)
                        {
                            if (!fgets(buffer, BUFSIZE, file))
                            {
                                std::cout << "Error: Found fewer cities then expected" << std::endl;
                                return;
                            }
                            strtok(buffer, " :");
                            char* tmp1 = strtok(NULL, " :");
                            char* tmp2 = strtok(NULL, " :");

                            cities[i] = std::make_pair<int, int>(atoi(tmp1), atoi(tmp2));
                        }
                        std::cout << "TSPLIB import successful" << std::endl;
                        return;
                    }
                    else 
                    {
                        std::cout << "Error: city count unknown" << std::endl;
                        return;
                    }
                }
            }
            fclose(file);
        }

        void loadSerialized(char* fileName, bool withSolution)
        {
            morph::HdfData data(fileName, morph::FileAccess::ReadOnly);
            std::string string;
            data.read_string("/type", string);
            if (string.compare("Eucl2D") == 0)
            {
                data.read_val("/cityCount", cityCount);
                cities = (std::pair<int, int>*)malloc(sizeof(*cities) * cityCount);

                for (unsigned i = 0; i < cityCount; i++)
                {
                    data.read_contained_vals(("/cities/" + std::to_string(i)).c_str(), cities[i]);
                }
                if (withSolution) 
                {
                    data.read_contained_vals("/solution", solution);
                }
            }
        }

        void saveSerialized(char* fileName, bool withSolution)
        {
            morph::HdfData data(fileName, morph::FileAccess::TruncateWrite);
            if (cities)
            {
                data.add_string("/type", "Eucl2D");
                data.add_val("cityCount", cityCount);
                for (unsigned i = 0; i < cityCount; i++)
                {
                    data.add_contained_vals(("/cities/" + std::to_string(i)).c_str(), cities[i]);
                }
                if (withSolution)
                    data.add_contained_vals("/solution", solution);  
            }
        }

        // Use cityCount = 0 to generate a random amount from 2 to CITY_LIMIT
        void randomInstance(long seed, unsigned cityCountarg)
        {
            srand(seed);
            if (cityCountarg == 0)
            {
                cityCountarg = rand() % (CITY_LIMIT - 1) + 2;
            }

            cityCount = cityCountarg;

            cities = (std::pair<int, int>*)malloc(sizeof(*cities) * cityCount);

            for (unsigned i = 0; i < cityCount; i++)
            {
                cities[i] = std::make_pair<int, int>(rand() % cityCount, rand() % cityCount);
            }
        }

        void deleteInstance()
        {
            if (cities)
            {
                free(cities);
            }
        }

        void printCities()
        {
            for(unsigned i = 0; i < cityCount; i++)
            {
                std::cout << cities[i].first << ", " << cities[i].second << std::endl;
            }
        }
};
