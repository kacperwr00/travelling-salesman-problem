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
#include <stack>

#define CITY_LIMIT 150
#define BUFSIZE 2048

// SPRAWOZDANIE:
// pomijamy wprowadzenie itp.; sekcja experimental results, krótka notatka nt złożoności obliczeniowej
// implementacja, sprzęt - jeśli podajemy czas działania, instancje - także kod generatora losowych i użyty seed
// metodologia / cel 
// opis wyników
// wnioski

class MatrixTSPInstance 
{
    private:
        unsigned cityCount;
        int** cities;
        morph::vVector<unsigned> solution;
        unsigned max2OptIterations = 1000;
        bool symmetric = false;

        //        std::pair<int, int>* cities;
        //ms; around 17ms are added to wait for user input
        unsigned targetVisualizationDelay = 300;
        int** citiesCached;

        typedef void (MatrixTSPInstance::*moveFunction)(unsigned i, unsigned j);
        typedef int (MatrixTSPInstance::*measureFunction)(unsigned i, unsigned j);
        typedef std::vector<std::pair<unsigned, unsigned>> (MatrixTSPInstance::*neighboorhoodFunction)();
        typedef void (MatrixTSPInstance::*startingSolution)();

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

        void setMax2OptIterations(const unsigned iterations)
        {
            max2OptIterations = iterations;
        }
        
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

