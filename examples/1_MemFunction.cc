#include <iostream>

#include <zaki/Util/Logger.h>

#include "../include/ContourFinder.h"

#include <unistd.h>
#include <chrono>
#include <thread>

// ...........................
class test_class
{
    public:
        double DoSth(double x, double y)
        {
            m_var = 10 ;  // Some dummy change in the state of class
            // usleep(500) ; // in ms
            // using namespace std::this_thread ;
            // using namespace std::chrono_literals ;
            // sleep_for(1ms);
            return x*y + m_var;
        }
    
    private:
        double m_var;
};

// ...........................
int main()
{
#if PROFILING
    Zaki::Util::Instrumentor::BeginSession("1_MemFunction", "examples/1_MemberFunction/1_MemFunction_Profile.json") ;
#endif
    // ...........................
    // Setting up the grid
    // ...........................
    // { {X_min, X_max}, X_Res, X_scale}, {Y_min, Y_max}, Y_Res, Y_scale} }
    Zaki::Math::Grid2D grid_in = {{{1, 15}, 200, "Log"},
                                {{1, 15}, 200, "Log"}};

    ContourFinder con    ;
    con.SetGrid(grid_in) ;

    // Setting the work directory
    char tmp[75] ;
    sprintf(tmp, "%s", "examples/1_MemberFunction") ;
    con.SetWrkDir(tmp) ;

    test_class tc;
    // Setting the member function 
    Zaki::Math::MemFuncWrapper<test_class, double (test_class::*) (double, double)> w(tc, &test_class::DoSth) ;
    con.SetMemFunc(&w);


    // Setting the contour level values
    con.SetContVal({20, 50, 100, 150}) ;
    // ...........................

    // ...........................
    // Generating results
    // ...........................
    con.SetGridVals() ;
    // con.Print() ;
    // con.ExportContour("independent_threshold_cont",  "w") ; 

    // Generating plot using Root
    // File name, plot name, x-axis label, y-axis label
    con.SetPlotConnected() ;
    con.Plot("1_MemFunction", "Member Function", "X", "Y") ;

#if PROFILING
    Zaki::Util::Instrumentor::EndSession();  
#endif
}