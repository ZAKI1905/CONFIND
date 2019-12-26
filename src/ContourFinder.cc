#include <omp.h>

// Root
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TStyle.h"

// Local headers
#include "../include/ContourFinder.h"
#include "../include/Common.h"
// #include "Instrumentor.h"

//==============================================================
// Cont2D struct begins
//--------------------------------------------------------------
// Constructors
Cont2D::Cont2D(double in_val) : val(in_val) {} 
//--------------------------------------------------------------
size_t Cont2D::size() const 
{
  return pts.size() ;
}
//--------------------------------------------------------------
std::ostream& operator << ( std::ostream &output, Cont2D c)
{ 
  output << "\n*        c = " << c.val << "        *\n" ;

  for (size_t i = 0 ; i < c.pts.size() ; i++)
  {
    output << "(" << c[i].x << ", " << c[i].y << ", " << c[i].z << ") ";
  }
  
  return output;            
}

//--------------------------------------------------------------
CONFIND::Coord3D Cont2D::operator[](size_t idx_in) const
{ 
  return pts[idx_in] ;
}

//--------------------------------------------------------------
void Cont2D::AddPts(const std::vector<CONFIND::Coord3D>& in_pts) 
{
  for (size_t i = 0; i < in_pts.size(); i++)
  {
    pts.push_back(in_pts[i]) ;
  }
}

//--------------------------------------------------------------
void Cont2D::Export(const std::string& f_name, const char* mode) const
{
  char tmp_char[100] ;
  sprintf(tmp_char, "%s_%.2e", f_name.c_str(), val ) ;
  saveVec(pts, tmp_char, mode) ;
}
//  Cont2D struct ends
//==============================================================

//==============================================================
//  ContourFinder Class begins
//--------------------------------------------------------------
// default Constructor
ContourFinder::ContourFinder() { logger.SetUnit("ContourFinder") ; } 

//--------------------------------------------------------------
// default Destructor
ContourFinder::~ContourFinder() 
{ 
// Log.Warn("\n ====>>> ContourFinder  destructor called! \n") ;
// std::cout<< "\n genFuncPtr: " << genFuncPtr  << " \n" ;
if (cpy_cons_called)  delete genFuncPtr; 
} 
//--------------------------------------------------------------
// Copy constructor
ContourFinder::ContourFinder(const ContourFinder &zc2) :
  set_n_x_flag(zc2.set_n_x_flag),
  set_n_y_flag(zc2.set_n_y_flag),
  set_x_min_flag(zc2.set_x_min_flag),
  set_x_scale_flag(zc2.set_x_scale_flag),
  set_y_scale_flag(zc2.set_y_scale_flag),
  set_x_max_flag(zc2.set_x_max_flag),
  set_y_min_flag(zc2.set_y_min_flag),
  set_y_max_flag(zc2.set_y_max_flag),
  set_grid_vals_flag(zc2.set_grid_vals_flag),
  set_func_flag(zc2.set_func_flag),
  set_cont_val_flag(zc2.set_cont_val_flag),
  set_mem_func_flag(zc2.set_mem_func_flag),
  n_x(zc2.n_x), n_y(zc2.n_y), x_min(zc2.x_min), 
  x_max(zc2.x_max), y_min(zc2.y_min), y_max(zc2.y_max),
  delta_x(zc2.delta_x), delta_y(zc2.delta_y), x_scale(zc2.x_scale),
  y_scale(zc2.y_scale), cont_set(zc2.cont_set), func(zc2.func)
{
  // std::cout << "ContourFinder copy constructor from " << &zc2 << " -> " << this << " \n" ;
  genFuncPtr = zc2.genFuncPtr->Clone() ;
  // cpy_cons_called = true ;
}

//--------------------------------------------------------------
void ContourFinder::SetGrid(const CONFIND::Grid2D& g) 
{
  SetN_X(g.xAxis.res) ;
  set_n_x_flag = true ;
  SetN_Y(g.yAxis.res) ;
  set_n_y_flag = true ;

  SetX_Max(g.xAxis.Max()) ;
  set_x_max_flag = true ;
  SetX_Min(g.xAxis.Min()) ; 
  set_x_min_flag = true ;

  SetY_Max(g.yAxis.Max()) ;
  set_y_min_flag = true ;
  SetY_Min(g.yAxis.Min()) ; 
  set_y_max_flag = true ;

  SetXScale(g.xAxis.scale) ;
  set_x_scale_flag = true ;
  SetYScale(g.yAxis.scale) ;
  set_y_scale_flag = true ;
}

