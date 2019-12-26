#include <iostream>

#include "../include/Logger.h"
#include "../include/ContourFinder.h"

// ...........................
class test_class
{
    public:
        double DoSth(double x, double y)
        {
            m_var = 10 ;  // Some dummy change in the state of class
            return x*y + m_var;
        }
    
    private:
        double m_var;
};

// ...........................
int main()
{
    Instrumentor::Get().BeginSession("1_MemFunction", "examples/1_MemFunction_Profile.json") ;
    // ...........................
    // Setting up the grid
    // ...........................
    // { {X_min, X_max}, X_Res, X_scale}, {Y_min, Y_max}, Y_Res, Y_scale} }
    CONFIND::Grid2D grid_in = {{{1, 15}, 30, "Log"},
                                {{1, 15}, 30, "Log"}};

    ContourFinder con    ;
    con.SetGrid(grid_in) ;

    // Setting the work directory
    char tmp[75] ;
    sprintf(tmp, "%s", "examples") ;
    con.SetWrkDir(tmp) ;

    test_class tc;
    // Setting the member function 
    MemFuncWrapper<test_class, double (test_class::*) (double, double)> w(tc, &test_class::DoSth) ;
    con.SetMemFunc(&w);


    // Setting the contour level values
    con.SetContVal({50, 100, 150, 200}) ;
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
    con.Plot("1_MemFunction", "Testing CONFIND", "X", "Y") ;
}