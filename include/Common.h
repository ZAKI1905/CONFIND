#ifndef Common_H
#define Common_H

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>

#include <TColor.h>

#include "Logger.h"

namespace CONFIND
{
// ==============================================================
void ReportTiming();
// ==============================================================
struct Operation
{
    std::string name ;
    double duration ;

    Operation(std::string in_name, double in_dur) 
        : name(in_name), duration(in_dur) {} 
};

// ==============================================================
struct Timer
{
    std::string scope ;
    static std::vector<Operation> Ops;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTimePt ;

    Timer(std::string in_scope) : scope(in_scope)
    {
        startTimePt = std::chrono::high_resolution_clock::now() ;
    }
    ~Timer()
    {
        Stop() ; 
    }
    void Stop()
    {
        auto endTimePt = std::chrono::high_resolution_clock::now() ;
        auto start     = std::chrono::time_point_cast<std::chrono::microseconds>(startTimePt).time_since_epoch().count() ;
        auto end     = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePt).time_since_epoch().count() ;

        auto duration = end - start ;
        double ms = duration * 0.001 ;
        std::cout<< "\033[1;32m  -> " << scope <<" took " << ms << " (ms).\033[0m\n" ;
        Ops.emplace_back(scope, ms) ;
    }
};

//==============================================================
// struct Quantity
// {
//     double val, err ;
// };

//==============================================================
template <typename T>
struct Range
{
    T min, max ;

    Range() : min(0), max(0) {}
    Range(T in_min, T in_max) : min(in_min), max(in_max) {}
};

//==============================================================
struct Axis
{
  Range<double> range ; 
  int   res ;
  std::string scale ;
  double Min() const { return range.min; }
  double Max() const { return range.max; }
};

//==============================================================
struct Grid2D
{
  Axis xAxis ;
  Axis yAxis ;
};

//==============================================================
struct Coord3D
{
    double x,y,z ;

    Coord3D() {}
    Coord3D(double in_x, double in_y, double in_z) : x(in_x), y(in_y), z(in_z) {}

    bool operator==(const Coord3D& rhs) const
    {
        return ( x == rhs.x && y == rhs.y && z == rhs.z ) ;
    }

    double XYDist2(const Coord3D& rhs) const
    {
        return pow(x - rhs.x, 2) + pow(y - rhs.y, 2) ;
    }
  // coords operator+(const coords& rhs) const
  // {
  //   return {x + rhs.x, y + rhs.y, z + rhs.z } ;
  // }

  // coords operator-(const coords& rhs) const
  // {
  //   return {x - rhs.x, y - rhs.y, z - rhs.z } ;
  // }
  
  // coords operator*(const double& fac) const
  // {
  //   return {fac*x, fac*y, fac*z } ;
  // }
};

//==============================================================
struct Color
{
    size_t idx ;
    std::string name() ;
};
// ........................................................
EColor RColorMap(size_t) ;
//==============================================================
// Saves any vector quantity to a file
template <class T> 
void SaveVec(const std::vector<T>&, std::string, const char* mode = "w") ;

template <class T> 
void SaveVec(const std::vector<T>&, std::string, std::string, const char* mode= "w") ;
//==============================================================

}
//......................CONFIND namespace ends.........................

std::ostream& operator << (std::ostream &output, CONFIND::Operation o) ;
// std::ostream& operator << ( std::ostream &output, CONFIND::Quantity q );
std::ostream& operator << (std::ostream &output, CONFIND::Range<double> r) ;
std::ostream& operator << (std::ostream &output, CONFIND::Range<double> r) ;
std::ostream& operator << (std::ostream &output, CONFIND::Range<int> r) ;
std::ostream& operator << (std::ostream &output, CONFIND::Coord3D c) ;

//==============================================================
#endif /*Common_H*/
