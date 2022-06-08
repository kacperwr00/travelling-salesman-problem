#pragma once

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
#include <stack>
#include "TabuList.hpp"

#define CITY_LIMIT 150
#define BUFSIZE 2048
#define MIN(a, b) a < b ? a : b
#define POS_MOD(i, n) ((i) % (n) + (n)) % (n)

class EuclideanTSPInstance 
{
    private:
        unsigned cityCount;
        std::pair<int, int>* cities;
        morph::vVector<unsigned> solution;
        //ms; around 17ms are added to wait for user input
        unsigned targetVisualizationDelay = 300;
        unsigned max2OptIterations = 1000;
        int** citiesCached;

        //tabu utility functions
        typedef void (EuclideanTSPInstance::*moveFunction)(unsigned i, unsigned j);
        typedef int (EuclideanTSPInstance::*measureFunction)(unsigned i, unsigned j);
        typedef std::vector<std::pair<unsigned, unsigned>> (EuclideanTSPInstance::*neighboorhoodFunction)();
        typedef void (EuclideanTSPInstance::*startingSolution)(bool visualization);


        clock_t geneticSeed = 0;
        std::mt19937 rng = std::mt19937(geneticSeed);

        //genetic utility functions
        //TODO arguments
        typedef std::vector<morph::vVector<unsigned>> (EuclideanTSPInstance::*getStartingPopulation)(unsigned);
        typedef std::vector<std::pair<unsigned, unsigned>> (EuclideanTSPInstance::*getCrossoverPairs)(const unsigned, const unsigned, const unsigned, const long, const int, std::set<std::pair<int, morph::vVector<unsigned>>>);
        typedef std::pair<int, morph::vVector<unsigned>> (EuclideanTSPInstance::*mutationFunction)(const std::pair<int, morph::vVector<unsigned>>);
        typedef std::pair<int, morph::vVector<unsigned>> (EuclideanTSPInstance::*crossoverFunction)(std::pair<int, morph::vVector<unsigned>>, std::pair<int, morph::vVector<unsigned>>);
        typedef std::set<std::pair<int, morph::vVector<unsigned>>> (EuclideanTSPInstance::*selectionFunction)(std::set<std::pair<int, morph::vVector<unsigned>>>, unsigned);

    public:
        unsigned getCityCount()
        {
            return cityCount;
        }

        void setTargetVisualizationDelay(const unsigned delay)
        {
            targetVisualizationDelay = delay;
        }

        void setMax2OptIterations(const unsigned iterations)
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

        void visualizeSolution(const bool repeat)
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

        int citiesDistance(const std::pair<int, int> first, const std::pair<int, int> second)
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


        // BADANIA TEORETYCZNE:
        // k-random: O(kn) - jeśli losowanie nie  jest bardziej obciążąjące
        // najblizszy sasiad: O(n^2) jeśli odległości mamy dostępne w O(1) można obniżyć używając drzew do O(nlogn), a może szybciej
        // dzielenie obszarów na mniejsze i większe w log liczbie krokow od najmniejszego do większego
        // 2-opt: O(n^3) w jednej iteracji bez akceleracji, iteracji O(n^2)?

        // trzeba to poszacować, nie trzeba dla równoległych

        // BADANIA EMPIRYCZNE:
        // sprawozdanie
        // wyniki do csv - wykresy w exccelu, latexu
        // błąd - (PI - PI*)/ PI* 0% - opt, 5%, pi* - wartość rozw opt, pi - naszego
        // testy statystyczne - Wilcoxon sparowany - pairwise
        // dla k-random - losowego - zrobić średnia (i ją trakrujemy jako wynik), std dev, a super- histogram ale zazwyczaj nie ma na to miejsca - dla każdej instancji
        // można go tu narysować dla jednej instancji

        // ten sam czas działania, albo liczba wywołań oracle - fkcji celu, albo zliczanie iteracji stop

        // dla wersji równoległej - wykres procesory - czas

        // 2-opt z różnymi startami

        // tabelka - instancje - algorytmy

        void timedTestKRandom(const long timeLimit, const long seed)
        {
            clock_t startingTimestamp = clock();
            srand(seed);
            solution.clear();
            for (unsigned i = 0; i < cityCount; i++)
            {
                solution.push_back(i);
            }

            morph::vVector<unsigned> bestSolution;
            int bestResult = INT_MAX;

            long currentTime = 0;

            while (currentTime < timeLimit)
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
                currentTime = clock() - startingTimestamp;
            }