        void timed2OptSymmetric(const long timeLimit) 
        {
            // nie trzeba się martwić invertami traktującymi solution cyklicznie - bo problem symetryczny
            long startTimestamp = clock();
            solveKRandom(1, clock());

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
                    //bo problem symetryczny
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
                            for (unsigned m = 1; m < (i - j) / 2 + 1; m++)
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

        void timed2OptNonSymmetric(const long timeLimit) 
        {
            long startTimestamp = clock();
            solveKRandom(1, clock());

            int currentCost = objectiveFunction();

            if (currentCost == 0)
            {
                return;
            }

            bool changes = true;

            while (changes && clock() - startTimestamp < timeLimit)
            {
                changes = false;
                for (unsigned i = 0; i < cityCount - 1; i++)
                {
                    for (unsigned j = 0; j < cityCount - 1; j++)
                    {
                        if (i == j)
                            continue;

                        //rozerwij i-tą oraz j-tą krawędź
                        //sklej po odwróceniu jeśli lepiej

                        int costDifference = cities[solution[i]][solution[j]]; 
                        costDifference += cities[solution[j + 1]][solution[i + 1]];
                        costDifference -= cities[solution[j]][solution[j + 1]];
                        costDifference -= cities[solution[i]][solution[i + 1]];

                        //krawędzie od j + 1 do i - 1 będą iść w drugą stronę:
                        //(jeśli j większe od i to musimy liczyć modulo cityCount) 

                        if (j > i)
                        {
                            for (unsigned k = j + 1; k < cityCount - 1; k++)
                            {
                                costDifference -= cities[solution[k]][solution[k + 1]];
                                costDifference += cities[solution[k + 1]][solution[k]];
                            }
                            costDifference -= cities[solution[cityCount]][solution[0]];
                            costDifference += cities[solution[0]][solution[cityCount]];
                            for (unsigned k = 0; k < i; k++)
                            {
                                costDifference -= cities[solution[k]][solution[k + 1]];
                                costDifference += cities[solution[k + 1]][solution[k]];
                            }
                        }
                        else
                        {
                            for (unsigned k = j + 1; k < i; k++)
                            {
                                costDifference -= cities[solution[k]][solution[k + 1]];
                                costDifference += cities[solution[k + 1]][solution[k]];
                            }
                        }
                        
                        if (costDifference < 0)
                        {
                            changes = true;
                            if (i > j)
                            {
                                for (unsigned m = 1; m < (i - j) / 2 + 1; m++)
                                {
                                    unsigned tmp = solution[j + m];
                                    solution[j + m] = solution[i - (m - 1)];
                                    solution[i - (m - 1)] = tmp;
                                }
                            }
                            else
                            {
                                for (unsigned m = 1; m < (j - i) / 2 + 1; m++)
                                {
                                    unsigned tmp = solution[(j + m) % cityCount];
                                    solution[(j + m) % cityCount] = solution[(i - (m - 1)) % cityCount];
                                    solution[(i - (m - 1)) % cityCount] = tmp;
                                }
                            }
                        }
                    }
                }
            }
        }

        void timedTest2Opt(const long timeLimit)
        {
            if (symmetric)
            {
                timed2OptSymmetric(timeLimit);
            }
            else
            {
                timed2OptNonSymmetric(timeLimit);
            }
        }

        void variant2OptSymmetric()
        {
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
                    //bo problem symetryczny
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
                            for (unsigned m = 1; m < (i - j) / 2 + 1; m++)
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

        void variant2OptNonSymmetric()
        {
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
                for (unsigned i = 0; i < cityCount - 1; i++)
                {
                    for (unsigned j = 0; j < cityCount - 1; j++)
                    {
                        if (i == j)
                            continue;

                        //rozerwij i-tą oraz j-tą krawędź
                        //sklej po odwróceniu jeśli lepiej

                        int costDifference = cities[solution[i]][solution[j]]; 
                        costDifference += cities[solution[j + 1]][solution[i + 1]];
                        costDifference -= cities[solution[j]][solution[j + 1]];
                        costDifference -= cities[solution[i]][solution[i + 1]];

                        //krawędzie od j + 1 do i - 1 będą iść w drugą stronę:
                        //(jeśli j większe od i to musimy liczyć modulo cityCount) 

                        if (j > i)
                        {
                            for (unsigned k = j + 1; k < cityCount - 1; k++)
                            {
                                costDifference -= cities[solution[k]][solution[k + 1]];
                                costDifference += cities[solution[k + 1]][solution[k]];
                            }
                            costDifference -= cities[solution[cityCount]][solution[0]];
                            costDifference += cities[solution[0]][solution[cityCount]];
                            for (unsigned k = 0; k < i; k++)
                            {
                                costDifference -= cities[solution[k]][solution[k + 1]];
                                costDifference += cities[solution[k + 1]][solution[k]];
                            }
                        }
                        else
                        {
                            for (unsigned k = j + 1; k < i; k++)
                            {
                                costDifference -= cities[solution[k]][solution[k + 1]];
                                costDifference += cities[solution[k + 1]][solution[k]];
                            }
                        }
                        
                        if (costDifference < 0)
                        {
                            changes = true;
                            if (i > j)
                            {
                                for (unsigned m = 1; m < (i - j) / 2 + 1; m++)
                                {
                                    unsigned tmp = solution[j + m];
                                    solution[j + m] = solution[i - (m - 1)];
                                    solution[i - (m - 1)] = tmp;
                                }
                            }
                            else
                            {
                                for (unsigned m = 1; m < (j - i) / 2 + 1; m++)
                                {
                                    unsigned tmp = solution[(j + m) % cityCount];
                                    solution[(j + m) % cityCount] = solution[(i - (m - 1)) % cityCount];
                                    solution[(i - (m - 1)) % cityCount] = tmp;
                                }
                            }
                        }
                    }
                }
            }
        }

        void variant2Opt(uint8_t variant)
        {
            if (variant == 0)
            {
                solveKRandom(1, 123);
            }
            else if (variant == 1)
            {
                solveKRandom(100, 123);
            }
            else if (variant == 2)
            {
                solveKRandom(10000, 123);
            }
            else if (variant == 3)
            {
                solveNearestNeighboor();
            }
            else if (variant == 4)
            {
                solveNNearestNeighboor();
            }

            if (symmetric)
            {
                variant2OptSymmetric();
            }
            else
            {
                variant2OptNonSymmetric();
            }
        }

        void timedTestNNearestNeighboor(const long timeLimit)
        {
            long startTimestamp = clock();
            morph::vVector<unsigned> bestSolution;
            int bestResult = INT_MAX;
            bool* visited = (bool*)malloc(cityCount * sizeof(*visited));

            for (unsigned k = 0; k < cityCount && clock() - startTimestamp < timeLimit; k++)
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
        }

        //test KRandom and 2opt with time limit set to execution time of NearestNeighboor
        void testAlgorithmsForSetInstance(const char* instanceName)
        {
            clock_t timeLimit = clock();

            solveNearestNeighboor();
            int nearestNeighboorObjectiveFunction = objectiveFunction();

            timeLimit = clock() - timeLimit;
            
            timedTestKRandom(timeLimit, clock());
            int KRandomObjectiveFunction = objectiveFunction();

            timedTest2Opt(timeLimit);
            int twoOptObjectiveFunction = objectiveFunction();

            printf("%s, %d, %d, %d\n", instanceName, KRandomObjectiveFunction, nearestNeighboorObjectiveFunction, twoOptObjectiveFunction);
        }
        
        //test KRandom and 2opt with time limit set to execution time of NNearestNeighboor
        void testAlgorithmsNForSetInstance(const char* instanceName)
        {
            clock_t timeLimit = clock();

            solveNNearestNeighboor();
            int nNearestNeighboorObjectiveFunction = objectiveFunction();

            timeLimit = clock() - timeLimit;
            
            timedTestKRandom(timeLimit, clock());
            int KRandomObjectiveFunction = objectiveFunction();

            timedTest2Opt(timeLimit);
            int twoOptObjectiveFunction = objectiveFunction();

            printf("%s, %d, %d, %d\n", instanceName, KRandomObjectiveFunction, nNearestNeighboorObjectiveFunction, twoOptObjectiveFunction);
        }

        void solveKRandom(const unsigned K, const long seed) 
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

        void solveNearestNeighboor() 
        {
            solution.clear();
            bool* visited = (bool*)calloc(cityCount, sizeof(*visited));

            unsigned currentCity = 0;
            visited[currentCity] = true;
            unsigned visitedCount = 1;
            solution.push_back(currentCity);

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
            // std::cout << "Nearest Neighboor result: " << objectiveFunction() << std::endl;
        }

        void solveNNearestNeighboor() 
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
            // std::cout << "NNearest Neighboor result: " << bestResult << std::endl;
        }

