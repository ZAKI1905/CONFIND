#ifndef ContourFinder_H
#define ContourFinder_H

#include "MemFuncWrapper.h"
#include "Cell.h"

// namespace CONFIND
// {
//==============================================================
struct Cont2D
{
  double val;
  std::vector<CONFIND::Coord3D> pts;

  Cont2D(double) ;

  CONFIND::Coord3D operator[](size_t) const ;
  size_t size() const ;

  void AddPts(const std::vector<CONFIND::Coord3D>&) ;

  void Export(const std::string& f_name, const char* mode) const ;
};

std::ostream& operator << ( std::ostream &output, Cont2D c ) ;
//==============================================================
class ContourFinder : public Base
{

  friend class Cell ; 
 //--------------------------------------------------------------
  public:

    ContourFinder() ;
    ~ContourFinder() ;

    // Copy constructor
    ContourFinder(const ContourFinder &zc2) ;

    //............................................
    // Setters
    //............................................
    // void SetWrkDir(const std::string&) ;
    void SetN_X(size_t)   ;
    void SetN_Y(size_t)   ;
    void SetXScale(const std::string&) ;
    void SetYScale(const std::string&) ;
    void SetX_Min(double) ;
    void SetX_Max(double) ;
    void SetY_Min(double) ;
    void SetY_Max(double) ;
    void SetGrid(const CONFIND::Grid2D&)  ;
    void SetDeltas()      ;
    void SetGridVals()    ;
    void SetFunc(double (*f) (double, double) ) ; // Normal funcs 
    void SetMemFunc(Func2D*) ;  // Non-static mem-funcs
    void SetContVal(const std::vector<double>&) ;
    //............................................

    void FindContour(Cont2D& cont) ;
    void FindContourParallel(Cont2D& cont) ;

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
    // std::vector<std::vector<double> > GetGridVals() const ;
    // std::vector<double> GetContVal() const ;
    void GetSortedContourCoords() const ;

    std::pair<double, double> ij_2_xy(size_t i, size_t j) const ;
    
    void Print() const override;
    void ExportContour(const std::string& f_name, const char* mode) const ;
    void Plot(const std::string& f_name, const std::string& ="", const std::string& ="X", const std::string& ="Y") const ;

 //--------------------------------------------------------------
  private:

    // flags
    bool set_n_x_flag         = false ;
    bool set_n_y_flag         = false ;
    bool set_x_min_flag       = false ;
    bool set_x_scale_flag     = false ;
    bool set_y_scale_flag     = false ;
    bool set_x_max_flag       = false ;
    bool set_y_min_flag       = false ;
    bool set_y_max_flag       = false ;
    bool set_grid_vals_flag   = false ;
    bool set_func_flag        = false ;
    bool set_cont_val_flag    = false ;
    bool set_mem_func_flag    = false ;

    bool cpy_cons_called      = false ;

    double (*func) (double, double)   ;
    Func2D* genFuncPtr = NULL ;

    size_t n_x, n_y ;
    double x_min, x_max, y_min, y_max, delta_x, delta_y ;
    std::string x_scale = "Linear" ; std::string y_scale = "Linear"     ;
    std::vector<Cont2D> cont_set            ;
};

// }
//==============================================================
#endif /*ContourFinder_H*/