            solution = bestSolution;
        }

        void timedTest2Opt(const long timeLimit)
        {
            // nie trzeba się martwić invertami traktującymi solution cyklicznie - bo problem symetryczny
            long startTimestamp = clock();
            solveKRandom(1, clock(), false);

            int currentCost = objectiveFunction();

            if (currentCost == 0)
            {
                return;
            }

            bool changes = true;

            while (changes && clock() - startTimestamp < timeLimit)
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
                            for (unsigned m = 1; m < (i - j) / 2 +1; m++)
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
                        for (unsigned m = 1; m < (i - j) / 2 + 1; m++)
                        {
                            unsigned tmp = solution[j + m];
                            solution[j + m] = solution[i - (m - 1)];
                            solution[i - (m - 1)] = tmp;
                        }
                    }
                }
            }
        }

        void timedTestNNearestNeighboor(const long timeLimit)
        {
            long startTimestamp = clock();

            morph::vVector<unsigned> bestSolution;
            int bestResult = INT_MAX;
            bool* visited = (bool*)malloc(cityCount * sizeof(*visited));

            for (unsigned k = 0; k < cityCount && clock() - startTimestamp < timeLimit && k; k++)
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
            // std::cout << "NNearest Neighboor result: " << bestResult << std::endl;
        }

        //test KRandom and 2opt with time limit set to execution time of NearestNeighboor
        void testAlgorithmsForSetInstance(const char* instanceName)
        {
            clock_t timeLimit = clock();

            solveNearestNeighboor(false);
            int nearestNeighboorObjectiveFunction = objectiveFunction();

            timeLimit = clock() - timeLimit;
            
            timedTestKRandom(timeLimit, clock());
            int KRandomObjectiveFunction = objectiveFunction();

            timedTest2Opt(timeLimit);
            int twoOptObjectiveFunction = objectiveFunction();

            printf("%s, %d, %d, %d\n", instanceName, KRandomObjectiveFunction, nearestNeighboorObjectiveFunction, twoOptObjectiveFunction);
        }

        void variant2Opt(uint8_t variant)
        {
            if (variant == 0)
            {
                solveKRandom(1, 123, false);
            }
            else if (variant == 1)
            {
                solveKRandom(100, 123, false);
            }
            else if (variant == 2)
            {
                solveKRandom(10000, 123, false);
            }
            else if (variant == 3)
            {
                solveNearestNeighboor(false);
            }
            else if (variant == 4)
            {
                solveNNearestNeighboor(false);
            }

            // std::cout << "Before inverts: " << objectiveFunction() << std::endl; 

            int currentCost = objectiveFunction();

            if (currentCost == 0)
            {
                return;
            }

            bool changes = true;

            while (changes)
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
                            for (unsigned m = 1; m < (i - j) / 2 +1; m++)
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
                        for (unsigned m = 1; m < (i - j) / 2 + 1; m++)
                        {
                            unsigned tmp = solution[j + m];
                            solution[j + m] = solution[i - (m - 1)];
                            solution[i - (m - 1)] = tmp;
                        }
                    }
                }
            }
        }

        //test KRandom and 2opt with time limit set to execution time of NNearestNeighboor
        void testAlgorithmsNForSetInstance(const char* instanceName)
        {
            clock_t timeLimit = clock();

            solveNNearestNeighboor(false);
            int nNearestNeighboorObjectiveFunction = objectiveFunction();

            timeLimit = clock() - timeLimit;
            
            timedTestKRandom(timeLimit, clock());
            int KRandomObjectiveFunction = objectiveFunction();

            timedTest2Opt(timeLimit);
            int twoOptObjectiveFunction = objectiveFunction();

            printf("%s, %d, %d, %d\n", instanceName, KRandomObjectiveFunction, nNearestNeighboorObjectiveFunction, twoOptObjectiveFunction);
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
            // std::cout << "K random result for k = " << K << ":" << bestResult << std::endl;
        }

        void solveNearestNeighboor(const bool withRealtimeVisualization) 
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
            // std::cout << "Nearest Neighboor result: " << objectiveFunction() << std::endl;
        }

        void solveNNearestNeighboor(const bool withVisualization) 
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
            // std::cout << "NNearest Neighboor result: " << bestResult << std::endl;
        }

        void solve2Opt(const bool withVisualization) 
        {
            // nie trzeba się martwić invertami traktującymi solution cyklicznie - bo problem symetryczny
            solveNearestNeighboor(false);

            // std::cout << "Before inverts: " << objectiveFunction() << std::endl; 

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
                            for (unsigned m = 1; m < (i - j) / 2 +1; m++)
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
                        for (unsigned m = 1; m < (i - j) / 2 + 1; m++)
                        {
                            unsigned tmp = solution[j + m];
                            solution[j + m] = solution[i - (m - 1)];
                            solution[i - (m - 1)] = tmp;
                        }
                    }
                }
            }

            // std::cout << "After inverts: " << objectiveFunction() << "after iterations: " << iteration << std::endl; 
        }

        void solveTabuSearch(const int listLength, const clock_t timeLimit, const bool verbose, const bool withVisualization, const bool aspiration, 
                startingSolution solver, moveFunction move, neighboorhoodFunction neighboors, measureFunction measure)
        {
            clock_t finishTimestamp = clock() + timeLimit;

            //Euclidean are always symmetric
            TabuList tabuList(listLength, cityCount, true);

            //get starting position
            (this->*solver)(false);
            // solveKRandom(1, clock(), false);
            
            //initialize longtermList
            std::stack<std::pair<TabuList, morph::vVector<unsigned>>> longtermList;
            longtermList.push(std::make_pair(tabuList, solution));
            bool toBanLongterm = true;

            morph::vVector<unsigned> bestSolution = solution;
            int currentCost = objectiveFunction();
            int bestCost = currentCost;

            unsigned iterationsWithoutImprovement = 0;
            long unsigned iterationCount = 0;

            if (verbose)
                std::cout << "Before Tabu: " << currentCost << std::endl; 

            while (clock() < finishTimestamp)
            {
                std::pair<int, int> bestMove = std::make_pair(-1, -1);
                int bestDifference = INT_MAX;

                for (auto neighboor: (this->*neighboors)())
                {
                    unsigned i = neighboor.first, j = neighboor.second;
                    
                    //kryterium aspiracji - bez niego measure wepchnac do drugiego ifa, a pierwszego usunac
                    if (aspiration)
                    {
                        int costDifference = (this->*measure)(i, j);
                        
                        //if the best - don't even check if it is on tabu list
                        if (currentCost + costDifference < bestCost)
                        {   
                            if (costDifference < bestDifference)
                            {    
                                bestDifference = costDifference;
                                bestMove = std::make_pair(i, j);
                            }
                        }
                        else if (tabuList.checkMoveLegal(i, j))
                        {
                            if (costDifference < bestDifference)
                            {
                                bestDifference = costDifference;
                                bestMove = std::make_pair(i, j);
                            }
                        }
                    }
                    else
                    {
                        if (tabuList.checkMoveLegal(i, j))
                        {
                            int costDifference = (this->*measure)(i, j);
                            if (costDifference < bestDifference)
                            {
                                bestDifference = costDifference;
                                bestMove = std::make_pair(i, j);
                            }
                        }
                    }
                }

                if (bestDifference < INT_MAX)
                {
                    currentCost += bestDifference;
                    iterationCount++;

                    if (toBanLongterm)
                    {
                        toBanLongterm = false;

                        longtermList.top().first.addMoveToTabu(bestMove.first, bestMove.second);
                    }

                    if (currentCost < bestCost)
                    {
                        // if (currentCost < objectiveFunction())
                        // {
                        //     std::cout << "ALERT " << bestMove.first << " " << bestMove.second << " " << iterationCount << " costDifference: " << bestDifference << " actual: " << currentCost - objectiveFunction() << std::endl;
                        // }
                        bestCost = currentCost;
                        bestSolution = solution;

                        iterationsWithoutImprovement = 0;

                        //TODO: save for longterm list here
                        toBanLongterm = true;

                        longtermList.push(std::make_pair(tabuList, solution));
                    }
                    else
                    {
                        iterationsWithoutImprovement++;

                        if (iterationsWithoutImprovement == ((unsigned)sqrt(iterationCount) + (cityCount >> 1)))
                        {
                            //TODO: kick or restore from longterm list
                            if (longtermList.empty())
                            {
                                //nothing on longterm list kick instead
                                // std::cout << "Starting a kick " << iterationCount << std::endl;
                                iterationsWithoutImprovement = 0;

                                //deterministc but maybe random looking kick?
                                for (unsigned index = (((iterationsWithoutImprovement << 1) + iterationCount) % cityCount) >> 2; 
                                                    index < cityCount; index += 7)
                                {
                                    symmetricInvert(index, index - (index & 13)); 
                                }

                                currentCost = objectiveFunction();
                                // do 
                                // {
                                //     solveKRandom(1, iterationCount, false);
                                // } while (clock() < finishTimestamp && objectiveFunction() > 4 * bestCost);
                                // std::cout << "Finished a kick" << std::endl;

                            }
                            else 
                            {
                                auto restored = longtermList.top();
                                longtermList.pop();

                                tabuList = restored.first;
                                solution = restored.second;
                                currentCost = objectiveFunction();

                                // std::cout << "KICKING RN " << iterationCount << std::endl;

                                iterationsWithoutImprovement = 0;
                            }
                        }
                    }
                    tabuList.addMoveToTabu(bestMove.first, bestMove.second);

                    (this->*move)(bestMove.first, bestMove.second);
                }
                else
                {
                    std::cout << "STUCK " << iterationCount << std::endl;
                }
            }
            solution = bestSolution;

            if (verbose)
                std::cout << "After Tabu: " << objectiveFunction() << std::endl; 

            tabuList.deleteList();
        }

        //insert solution[i] at index j
        inline void symmetricInsert(unsigned i, unsigned j)
        {
            unsigned toInclude = solution[i];

            // if (i < j)
            // {
            //     solution.erase(solution.begin() + i);
            //     solution.insert(solution.begin() + j, toInclude);

            // }
            // else
            // {
                solution.erase(solution.begin() + i);
                solution.insert(solution.begin() + j, toInclude);
            // }
        }
        
        inline int insertAcceleratedMeasurement(unsigned i, unsigned j)
        {
            if (i == j - 1)
            {
                unsigned iPred = (i - 1) % cityCount, jSucc = (j + 1) % cityCount; 
                if (iPred < 0)
                {
                    iPred += cityCount;
                }
                int costDifference = citiesDistance(cities[solution[i]], cities[solution[jSucc]]); 
                costDifference += citiesDistance(cities[solution[iPred]], cities[solution[j]]);
                costDifference -= citiesDistance(cities[solution[j]], cities[solution[jSucc]]);
                costDifference -= citiesDistance(cities[solution[iPred]], cities[solution[i]]);

                return costDifference;
            }
            if (i == j + 1)
            {
                return swapAcceleratedMeasurement(i, j);
            }

            int jpred = (j - 1) % cityCount;
            if (jpred < 0)
            {
                jpred += cityCount;
            }
            int ipred = (i - 1) % cityCount;
            if (ipred < 0)
            {
                ipred += cityCount;
            }
            int isucc = (i + 1) % cityCount;

            int costDifference = citiesDistance(cities[solution[jpred]], cities[solution[i]]);
            costDifference += citiesDistance(cities[solution[i]], cities[solution[j]]);
            costDifference += citiesDistance(cities[solution[ipred]], cities[solution[isucc]]);
            costDifference -= citiesDistance(cities[solution[ipred]], cities[solution[i]]);
            costDifference -= citiesDistance(cities[solution[i]], cities[solution[isucc]]);
            costDifference -= citiesDistance(cities[solution[jpred]], cities[solution[j]]);

            return costDifference;
            
            // int a = objectiveFunction();
            // symmetricInsert(i, j);

            // int b = objectiveFunction();
            // symmetricInsert(j, i);

            // return b-a;
        }

        inline std::vector<std::pair<unsigned, unsigned>> symmetricInsertNeighboorhood()
        {
            std::vector<std::pair<unsigned, unsigned>> res;

            for (unsigned i = 1; i < cityCount; i++)
            {
                for (unsigned j = 1; j < cityCount; j++)
                {
                    if (i != j)
                        res.push_back(std::make_pair(i, j));
                }
            }

            return res;
        }

        inline void symmetricInvert(unsigned i, unsigned j)
        {
            for (unsigned m = 1; m < (i - j) / 2 + 1; m++)
            {
                unsigned tmp = solution[j + m];
                solution[j + m] = solution[i - (m - 1)];
                solution[i - (m - 1)] = tmp;
            }
        }

        inline int invertAcceleratedMeasurement(unsigned i, unsigned j)
        {
            if (i == cityCount - 1)
            {
                int costDifference = citiesDistance(cities[solution[i]], cities[solution[j]]); 
                costDifference += citiesDistance(cities[solution[j + 1]], cities[solution[0]]);
                costDifference -= citiesDistance(cities[solution[j]], cities[solution[j + 1]]);
                costDifference -= citiesDistance(cities[solution[i]], cities[solution[0]]);

                return costDifference;
            }

            int costDifference = citiesDistance(cities[solution[i]], cities[solution[j]]); 
            costDifference += citiesDistance(cities[solution[j + 1]], cities[solution[i + 1]]);
            costDifference -= citiesDistance(cities[solution[j]], cities[solution[j + 1]]);
            costDifference -= citiesDistance(cities[solution[i]], cities[solution[i + 1]]);

            return costDifference; 
        }

        inline std::vector<std::pair<unsigned, unsigned>> symmetricInvertNeighboorhood()
        {
            std::vector<std::pair<unsigned, unsigned>> res;
            for (unsigned i = 1; i < cityCount; i++)
            {
                for (unsigned j = 0; j < i; j++)
                {
                    res.push_back(std::make_pair(i, j));
                }
            }

            return res;
        } 

        inline void symmetricSwap(unsigned i, unsigned j)
        {
            unsigned tmp = solution[j];
            solution[j] = solution[i];
            solution[i] = tmp;
        }

        inline int swapAcceleratedMeasurement(unsigned i, unsigned j)
        {
            if (!j && i == cityCount - 1)
            {
                unsigned iPred = (i - 1) % cityCount; 
                int costDifference = citiesDistance(cities[solution[iPred]], cities[solution[j]]); 
                costDifference += citiesDistance(cities[solution[i]], cities[solution[1]]);
                costDifference -= citiesDistance(cities[solution[iPred]], cities[solution[i]]);
                costDifference -= citiesDistance(cities[solution[j]], cities[solution[1]]);

                return costDifference;
            }
            if (i == (j + 1) % cityCount)
            {
                unsigned iSucc = (i + 1) % cityCount, jPred = (j - 1) % cityCount; 
                if (jPred < 0)
                {
                    jPred += cityCount;
                }
                int costDifference = citiesDistance(cities[solution[j]], cities[solution[iSucc]]); 
                costDifference += citiesDistance(cities[solution[jPred]], cities[solution[i]]);
                costDifference -= citiesDistance(cities[solution[i]], cities[solution[iSucc]]);
                costDifference -= citiesDistance(cities[solution[jPred]], cities[solution[j]]);

                return costDifference;
            }
            unsigned jSucc = (j + 1) % cityCount, jPred = (j - 1) % cityCount,
                    iSucc = (i + 1) % cityCount, iPred = (i - 1) % cityCount;

            if (jPred < 0)
            {
                jPred += cityCount;
            }
            if (iPred < 0)
            {
                iPred += cityCount;
            }

            int costDifference = citiesDistance(cities[solution[i]], cities[solution[jSucc]]); 
            costDifference += citiesDistance(cities[solution[jPred]], cities[solution[i]]);

            costDifference += citiesDistance(cities[solution[j]], cities[solution[iSucc]]);
            costDifference += citiesDistance(cities[solution[iPred]], cities[solution[j]]);

            costDifference -= citiesDistance(cities[solution[j]], cities[solution[jSucc]]);
            costDifference -= citiesDistance(cities[solution[jPred]], cities[solution[j]]);

            costDifference -= citiesDistance(cities[solution[i]], cities[solution[iSucc]]);
            costDifference -= citiesDistance(cities[solution[iPred]], cities[solution[i]]);

            return costDifference; 
        }

        inline std::vector<std::pair<unsigned, unsigned>> symmetricSwapNeighboorhood()
        {
            std::vector<std::pair<unsigned, unsigned>> res;

            for (unsigned i = 1; i < cityCount; i++)
            {
                for (unsigned j = 0; j < i; j++)
                {
                    res.push_back(std::make_pair(i, j));
                }
            }

            return res;
        }


        /** GENETIC ALGORITHM
         */

        //threadsafe 64 bit int generation
        //each thread gets its own generator, creating distribution every call - its supposed to be fast according to stack overflow
        //each thread uses different seed, however they get it deterministically
        long longRand(const long & min, const long & max, clock_t seed) {
            static thread_local std::mt19937 generator(seed);
            std::uniform_int_distribution<long> distribution(min, max);
            return distribution(generator);
        }

        void solveGenetic(const clock_t timeLimit, const clock_t seed, const bool verbose, const unsigned populationSize, const unsigned proceedToNextCount, const unsigned eligibleForCrossOverCount,
                    const getStartingPopulation getStarting, const getCrossoverPairs getPairs, const mutationFunction mutate, const crossoverFunction crossover, const selectionFunction naturalSelection)
        {
            if (populationSize < eligibleForCrossOverCount || populationSize < proceedToNextCount)
            {
                std::cout << "Illegal arguments. Returning" << std::endl;
                return;
            }

            const clock_t deadline = clock() + timeLimit; 
            geneticSeed = seed;

            // START -> Populacja początkowa -> Ewaluacja i selekcja -> Krzyżowanie -> Mutacja -> Zapętlenie albo STOP
            
            //populacja początkowa
            std::vector<morph::vVector<unsigned>> startingSpecimen = (this->*getStarting)(populationSize);

            if (verbose)
            {
                std::cout << "Wygenerowanie populacji początkowej zajęło " << double(clock() - deadline + timeLimit) / CLOCKS_PER_SEC << " sekund." << std::endl; 
            }

            
            //ewaluacja
            std::set<std::pair<int, morph::vVector<unsigned>> > sortedObjectiveFunctions;
            long objectiveFunctionSum = 0;

            for (long unsigned i = 0; i < startingSpecimen.size(); i++)
            {
                solution = startingSpecimen[i];
                int tmp = objectiveFunction();
                sortedObjectiveFunctions.insert(std::make_pair(tmp, startingSpecimen[i]));
            }

            std::set<std::pair<int, morph::vVector<unsigned>> >::iterator it(sortedObjectiveFunctions.begin());
            int bestObjectiveValue = (*it).first;
            
            morph::vVector<unsigned> bestSolution = (*it).second;
                
            if (verbose)
                std::cout << "Najlepsze rozwiązanie populacji początkowej ma wartość funkcji celu równą: " << bestObjectiveValue << std::endl; 


            for (unsigned i = 0; i < eligibleForCrossOverCount && i < sortedObjectiveFunctions.size(); i++)
            {
                objectiveFunctionSum += (*it).first;
                it++;
            }
            int firstNonEligibleObjectiveFunction = (*(--it)).first;


            int iterationCounter = 0;

            while (clock() < deadline)
            {
                if (verbose)
                {
                    iterationCounter++;
                    // std::cout << " Iteration " << iterationCounter << " best solution: " << (*(sortedObjectiveFunctions.begin())).first << std::endl;
                }

                //KRZYŻOWANIE
                //wybierz POPULATION_SZIE - PROCEEDS par osobników 
                
                //będziemy losować osobniki z prawdopodobieństwem wprost proporcjonalnym do róznicy 
                //wartości funkcji celu między i-tym a pierwszym non-eligible
                //pomysł na później: do kwadratu tej wartości?
                // std::cout << "Total w main: " << firstNonEligibleObjectiveFunction * eligibleForCrossOverCount - objectiveFunctionSum << std::endl;
                std::vector<std::pair<unsigned, unsigned>> selectedPairs = (this->*getPairs)(populationSize, proceedToNextCount, 
                    MIN(eligibleForCrossOverCount, sortedObjectiveFunctions.size()), objectiveFunctionSum, firstNonEligibleObjectiveFunction, sortedObjectiveFunctions);

            // std::vector<std::pair<unsigned, unsigned>> selectedPairs;

            // long total = firstNonEligibleObjectiveFunction * eligibleForCrossOverCount - objectiveFunctionSum;

            // for (unsigned iter = 0; iter < populationSize - proceedToNextCount; iter++)
            // {
            //     long chosen = longRand(0, total - 1, geneticSeed);
            //     unsigned first = 0;

            //     std::set<std::pair<int, morph::vVector<unsigned>> >::iterator it(sortedObjectiveFunctions.begin());
            //     for (unsigned i = 0; i < eligibleForCrossOverCount; i++)
            //     {
            //         if ((chosen -= (*it).first) <= 0)
            //         {   
            //             first = std::distance(it, sortedObjectiveFunctions.begin());
            //             break;
            //         }
            //         it++;
            //     }

            //     chosen = longRand(0, total - 1, geneticSeed);

            //     it = sortedObjectiveFunctions.begin();
            //     for (unsigned i = 0; i < eligibleForCrossOverCount; i++)
            //     {
            //         if ((chosen -= (*it).first) <= 0)
            //         {   
            //             selectedPairs.push_back(std::make_pair(first, std::distance(it, sortedObjectiveFunctions.begin())));
            //             break;
            //         }
            //         it++;
            //     }
            // }

                //krzyżujemy
                auto start = sortedObjectiveFunctions.begin();
                std::vector<std::pair<int, morph::vVector<unsigned>>> children;

                for (auto selectedPair: selectedPairs)
                {
                    auto first = start, second = start;
                    std::advance(first, selectedPair.first);
                    std::advance(second, selectedPair.second);
                    

                    // std::cout << selectedPair.first << " " << selectedPair.second << " " << sortedObjectiveFunctions.size() << std::endl;
                    children.push_back((this->*crossover)(*first, *second));
                }

                //uśmiercamy za słabe osobniki
                sortedObjectiveFunctions = (this->*naturalSelection)(sortedObjectiveFunctions, proceedToNextCount);

                //dodajemy dzieci do populacji       
                for (auto child: children)
                {
                    sortedObjectiveFunctions.insert(child);
                }         

                //mutacja
                for (it = sortedObjectiveFunctions.begin(); it != sortedObjectiveFunctions.end(); )//std::advance(it, 1))
                {
                    auto mutationResult = (this->*mutate)(*it);
                    it = sortedObjectiveFunctions.erase(it);
                    sortedObjectiveFunctions.insert(mutationResult);
                }

                it = sortedObjectiveFunctions.begin();
                if ((*it).first < bestObjectiveValue)
                {
                    bestObjectiveValue = (*it).first;
                    bestSolution = (*it).second;
                }

                objectiveFunctionSum = 0;
                for (unsigned i = 0; i < eligibleForCrossOverCount && i < sortedObjectiveFunctions.size(); i++)
                {
                    objectiveFunctionSum += (*it).first;
                    it++;
                }
                firstNonEligibleObjectiveFunction = (*(--it)).first;
            }

            solution = bestSolution;
            if (verbose)
            {
                std::cout << "Wykonano " << iterationCounter << " iteracji." << std::endl;
                std::cout << "Najlepsze znalezione rozwiązanie ma wartość funkcji celu równą: " << bestObjectiveValue << " = " << objectiveFunction() << std::endl;
            }
        }

        std::vector<morph::vVector<unsigned>> startingPopulation(unsigned populationSize)
        {
            std::vector<morph::vVector<unsigned>> result;

            for (long unsigned i = 0; i < populationSize; i++)
            {
                solveKRandom(cityCount * 5, geneticSeed + i, false);
                result.push_back(solution);
            }

            return result;
        }

        //i-ty osobnik generowany jest za pomocą k-random z k równym ((i + 1) * cityCount) >> 1
        std::vector<morph::vVector<unsigned>> startingPopulationTwo(unsigned populationSize)
        {
            std::vector<morph::vVector<unsigned>> result;

            for (long unsigned i = 0; i < populationSize; i++)
            {
                solveKRandom(((i + 1) * cityCount) >> 1, geneticSeed + i, false);
                result.push_back(solution);
            }

            return result;
        }

        std::vector<morph::vVector<unsigned>> startingPopulationThree(unsigned populationSize)
        {
            std::vector<morph::vVector<unsigned>> result;

            long unsigned i;
            //80% populacji poczatkowej losowych
            for (i = 0; i < populationSize * 8 / 10; i++)
            {
                solveKRandom(cityCount * 5, geneticSeed + i, false);
                result.push_back(solution);
            }
            //pozostałe 20% - 2opt startowany z losowego rozwiania
            for (; i < populationSize; i++)
            {
                Random2OptImproved(geneticSeed + i);
                result.push_back(solution);
            }

            return result;
        }

        void Random2OptImproved(const long seed)
        {
            solveKRandom(1, seed, false);

            bool changes = true;
            // unsigned iteration = 0;
            // unsigned maxIterations = 5;


            while (changes)
            {
                changes = false;
                for (unsigned i = 1; i < cityCount - 1; i++)
                {
                    for (unsigned j = 0; j < i; j++)
                    {
                        int costDifference = citiesDistance(cities[solution[i]], cities[solution[j]]); 
                        costDifference += citiesDistance(cities[solution[j + 1]], cities[solution[i + 1]]);
                        costDifference -= citiesDistance(cities[solution[j]], cities[solution[j + 1]]);
                        costDifference -= citiesDistance(cities[solution[i]], cities[solution[i + 1]]);
                        
                        if (costDifference < 0)
                        {
                            changes = true;
                            for (unsigned m = 1; m < (i - j) / 2 +1; m++)
                            {
                                unsigned tmp = solution[j + m];
                                solution[j + m] = solution[i - (m - 1)];
                                solution[i - (m - 1)] = tmp;
                            }
                        }
                    }
                }

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
                        for (unsigned m = 1; m < (i - j) / 2 + 1; m++)
                        {
                            unsigned tmp = solution[j + m];
                            solution[j + m] = solution[i - (m - 1)];
                            solution[i - (m - 1)] = tmp;
                        }
                    }
                }
            }
        }

        std::vector<std::pair<unsigned, unsigned>> crossoverPairs(const unsigned populationSize, const unsigned proceedToNextCount, 
            const unsigned eligibleForCrossOverCount, const long objectiveFunctionSum, const int firstNonEligibleObjectiveFunction, 
            std::set<std::pair<int, morph::vVector<unsigned>> > sortedObjectiveFunctions)
        {
            std::vector<std::pair<unsigned, unsigned>> selectedPairs;
            //suma tych różnic w pierwszych eligible polach
            long total = firstNonEligibleObjectiveFunction * eligibleForCrossOverCount - objectiveFunctionSum;
            // std::cout << "total " << total << std::endl;

            for (unsigned iter = 0; iter < populationSize - proceedToNextCount; iter++)
            {
                long chosen = longRand(0, total - 1, geneticSeed);
                    // std::cout << "Chosen wylosowany: " << chosen << std::endl;
                unsigned first = 0;

                std::set<std::pair<int, morph::vVector<unsigned>> >::iterator it(sortedObjectiveFunctions.begin());
                for (unsigned i = 0; i < eligibleForCrossOverCount; i++)
                {
                    chosen -= (firstNonEligibleObjectiveFunction - (*it).first);
                    if (chosen <= 0)
                    {   
                        // std::cout << "Chosen <= 0: " << chosen << std::endl;
                        first = std::distance(sortedObjectiveFunctions.begin(), it);
                        // std::cout << "Distance: " << std::distance(sortedObjectiveFunctions.begin(), it) << std::endl;
                        break;
                    }
                    // std::cout << "Chosen " << chosen << std::endl;
                    it++;
                }

                chosen = longRand(0, total - 1, geneticSeed);
                    // std::cout << "Chosen wylosowany: " << chosen << std::endl;

                it = sortedObjectiveFunctions.begin();
                for (unsigned i = 0; i < eligibleForCrossOverCount; i++)
                {
                    chosen -= (firstNonEligibleObjectiveFunction - (*it).first);
                    if (chosen <= 0)
                    {   
                        // std::cout << "Chosen <= 0: " << chosen << std::endl;
                        selectedPairs.push_back(std::make_pair(first, std::distance(sortedObjectiveFunctions.begin(), it)));
                        // std::cout << "Distance: " << std::distance(sortedObjectiveFunctions.begin(), it) << std::endl;
                        break;
                    }
                    it++;
                }
            }

            // std::cout << "Selected " << selectedPairs.size() << " pairs." << std::endl;
            // std::cout << "firstNonEligibleObjectiveFunction " << firstNonEligibleObjectiveFunction << std::endl;

            return selectedPairs;
        }

    std::vector<std::pair<unsigned, unsigned>> crossoverPairsTwo(const unsigned populationSize, const unsigned proceedToNextCount,
                                                              const unsigned eligibleForCrossOverCount, const long objectiveFunctionSum, const int firstNonEligibleObjectiveFunction,
                                                              std::set<std::pair<int, morph::vVector<unsigned>> > sortedObjectiveFunctions)
    {
        std::vector<std::pair<unsigned, unsigned>> selectedPairs;

        for (unsigned iter = 0; iter < populationSize - proceedToNextCount; iter++)
        {
            long first = longRand(0, sortedObjectiveFunctions.size() - 1, geneticSeed);
            long second = longRand(0, sortedObjectiveFunctions.size() - 1, geneticSeed);
            selectedPairs.push_back(std::make_pair(first, second));
        }

        // std::cout << "Selected " << selectedPairs.size() << " pairs." << std::endl;
        // std::cout << "firstNonEligibleObjectiveFunction " << firstNonEligibleObjectiveFunction << std::endl;

        return selectedPairs;
    }

        std::pair<int, morph::vVector<unsigned>> mutation(const std::pair<int, morph::vVector<unsigned>> inputSolution)
        {
            auto input = inputSolution.second;
            auto inputValue = inputSolution.first;

            while (longRand(0, 100, geneticSeed) > 15)
            {
                unsigned long i = longRand(1, cityCount - 2, geneticSeed), j = longRand(1, cityCount - 2, geneticSeed);
                if (labs(j - i) > 2)// && )
                {
                    const unsigned jSucc = (j + 1), jPred = (j - 1), iSucc = (i + 1), iPred = (i - 1);
                    inputValue += citiesDistance(cities[input[i]], cities[input[jSucc]]); 
                    inputValue += citiesDistance(cities[input[jPred]], cities[input[i]]);

                    inputValue += citiesDistance(cities[input[j]], cities[input[iSucc]]);
                    inputValue += citiesDistance(cities[input[iPred]], cities[input[j]]);

                    inputValue -= citiesDistance(cities[input[j]], cities[input[jSucc]]);
                    inputValue -= citiesDistance(cities[input[jPred]], cities[input[j]]);

                    inputValue -= citiesDistance(cities[input[i]], cities[input[iSucc]]);
                    inputValue -= citiesDistance(cities[input[iPred]], cities[input[i]]);
                    
                    std::iter_swap(input.begin() + i, input.begin() + j);
                }
            }

            return std::make_pair(inputValue, input);
        }

        std::pair<int, morph::vVector<unsigned>> mutationTwo(const std::pair<int, morph::vVector<unsigned>> inputSolution)
        {
            auto input = inputSolution.second;
            auto inputValue = inputSolution.first;

            while (longRand(0, 100, geneticSeed) > 80)
            {
                unsigned long i = longRand(1, cityCount - 2, geneticSeed), j = longRand(1, cityCount - 2, geneticSeed);
                if (labs(j - i) > 2 && labs(j - i) < cityCount / 2)// && )
                {
                    const unsigned jSucc = (j + 1), jPred = (j - 1), iSucc = (i + 1), iPred = (i - 1);
                    inputValue += citiesDistance(cities[input[i]], cities[input[jSucc]]); 
                    inputValue += citiesDistance(cities[input[jPred]], cities[input[i]]);

                    inputValue += citiesDistance(cities[input[j]], cities[input[iSucc]]);
                    inputValue += citiesDistance(cities[input[iPred]], cities[input[j]]);

                    inputValue -= citiesDistance(cities[input[j]], cities[input[jSucc]]);
                    inputValue -= citiesDistance(cities[input[jPred]], cities[input[j]]);

                    inputValue -= citiesDistance(cities[input[i]], cities[input[iSucc]]);
                    inputValue -= citiesDistance(cities[input[iPred]], cities[input[i]]);
                    
                    std::iter_swap(input.begin() + i, input.begin() + j);
                }
            }

            return std::make_pair(inputValue, input);
        }

        std::pair<int, morph::vVector<unsigned>> mutationThree(const std::pair<int, morph::vVector<unsigned>> inputSolution)
        {
            auto input = inputSolution.second;
            auto inputValue = inputSolution.first;

            unsigned long i = longRand(1, cityCount - 2, geneticSeed), j = longRand(0, 10, geneticSeed);
            if (i + j < cityCount - 1)// && )
            {
                morph::vVector<unsigned> toAdd;
                auto it = input.begin() + i;
                inputValue -= citiesDistance(cities[*(it - 1)], cities[*it]); 

                for (long unsigned k = 0; k < j; k++)
                {
                    inputValue -= citiesDistance(cities[*it], cities[*(it + 1)]); 
                    toAdd.push_back(*it);
                    it = input.erase(it);
                }
                inputValue += citiesDistance(cities[*(it - 1)], cities[*it]); 

                
                inputValue -= citiesDistance(cities[input[input.size() - 1]], cities[input[0]]);

                while (toAdd.size())
                {
                    int currentMinCost = INT_MAX;
                    unsigned currentNearestNeighboor = UINT32_MAX;
                    for (unsigned i = 0; i < toAdd.size(); i++)
                    {
                        int dist = citiesDistance(cities[input[input.size() - 1]], cities[toAdd[i]]);
                        if (dist < currentMinCost)
                        {
                            currentMinCost = dist;
                            currentNearestNeighboor = i;
                        }
                    }

                    inputValue += currentMinCost;
                    input.push_back(toAdd[currentNearestNeighboor]);
                    toAdd.erase(toAdd.begin() + currentNearestNeighboor);
                }

                inputValue += citiesDistance(cities[input[input.size() - 1]], cities[input[0]]);
            }


            return std::make_pair(inputValue, input);
        }

        std::pair<int, morph::vVector<unsigned>> crossover(std::pair<int, morph::vVector<unsigned>> first, std::pair<int, morph::vVector<unsigned>> second)
        {
            //wybierz jakiś losowy przedział na second
            //TODO: jakieś przyjemniejsze to losowanie by się przydało
            unsigned i = longRand(0, cityCount - 1, geneticSeed), j;
            do 
            {
                j = longRand(0, cityCount - 1, geneticSeed);
            } while(j == i);

            if (i > j)
            {
                std::swap(i, j);
            }


            //spermutuj tą część second
            std::shuffle(second.second.begin() + i, second.second.begin() + j - 1, rng);

            //usuń te wartości z pierwszego
            for (unsigned k = i; k < j; k++)
            {    
                auto toBeDeleted = std::find(first.second.begin(), first.second.end(), second.second[k]);
                
                if ((toBeDeleted != first.second.begin()) && (toBeDeleted != first.second.end() - 1))
                {
                    first.first -= citiesDistance(cities[*(toBeDeleted - 1)], cities[*toBeDeleted]);
                    first.first -= citiesDistance(cities[*toBeDeleted], cities[*(toBeDeleted + 1)]);
                    first.first += citiesDistance(cities[*(toBeDeleted - 1)], cities[*(toBeDeleted + 1)]);
                }
                else if (toBeDeleted == first.second.begin())
                {
                    first.first -= citiesDistance(cities[*(first.second.end() - 1)], cities[*toBeDeleted]);
                    first.first -= citiesDistance(cities[*toBeDeleted], cities[*(toBeDeleted + 1)]);
                    first.first += citiesDistance(cities[*(first.second.end() - 1)], cities[*(toBeDeleted + 1)]);
                }
                else 
                {
                    first.first -= citiesDistance(cities[*(toBeDeleted - 1)], cities[*toBeDeleted]);
                    first.first -= citiesDistance(cities[*toBeDeleted], cities[*(first.second.begin())]);
                    first.first += citiesDistance(cities[*(toBeDeleted - 1)], cities[*(first.second.begin())]);
                }
                first.second.erase(toBeDeleted);
            }

            //wstaw spermutowaną część drugiego na koniec pierwszego
            int index = first.second.size() - 1;
            first.first -= citiesDistance(cities[*(first.second.begin() + index)], cities[*(first.second.begin())]);
            for (unsigned k = i; k < j; k++)
            {
                auto value = *(second.second.begin() + k);
                first.second.push_back(value);
                first.first += citiesDistance(cities[*(first.second.begin() + index++)], cities[value]);
            }
            first.first += citiesDistance(cities[*(first.second.begin() + index)], cities[*(first.second.begin())]);

            return first;
        }

        std::pair<int, morph::vVector<unsigned>> crossoverTwo(std::pair<int, morph::vVector<unsigned>> first, std::pair<int, morph::vVector<unsigned>> second)
        {
            //losowe miasto startowe
            unsigned i = longRand(0, cityCount - 1, geneticSeed);
            auto currentCity = cities[i];

            bool* visitedFirst = (bool*)calloc(cityCount, sizeof(*visitedFirst)), 
                    *visitedSecond = (bool*)calloc(cityCount, sizeof(*visitedFirst));
            
            unsigned firstIIndex = std::find(first.second.begin(), first.second.end(), i) - first.second.begin();
            visitedFirst[firstIIndex] = true;

            unsigned secondIIndex = std::find(second.second.begin(), second.second.end(), i) - second.second.begin();
            visitedSecond[secondIIndex] = true;

            morph::vVector<unsigned> result;
            result.push_back(i);
            int resultValue = 0;

            for (unsigned visitedCount = 1; visitedCount < cityCount; visitedCount++)
            {
                int currentMinCost = INT_MAX;
                unsigned currentNearestNeighboorFirst = UINT32_MAX, currentNearestNeighboorSecond = UINT32_MAX;
                
                unsigned firstPrev = POS_MOD(firstIIndex - 1, cityCount);
                unsigned firstSucc = POS_MOD(firstIIndex + 1, cityCount);
                unsigned secondPrev = POS_MOD(secondIIndex - 1, cityCount);
                unsigned secondSucc = POS_MOD(secondIIndex + 1, cityCount);

                if (!visitedFirst[firstPrev])
                {
                    int dist = citiesDistance(currentCity, cities[first.second[firstPrev]]);
                    if (dist < currentMinCost)
                    {
                        currentMinCost = dist;
                        currentNearestNeighboorFirst = firstPrev;
                        currentNearestNeighboorSecond = UINT32_MAX;
                    }
                }
                if (!visitedFirst[firstSucc])
                {
                    int dist = citiesDistance(currentCity, cities[first.second[firstSucc]]);
                    if (dist < currentMinCost)
                    {
                        currentMinCost = dist;
                        currentNearestNeighboorFirst = firstSucc;
                        currentNearestNeighboorSecond = UINT32_MAX;
                    }
                }
                if (!visitedSecond[secondPrev])
                {
                    int dist = citiesDistance(currentCity, cities[second.second[secondPrev]]);
                    if (dist < currentMinCost)
                    {
                        currentMinCost = dist;
                        currentNearestNeighboorSecond = secondPrev;
                        currentNearestNeighboorFirst = UINT32_MAX;
                    }
                }
                if (!visitedSecond[secondSucc])
                {
                    int dist = citiesDistance(currentCity, cities[second.second[secondSucc]]);
                    if (dist < currentMinCost)
                    {
                        currentMinCost = dist;
                        currentNearestNeighboorSecond = secondSucc;
                        currentNearestNeighboorFirst = UINT32_MAX;
                    }
                }

                if (currentMinCost < INT_MAX)
                {
                    if (currentNearestNeighboorFirst != UINT32_MAX)
                    {
                        //wybraliśmy miasto z pierwszego rodzica
                        i = first.second[currentNearestNeighboorFirst];
                        firstIIndex = currentNearestNeighboorFirst;
                        secondIIndex = std::find(second.second.begin(), second.second.end(), i) - second.second.begin();
                    }
                    else
                    {
                        //wybraliśmy miasto z drugiego rodzica
                        i = second.second[currentNearestNeighboorSecond];
                        firstIIndex = std::find(first.second.begin(), first.second.end(), i) - first.second.begin();
                        secondIIndex = currentNearestNeighboorSecond;
                    }
                    visitedFirst[firstIIndex] = true;
                    visitedSecond[secondIIndex] = true;

                    currentCity = cities[i];
                    resultValue += citiesDistance(cities[result[result.size() - 1]], currentCity);
                    result.push_back(i);
                }
                else
                {
                    //nie znalezlismy nieodwiedzonego sasiada
                    currentMinCost = INT_MAX;
                    currentNearestNeighboorFirst = UINT32_MAX;
                    currentNearestNeighboorSecond = UINT32_MAX;

                    //dodaj najbliższe jeszcze nie odwiedzone miasto
                    //bardzo podobna procedura, ale k ma szerszy zakres
                    for (unsigned k = 0; k < cityCount; k++)
                    {
                        //claim -- dla sprawdzonych juz wcześniej miast nie bedziemy liczyc i porownywac bo są już odwiedzone
                        if (!visitedFirst[k])
                        {
                            int dist = citiesDistance(currentCity, cities[first.second[k]]);
                            if (dist < currentMinCost)
                            {
                                currentMinCost = dist;
                                currentNearestNeighboorFirst = k;
                                currentNearestNeighboorSecond = UINT32_MAX;
                            }
                        }
                        if (!visitedSecond[k])
                        {
                            int dist = citiesDistance(currentCity, cities[second.second[k]]);
                            if (dist < currentMinCost)
                            {
                                currentMinCost = dist;
                                currentNearestNeighboorSecond = k;
                                currentNearestNeighboorFirst = UINT32_MAX;
                            }
                        }
                    }

                    //claim -- znalezlismy nieodwiedzone miasto
                    if (currentNearestNeighboorFirst != UINT32_MAX)
                    {
                        //wybraliśmy miasto z pierwszego rodzica
                        i = first.second[currentNearestNeighboorFirst];
                        firstIIndex = currentNearestNeighboorFirst;
                        secondIIndex = std::find(second.second.begin(), second.second.end(), i) - second.second.begin();
                    }
                    else
                    {
                        //wybraliśmy miasto z drugiego rodzica
                        i = second.second[currentNearestNeighboorSecond];
                        firstIIndex = std::find(first.second.begin(), first.second.end(), i) - first.second.begin();
                        secondIIndex = currentNearestNeighboorSecond;
                    }

                    visitedFirst[firstIIndex] = true;
                    visitedSecond[secondIIndex] = true;

                    currentCity = cities[i];
                    resultValue += citiesDistance(cities[result[result.size() - 1]], currentCity);
                    result.push_back(i);
                }    
            }

            resultValue += citiesDistance(cities[result[result.size() - 1]], cities[result[0]]);
            
            // zwroc potomstwo
            free(visitedFirst);
            free(visitedSecond);
            return std::make_pair(resultValue, result);
        }

        std::pair<int, morph::vVector<unsigned>> crossoverThree(std::pair<int, morph::vVector<unsigned>> first, std::pair<int, morph::vVector<unsigned>> second)
        {
            //1 wybor losowego miasta poczatkowego i oraz kopia do pierwszego genu dziecka
            unsigned i = longRand(0, cityCount - 1, geneticSeed);
            auto currentCity = cities[i];

            bool* visitedFirst = (bool*)calloc(cityCount, sizeof(*visitedFirst)), 
                    *visitedSecond = (bool*)calloc(cityCount, sizeof(*visitedFirst));
            
            unsigned firstIIndex = std::find(first.second.begin(), first.second.end(), i) - first.second.begin();
            visitedFirst[firstIIndex] = true;

            unsigned secondIIndex = std::find(second.second.begin(), second.second.end(), i) - second.second.begin();
            visitedSecond[secondIIndex] = true;

            morph::vVector<unsigned> result;
            result.push_back(i);
            int resultValue = 0;

            //2 jezeli wszystkie miasta sa odwiedzone to return
            for (unsigned visitedCount = 1; visitedCount < cityCount; visitedCount++)
            {
                int currentMinCost = INT_MAX;
                unsigned currentNearestNeighboorFirst = UINT32_MAX, currentNearestNeighboorSecond = UINT32_MAX;
                    
                //3 oznacz j1 i j2 miasta zlokalizowane po miescie i w pierwszym i drugim rodzicu
                for (unsigned k = firstIIndex; k < cityCount; k++)
                {
                    //4 d min = min (d ij1, d ij2)
                    if (!visitedFirst[k])
                    {
                        int dist = citiesDistance(currentCity, cities[first.second[k]]);
                        if (dist < currentMinCost)
                        {
                            currentMinCost = dist;
                            currentNearestNeighboorFirst = k;
                            currentNearestNeighboorSecond = UINT32_MAX;
                        }
                    }
                }
                for (unsigned k = secondIIndex; k < cityCount; k++)
                {
                    if (!visitedSecond[k])
                    {
                        int dist = citiesDistance(currentCity, cities[second.second[k]]);
                        if (dist < currentMinCost)
                        {
                            currentMinCost = dist;
                            currentNearestNeighboorSecond = k;
                            currentNearestNeighboorFirst = UINT32_MAX;
                        }
                    }
                }

                if (currentMinCost < INT_MAX)
                {
                    if (currentNearestNeighboorFirst != UINT32_MAX)
                    {
                        //wybraliśmy miasto z pierwszego rodzica
                        i = first.second[currentNearestNeighboorFirst];
                        firstIIndex = currentNearestNeighboorFirst;
                        secondIIndex = std::find(second.second.begin(), second.second.end(), i) - second.second.begin();
                    }
                    else
                    {
                        //wybraliśmy miasto z drugiego rodzica
                        i = second.second[currentNearestNeighboorSecond];
                        firstIIndex = std::find(first.second.begin(), first.second.end(), i) - first.second.begin();
                        secondIIndex = currentNearestNeighboorSecond;
                    }
                    visitedFirst[firstIIndex] = true;
                    visitedSecond[secondIIndex] = true;

                    currentCity = cities[i];
                    resultValue += citiesDistance(cities[result[result.size() - 1]], currentCity);
                    result.push_back(i);
                }
                else
                {
                    currentMinCost = INT_MAX;
                    currentNearestNeighboorFirst = UINT32_MAX;
                    currentNearestNeighboorSecond = UINT32_MAX;

                    //dodaj najbliższe jeszcze nie odwiedzone miasto
                    //bardzo podobna procedura, ale k ma szerszy zakres
                    for (unsigned k = 0; k < cityCount; k++)
                    {
                        //claim -- dla sprawdzonych juz wcześniej miast nie bedziemy liczyc i porownywac bo są już odwiedzone
                        if (!visitedFirst[k])
                        {
                            int dist = citiesDistance(currentCity, cities[first.second[k]]);
                            if (dist < currentMinCost)
                            {
                                currentMinCost = dist;
                                currentNearestNeighboorFirst = k;
                                currentNearestNeighboorSecond = UINT32_MAX;
                            }
                        }
                        if (!visitedSecond[k])
                        {
                            int dist = citiesDistance(currentCity, cities[second.second[k]]);
                            if (dist < currentMinCost)
                            {
                                currentMinCost = dist;
                                currentNearestNeighboorSecond = k;
                                currentNearestNeighboorFirst = UINT32_MAX;
                            }
                        }
                    }

                    //claim -- znalezlismy nieodwiedzone miasto
                    if (currentNearestNeighboorFirst != UINT32_MAX)
                    {
                        //wybraliśmy miasto z pierwszego rodzica
                        i = first.second[currentNearestNeighboorFirst];
                        firstIIndex = currentNearestNeighboorFirst;
                        secondIIndex = std::find(second.second.begin(), second.second.end(), i) - second.second.begin();
                    }
                    else
                    {
                        //wybraliśmy miasto z drugiego rodzica
                        i = second.second[currentNearestNeighboorSecond];
                        firstIIndex = std::find(first.second.begin(), first.second.end(), i) - first.second.begin();
                        secondIIndex = currentNearestNeighboorSecond;
                    }

                    visitedFirst[firstIIndex] = true;
                    visitedSecond[secondIIndex] = true;

                    currentCity = cities[i];
                    resultValue += citiesDistance(cities[result[result.size() - 1]], currentCity);
                    result.push_back(i);
                }
            }

            resultValue += citiesDistance(cities[result[result.size() - 1]], cities[result[0]]);
            
            // zwroc potomstwo
            free(visitedFirst);
            free(visitedSecond);
            return std::make_pair(resultValue, result);
        }

        std::set<std::pair<int, morph::vVector<unsigned>>> selection(std::set<std::pair<int, morph::vVector<unsigned>>> population, unsigned proceedToNextCount)
        {
            if (proceedToNextCount < population.size())
            {        
                auto it = population.begin();
                std::advance(it, proceedToNextCount);
                
                while (population.end() != it)
                {
                    it = population.erase(it);
                }
            }

            return population;
        }

    std::set<std::pair<int, morph::vVector<unsigned>>> selectionTwo(std::set<std::pair<int, morph::vVector<unsigned>>> population, unsigned proceedToNextCount)
    {
        std::set<std::pair<int, morph::vVector<unsigned>>> newPopulation;
        if(proceedToNextCount < population.size())
        {
            unsigned groupSize = population.size()/proceedToNextCount;

            auto it = population.begin();

            unsigned i = 0;
            do
            {
                unsigned randomInGroup = longRand(0, groupSize, geneticSeed);
                std::advance(it, randomInGroup);
                newPopulation.insert(*it);
                if(i < population.size()-groupSize){
                    std::advance(it, groupSize-randomInGroup);
                    i+=groupSize;
                }
            }
            while(i < population.size()-groupSize);

        }
        return newPopulation;
    }


        /**
         * UTILS
         */

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
                        // std::cout << "TSPLIB import successful" << std::endl;
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

        void loadSerialized(const char* fileName, const bool withSolution)
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

        void saveSerialized(const char* fileName, const bool withSolution)
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
        void randomInstance(const long seed, unsigned cityCountarg)
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
            // if (*citiesCached)
            // {
            //     free(citiesCached);
            // }
        }

        void printCities()
        {
            for(unsigned i = 0; i < cityCount; i++)
            {
                std::cout << cities[i].first << ", " << cities[i].second << std::endl;
            }
        }

        //TODO: 
        void cacheCitiesDistance()
        {
            citiesCached = (int**)malloc(sizeof(*citiesCached) * cityCount);
            for (unsigned i = 0; i < cityCount; i++)
            {
                citiesCached[i] = (int*)malloc(sizeof(**citiesCached) * cityCount);
            }

            for (unsigned i = 0; i < cityCount; i++)
            {
                for (unsigned j = 0; j < cityCount; j++)
                {
                    // if (i == j)
                    // {
                    //     citiesCached[i][i] = 0;
                    //     if (symmetric)
                    //     {
                    //         break;
                    //     }
                    //     continue;
                    // }
                    int tmp = citiesDistance(cities[i], cities[j]);
                    citiesCached[i][j] = tmp;
                    if (i == j)
                        break;
                    citiesCached[j][i] = tmp;
                }
            }
        }
};
