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
            return (x*y + (10 + m_var)*sin(x));
        }
    
        void modify(const double in_var) { m_var = in_var ;}

    private:
        double m_var = 0;
};

// ...........................
int main()
{
#if PROFILING
    Zaki::Util::Instrumentor::BeginSession("2_MemFunctionOptimized", "examples/3_Modify_Object/3_Modify_Object_Profile.json") ;
#endif
    // ...........................
    // Setting up the grid
    // ...........................
    // { {X_min, X_max}, X_Res, X_scale}, {Y_min, Y_max}, Y_Res, Y_scale} }
    Zaki::Math::Grid2D grid_in = {{{1e-1, 30}, 100, "Linear"},
                                {{1e-1, 30}, 100, "Linear"}};

    ContourFinder con    ;
    con.SetGrid(grid_in) ;

    // Setting the work directory
    char tmp[75] ;
    sprintf(tmp, "%s", "examples/3_Modify_Object") ;
    con.SetWrkDir(tmp) ;

    test_class tc;
    // Setting the member function 
    Zaki::Math::MemFuncWrapper<test_class, double (test_class::*) (double, double)> w(tc, &test_class::DoSth) ;
    con.SetMemFunc(&w);


    // Setting the contour level values (m_var = 0)
    con.SetContVal({20, 40, 50}) ;
    con.SetGridVals(ContourFinder::Mode::Optimal) ;

    // Changing the state of the object 'tc' for the other contours
    // m_var += 3
    tc.modify(10) ;
    con.SetContVal({60, 70}) ;
    con.SetGridVals(ContourFinder::Mode::Optimal) ;
 
    // ...........................

    // ...........................
    // Generating results
    // ...........................
    // con.Print() ;
    con.ExportContour("independent_threshold_cont",  "w") ; 

    // Generating plot using Root
    // File name, plot name, x-axis label, y-axis label
    con.SetHeight(1000) ;
    con.SetWidth(1000)   ;
    con.SetPlotConnected() ;

    con.MakeLegend(true, "Contours", "user") ;
    con.GetLegend()->SetX1(0.75) ; con.GetLegend()->SetY1(0.75) ;
    con.GetLegend()->SetX2(0.90) ; con.GetLegend()->SetY2(0.90) ;
    con.GetLegend()->SetTextSize(0.025) ;
    // con.Plot("3_Modify_Object", "#splitline{f(x,y) = x*y + (var + 5) sin(x)}{Member function can modify the object for each contour}", "X", "Y") ;
    con.Plot("3_Modify_Object", "f(x,y) = x*y + (var + 5) sin(x)", "X", "Y") ;

#if PROFILING
    Zaki::Util::Instrumentor::EndSession();  
#endif
}