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
    // morph::Visual v(600, 400, "TSP visualized (press \"x\" to exit)");
    
    // auto gv = new morph::GraphVisual<double> (v.shaderprog, v.tshaderprog, {-1.6,-1,0});

    // std::vector<std::pair<double, double>> x;

    // morph::vVector<double> xFirsts;
    // morph::vVector<double> xSeconds;

    // double xMax = 10, yMax = 12;

    // x.push_back(std::make_pair(3.0, 3.5));
    // x.push_back(std::make_pair(4.0, -3.5));
    // x.push_back(std::make_pair(5.0, 7.5));
    // x.push_back(std::make_pair(-5.0, 7.5));
    // x.push_back(std::make_pair(-7.0, -7.5));
    // x.push_back(std::make_pair(10.0, 11.5));

    // gv->setsize(3.5, 2.2);
    // gv->setlimits(-xMax, xMax, -yMax, yMax);
    // gv->policy = morph::stylepolicy::both; // markers, lines, both, allcolour

    // gv->setdata(xFirsts, xSeconds);
    // gv->finalize();

    // v.addVisualModel(gv);

    // std::chrono::milliseconds timespan(100);

    // while (!v.readyToFinish) {
    //     // dx += 0.01;
    //     gv->clear();
    //     xFirsts.clear();
    //     xSeconds.clear();

    //     for (unsigned long i = 0; i < x.size(); i++)
    //     {
    //         if (v.readyToFinish)
    //         {
    //             break;
    //         }
            
    //         if (i == 33)
    //         {
    //             xFirsts.pop_back();
    //             xSeconds.pop_back();
    //         }
    //         else 
    //         {
    //             xFirsts.push_back(x[i].first);
    //             xSeconds.push_back(x[i].second);
    //         }

    //         gv->update(xFirsts, xSeconds, 0);
    //         // }
    //         // gv->append(x[i].first, x[i].second, 0);

            

    //         glfwWaitEventsTimeout (0.01667); // 16.67 ms ~ 60 Hz
    //         std::this_thread::sleep_for(timespan);

    //         v.render();

    //     }
    // }


    // {
    //     morph::HdfData data("serializeTest.h5", morph::FileAccess::TruncateWrite);
    //     data.add_contained_vals ("/x", x);
    //     // data.add_contained_vals ("/vd2", vd2);
    // }

    // x.clear();

    // {
    //     morph::HdfData data("serializeTest.h5", morph::FileAccess::ReadOnly);
    //     data.read_contained_vals ("/x", x);
    // }

    // for(auto p: x)
    // {
    //     std::cout << p.first << ", " << p.second << std::endl;
    // }

    EuclideanTSPInstance instance;
    // MatrixTSPInstance instance;
    
    // instance.randomInstance(4, 0);
    
    //Euclidean
    instance.loadTSPLIB("/home/kacper/semestr6/metaheurystyczne/ALL_tsp/a280.tsp");
    //FULL_MATRIX
    // instance.loadTSPLIB("/home/kacper/semestr6/metaheurystyczne/ALL_tsp/bays29.tsp");
    //lowerdiagrow
    // instance.loadTSPLIB("/home/kacper/semestr6/metaheurystyczne/ALL_tsp/dantzig42.tsp");
    
    // std::cout << ", " << std::endl;
    // instance.printCities();
    instance.visualizeInstance();

    instance.setMax2OptIterations(instance.getCityCount() * instance.getCityCount());
    // instance.solve2Opt(false);
    // instance.solveKRandom(1000000, time(NULL));
    instance.solveNearestNeighboor(false);
    // instance.solveNNearestNeighboor();

    instance.setTargetVisualizationDelay(50);
    instance.visualizeSolution(false);

    instance.deleteInstance();

    return 0;
}