#ifndef ContourFinder_H
#define ContourFinder_H

#include <omp.h>
#include <random>

// Root
#include <TMultiGraph.h>
#include <TLegend.h>


#include <zaki/Math/Func2D.h>
#include <zaki/Math/MemFuncWrapper.h>

#include <zaki/Math/Math_Core.h>

// Local headers
#include "Cell.h"

namespace CONFIND
{
//==============================================================
class Cont2D : public Base
{

  friend class ContourFinder  ;
  friend std::ostream  & operator<<(std::ostream &os, const Cont2D& p);
  //--------------------------------------------------------------
  public:
    Cont2D(double) ;
    void SetColor(const CONFIND::Color) ;
    void SetColor(const size_t) ;
    void SetFound(const bool=true) ;

    CONFIND::Color GetColor() const ;
    bool GetFound() const ;

    // temproray public
    void AddPts(const std::vector<Zaki::Physics::Coord3D>&) ;
    void RMDuplicates() ;
    Zaki::Physics::Coord3D operator[](size_t) const ;
    Cont2D operator+(const Cont2D& c) const ;
    void operator+=(const Cont2D& c) ;
    size_t size() const ;
    void SortNew(const std::pair<double, double>) ;

  private:
    double val;
    std::vector<Zaki::Physics::Coord3D> pts;
    CONFIND::Color color ;
    bool is_found_flag = false ;


    // The most bottom_left point (used for sorting)
    Zaki::Physics::Coord3D bottom_left ;
    int Orientation(const Zaki::Physics::Coord3D&, const Zaki::Physics::Coord3D&, const Zaki::Physics::Coord3D&) const;


    void Export(const std::string& f_name, const char* mode) ;

    void Sort() ;

    bool comp_Orient(const Zaki::Physics::Coord3D &a, const Zaki::Physics::Coord3D &b) const; 
    bool sort_cw = true ;
    bool already_sorted = false ;
    void Clear() ; 
};

std::ostream& operator << ( std::ostream &output, const Cont2D& c ) ;
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

    enum Mode
    {
      Normal = 0, Fast, Parallel, Ludicrous, Optimal    
    } ;

    //............................................
    // Setters
    //............................................
    void SetGrid(const Zaki::Math::Grid2D&)  ;
    void SetWidth(const size_t) ;
    void SetHeight(const size_t) ;
    void SetDeltas()      ;
    void SetGridVals(const Mode& = Optimal)    ;
    void SetFunc(double (*f) (double, double) ) ; // Normal funcs 
    void SetMemFunc(Zaki::Math::Func2D*) ;  // Non-static mem-funcs
    void SetContVal(const std::vector<double>&) ;
    void SetScanMode(const char) ;
    void SetOptimizationTrials(int) ;

    // Plot options
    void SetPlotXLabel(const char*) ;
    void SetPlotYLabel(const char*) ;
    void SetPlotLabel(const char*)  ;
    void SetPlotConnected(const bool=true) ;
    void SetLegendLabels(const std::vector<std::string>& ); 
    void MakeLegend(const bool=true, const char* const=NULL, const char* const=NULL) ;
    //............................................

    void FindContour(Cont2D& cont) ;
    void FindContourFast(Cont2D& cont, double*) ;
    void FindNextContours(double*) ;
    void FindContourLudicrous() ;
    void ThreadNextTaskLudicrous(Bundle& in_b, std::vector<double>&) ;
    void ThreadTaskLudicrous(Bundle& in_b, std::vector<double>&) ;
    void FindContourParallel(Cont2D& cont) ;
    void SetThreads(int) ;
    void ThreadTask(Bundle& in_b) ;
    


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

    std::pair<double, double> ij_2_xy(size_t i, size_t j) const ;
    
    void Print() const override;
    void ExportContour(const std::string& f_name, const char* mode) ;

    TMultiGraph* GetGraph() ;
    TLegend* GetLegend() ;

    void Plot(const std::string& f_name, const char* const=NULL, const char* const=NULL, const char* const=NULL) ;

 //--------------------------------------------------------------
  private:

    // flags
    bool set_height_flag      = false ;
    bool set_width_flag       = false ;
    bool set_grid_flag        = false ;
    bool set_grid_vals_flag   = false ;
    bool set_func_flag        = false ;
    bool set_cont_val_flag    = false ;
    bool set_mem_func_flag    = false ;
    bool set_plotX_label_flag = false ;
    bool set_plotY_label_flag = false ;
    bool set_plot_label_flag  = false ;
    bool set_plot_connected_flag = false ;
    bool cpy_cons_called      = false ;
    bool set_scan_mode_flage  = false ;
    bool set_leg_lab_flag     = false ;
    bool make_legend_flag     = false  ;

    char scan_mode = 'X' ; 
    Mode algorithm ;

    double (*func) (double, double)= NULL ;
    Zaki::Math::Func2D* genFuncPtr = NULL ;
    TMultiGraph *graph = NULL ;
    TLegend *legend    = NULL ;
    std::vector<std::string> legend_label_set ;
    std::string legend_header = "Contours" ;
    bool default_legend_opt = true ; 

    void FindOptimalMode() ;
    int optimization_trials = 50 ;
    Mode TimeFunc(std::uniform_real_distribution<double>&, 
                  std::uniform_real_distribution<double>&) ;

    std::default_random_engine random_engine ;

    Zaki::Math::Grid2D grid ;
    size_t width = 1000, height = 1000 ;
    double delta_x, delta_y ;
    std::string x_label="X", y_label ="Y", plot_label="" ;
    bool connected_plot = false  ;
    std::vector<Cont2D> cont_set ;

    int req_threads = 1 ;
    int unfound_contours = 0 ;
};

//==============================================================
// Bundle Class is used for sending information to each thread
//  in a multithreaded scenario to avoid racing.
class Bundle : public Base
{
  friend class ContourFinder;
  friend class Cell;

  public:
    // Constructor 1
    Bundle(const Zaki::Math::Grid2D& in_g, const Cont2D& in_c,
    Zaki::Math::Func2D* in_mf) 
        : Grid(in_g), Con(in_c), MemFunc(in_mf) {}

    // Constructor 2
    Bundle(const Zaki::Math::Grid2D& in_g, const Cont2D& in_c,
    double (*in_f)(double, double)) 
        : Grid(in_g), Con(in_c), Func(in_f) {}

    // Constructor 3
    Bundle(const Zaki::Math::Grid2D& in_g, const Cont2D& in_c,
    Zaki::Math::Func2D* in_mf,
    double (*in_f)(double, double)) 
        : Grid(in_g), Con(in_c), MemFunc(in_mf), Func(in_f) {}
    
    // Constructor 4
    Bundle(const Zaki::Math::Grid2D& in_g,
    Zaki::Math::Func2D* in_mf,
    double (*in_f)(double, double)) 
        : Grid(in_g), Con(0), MemFunc(in_mf), Func(in_f) {}

    void AddCont(const Cont2D& in_c) { Con = in_c; }
    Cont2D GetCont() {return Con;}
      
  private:
    Zaki::Math::Grid2D Grid;
    Cont2D Con ;
    Zaki::Math::Func2D* MemFunc = NULL;
    double (*Func)(double, double) = NULL ;

};
//--------------------------------------------------------------
} // CONFIND namespace
//==============================================================
#endif /*ContourFinder_H*/