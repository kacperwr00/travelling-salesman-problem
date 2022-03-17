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

class MatrixTSPInstance 
{
    private:
        unsigned cityCount;
        int** cities;
        morph::vVector<unsigned> solution;

    public:
        void solve(bool withVisualization) 
        {
            std::cout << "Solving" << std::endl; 
        }

        int objectiveFunction()
        {
            int result = 0;
            for (unsigned long i = 0; i < solution.size() - 1; i++)
            {
                result += cities[solution[i]][solution[i + 1]];
            }
            return result;
        }

        void loadTSPLIB(const char* fileName) 
        {
            FILE* file = fopen(fileName, "r");

            bool symmetric = false;

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
                if (strcmp(token, "EDGE_WEIGHT_FORMAT") == 0)
                {
                    char* tmp = strtok(NULL, " :");
                    if (!strstr(tmp, "LOWER_DIAG_ROW") && !strstr(tmp, "FULL_MATRIX"))
                    {
                        std::cout << "Wrong EDGE_WEIGHT_FORMAT: " << tmp << std::endl;
                        return;
                    }
                    if (strstr(tmp, "LOWER_DIAG_ROW"))
                    {
                        symmetric = true;
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

                    cities = (int**)malloc(sizeof(*cities) * cityCount);
                    for (unsigned i = 0; i < cityCount; i++)
                    {
                        cities[i] = (int*)malloc(sizeof(**cities) * cityCount);
                    }
                }
                else if (strstr(token, "EDGE_WEIGHT_SECTION"))
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
                            int tmp = atoi(strtok(buffer, " :"));

                            for (unsigned j = 0; j < cityCount - 1; j++)
                            {
                                cities[i][j] = tmp;
                                if (symmetric)
                                {
                                    cities[j][i] = tmp;
                                    if (i == j)
                                    {
                                        break;
                                    }
                                }

                                tmp = atoi(strtok(NULL, " :"));
                            }

                            cities[i][cityCount - 1] = tmp;
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

        void loadSerialized(const char* fileName, bool withSolution)
        {
            morph::HdfData data(fileName, morph::FileAccess::ReadOnly);
            std::string string;
            data.read_string("/type", string);
            if (string.compare("LOWER_DIAG_ROW") == 0 || string.compare("FULL_MATRIX") == 0)
            {
                data.read_val("/cityCount", cityCount);
                
                cities = (int**)malloc(sizeof(*cities) * cityCount);
                for (unsigned i = 0; i < cityCount; i++)
                {
                    cities[i] = (int*)malloc(sizeof(**cities) * cityCount);
                }

                for (unsigned i = 0; i < cityCount; i++)
                {
                    for (unsigned j = 0; j < cityCount; j++)   
                    { 
                        data.read_val(("/cities" + std::to_string(i) + "," + std::to_string(j)).c_str(), cities[i][j]);
                    }
                }
                if (withSolution) 
                {
                    data.read_contained_vals("/solution", solution);
                }
            }
        }

        void saveSerialized(const char* fileName, bool withSolution)
        {
            morph::HdfData data(fileName, morph::FileAccess::TruncateWrite);
            if (cities)
            {
                data.add_string("/type", "FULL_MATRIX");
                data.add_val("cityCount", cityCount);
                for (unsigned i = 0; i < cityCount; i++)
                {
                    for (unsigned j = 0; j < cityCount; j++)   
                    { 
                        data.add_val(("/cities" + std::to_string(i) + "," + std::to_string(j)).c_str(), cities[i][j]);
                    }
                }
                if (withSolution)
                {
                    data.add_contained_vals("/solution", solution);  
                }
            }
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

            cities = (int**)malloc(sizeof(*cities) * cityCount);
            for (unsigned i = 0; i < cityCount; i++)
            {
                cities[i] = (int*)malloc(sizeof(**cities) * cityCount);
            }

            for (unsigned i = 0; i < cityCount; i++)
            {
                for (unsigned j = 0; j < cityCount; j++)
                {
                    if (i == j)
                    {
                        cities[i][i] = 0;
                        if (symmetric)
                        {
                            break;
                        }
                        continue;
                    }
                    cities[i][j] = rand() % (cityCount * 3);
                }
            }
        }

        void printCities()
        {
            for(unsigned i = 0; i < cityCount; i++)
            {
                for(unsigned j = 0; j < cityCount; j++)
                {
                    std::cout << cities[i][j]  << " ";
                }
                std::cout << std::endl;
            }
        }

        void deleteInstance()
        {
            if (cities)
            {
                // cities = (int**)malloc(sizeof(*cities) * cityCount);
                // for (unsigned i = cityCount; i > 0; i--)
                // {
                //     delete[](cities[i]);
                // }
                // while (*cities) 
                // {
                //     auto city = *cities++;
                //     free(city);
                // }
                free(cities);
            }
        }
};