//--------------------------------------------------------------
void ContourFinder::SetN_X(size_t nx_in)  
{
  n_x = nx_in ;
  set_n_x_flag = true ;
}

//--------------------------------------------------------------
void ContourFinder::SetN_Y(size_t ny_in)  
{
  n_y = ny_in ;
  set_n_y_flag = true ;
}

//--------------------------------------------------------------
void ContourFinder::SetXScale(const std::string& xsc_in)  
{
  x_scale = xsc_in ;
  set_x_scale_flag = true ;
}

//--------------------------------------------------------------
void ContourFinder::SetYScale(const std::string& ysc_in)  
{
  y_scale = ysc_in ;
  set_y_scale_flag = true ;
}

//--------------------------------------------------------------
void ContourFinder::SetX_Min(double x_min_in)
{
  x_min = x_min_in ;
  set_x_min_flag = true ;
}

//--------------------------------------------------------------
void ContourFinder::SetX_Max(double x_max_in)
{
  x_max = x_max_in ;
  set_x_max_flag = true ;
}

//--------------------------------------------------------------
void ContourFinder::SetY_Min(double y_min_in)
{
  y_min = y_min_in ;
  set_y_min_flag = true ;
}

//--------------------------------------------------------------
void ContourFinder::SetY_Max(double y_max_in)
{
  y_max = y_max_in ;
  set_y_max_flag = true ;
}

//--------------------------------------------------------------
size_t ContourFinder::GetN_X() const
{
  if(! set_n_x_flag)
  {
    LOG_ERROR("variable not set!") ;
    return 0 ;
  }

  return n_x;
}

//--------------------------------------------------------------
size_t ContourFinder::GetN_Y() const 
{
  if(! set_n_y_flag)
  {
    LOG_ERROR("variable not set!") ;
    return 0 ;
  }

  return n_y;
}

//--------------------------------------------------------------
double ContourFinder::GetX_Min() const
{
  if(! set_x_min_flag)
  {
    LOG_ERROR("variable not set!") ;
    return 0 ;
  }

  return x_min;
}

//--------------------------------------------------------------
double ContourFinder::GetX_Max() const
{  
  if(! set_x_max_flag)
  {
    LOG_ERROR("variable not set!") ;
    return 0 ;
  }

  return x_max;
}

//--------------------------------------------------------------
double ContourFinder::GetY_Min() const
{
  if(! set_y_min_flag)
  {
    LOG_ERROR("variable not set!") ;
    return 0 ;
  }

  return y_min;
}

//--------------------------------------------------------------
double ContourFinder::GetY_Max() const
{
  if(! set_y_max_flag)
  {
    LOG_ERROR("variable not set!") ;
    return 0 ;
  }

  return y_max;
}

//--------------------------------------------------------------
std::pair<double, double> ContourFinder::ij_2_xy(size_t i, size_t j) const
{
  // double delta_x = (x_max - x_min) / n_x ;
  // double delta_y = (y_max - y_min) / n_y ;

  double x = x_min + i*delta_x ;
  double y = y_min + j*delta_y ;

  std::pair<double, double> coordinate = {x, y} ;

  return coordinate ;
}

//--------------------------------------------------------------
void ContourFinder::SetGridVals()
{
  if(!set_func_flag && !set_mem_func_flag)
  {
    LOG_ERROR("Function not set!");
    return ;
  }

  LOG_INFO("==> Setting grid values ...");

  // contour_coords.reserve( cont_set.size() ) ;
  for (size_t i = 0; i < cont_set.size(); i++)
  {
    FindContour(cont_set[i]) ;
  }

  set_grid_vals_flag = true ;
}

//--------------------------------------------------------------
void ContourFinder::SetDeltas()
{
  if (x_scale == "Linear")
  {
    delta_x = (x_max - x_min) / n_x ;
  }
  else if (x_scale == "Log")
  {
    delta_x = (log10(x_max) - log10(x_min)) / n_x ;
  }

  if (y_scale == "Linear")
  {
    delta_y = (y_max - y_min) / n_y ;
  }
  else if (y_scale == "Log")
  {
    delta_y = (log10(y_max) - log10(y_min)) / n_y ;
  }
}

