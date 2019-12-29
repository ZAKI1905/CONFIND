#include <iostream>

#include "../include/Logger.h"
#include "../include/ContourFinder.h"

// Just for std::iota
#include <numeric>

// ...........................
double test_f(double x, double y)
{
    return sqrt(x*x + y*y) ;
}

// ...........................
int main()
{
    Instrumentor::Get().BeginSession("0_Function", "examples/0_Function_Profile.json") ;
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
    double colorArr[30];
    std::iota(colorArr, colorArr + 30, 1);
    std::vector<double> colorVec(colorArr, colorArr + sizeof(colorArr) / sizeof (colorArr[0]));

    con.SetContVal(colorVec) ;
    // ...........................

    // ...........................
    // Generating results
    // ...........................
    con.SetGridVals(ContourFinder::Mode::Optimal) ;
    // con.Print() ;
    // con.ExportContour("independent_threshold_cont",  "w") ; 

    // Generating plot using Root
    // File name, plot name, x-axis label, y-axis label
    con.Plot("0_Function", "Testing CONFIND", "X", "Y") ;

    Instrumentor::Get().EndSession();  
}