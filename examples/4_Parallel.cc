#include <iostream>

#include <zaki/Util/Logger.h>
#include <zaki/Util/Profile_Timer.h>
// #include <zaki/Util/Simple_Timer.h>

#include "../include/ContourFinder.h"

double r = 0.00001 ;
std::vector<int> v ;
// ...........................
double test_f(double x, double y)
{

    // Z_TIME_PROFILE_SIMPLE("test_f") ;
    // Parabola
    // return y - 0.15*(x - 20)*(x - 10) + 1; 

    // Circle
    // return sqrt(x*x + y*y);
    // if(r > 0.0001)
    // {
        using namespace std::this_thread ;
        using namespace std::chrono_literals ;
        sleep_for(r*1ms);
    // }

    return y - 3*sin(x) ;
}

// ...........................
int main()
{
#if PROFILING
    Zaki::Util::Instrumentor::BeginSession("4_Parallel", "examples/4_Parallel/4_Parallel_Profile.json") ;
#endif

// Finding the actual function call time:
// for(double g = 5 ; g <= 5 ; g+= 0.001)
//    { 
//     char tmp_g[100] ;
//     sprintf(tmp_g, "examples/4_Parallel/Func_Call/%.3f.dat", g) ;   
//     Zaki::Util::TimeProfileManager::BeginSession("4_Parallel", tmp_g) ;
//     for(int idx = 0 ; idx < 100 ; idx++)
//     {
//         v = {idx} ;
//         r = g;
//         test_f(5, 5) ;
//     }
//     Zaki::Util::TimeProfileManager::EndSession() ;
//    }
//     return 0;
// ........................................


for(int run=1 ; run < 6; run++)
{
Zaki::Util::TimeProfileManager::BeginSession("4_Parallel", "examples/4_Parallel/Multi_Runs/30x30/4_Comparison_"+ std::to_string(run) + ".dat") ;



std::vector<double> r_set = {
    0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.007, 0.008, 0.009,
    0.010, 0.011, 0.012,  0.013,  0.014,  0.015,  0.016, 0.017, 0.018, 0.019,  
    0.020, 0.021, 0.022, 0.023, 0.024, 0.025, 0.026, 0.027, 0.028, 0.029, 
    0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1, 0.15, 0.2, 0.25, 0.3,
    0.35, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6,
    1.7, 1.8, 1.9, 2, 2.5, 3, 3.5, 4, 4.5, 5} ;

for(int k=0 ; k < 4; k++)
{
    
for(int j=0 ; j < r_set.size(); j++)
{
    r = r_set[j] ;
int Res = 30;
for(int core=8 ; core < 9; core+=1)
{   
    char tmp[100] ;
    sprintf(tmp, "Run=%d Mode=%d (t=%.4f) threads=%d (%d X %d)", run, k, r, core, Res, Res) ;
    std::vector<int> tmp_vec = {k, j} ;
    Z_LOG_INFO(tmp) ;
    // ...........................
    // Setting up the grid
    // ...........................
    // { {X_min, X_max}, X_Res, X_scale}, {Y_min, Y_max}, Y_Res, Y_scale} }
    Zaki::Math::Grid2D grid_in = {{{2, 10}, Res, "Linear"},
                                {{0, 10}, Res, "Linear"}};

    CONFIND::ContourFinder con    ;
    con.SetGrid(grid_in) ;

    // Setting the work directory
    // char tmp[75] ;
    sprintf(tmp, "%s", "examples/4_Parallel") ;
    con.SetWrkDir(tmp) ;

    // Setting the function 
    con.SetFunc(test_f) ;

    // Setting the contour level values
    // con.SetContVal({5, 10, 25}) ;
    con.SetContVal({5}) ;

    // You can keep adding contours 
    // con.SetContVal({7.5}) ;
    // ...........................

    // ...........................
    // Generating results
    // ...........................
    con.SetThreads(core) ;
    {
        Z_TIME_PROFILE(tmp, tmp_vec) ;
        con.SetGridVals((CONFIND::ContourFinder::Mode)k) ;
    }
    // con.Print() ;
    // con.ExportContour("4_Parallel_Contour_Pts",  "w") ; 

    //..............................................................
    // Generating plot using Root
    // File name, plot name, x-axis label, y-axis label
    // con.SetPlotConnected() ;

    // If choosing "user" option make sure to set the coordinates!
    // con.MakeLegend(true, "Contours", "user") ;
    // con.GetLegend()->SetX1(0.75) ; con.GetLegend()->SetY1(0.75) ;
    // con.GetLegend()->SetX2(0.90) ; con.GetLegend()->SetY2(0.90) ;
    // con.GetLegend()->SetTextSize(0.025) ;
    // con.Plot("4_Parallel", "f(x,y) = y - 0.15*(x - 20)*(x - 20) + 1", "X", "Y") ;
    //..............................................................
}
}
}
    Zaki::Util::TimeProfileManager::EndSession() ;
}

#if PROFILING
    Zaki::Util::Instrumentor::EndSession();  
#endif
}