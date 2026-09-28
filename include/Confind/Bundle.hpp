#ifndef CONFIND_Bundle_H
#define CONFIND_Bundle_H

// #include <random>

#include <Zaki/Math/Func2D.hpp>
#include <Zaki/Math/Math_Core.hpp>
// #include <Zaki/File/VecSaver.hpp>

// Local headers
// #include "Confind/Cell.hpp"
#include "Confind/Cont2D.hpp"
#include "Confind/Base.hpp"

namespace CONFIND
{

//==============================================================
class Bundle : public Base
{
  friend class ContourFinder;
  friend class Cell;

  public:
    // Constructor 0
    Bundle() : Base("Bundle"), Con(0)
    {
    }

    // Constructor 1
    Bundle(const Zaki::Math::Grid2D& in_g, const Cont2D& in_c,
    const std::unique_ptr<Zaki::Math::Func2D>& in_mf)
        : Base("Bundle"), Grid(in_g), Con(in_c)
    {
      if(in_mf)
        MemFunc = in_mf->Clone() ;
    }

    // Constructor 2
    Bundle(const Zaki::Math::Grid2D& in_g, const Cont2D& in_c,
    double (*in_f)(const double, const double))
        : Base("Bundle"), Grid(in_g), Con(in_c), Func(in_f)
    {
    }

    // Constructor 3
    Bundle(const Zaki::Math::Grid2D& in_g, const Cont2D& in_c,
    const std::unique_ptr<Zaki::Math::Func2D>& in_mf,
    double (*in_f)(const double, const double))
        : Base("Bundle"), Grid(in_g), Con(in_c), Func(in_f)
    {
      if(in_mf)
        MemFunc = in_mf->Clone() ;
    }

    // Constructor 4
    Bundle(const Zaki::Math::Grid2D& in_g,
    const std::unique_ptr<Zaki::Math::Func2D>& in_mf,
    double (*in_f)(const double, const double))
        : Base("Bundle"), Grid(in_g), Con(0), Func(in_f)
    {
      if(in_mf)
        MemFunc = in_mf->Clone() ;
    }

    // Copy constructor
    Bundle(const Bundle& other)
      : Base("Bundle"), Grid(other.Grid), Con(other.Con), Func(other.Func)
    {

        if(other.MemFunc)
        MemFunc = other.MemFunc->Clone() ;

    }

    ~Bundle()
    {
    }

    void AddCont(const Cont2D& in_c) { Con = in_c; }
    void AddGrid(const Zaki::Math::Grid2D& in_g) { Grid = in_g; }

    void AddMemFunc(const std::unique_ptr<Zaki::Math::Func2D>& in_mf)
    {
      if(in_mf)
        MemFunc  = in_mf->Clone();
    }

    void AddFunc(double (*in_f)(const double, const double))
    {
      if(in_f)
        Func = in_f;
    }

    Cont2D GetCont() {return Con;}

  private:
    Zaki::Math::Grid2D Grid;
    Cont2D Con ;
    std::unique_ptr<Zaki::Math::Func2D> MemFunc = nullptr;
    double (*Func)(const double, const double) = nullptr ;
    // std::unique_ptr<double (*)(const double, const double)> func ;

};

//==============================================================

//--------------------------------------------------------------
} // CONFIND namespace
//==============================================================
#endif /*CONFIND_Bundle_H*/