        void solve2Opt() 
        {
            if (symmetric)
                return solve2OptSymmetric();
            solve2OptNonSymmetric();
        }

        void solveTabuSearch(const int listLength, const clock_t timeLimit, const bool verbose, const bool aspiration, 
                startingSolution solver, moveFunction move, neighboorhoodFunction neighboors, measureFunction measure)
        {
            clock_t finishTimestamp = clock() + timeLimit;

            //Euclidean are always symmetric
            TabuList tabuList(listLength, cityCount, symmetric);

            //get starting position
            (this->*solver)();
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
                    
                    //kryterium aspiracji
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

                //znaleziona legalną zmianę
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
                        //znalezion najlepsze aktualnie rozwiązanie

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

                        //example condition
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
                                    invert(index, index - (index & 13)); 
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
            // for (int i = 0; i < )

            tabuList.deleteList();
        }

        inline void insert(unsigned i, unsigned j)
        {
            //TODO check if ok
            unsigned toInclude = solution[i];
            
            solution.erase(solution.begin() + i);
            solution.insert(solution.begin() + j, toInclude);
        }

        // //insert solution[i] at index j
        // inline void symmetricInsert(unsigned i, unsigned j)
        // {
        //     unsigned toInclude = solution[i];

        //     // if (i < j)
        //     // {
        //     //     solution.erase(solution.begin() + i);
        //     //     solution.insert(solution.begin() + j, toInclude);

        //     // }
        //     // else
        //     // {
        //         solution.erase(solution.begin() + i);
        //         solution.insert(solution.begin() + j, toInclude);
        //     // }
        // }

