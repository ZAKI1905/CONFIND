#ifndef ContourFinder_H
#define ContourFinder_H

// Root
#include <TMultiGraph.h>
#include <TLegend.h>

// Local headers
#include "MemFuncWrapper.h"
#include "Cell.h"

// namespace CONFIND
// {
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

  private:
    double val;
    std::vector<CONFIND::Coord3D> pts;
    CONFIND::Color color ;
    bool is_found_flag = false ;


    // The most bottom_left point (used for sorting)
    CONFIND::Coord3D bottom_left ;
    int Orientation(const CONFIND::Coord3D&, const CONFIND::Coord3D&, const CONFIND::Coord3D&) const;

    CONFIND::Coord3D operator[](size_t) const ;
    size_t size() const ;

    void AddPts(const std::vector<CONFIND::Coord3D>&) ;

    void Export(const std::string& f_name, const char* mode) ;

    void Sort() ;
    // A utility function to swap two points 
    // void swap(CONFIND::Coord3D &p1, CONFIND::Coord3D &p2) ;
    bool comp_Orient(const CONFIND::Coord3D &a, const CONFIND::Coord3D &b) const; 
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
      Optimal = 0, Normal, Parallel    
    } ;

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
    void SetGridVals(Mode = Normal)    ;
    void SetFunc(double (*f) (double, double) ) ; // Normal funcs 
    void SetMemFunc(Func2D*) ;  // Non-static mem-funcs
    void SetContVal(const std::vector<double>&) ;
    void SetScanMode(const char) ;

    // Plot options
    void SetPlotXLabel(const char*) ;
    void SetPlotYLabel(const char*) ;
    void SetPlotLabel(const char*)  ;
    void SetPlotConnected(const bool=true) ;
    void SetLegendLabels(std::vector<std::string> ); 
    void MakeLegend(const bool=true, const char* const=NULL, const char* const=NULL) ;
    //............................................

    void FindContour(Cont2D& cont) ;
    void FindContourOptimal(Cont2D& cont, double*) ;
    void FindNextContours(double*) ;
    void FindContourParallel(Cont2D& cont) ;

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
    bool set_plotX_label_flag = false ;
    bool set_plotY_label_flag = false ;
    bool set_plot_label_flag  = false ;
    bool set_plot_connected_flag = false ;
    bool cpy_cons_called      = false ;
    bool set_scan_mode_flage  = false ;
    bool set_leg_lab_flag     = false ;
    bool make_legend_flag     = false  ;

    char scan_mode = 'X' ; 

    double (*func) (double, double)   ;
    Func2D* genFuncPtr = NULL ;
    TMultiGraph *graph = NULL ;
    TLegend *legend    = NULL ;
    std::vector<std::string> legend_label_set ;
    std::string legend_header = "Contours" ;
    bool default_legend_opt = true ; 

    size_t n_x, n_y ;
    double x_min, x_max, y_min, y_max, delta_x, delta_y ;
    std::string x_scale = "Linear" ; std::string y_scale = "Linear"     ;
    std::string x_label="X", y_label ="Y", plot_label="" ;
    bool connected_plot = false  ;
    std::vector<Cont2D> cont_set ;

    int unfound_contours = 0 ;
};

// }
//==============================================================
#endif /*ContourFinder_H*/