#include <iostream>

#include "../include/Logger.h"
#include "../include/ContourFinder.h"

// Just for std::iota
#include <numeric>

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
    Instrumentor::Get().BeginSession("0_Function", "examples/0_Function_Profile.json") ;
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
    sprintf(tmp, "%s", "examples") ;
    con.SetWrkDir(tmp) ;

    // Setting the function 
    con.SetFunc(test_f);


    // Setting the contour level values
    // double colorArr[2];
    // std::iota(colorArr, colorArr + 2, 1);
    // std::vector<double> colorVec(colorArr, colorArr + sizeof(colorArr) / sizeof (colorArr[0]));

    con.SetContVal({5, 10, 25}) ;
    // ...........................

    // ...........................
    // Generating results
    // ...........................
    con.SetGridVals(ContourFinder::Mode::Optimal) ;
    // con.Print() ;
    // con.ExportContour("0_Function_Contour_Pts",  "w") ; 

    // Generating plot using Root
    // File name, plot name, x-axis label, y-axis label
    con.SetPlotConnected() ;
    con.Plot("0_Function", "Testing CONFIND", "X", "Y") ;

#if PROFILING
    Instrumentor::Get().EndSession();  
#endif
}