#include <iostream>

#include "../include/Logger.h"
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
            return (x*y + m_var);
        }
    
        void modify(const double in_var) { m_var = in_var ;}

    private:
        double m_var = 0;
};

// ...........................
int main()
{
#if PROFILING
    Instrumentor::Get().BeginSession("2_MemFunctionOptimized", "examples/3_Modify_Object/3_Modify_Object_Profile.json") ;
#endif
    // ...........................
    // Setting up the grid
    // ...........................
    // { {X_min, X_max}, X_Res, X_scale}, {Y_min, Y_max}, Y_Res, Y_scale} }
    CONFIND::Grid2D grid_in = {{{1, 30}, 50, "Log"},
                                {{1, 30}, 50, "Log"}};

    ContourFinder con    ;
    con.SetGrid(grid_in) ;

    // Setting the work directory
    char tmp[75] ;
    sprintf(tmp, "%s", "examples/3_Modify_Object") ;
    con.SetWrkDir(tmp) ;

    test_class tc;
    // Setting the member function 
    MemFuncWrapper<test_class, double (test_class::*) (double, double)> w(tc, &test_class::DoSth) ;
    con.SetMemFunc(&w);


    // Setting the contour level values (m_var = 0)
    con.SetContVal({2, 5}) ;
    con.SetGridVals(ContourFinder::Mode::Optimal) ;

    // Changing the state of the object 'tc' for the other contours
    // m_var = -5
    tc.modify(-5) ;
    con.SetContVal({15, 1500}) ;
    con.SetGridVals(ContourFinder::Mode::Optimal) ;
 
    // ...........................

    // ...........................
    // Generating results
    // ...........................
    // con.Print() ;
    con.ExportContour("independent_threshold_cont",  "w") ; 

    // Generating plot using Root
    // File name, plot name, x-axis label, y-axis label
    con.SetPlotConnected() ;
    con.Plot("3_Modify_Object", "CONFIND Example 3", "X", "Y") ;

#if PROFILING
    Instrumentor::Get().EndSession();  
#endif
}