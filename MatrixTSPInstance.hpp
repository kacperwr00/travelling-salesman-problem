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
        unsigned max2OptIterations = 1000;

    public:
        // ALGORYTMY HEURYSTYCZNE:
        // k-random - losujemy k rozwiązań i bierzemy najlepsze

        // najlepiej w k -random losować tak żeby sensownie przeglądać przestrzeń rozwiązań
        // np wybrać losową permutację z rozkładem jednostajnym 

        // najbliższy sąsiad z jednego losowego miasta
        // najbliższy sąsiad n razy - raz z każdego startowego miasta
        // remis - taka sama odległość do dwóch różnych miast - a) wybieramy dowolny, 
        // b) wybieramy obydwa i liczymy osobno dla każdego z nich - możemy dostać dużo rozkrzewień i problemy obliczeniowe
        
        // 2-opt 
        // startujemy od gotowego rozwiązania, np w porzadku leksykalnym, lepiej np wynik najbliższego sąsiada, i staramy się je poprawić
        // rozcinamy dwie krawędzie i sklejamy wierzchołki odwrotnie
        // 1 2 3 4 5 -> 1 2 5 4 3
        // sprawdzamy czy zamiana poprawila sprawe
        // bierzemy wszystkie możliwe pary krawędzi w rozwiązaniu
        // w euklidesowym, symetrycznym tsp nie trzeba sprawdzać odwracania cyklicznie poza listą,
        // np nie trzeba w 2 3 4 1 odwracać po 2, 1

        // po sprawdzeniu wszystkich zaczynamy od nowa - tak długo jak coś się w jednej implementacji zmienia

        // przyspieszenie 2 opt - jedna iteracja w O(n^2), a nie O(n^3)
        // pamiętamy długość starej trasy - odejmujemy długość usuniętych krawędzi,
        // dodajemy długość dodanych, zamiast liczyć fkcji celu na nowym grafie O(1) zamiast O(n)
        // problem jak nie jest symetryczny - jeden odcinek musimy przejechać w drugą stronę
        // wtedy nie trzeba tego robić

        // na 5.5 - zrównoleglenie 2 opt, 3 opt, wybieranie obydwu zremisowanych dla najbliższego sąsiada, 

        unsigned getCityCount()
        {
            return cityCount;
        }

        void setMax2OptIterations(unsigned iterations)
        {
            max2OptIterations = iterations;
        }
        
        void solveKRandom(const unsigned K, const long seed, const bool withVisualization) 
        {
            srand(seed);
            solution.clear();
            for (unsigned i = 0; i < cityCount; i++)
            {
                solution.push_back(i);
            }

            morph::vVector<unsigned> bestSolution;
            int bestResult = INT_MAX;

            for (unsigned k = 0; k < K; k++)
            {
                //Fisher-Yates shuffle on solution
                for (unsigned i = cityCount - 1; i > 0; i--)
                {
                    int j = rand() % (i + 1);
                    unsigned tmp = solution[j];
                    solution[j] = solution[i];
                    solution[i] = tmp;
                }
                int currentResult = objectiveFunction();
                if (currentResult < bestResult)
                {
                    bestResult = currentResult;
                    bestSolution = solution;
                }
            }

            solution = bestSolution;
            std::cout << "K random result for k = " << K << ":" << bestResult << std::endl;
        }

        void solveNearestNeighboor(bool withRealtimeVisualization) 
        {
            solution.clear();
            bool* visited = (bool*)calloc(cityCount, sizeof(*visited));

            unsigned currentCity = 0;
            visited[currentCity] = true;
            unsigned visitedCount = 1;
            solution.push_back(currentCity);

            // if (withRealtimeVisualization)
            // {

            // }

            while (visitedCount != cityCount) 
            {
                int currentMinCost = INT_MAX;
                unsigned currentNearestNeighboor;

                for (unsigned i = 0; i < cityCount; i++)
                {
                    if (i == currentCity || visited[i])
                    {
                        continue;
                    }
                    int dist = cities[i][currentCity];
                    if (dist < currentMinCost)
                    {
                        currentMinCost = dist;
                        currentNearestNeighboor = i;
                    }
                }
                visitedCount++;
                visited[currentNearestNeighboor] = true;
                // std::cout << "Pushing " << currentNearestNeighboor << std::endl; 

                solution.push_back(currentNearestNeighboor);
                currentCity = currentNearestNeighboor;
            }

            free(visited);
            std::cout << "Nearest Neighboor result: " << objectiveFunction() << std::endl;
        }

        void solveNNearestNeighboor(bool withVisualization) 
        {
            morph::vVector<unsigned> bestSolution;
            int bestResult = INT_MAX;
            bool* visited = (bool*)malloc(cityCount * sizeof(*visited));

            for (unsigned k = 0; k < cityCount; k++)
            {
                solution.clear();
                
                for (unsigned i = 0; i < cityCount; i++)
                {
                    visited[i] = false;            
                }

                unsigned currentCity = k;
                visited[currentCity] = true;
                unsigned visitedCount = 1;
                solution.push_back(currentCity);


                // if (withRealtimeVisualization)
                // {

                // }

                while (visitedCount != cityCount) 
                {
                    int currentMinCost = INT_MAX;
                    unsigned currentNearestNeighboor;

                    for (unsigned i = 0; i < cityCount; i++)
                    {
                        if (i == currentCity || visited[i])
                        {
                            continue;
                        }
                        int dist = cities[i][currentCity];
                        if (dist < currentMinCost)
                        {
                            currentMinCost = dist;
                            currentNearestNeighboor = i;
                        }
                    }
                    visitedCount++;
                    visited[currentNearestNeighboor] = true;
                    // std::cout << "Pushing " << currentNearestNeighboor << std::endl; 

                    solution.push_back(currentNearestNeighboor);
                    currentCity = currentNearestNeighboor;
                }

                int currentResult = objectiveFunction();
                if (currentResult < bestResult)
                {
                    bestResult = currentResult;
                    bestSolution = solution;
                }
            }

            free(visited);
            solution = bestSolution;
            std::cout << "NNearest Neighboor result: " << bestResult << std::endl;
        }

        void solve2Opt(bool withVisualization) 
        {
            // nie trzeba się martwić invertami traktującymi solution cyklicznie - bo problem symetryczny
            solveNearestNeighboor(false);

            std::cout << "Before inverts: " << objectiveFunction() << std::endl; 

            int currentCost = objectiveFunction();

            if (currentCost == 0)
            {
                return;
            }

            bool changes = true;
            unsigned iteration = 0;

            while (changes && ++iteration <= max2OptIterations)
            {
                changes = false;
                for (unsigned i = 1; i < cityCount - 1; i++)
                {
                    for (unsigned j = 0; j < i; j++)
                    {
                        //rozerwij i-tą oraz j-tą krawędź
                        //sklej po odwróceniu jeśli lepiej

                        int costDifference = cities[solution[i]][solution[j]]; 
                        costDifference += cities[solution[j + 1]][solution[i + 1]];
                        costDifference -= cities[solution[j]][solution[j + 1]];
                        costDifference -= cities[solution[i]][solution[i + 1]];
                        
                        if (costDifference < 0)
                        {
                            changes = true;
                            for (unsigned m = 1; m < (i - j) / 2; m++)
                            {
                                unsigned tmp = solution[j + m];
                                solution[j + m] = solution[i - (m - 1)];
                                solution[i - (m - 1)] = tmp;
                            }
                        }
                    }
                }

                //handle i = cityCount - 1 separately to have less branches / mods
                unsigned i = cityCount - 1;
                for (unsigned j = 0; j < i; j++)
                {
                    int costDifference = cities[solution[i]][solution[j]]; 
                    costDifference += cities[solution[j + 1]][solution[0]];
                    costDifference -= cities[solution[j]][solution[j + 1]];
                    costDifference -= cities[solution[i]][solution[0]];

                    if (costDifference < 0)
                    {
                        changes = true;
                        for (unsigned m = 1; m < (i - j) / 2; m++)
                        {
                            unsigned tmp = solution[j + m];
                            solution[j + m] = solution[i - (m - 1)];
                            solution[i - (m - 1)] = tmp;
                        }
                    }
                }
            }

            std::cout << "After inverts: " << objectiveFunction() << std::endl; 
        }

        int objectiveFunction()
        {
            int result = 0;
            for (unsigned long i = 0; i < solution.size() - 1; i++)
            {
                result += cities[solution[i]][solution[i + 1]];
            }
            result += cities[solution[solution.size() - 1]][solution[0]];
            
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
