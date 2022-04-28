#pragma once
#include <utility>

//for invert/swap style neighboorhoods
class TabuList {

    private:
        int length;
        int currentIndex;
        int cityCount;
        bool symmetric;
        
        //lower than O(n^2) space complexity possible if necessary but search becomes O(n)
        int** tabuList;
        std::pair<int, int>* tabuListQueue;

    public:

        TabuList(int len, int cityCnt, bool symm)
        {
            symmetric = symm;
            cityCount = cityCnt;
            length = len;
            currentIndex = 0;
            tabuList = new int*[cityCount];
            for (int i = 0; i< cityCount; i++)
            {
                tabuList[i] = new int[cityCount];
            }
            tabuListQueue = new std::pair<int, int>[length]();
        }

        //will add the move to tabu list if not on it and return true, or return false with no side effects otherwise 
        bool checkMoveLegal(int i, int j)
        {
            if (symmetric && j > i)
            {
                std::swap(i, j);
            }
            return !tabuList[i][j];
        }

        bool addMoveToTabu(int i, int j)
        {
            if (symmetric && j > i)
            {
                std::swap(i, j);
            }
            if (tabuList[i][j])
            {
                return false;
            }

            auto tmp = tabuListQueue[currentIndex];
            tabuListQueue[currentIndex++] = std::make_pair(i, j);
            currentIndex %= length;

            tabuList[tmp.first][tmp.second] = false;
            tabuList[i][j] = true;

            return true;
        }

        void deleteList()
        {
            for (int i = 0; i < cityCount; i++)
            {
                delete[](tabuList[i]);
            }   
            delete[](tabuList);
            delete[](tabuListQueue);
        }
};