        // inline void asymmetricInsert(unsigned i, unsigned j)
        // {
        //     //TODO check if ok
        //     unsigned toInclude = solution[i];
            
        //     solution.erase(solution.begin() + i);
        //     solution.insert(solution.begin() + j, toInclude);
        // }
        
        inline int insertMeasurement(unsigned i, unsigned j)
        {
            if (symmetric)
                return insertAcceleratedMeasurement(i, j);
            return insertAsymmetricMeasurement(i, j);
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
                int costDifference = cities[solution[i]][solution[jSucc]]; //citiesDistance(cities[solution[i]], cities[solution[jSucc]]); 
                costDifference += cities[solution[iPred]][solution[j]];  //citiesDistance(cities[solution[iPred]], cities[solution[j]]);
                costDifference -= cities[solution[j]][solution[jSucc]];//citiesDistance(cities[solution[j]], cities[solution[jSucc]]);
                costDifference -= cities[solution[iPred]][solution[i]]; //citiesDistance(cities[solution[iPred]], cities[solution[i]]);

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

            int costDifference = cities[solution[jpred]][solution[i]]; //citiesDistance(cities[solution[jpred]], cities[solution[i]]);
            costDifference += cities[solution[i]][solution[j]]; //citiesDistance(cities[solution[i]], cities[solution[j]]);
            costDifference += cities[solution[ipred]][solution[isucc]]; //citiesDistance(cities[solution[ipred]], cities[solution[isucc]]);
            costDifference -= cities[solution[ipred]][solution[i]]; //citiesDistance(cities[solution[ipred]], cities[solution[i]]);
            costDifference -= cities[solution[i]][solution[isucc]]; //citiesDistance(cities[solution[i]], cities[solution[isucc]]);
            costDifference -= cities[solution[jpred]][solution[j]]; //citiesDistance(cities[solution[jpred]], cities[solution[j]]);

            return costDifference;
            
            // int a = objectiveFunction();
            // symmetricInsert(i, j);

            // int b = objectiveFunction();
            // symmetricInsert(j, i);

            // return b-a;
        }

        inline int insertAsymmetricMeasurement(unsigned i, unsigned j)
        {
            //TODO check if okay 
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

            int costDifference = cities[solution[jpred]][solution[i]]; //citiesDistance(cities[solution[jpred]], cities[solution[i]]);
            costDifference += cities[solution[i]][solution[j]]; //citiesDistance(cities[solution[i]], cities[solution[j]]);
            costDifference += cities[solution[ipred]][solution[isucc]]; //citiesDistance(cities[solution[ipred]], cities[solution[isucc]]);
            costDifference -= cities[solution[ipred]][solution[i]]; //citiesDistance(cities[solution[ipred]], cities[solution[i]]);
            costDifference -= cities[solution[i]][solution[isucc]]; //citiesDistance(cities[solution[i]], cities[solution[isucc]]);
            costDifference -= cities[solution[jpred]][solution[j]]; //citiesDistance(cities[solution[jpred]], cities[solution[j]]);

            return costDifference;
        }

        inline std::vector<std::pair<unsigned, unsigned>> insertNeighboorhood()
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

        inline void invert(unsigned i, unsigned j)
        {
            for (unsigned m = 1; m < (i - j) / 2 + 1; m++)
            {
                unsigned tmp = solution[j + m];
                solution[j + m] = solution[i - (m - 1)];
                solution[i - (m - 1)] = tmp;
            }
        }

        inline int invertMeasurement(unsigned i, unsigned j)
        {
            if (symmetric)
            {
                return invertAcceleratedMeasurement(i, j);
            }
            return invertAsymmetricMeasurement(i, j);
        }

