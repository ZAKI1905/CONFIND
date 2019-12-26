#include <iostream>

#include "../include/Logger.h"
#include "../include/ContourFinder.h"

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
    CONFIND::Grid2D grid_in = {{{1, 25}, 30, "Linear"},
                                {{1, 25}, 30, "Linear"}};

    ContourFinder con    ;
    con.SetGrid(grid_in) ;

    // Setting the work directory
    char tmp[75] ;
    sprintf(tmp, "%s", "examples") ;
    con.SetWrkDir(tmp) ;

    // Setting the function 
    con.SetFunc(test_f);


    // Setting the contour level values
    con.SetContVal({10, 20}) ;
    // ...........................

    // ...........................
    // Generating results
    // ...........................
    con.SetGridVals() ;
    // con.Print() ;
    // con.ExportContour("independent_threshold_cont",  "w") ; 

    Instrumentor::Get().EndSession();  

    // Generating plot using Root
    // File name, plot name, x-axis label, y-axis label
    con.Plot("0_Function", "Testing CONFIND", "X", "Y") ;
}