//--------------------------------------------------------------
std::pair<double, double> ContourFinder::GetDeltas() const
{
  if (! set_n_x_flag || ! set_n_y_flag || ! set_x_max_flag || 
      !set_x_min_flag || !set_y_max_flag || !set_y_min_flag)
    {
      LOG_ERROR("Lengths are not set!") ;
      return {-1, -1};
    }
  return {delta_x, delta_y} ;
}

//--------------------------------------------------------------
void ContourFinder::FindContour(Cont2D& cont)
{
  PROFILE_FUNCTION() ;

  char tmp[150] ;
  sprintf(tmp, "==> Finding contour for c = %.2e ...", cont.val) ;
  LOG_INFO(tmp) ;

  SetDeltas() ;

  for (size_t j = 0; j < n_y; j++)
  {
    for (size_t i = 0; i < n_x; i++)
    {
      Cell new_cell(i, delta_x, j, delta_y, this, cont.val) ;
      new_cell.GetStatus() ;
      cont.AddPts(new_cell.GetContourCoords()) ;
    }
  }
}

//--------------------------------------------------------------
//              !!! Under Construction !!!
void ContourFinder::FindContourParallel(Cont2D& cont)
{
  char tmp[150] ;
  sprintf(tmp, "==> Finding contour for c = %.2e ...", cont.val) ;
  LOG_INFO(tmp);

  SetDeltas() ;
 
  omp_set_num_threads(4) ;
  LOG_INFO("Threads requested: 4");
  // ContourFinder a[4] = {*this, *this, *this, *this} ;

  // ............... PARALLEL REGION BEGINS.....................
  #pragma omp parallel
  {
    ContourFinder a = *this ;
    // Asking one of the threads to save the available threads
    #pragma omp single
    {
      char tmp[50] ;
      sprintf(tmp, "Threads provided:  %d.", omp_get_num_threads()) ;
      LOG_INFO(tmp);
    }
  #pragma omp critical
  std::cout << "\n Pointer to a["<< omp_get_thread_num() <<"]: " << a.genFuncPtr << "\n";

  #pragma omp parallel for
    for (size_t j = 0; j < n_y; j++)
    {
      for (size_t i = 0; i < n_x; i++)
      {
        Cell new_cell(i, delta_x, j, delta_y, &a, cont.val) ;
        new_cell.GetStatus() ;
        #pragma omp critical
        {cont.AddPts(new_cell.GetContourCoords()) ;}
      }
    }
  }
  // ............... PARALLEL REGION ENDS......................
}

//--------------------------------------------------------------
void ContourFinder::Print() const
{
  if (! set_grid_vals_flag )
    {
      LOG_ERROR("Grid values are not set yet, use 'SetGridVals()' first!");
      return ;
    }
  
  for (size_t i = 0; i < cont_set.size(); i++)
  {
    char tmp[100] ;
    sprintf(tmp, "==> Printing Contour = %f ...", cont_set[i].val) ;
    LOG_INFO(tmp);

    std::cout << cont_set[i] ;
  }
}

//--------------------------------------------------------------
// std::vector<std::vector<double> > ContourFinder::GetGridVals()
// {
//   return grid_vals ;
// }

//--------------------------------------------------------------
void ContourFinder::SetFunc(double (*f)(double, double) ) 
{
  func = f;
  set_func_flag = true ;
}

//--------------------------------------------------------------
// Non-static mem-funcs
void ContourFinder::SetMemFunc(Func2D* gen_Fun) 
{
  genFuncPtr = gen_Fun ;
  set_mem_func_flag = true ;
}

//--------------------------------------------------------------
void ContourFinder::SetContVal(const std::vector<double>& cont_val_in) 
{
  cont_set.clear() ;
  for (size_t i = 0; i < cont_val_in.size(); i++)
  {
    cont_set.emplace_back(cont_val_in[i]) ;
  }
  
  set_cont_val_flag = true ;
}

//--------------------------------------------------------------
// std::vector<double> ContourFinder::GetContVal() const
// {
//   if (!set_cont_val_flag)
//   {
//     LOG_ERROR("Contour values are not set!");
//     return {-1} ;
//   }
//   return cont_val_set ;
// }