        inline int invertAcceleratedMeasurement(unsigned i, unsigned j)
        {
            if (i == cityCount - 1)
            {
                int costDifference = cities[solution[i]][solution[j]]; //citiesDistance(cities[solution[i]], cities[solution[j]]); 
                costDifference += cities[solution[j + 1]][solution[0]]; //citiesDistance(cities[solution[j + 1]], cities[solution[0]]);
                costDifference -= cities[solution[j]][solution[j + 1]]; //citiesDistance(cities[solution[j]], cities[solution[j + 1]]);
                costDifference -= cities[solution[i]][solution[0]]; //citiesDistance(cities[solution[i]], cities[solution[0]]);

                return costDifference;
            }

            int costDifference = cities[solution[i]][solution[j]];//citiesDistance(cities[solution[i]], cities[solution[j]]); 
            costDifference += cities[solution[j + 1]][solution[i + 1]]; //citiesDistance(cities[solution[j + 1]], cities[solution[i + 1]]);
            costDifference -= cities[solution[j]][solution[j + 1]]; //citiesDistance(cities[solution[j]], cities[solution[j + 1]]);
            costDifference -= cities[solution[i]][solution[i + 1]]; //citiesDistance(cities[solution[i]], cities[solution[i + 1]]);

            return costDifference; 
        }

        inline int invertAsymmetricMeasurement(unsigned i, unsigned j)
        {
            //TODO check if okay

            int costDifference = cities[solution[i]][solution[j]]; 
            costDifference += cities[solution[j + 1]][solution[i + 1]];
            costDifference -= cities[solution[j]][solution[j + 1]];
            costDifference -= cities[solution[i]][solution[i + 1]];

            //krawędzie od j + 1 do i - 1 będą iść w drugą stronę:
            //(jeśli j większe od i to musimy liczyć modulo cityCount) 

            if (j > i)
            {
                for (unsigned k = j + 1; k < cityCount - 1; k++)
                {
                    costDifference -= cities[solution[k]][solution[k + 1]];
                    costDifference += cities[solution[k + 1]][solution[k]];
                }
                costDifference -= cities[solution[cityCount]][solution[0]];
                costDifference += cities[solution[0]][solution[cityCount]];
                for (unsigned k = 0; k < i; k++)
                {
                    costDifference -= cities[solution[k]][solution[k + 1]];
                    costDifference += cities[solution[k + 1]][solution[k]];
                }
            }
            else
            {
                for (unsigned k = j + 1; k < i; k++)
                {
                    costDifference -= cities[solution[k]][solution[k + 1]];
                    costDifference += cities[solution[k + 1]][solution[k]];
                }
            }

            return costDifference;
        }

        inline std::vector<std::pair<unsigned, unsigned>> invertNeighboorhood()
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

        inline void swap(unsigned i, unsigned j)
        {
            unsigned tmp = solution[j];
            solution[j] = solution[i];
            solution[i] = tmp;
        }

        inline int swapMeasurement(unsigned i, unsigned j)
        {
            if (symmetric)
            {
                return swapAcceleratedMeasurement(i, j);
            }
            return swapAsymmetricMeasurement(i, j);
        }

