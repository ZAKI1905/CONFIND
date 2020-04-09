#include <iostream>

#include <zaki/Util/Logger.h>
#include <zaki/Util/Profile_Timer.h>


#include "../include/ContourFinder.h"

#include <unistd.h>
#include <chrono>
#include <thread>

#define LOCAL_PROFILING 0
// ...........................
class test_class
{
    public:
        double DoSth(double x, double y)
        {
            using namespace std::this_thread ;
            using namespace std::chrono_literals ;
            sleep_for(0.1*1ms);

            return (x*y + (10 + m_var)*sin(x));
        }
    
        void modify(const double in_var) { m_var = in_var ;}

    private:
        double m_var = 0;
};

// ...........................
int main()
{
#if LOCAL_PROFILING
    Zaki::Util::Instrumentor::BeginSession("3_Modify_Object", "examples/3_Modify_Object/3_Modify_Object_Profile.json") ;
#endif

    Zaki::Util::TimeProfileManager::BeginSession("3_Modify_Object", "examples/3_Modify_Object/3_Fast_Timing.dat") ;
    
   {
    std::vector<int> idx_1 = {1} ;
    Z_TIME_PROFILE("main()", idx_1) ;
    // ...........................
    // Setting up the grid
    // ...........................
    // { {X_min, X_max}, X_Res, X_scale}, {Y_min, Y_max}, Y_Res, Y_scale} }
    Zaki::Math::Grid2D grid_in = {{{1e-1, 30}, 50, "Linear"},
                                {{1e-1, 30}, 50, "Linear"}};

    CONFIND::ContourFinder con    ;
    con.SetGrid(grid_in) ;

    // Setting the work directory
    char tmp[75] ;
    sprintf(tmp, "%s", "examples/3_Modify_Object") ;
    con.SetWrkDir(tmp) ;

    test_class tc;
    // Setting the member function 
    Zaki::Math::MemFuncWrapper<test_class, double (test_class::*) (double, double)>
        w(tc, &test_class::DoSth) ;
    con.SetMemFunc(&w);

    con.SetThreads(8);
{   
    std::vector<int> idx_2 = {2} ;
    Z_TIME_PROFILE("SetGridVals()", idx_2) ;
    // Setting the contour level values (m_var = 0)
    con.SetContVal({20, 40, 50}) ;
    con.SetGridVals(CONFIND::ContourFinder::Fast) ;

    // Changing the state of the object 'tc' for the other contours
    // m_var += 3
    tc.modify(10) ;
    con.SetContVal({60, 70}) ;
    con.SetGridVals(CONFIND::ContourFinder::Fast) ;
 }
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

   }

    Zaki::Util::TimeProfileManager::EndSession() ;


#if LOCAL_PROFILING
    Zaki::Util::Instrumentor::EndSession();  
#endif
}