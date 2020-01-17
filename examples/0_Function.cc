#include <iostream>

#include "../include/Logger.h"
#include "../include/ContourFinder.h"

// ...........................
double test_f(double x, double y)
{
    // Parabola
    return y - 0.15*(x - 20)*(x - 20) + 1; 

    // Circle
    // return sqrt(x*x + y*y);
}

// ...........................
int main()
{
#if PROFILING
    Instrumentor::Get().BeginSession("0_Function", "examples/0_Basic_Function/0_Function_Profile.json") ;
#endif

    // ...........................
    // Setting up the grid
    // ...........................
    // { {X_min, X_max}, X_Res, X_scale}, {Y_min, Y_max}, Y_Res, Y_scale} }
    CONFIND::Grid2D grid_in = {{{0, 35}, 50, "Linear"},
                                {{0, 35}, 50, "Linear"}};

    ContourFinder con    ;
    con.SetGrid(grid_in) ;

    // Setting the work directory
    char tmp[75] ;
    sprintf(tmp, "%s", "examples/0_Basic_Function") ;
    con.SetWrkDir(tmp) ;

    // Setting the function 
    con.SetFunc(test_f);

    // Setting the contour level values
    con.SetContVal({5, 10, 25}) ;
    // You can keep adding contours 
    con.SetContVal({7.5}) ;
    // ...........................

    // ...........................
    // Generating results
    // ...........................
    con.SetGridVals(ContourFinder::Mode::Optimal) ;
    // con.Print() ;
    con.ExportContour("0_Function_Contour_Pts",  "w") ; 

    //..............................................................
    // Generating plot using Root
    // File name, plot name, x-axis label, y-axis label
    con.SetPlotConnected() ;

    // If choosing "user" option make sure to set the coordinates!
    con.MakeLegend(true, "Function", "user") ;
    con.GetLegend()->SetX1(0.75) ; con.GetLegend()->SetY1(0.75) ;
    con.GetLegend()->SetX2(0.90) ; con.GetLegend()->SetY2(0.90) ;
    con.GetLegend()->SetTextSize(0.025) ;
    con.Plot("0_Function", "CONFIND Example 0", "X", "Y") ;
    //..............................................................

#if PROFILING
    Instrumentor::Get().EndSession();  
#endif
}