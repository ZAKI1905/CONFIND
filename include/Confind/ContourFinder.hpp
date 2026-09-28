#ifndef ContourFinder_H
#define ContourFinder_H

#include <functional>
#include "Confind/ThreadingOptions.hpp"

#include <Zaki/Math/Func2D.hpp>
#include <Zaki/Math/MemFuncWrapper.hpp>
#include <Zaki/Math/Math_Core.hpp>
#include <Zaki/Vector/DataSet.hpp>

// Local headers
#include "Confind/Cell.hpp"
#include "Confind/Cont2D.hpp"

//==============================================================

namespace CONFIND
{

//==============================================================
class ContourFinder : public Base
{

  friend class Cell ;
 //--------------------------------------------------------------
  public:

    //............................................
    /// Constructor
    ContourFinder() ;

    /// Destructor
    ~ContourFinder() ;
    ContourFinder(ContourFinder&&);
    ContourFinder& operator=(ContourFinder&&);

    /// Copy constructor
    ContourFinder(const ContourFinder &zc2) ;

    /// Assignment operator
    ContourFinder& operator=(const ContourFinder &zc2) ;
    //............................................

    enum Mode
    {
      Normal = 0, Fast
    } ;

    //............................................
    // Setters
    //............................................
    void SetGrid(const Zaki::Math::Grid2D&)  ;
    void SetDeltas()      ;
    // Historical callable modes remain serial (including their coordinate rounding).
    void SetGridVals(const Mode& = Fast)    ;
    using Evaluator = std::function<double(double, double)>;
    using EvaluatorFactory = std::function<Evaluator(size_t)>;
    // Factory must provide independent state and worker-independent results.
    // Factory calls and evaluator destruction are serialized on the caller.
    void Evaluate(const EvaluatorFactory&, ThreadingOptions = {});
    void SetGridVals(Zaki::Math::GridVals_2D*)    ;

    void SetFunc(double (*f) (const double, const double) ) ; // Normal funcs
    void SetMemFunc(std::unique_ptr<Zaki::Math::Func2D>) ;  // Non-static mem-funcs
    void SetContVal(const std::vector<double>&) ;
    void SetContVal(const std::vector<double>&, const std::vector<std::string>& label) ;
    void SetScanMode(const char) ;

    //............................................

    void Clear() ;

    //............................................
    // Getters
    //............................................
    size_t GetN_X()   const ;
    size_t GetN_Y()   const ;
    std::string GetXScale() const ;
    std::string GetYScale() const ;
    std::pair<double, double> GetDeltas()  const ;
    double GetX_Min() const ;
    double GetX_Max() const ;
    double GetY_Min() const ;
    double GetY_Max() const ;
    char GetScanMode() const;

    /// Returns 'cont_set'
    std::vector<Cont2D> GetContourSet() const ;

    std::pair<double, double> ij_2_xy(const size_t i, const size_t j) const ;

    void Print() const override;
    void ExportContour(const Zaki::String::Directory& f_name, const Zaki::File::FileMode& mode) ;

 //--------------------------------------------------------------
  private:

    void Swap(ContourFinder&);
    void ValidateReady() const;
    void FindContour(Cont2D& cont) ;
    void FindContourFast(Cont2D& cont, double*) ;
    void FindNextContours(double*) ;

    // flags
    bool set_grid_flag        = false ;
    bool set_grid_vals_flag   = false ;
    bool set_func_flag        = false ;
    bool set_cont_val_flag    = false ;
    bool set_mem_func_flag    = false ;
    bool cpy_cons_called      = false ;
    bool set_scan_mode_flage  = false ;

    char scan_mode = 'X' ;
    Mode algorithm = Fast ;

    //............................................
    /// Pointer to the function
    double (*func) (double, double)= nullptr ;
    /// Unique pointer to the member-function
    std::unique_ptr<Zaki::Math::Func2D> genFuncPtr = nullptr ;
    //............................................

    Zaki::Math::Grid2D grid{} ;
    double delta_x = 0, delta_y = 0 ;
    std::vector<Cont2D> cont_set ;

    size_t unfound_contours = 0 ;
};

//==============================================================
/// Interface for the user to set member function pointers
template<typename FuncObj, typename MemFuncPtr >
class MemFuncContWrapper : public Base
{

  private:
    ContourFinder cont_finder;

  public:
    MemFuncContWrapper(const FuncObj& obj, const MemFuncPtr& memFn)
      : Base("MemFuncContWrapper")
    {
      cont_finder.SetMemFunc(std::make_unique<Zaki::Math::MemFuncWrapper<FuncObj, double (FuncObj::*)(double, double)>>(obj, memFn)) ;
    }
    ~MemFuncContWrapper(){}

    MemFuncContWrapper(const MemFuncContWrapper& other)
    : Base("MemFuncContWrapper"), cont_finder(other.cont_finder)
    {
    }

    void UpdateMemFunc(const FuncObj& obj, const MemFuncPtr& memFn)
    {
      cont_finder.SetMemFunc(std::make_unique<Zaki::Math::MemFuncWrapper<FuncObj, double (FuncObj::*)(double, double)>>(obj, memFn)) ;
    }

    ContourFinder* operator->() {
      return &cont_finder;
    }
};
//==============================================================

//--------------------------------------------------------------
} // CONFIND namespace
//==============================================================
#endif /*ContourFinder_H*/
