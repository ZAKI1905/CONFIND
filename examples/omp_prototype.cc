#include <iostream>

// #include <zaki/Util/Logger.h>

#include "../include/ContourFinder.h"
#include <vector>
#include <omp.h>

// #include <zaki/Math/Math_Core.h>

using namespace Zaki::Math ;
using namespace CONFIND ;

// ..........................................................
// Should be a ContourFinder class method
std::vector<Grid2D> ChopGrid(const Grid2D& in_grid, size_t n)
{
    // returns n chopped grid
    return {};
}

// ..........................................................
// Should be a Cont2D class method (operator + overloading)
void AddConts(Cont2D& in_c1, const Cont2D& in_c2)
{
    // returns in_c1 = in_c1 + in_c2
    // write a GetPts() method for Contour2D
    for(size_t i=0 ; i < in_c2.size() ; ++i)
    in_c1.AddPts({in_c2[i]}) ;

    // pts = in_c1.GetPts()
    // pts.reserve(in_c2.size()) ;
    // pts.insert() = (pts.end(), in_c2.pts.begin(), in_c2.pts.end())  ;
}

// ..........................................................
double test_f(double x, double y)
{
    return x+ y ;
}

// ..........................................................
void thread_task(CONFIND::Bundle& in_b)
{
    // does some task and edits the contour pts
}

// ..........................................................
int main()
{

    Grid2D main_grid = {{{1, 10}, 100, "Linear"}, {{1, 10}, 100, "Linear"}};

    // std::vector<Grid2D> grids = ChopGrid(grid, 2) ;

    double value = 10 ;
    // takes the contour level (value)
    Cont2D main_cont(value) ;

    // CONFIND::Bundle b1(grids[0], cont1, test_f);
    // CONFIND::Bundle b2(grids[1], cont2, test_f) ;

    Axis y_0 = main_grid.yAxis ;

    int req_threads = 2 ;
    omp_set_num_threads(req_threads) ;
    Z_LOG_INFO( ("Threads requested: " + std::to_string(req_threads)).c_str()) ;
    int n_t = 1 ;
    // ..............................
    // Parallel Region
    // ..............................
    #pragma omp parallel
    {
        #pragma omp single
        n_t = omp_get_num_threads() ;

        int t_n = omp_get_thread_num() ;

        double y_min = y_0.Min() + t_n * (y_0.Max() - y_0.Min())/(1.0*n_t) ;
        double y_max = y_0.Min() + (t_n+1) * (y_0.Max() - y_0.Min())/(1.0*n_t) ;
        int res      = y_0.res / n_t + 1  ; 

        Grid2D grid = {main_grid.xAxis, 
                        {{y_min, y_max}, res, y_0.scale}};

        // takes the contour level (value)
        Cont2D cont(value);
        CONFIND::Bundle b(grid, cont, test_f);

        // This should be intensive
        thread_task(b) ;

        #pragma omp critical
        main_cont += b.GetCont() ;
    }
    // ..............................

    return 0 ;

}
// ..........................................................