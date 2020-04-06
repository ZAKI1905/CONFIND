#include <iostream>

#include <zaki/Util/Logger.h>

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

    std::vector<Zaki::Physics::Coord3D> my_pts = {{1.3, 1.9, 3}, {1, 2, 3}, {1.5, 1.1, 2}, {0.5, 2.6, 5}
    , {1, 2, 3}, {1.5, 2.8, 1}, {1.5, 1.1, 2}, {0.5, 2.6, 5}} ;
    Cont2D c(10) ;
    c.AddPts(my_pts) ;

    // c.RMDuplicates() ;

    // std::set<Zaki::Physics::Coord3D> un_set(my_pts.begin(), my_pts.end());
    // std::cout << "\n" << c.size() << "\n" ;

    // for (size_t i=0 ; i < c.size() ; i++)
    //   std::cout << c[i] << "\n";
    c.SetFound();
    c.SortNew({1, 1}) ;

    return 0 ;

}