#include <morph/Visual.h>
#include <morph/GraphVisual.h>
#include <morph/vVector.h>
#include <morph/HdfData.h>

#include <vector>
#include <chrono>
#include <thread>

#include <cstring>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <climits>

#define CITY_LIMIT 150
#define BUFSIZE 2048

class EuclideanTSPInstance 
{
    private:
        unsigned cityCount;
        std::pair<int, int>* cities;
        morph::vVector<unsigned> solution;
        //ms; around 17ms are added to wait for user input
        unsigned targetVisualizationDelay = 300;
        unsigned max2OptIterations = 1000;

    public:
        unsigned getCityCount()
        {
            return cityCount;
        }

        void setTargetVisualizationDelay(unsigned delay)
        {
            targetVisualizationDelay = delay;
        }

        void setMax2OptIterations(unsigned iterations)
        {
            max2OptIterations = iterations;
        }

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

        void visualizeSolution(bool repeat)
        {
            if (solution.size())
            {
                morph::Visual v(600, 400, "TSP solution visualized (press \"x\" to exit)");
        
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
                gv->policy = morph::stylepolicy::both; // markers, lines, both, allcolour

                gv->setdata(cordFirsts, cordSeconds);
                gv->finalize();

                v.addVisualModel(gv);

                std::chrono::milliseconds timespan(targetVisualizationDelay);

                while (!v.readyToFinish) 
                {
                    gv->clear();
                    cordFirsts.clear();
                    cordSeconds.clear();

                    for (unsigned long i = 0; i < solution.size(); i++)
                    {
                        if (v.readyToFinish)
                        {
                            break;
                        }
                        // xFirsts.push_back(x[i].first);
                        // xSeconds.push_back(x[i].second);
                    

                        gv->append(cities[solution[i]].first, cities[solution[i]].second, 0);

                        glfwWaitEventsTimeout (0.01667); // 16.67 ms ~ 60 Hz
                        std::this_thread::sleep_for(timespan);

                        v.render();
                    }
                    
                    //draw edge from last to first
                    gv->append(cities[solution[0]].first, cities[solution[0]].second, 0);
                    v.render();
                    
                    if (!repeat)
                    {
                        v.keepOpen();
                        break;
                    }

                    glfwWaitEventsTimeout (0.01667); // 16.67 ms ~ 60 Hz
                    std::this_thread::sleep_for(timespan * 100);
                }
            }
        }

        int citiesDistance(std::pair<int, int> first, std::pair<int, int> second)
        {
            return round(sqrt((first.first - second.first) * (first.first - second.first) + (first.second - second.second) * (first.second - second.second)));
        }

        int objectiveFunction()
        {
            int result = 0;
            for (long unsigned i = 0; i < solution.size() - 1; i++)
            {
                result += citiesDistance(cities[solution[i]], cities[solution[i + 1]]);
            }
            result += citiesDistance(cities[solution[solution.size() - 1]], cities[solution[0]]);
            
            return result;
        }

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

        // na 5.5 - zrównoleglenie czegoś (za proste k -random, n -wierzchołków startowych nearest - n wątków, 
        // 2 opt - każdy wątek liczy inne inverty, w czasie stałym musi wiedzieć, które inverty ma robić)
        // 3 opt, wybieranie obydwu zremisowanych dla najbliższego sąsiada - wystarczy rekurencja, 
        // 3 opt - rozcinamy 3 krawędzie i testujemy wszystkie możliwe naprawienia tej trasy
        // stały czas z akceleracją

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
                    int dist = citiesDistance(cities[i], cities[currentCity]);
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
                        int dist = citiesDistance(cities[i], cities[currentCity]);
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

                        int costDifference = citiesDistance(cities[solution[i]], cities[solution[j]]); 
                        costDifference += citiesDistance(cities[solution[j + 1]], cities[solution[i + 1]]);
                        costDifference -= citiesDistance(cities[solution[j]], cities[solution[j + 1]]);
                        costDifference -= citiesDistance(cities[solution[i]], cities[solution[i + 1]]);
                        
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
                    int costDifference = citiesDistance(cities[solution[i]], cities[solution[j]]); 
                    costDifference += citiesDistance(cities[solution[j + 1]], cities[solution[0]]);
                    costDifference -= citiesDistance(cities[solution[j]], cities[solution[j + 1]]);
                    costDifference -= citiesDistance(cities[solution[i]], cities[solution[0]]);

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

        void loadSerialized(const char* fileName, bool withSolution)
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

        void saveSerialized(const char* fileName, bool withSolution)
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