        inline int swapAcceleratedMeasurement(unsigned i, unsigned j)
        {
            if (!j && i == cityCount - 1)
            {
                unsigned iPred = (i - 1) % cityCount; 
                int costDifference = cities[solution[iPred]][solution[j]]; //citiesDistance(cities[solution[iPred]], cities[solution[j]]); 
                costDifference += cities[solution[i]][solution[1]]; //citiesDistance(cities[solution[i]], cities[solution[1]]);
                costDifference -= cities[solution[iPred]][solution[i]]; //citiesDistance(cities[solution[iPred]], cities[solution[i]]);
                costDifference -= cities[solution[j]][solution[1]]; //citiesDistance(cities[solution[j]], cities[solution[1]]);

                return costDifference;
            }
            if (i == (j + 1) % cityCount)
            {
                unsigned iSucc = (i + 1) % cityCount, jPred = (j - 1) % cityCount; 
                if (jPred < 0)
                {
                    jPred += cityCount;
                }
                int costDifference = cities[solution[j]][solution[iSucc]]; //citiesDistance(cities[solution[j]], cities[solution[iSucc]]); 
                costDifference += cities[solution[jPred]][solution[i]]; //citiesDistance(cities[solution[jPred]], cities[solution[i]]);
                costDifference -= cities[solution[i]][solution[iSucc]]; //citiesDistance(cities[solution[i]], cities[solution[iSucc]]);
                costDifference -= cities[solution[jPred]][solution[j]]; //citiesDistance(cities[solution[jPred]], cities[solution[j]]);

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

            int costDifference = cities[solution[i]][solution[jSucc]];//citiesDistance(cities[solution[i]], cities[solution[jSucc]]); 
            costDifference += cities[solution[jPred]][solution[i]];//citiesDistance(cities[solution[jPred]], cities[solution[i]]);

            costDifference += cities[solution[j]][solution[iSucc]]; //citiesDistance(cities[solution[j]], cities[solution[iSucc]]);
            costDifference += cities[solution[iPred]][solution[j]]; //citiesDistance(cities[solution[iPred]], cities[solution[j]]);

            costDifference -= cities[solution[j]][solution[jSucc]]; //citiesDistance(cities[solution[j]], cities[solution[jSucc]]);
            costDifference -= cities[solution[jPred]][solution[j]]; //citiesDistance(cities[solution[jPred]], cities[solution[j]]);

            costDifference -= cities[solution[i]][solution[iSucc]]; //citiesDistance(cities[solution[i]], cities[solution[iSucc]]);
            costDifference -= cities[solution[iPred]][solution[i]]; //citiesDistance(cities[solution[iPred]], cities[solution[i]]);

            return costDifference; 
        }

        inline int swapAsymmetricMeasurement(unsigned i, unsigned j)
        {
            if (!j && i == cityCount - 1)
            {
                unsigned iPred = (i - 1) % cityCount; 
                int costDifference = cities[solution[iPred]][solution[j]]; //citiesDistance(cities[solution[iPred]], cities[solution[j]]); 
                costDifference += cities[solution[i]][solution[1]]; //citiesDistance(cities[solution[i]], cities[solution[1]]);
                costDifference -= cities[solution[i]][solution[j]];
                costDifference += cities[solution[j]][solution[i]];
                costDifference -= cities[solution[iPred]][solution[i]]; //citiesDistance(cities[solution[iPred]], cities[solution[i]]);
                costDifference -= cities[solution[j]][solution[1]]; //citiesDistance(cities[solution[j]], cities[solution[1]]);

                return costDifference;
            }
            if (i == (j + 1) % cityCount)
            {
                unsigned iSucc = (i + 1) % cityCount, jPred = (j - 1) % cityCount; 
                if (jPred < 0)
                {
                    jPred += cityCount;
                }
                int costDifference = cities[solution[j]][solution[iSucc]]; //citiesDistance(cities[solution[j]], cities[solution[iSucc]]); 
                costDifference += cities[solution[jPred]][solution[i]]; //citiesDistance(cities[solution[jPred]], cities[solution[i]]);
                costDifference -= cities[solution[i]][solution[j]];
                costDifference += cities[solution[j]][solution[i]];
                costDifference -= cities[solution[i]][solution[iSucc]]; //citiesDistance(cities[solution[i]], cities[solution[iSucc]]);
                costDifference -= cities[solution[jPred]][solution[j]]; //citiesDistance(cities[solution[jPred]], cities[solution[j]]);

                return costDifference;
            }

            //TODO check if ok
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

            int costDifference = cities[solution[i]][solution[jSucc]];//citiesDistance(cities[solution[i]], cities[solution[jSucc]]); 
            costDifference += cities[solution[jPred]][solution[i]];//citiesDistance(cities[solution[jPred]], cities[solution[i]]);

            costDifference += cities[solution[j]][solution[iSucc]]; //citiesDistance(cities[solution[j]], cities[solution[iSucc]]);
            costDifference += cities[solution[iPred]][solution[j]]; //citiesDistance(cities[solution[iPred]], cities[solution[j]]);

            costDifference -= cities[solution[j]][solution[jSucc]]; //citiesDistance(cities[solution[j]], cities[solution[jSucc]]);
            costDifference -= cities[solution[jPred]][solution[j]]; //citiesDistance(cities[solution[jPred]], cities[solution[j]]);

            costDifference -= cities[solution[i]][solution[iSucc]]; //citiesDistance(cities[solution[i]], cities[solution[iSucc]]);
            costDifference -= cities[solution[iPred]][solution[i]]; //citiesDistance(cities[solution[iPred]], cities[solution[i]]);

            return costDifference; 
        }

        inline std::vector<std::pair<unsigned, unsigned>> swapNeighboorhood()
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

        void solve2OptSymmetric() 
        {
            // nie trzeba się martwić invertami traktującymi solution cyklicznie - bo problem symetryczny
            solveNNearestNeighboor();

            // std::cout << "Before inverts: " << objectiveFunction() << std::endl; 

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
                    //bo problem symetryczny
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
                            for (unsigned m = 1; m < (i - j) / 2 + 1; m++)
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

        void solve2OptNonSymmetric() 
        {
            solveNNearestNeighboor();

            // std::cout << "Before inverts: " << objectiveFunction() << std::endl; 

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
                for (unsigned i = 0; i < cityCount - 1; i++)
                {
                    for (unsigned j = 0; j < cityCount - 1; j++)
                    {
                        if (i == j)
                            continue;

                        //rozerwij i-tą oraz j-tą krawędź
                        //sklej po odwróceniu jeśli lepiej

                        int costDifference = cities[solution[i]][solution[j]]; 
                        costDifference += cities[solution[j + 1]][solution[i + 1]];
                        costDifference -= cities[solution[j]][solution[j + 1]];
                        costDifference -= cities[solution[i]][solution[i + 1]];

                        //krawędzie od j + 1 do i - 1 będą iść w drugą stronę:
                        //(jeśli j większe od i to musimy liczyć modulo cityCount) 

                        if (j > i)
                        {
                            for (unsigned k = j + 1; k < cityCount - 1; k++)
                            {
                                costDifference -= cities[solution[k]][solution[k + 1]];
                                costDifference += cities[solution[k + 1]][solution[k]];
                            }
                            costDifference -= cities[solution[cityCount]][solution[0]];
                            costDifference += cities[solution[0]][solution[cityCount]];
                            for (unsigned k = 0; k < i; k++)
                            {
                                costDifference -= cities[solution[k]][solution[k + 1]];
                                costDifference += cities[solution[k + 1]][solution[k]];
                            }
                        }
                        else
                        {
                            for (unsigned k = j + 1; k < i; k++)
                            {
                                costDifference -= cities[solution[k]][solution[k + 1]];
                                costDifference += cities[solution[k + 1]][solution[k]];
                            }
                        }
                        
                        if (costDifference < 0)
                        {
                            changes = true;
                            if (i > j)
                            {
                                for (unsigned m = 1; m < (i - j) / 2 + 1; m++)
                                {
                                    unsigned tmp = solution[j + m];
                                    solution[j + m] = solution[i - (m - 1)];
                                    solution[i - (m - 1)] = tmp;
                                }
                            }
                            else
                            {
                                for (unsigned m = 1; m < (j - i) / 2 + 1; m++)
                                {
                                    unsigned tmp = solution[(j + m) % cityCount];
                                    solution[(j + m) % cityCount] = solution[(i - (m - 1)) % cityCount];
                                    solution[(i - (m - 1)) % cityCount] = tmp;
                                }
                            }
                        }
                    }
                }
            }

            // std::cout << "After inverts: " << objectiveFunction() << "after iterations: " << iteration << std::endl; 
        }

        int objectiveFunction()
        {
            int result = 0;
            if (solution.size() != cityCount)
            {
                std::cout << "ALERT SOME CITIES ARE MISSING FROM SOLUTION" << std::endl;
            }
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
                        if (symmetric)
                        {
                            //LOWER_DIAG_ROW
                            for (unsigned i = 0; i < cityCount; i++)
                            {
                                for (unsigned j = 0; j <= i; j++)
                                {    
                                    int tmp; 
                                    fscanf(file, "%d", &tmp);
                                    cities[i][j] = tmp;
                                    cities[j][i] = tmp;
                                }
                            }
                            //     ptr = strtok(buffer, " :");
                            //     if (!ptr)
                            //     {
                            //         if (!fgets(buffer, BUFSIZE, file))
                            //         {
                            //             std::cout << "Error: Found fewer cities then expected" << std::endl;
                            //             return;
                            //         }
                            //         ptr = strtok(buffer, " :");
                            //     }
                            //     int tmp = atoi(ptr);

                            //     

                            //         ptr = strtok(NULL, " :");
                            //         if (ptr)
                            //         {
                            //             tmp = atoi(ptr);
                            //         }
                            //         else
                            //         {
                            //             if (!fgets(buffer, BUFSIZE, file))
                            //             {
                            //                 std::cout << "Error: Found fewer cities then expected i: " << i << ", j: " << j << ", dimension: " << cityCount << ", tmp: " << tmp << std::endl;
                            //                 return;
                            //             }
                            //             ptr = strtok(buffer, " :");
                            //             tmp = atoi(ptr);
                            //         }
                            //     }

                            //     cities[i][i] = tmp;
                            // }

                            // if (!fgets(buffer, BUFSIZE, file))
                            // {
                            //     std::cout << "Error: Found fewer cities then expected" << std::endl;
                            //     return;
                            // }

                            // char* ptr;
                            // for (unsigned i = 0; i < cityCount; i++)
                            // {
                            //     ptr = strtok(buffer, " :");
                            //     if (!ptr)
                            //     {
                            //         if (!fgets(buffer, BUFSIZE, file))
                            //         {
                            //             std::cout << "Error: Found fewer cities then expected" << std::endl;
                            //             return;
                            //         }
                            //         ptr = strtok(buffer, " :");
                            //     }
                            //     int tmp = atoi(ptr);

                            //     for (unsigned j = 0; j < i; j++)
                            //     {
                            //         cities[i][j] = tmp;
                            //         cities[j][i] = tmp;

                            //         ptr = strtok(NULL, " :");
                            //         if (ptr)
                            //         {
                            //             tmp = atoi(ptr);
                            //         }
                            //         else
                            //         {
                            //             if (!fgets(buffer, BUFSIZE, file))
                            //             {
                            //                 std::cout << "Error: Found fewer cities then expected i: " << i << ", j: " << j << ", dimension: " << cityCount << ", tmp: " << tmp << std::endl;
                            //                 return;
                            //             }
                            //             ptr = strtok(buffer, " :");
                            //             tmp = atoi(ptr);
                            //         }
                            //     }

                            //     cities[i][i] = tmp;
                            // }
                        }
                        else
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
            if (string.compare("LOWER_DIAG_ROW") == 0 || string.compare("FULL_MATRIX") == 0)
            {
                if (string.compare("LOWER_DIAG_ROW") == 0)
                {
                    symmetric = true;
                }

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

        void saveSerialized(const char* fileName, const bool withSolution)
        {
            morph::HdfData data(fileName, morph::FileAccess::TruncateWrite);
            if (cities)
            {
                if (!symmetric)
                    data.add_string("/type", "FULL_MATRIX");
                else
                    data.add_string("/type", "LOWER_DIAG_ROW");

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
        void randomInstance(const long seed, unsigned cityCountarg, const bool symmetricArg)
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
                        if (symmetricArg)
                        {
                            break;
                        }
                        continue;
                    }
                    int tmp = rand() % (cityCount * 3);
                    cities[i][j] = tmp;
                    if (symmetricArg)
                    {
                        cities[j][i] = tmp;
                    }
                }
            }
        }

        void printCities()
        {
            for(unsigned i = 0; i < cityCount; i++)
            {
                for(unsigned j = 0; j < cityCount; j++)
                {
                    printf("%4d", cities[i][j]);
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