//--------------------------------------------------------------
void ContourFinder::ExportContour(const std::string& f_name, const char* mode) const
{
  if (! set_grid_vals_flag )
  {
    LOG_ERROR("Grid values are not set yet, use 'SetGridVals()' first!");
    return ;
  }

  for(size_t i = 0 ; i < cont_set.size() ; ++i)
  {  
    cont_set[i].Export(wrk_dir_str + "/" + f_name, mode) ;
  }

  char tmp_char[150] ;
  sprintf(tmp_char, "Contours exported to %s_[CONT].dat.", f_name.c_str() ) ;
  LOG_INFO(tmp_char) ;
}

//--------------------------------------------------------------
void ContourFinder::Plot(const std::string& f_name, const std::string& main_title,
                const std::string& x_title, const std::string& y_title) const
{
  if (! set_grid_vals_flag )
  {
    LOG_ERROR("Grid values are not set yet, use 'SetGridVals()' first!");
    return ;
  }

  TCanvas c("c", "Contour Plot", 1200, 1200) ;
  c.SetGrid();
  
  gStyle->SetOptStat(0);

  TMultiGraph *mg = new TMultiGraph()  ;
  TGraph *g[cont_set.size()] ;

  std::vector<double> x_vals ;
  std::vector<double> y_vals ;

  for(size_t i = 0 ; i < cont_set.size() ; ++i)
  {
    x_vals.reserve(cont_set[i].size()) ;
    y_vals.reserve(cont_set[i].size()) ;
    for(size_t j = 0 ; j < cont_set[i].size() ; ++j)
    {
      x_vals.push_back(cont_set[i][j].x) ;
      y_vals.push_back(cont_set[i][j].y) ;
    }

    g[i] = new TGraph(y_vals.size(), &x_vals[0], &y_vals[0]);
    // g[i]->SetMaximum(y_max) ; 
    // g[i]->SetMinimum(y_min) ;
    // g[i] ->SetTitle("Contour Plot") ;
    g[i] ->SetLineColor(CONFIND::RColorMap(i)) ;
    g[i] ->SetLineStyle(1) ;
    g[i] ->SetLineWidth(2) ;
    g[i] ->SetMarkerColor(CONFIND::RColorMap(i)) ;
    // g.SetMarkerSize(2) ;
    // g.SetMarkerStyle(29) ;
    mg->Add(g[i]) ;

    x_vals.clear() ;
    y_vals.clear() ;
  }

  
  if (x_scale == "Log")
    c.SetLogx() ;
  if (y_scale == "Log")
    c.SetLogy() ;

  mg->GetXaxis()->SetLimits(x_min, x_max);
  mg->GetYaxis()->SetLimits(y_min, y_max);

  mg->SetTitle(main_title.c_str()) ;

  mg->GetXaxis()->SetTitle(x_title.c_str()) ; 
  mg->GetXaxis()->SetTitleSize(0.03) ;
  mg->GetXaxis()-> SetTitleOffset(1.6);

  mg->GetYaxis()->SetTitle(y_title.c_str()) ;
  mg->GetYaxis()->SetTitleSize(0.03) ;
  mg->GetYaxis()-> SetTitleOffset(1.5);

  // gStyle->SetTitleFontSize(.08);
  // gStyle->SetLabelSize(.005, "XY");

  mg->Draw("AP") ;

  c.Update() ;

  // Output file
  char out_file_char[150] ;    
  sprintf(out_file_char, "%s/%s.pdf", wrk_dir_str.c_str(), f_name.c_str()) ;

  c.SaveAs(out_file_char) ;

  delete mg ;
  
}

//--------------------------------------------------------------
std::string ContourFinder::GetXScale()  const
{
  return x_scale;
}

//--------------------------------------------------------------
std::string ContourFinder::GetYScale()  const
{
  return y_scale;
}

//--------------------------------------------------------------
// double DistSq(coords x_1, coords x_2)
// {
//   return  pow(x_1.x - x_2.x, 2) + pow(x_1.y - x_2.y, 2) ;
// }

//--------------------------------------------------------------
void ContourFinder::GetSortedContourCoords() const
{
  // std::vector<coords> test_list ;
  // double tmp_dis = DistSq(test_list[0], test_list[1]);

  // for(size_t i = 0 ; i < test_list.size() - 1 ; ++i)
  // {
  //   for(size_t j = i+1 ; j < test_list.size() - 1 ; ++j)
  //     if( tmp_dis > DistSq(test_list[i], test_list[j]) )
  //       tmp_dis = DistSq(test_list[i], test_list[j]);

  // }
  
}

//==============================================================
//  ContourFinder Class ends