#include <iostream>

#include <zaki/Util/Logger.h>
// #include <zaki/Util/Instrumentor.h>

#include "../include/ContourFinder.h"

// ...........................
double test_f(double x, double y)
{
    // Parabola
    // return y - 0.15*(x - 20)*(x - 10) + 1; 

    // Circle
    // return sqrt(x*x + y*y);

    return y - 3*sin(x) ;
}

// ...........................
int main()
{
#if PROFILING
    Zaki::Util::Instrumentor::BeginSession("0_Function", "examples/0_Basic_Function/0_Function_Profile.json") ;
#endif

    // ...........................
    // Setting up the grid
    // ...........................
    // { {X_min, X_max}, X_Res, X_scale}, {Y_min, Y_max}, Y_Res, Y_scale} }
    Zaki::Math::Grid2D grid_in = {{{2, 10}, 30, "Linear"},
                                {{0, 10}, 30, "Linear"}};

    CONFIND::ContourFinder con    ;
    con.SetGrid(grid_in) ;

    // Setting the work directory
    char tmp[75] ;
    sprintf(tmp, "%s", "examples/0_Basic_Function") ;
    con.SetWrkDir(tmp) ;

    // Setting the function 
    con.SetFunc(test_f) ;

    // Setting the contour level values
    con.SetContVal({5, 6, 7}) ;
    // con.SetContVal({5}) ;

    // You can keep adding contours 
    // con.SetContVal({7.5}) ;
    // ...........................

    // ...........................
    // Generating results
    // ...........................
    con.SetThreads(2) ;
    con.SetGridVals(CONFIND::ContourFinder::Mode::Normal) ;
    // con.Print() ;
    con.ExportContour("0_Function_Contour_Pts",  "w") ; 

    //..............................................................
    // Generating plot using Root
    // File name, plot name, x-axis label, y-axis label
    con.SetPlotConnected() ;

    // If choosing "user" option make sure to set the coordinates!
    con.MakeLegend(true, "Contours", "user") ;
    con.GetLegend()->SetX1(0.75) ; con.GetLegend()->SetY1(0.75) ;
    con.GetLegend()->SetX2(0.90) ; con.GetLegend()->SetY2(0.90) ;
    con.GetLegend()->SetTextSize(0.025) ;
    con.Plot("0_Function", "f(x,y) = y - 0.15*(x - 20)*(x - 20) + 1", "X", "Y") ;
    //..............................................................

#if PROFILING
    Zaki::Util::Instrumentor::EndSession();  
#endif
}