#include <omp.h>

// Root
#include <TGraph.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>

// Local headers
#include "../include/ContourFinder.h"
#include "../include/Common.h"
// #include "Instrumentor.h"

//==============================================================
// Cont2D struct begins
//--------------------------------------------------------------
// Constructors
Cont2D::Cont2D(double in_val) : val(in_val) { } 
//--------------------------------------------------------------
size_t Cont2D::size() const 
{
  return pts.size() ;
}

//--------------------------------------------------------------
void Cont2D::SetColor(const CONFIND::Color in_color)
{
  color = in_color ;

  // char tmp[100] ;
  // sprintf(tmp, "Contour color set to '%s'.", color.name().c_str()) ;
  // LOG_INFO(tmp) ;
}

//--------------------------------------------------------------
void Cont2D::SetColor(const size_t in_idx)
{
  color.idx = in_idx ;

  // char tmp[100] ;
  // sprintf(tmp, "Contour color set to '%s'.", color.name().c_str()) ;
  // LOG_INFO(tmp) ;
}

//--------------------------------------------------------------    
CONFIND::Color Cont2D::GetColor() const 
{
  return color;
}

//--------------------------------------------------------------
std::ostream& operator << ( std::ostream &output, const Cont2D& c)
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
void Cont2D::Export(const std::string& f_name, const char* mode)
{
  char tmp_char[100] ;
  sprintf(tmp_char, "%s_%.2e", f_name.c_str(), val ) ;
  Sort() ;
  SaveVec(pts, tmp_char, mode) ;
}

//--------------------------------------------------------------
// Ref: 
// https://www.geeksforgeeks.org/find-simple-closed-path-for-a-given-set-of-points/
//
// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
int Cont2D::Orientation(const CONFIND::Coord3D& p, const CONFIND::Coord3D& q, const CONFIND::Coord3D& r) const
{ 
  double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y); 

  if (val == 0) return 0;  // colinear 
  return (val > 0)? 1: 2; // clockwise or counterclock wise 
} 

//--------------------------------------------------------------
bool Cont2D::comp_Orient(const CONFIND::Coord3D &a, const CONFIND::Coord3D &b) const
{
  // if ( a == bottom_left )
  //   return true;
  // else if ( b == bottom_left)
  //   return false;

  // std::cout << " --> comp_CW " << "\n"<< std::flush ;
  // Find orientation 
  int o = Orientation(bottom_left, a, b); 
  // std::cout << " --> Orientation: " << o << ", a: " << a << ", b: " << b<< "\n"<< std::flush ;

  if (o == 0) 
    // std::cout << " --> Orientation: " << o << ", a: " << a << ", b: " << b<< "\n"<< std::flush ;
    return (bottom_left.XYDist2(a) < bottom_left.XYDist2(b) )? true : false; 

  // CW
  if(sort_cw)
    return (o == 2) ? false: true; 
  else
  // CCW
    return (o == 2) ? true: false; 

  // return ( pow(a.x - pts[0].x, 2) + pow(a.y - pts[0].y, 2) < pow(b.x - pts[0].x, 2) + pow(b.y - pts[0].y, 2)) ;
}
//--------------------------------------------------------------
// // A utility function to swap two points 
// void Cont2D::swap(CONFIND::Coord3D &p1, CONFIND::Coord3D &p2) 
// { 
//   CONFIND::Coord3D temp = p1; 
//   p1 = p2; 
//   p2 = temp; 
// } 

//--------------------------------------------------------------
void Cont2D::Sort()
{
  PROFILE_FUNCTION() ;

  if (already_sorted)
    return;

  // std::vector<CONFIND::Coord3D>::iterator BotLef_it ;

  // // Finding the bottom left point as a reference
  // BotLef_it = std::min_element(pts.begin(), pts.end(), [] (const CONFIND::Coord3D& a, const CONFIND::Coord3D& b) 
  // {
  //   if ( a.y == b.y )
  //     return a.x < b.x;

  //   return a.y < b.y ;
  // } ) ;

  // bottom_left = *BotLef_it ;


  //.......xXXXXXXXXX
    // Find the bottom_left point 
  //  double ymin = pts[0].y ; size_t min = 0; 
  //  for (size_t i = 1; i < pts.size() ; i++) 
  //  { 
  //   // Pick the bottom-most. In case of tie, chose the 
  //   // left most point 
  //   if ( (pts[i].y < ymin) || (ymin == pts[i].y && pts[i].x < pts[min].x)) 
  //     ymin = pts[i].y, min = i; 
  //  } 

  // Find the upper_left point 
    double ymax = pts[0].y ; size_t max = 0; 
   for (size_t i = 1; i < pts.size() ; i++) 
   { 
    // Pick the bottom-most. In case of tie, chose the 
    // left most point 
    if ( (pts[i].y > ymax) || (ymax == pts[i].y && pts[i].x < pts[max].x)) 
      ymax = pts[i].y, max = i; 
   } 
  
   // Place the bottom-most point at first position 
   std::swap(pts[0], pts[max]); 
  //.......xXXXXXXXXX

  bottom_left =  pts[0];
  // std::cout << " bottom_left: " <<bottom_left << ", min_idx: " << min << "\n" ;

  std::sort(pts.begin() + 1, pts.end(), [this] (const CONFIND::Coord3D& a, const CONFIND::Coord3D& b) {
    return comp_Orient(a, b); }) ;

  // Checking if the orientation was correct:
  if ( pts[0].XYDist2(pts[1]) >  pts[0].XYDist2(pts[pts.size() - 1]) )
  {
    sort_cw = !sort_cw ;
    std::sort(pts.begin() + 1, pts.end(), [this] (const CONFIND::Coord3D& a, const CONFIND::Coord3D& b) 
      { return comp_Orient(a, b); }) ;
  }
}

//--------------------------------------------------------------
void Cont2D::Clear()
{
  pts.clear() ;
  already_sorted = false ;
}
//  Cont2D struct ends
//==============================================================

//==============================================================
//  ContourFinder Class begins
//--------------------------------------------------------------
// default Constructor
ContourFinder::ContourFinder() 
{
  logger.SetUnit("ContourFinder") ; 
  graph = new TMultiGraph()  ; 
} 

//--------------------------------------------------------------
// default Destructor
ContourFinder::~ContourFinder() 
{ 
// Log.Warn("\n ====>>> ContourFinder  destructor called! \n") ;
// std::cout<< "\n genFuncPtr: " << genFuncPtr  << " \n" ;
  delete graph ;
  if (cpy_cons_called)  delete genFuncPtr; 
} 
//--------------------------------------------------------------
// Copy constructor
// It will set the graph pointer to NULL
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
  set_mem_func_flag(zc2.set_mem_func_flag), func(zc2.func),
  n_x(zc2.n_x), n_y(zc2.n_y), x_min(zc2.x_min), 
  x_max(zc2.x_max), y_min(zc2.y_min), y_max(zc2.y_max),
  delta_x(zc2.delta_x), delta_y(zc2.delta_y), x_scale(zc2.x_scale),
  y_scale(zc2.y_scale), cont_set(zc2.cont_set)
{
  // std::cout << "ContourFinder copy constructor from " << &zc2 << " -> " << this << " \n" ;
  genFuncPtr = zc2.genFuncPtr->Clone() ;
  graph = NULL;
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
void ContourFinder::SetGridVals(Mode in_mode)
{
  if(!set_func_flag && !set_mem_func_flag)
  {
    LOG_ERROR("Function not set!");
    return ;
  }

  LOG_INFO("==> Setting grid values ...");

  if (in_mode == Optimal)
  {
    double* m_GridValArr = NULL;
    //..........................................
    if (cont_set.size() > 1)
    {
      // # of total corners: (n_x+1)*(n_y+1)
      m_GridValArr = new double[(n_x+1)*(n_y+1)] ;
    }
    //..........................................
    // Finding the first contour
    FindContourOptimal(cont_set[0], m_GridValArr) ;

    if (cont_set.size() > 1)
      // Now the function values are all set inside  'm_GridValArr'
      FindNextContours(m_GridValArr) ;

    set_grid_vals_flag = true ;

    if(m_GridValArr) delete m_GridValArr;
    return ;
  }

  //..........................................
  // Normal & parallel mode
  for (size_t i = 0; i < cont_set.size(); i++)
  {
    if (in_mode == Normal)
      FindContour(cont_set[i]) ;
    else if (in_mode == Parallel)
      FindContourParallel(cont_set[i]) ;
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
void ContourFinder::SetScanMode(const char in_scan_mode)
{
  if(in_scan_mode != 'X' || in_scan_mode != 'Y')
  {
    LOG_ERROR("Valid scan modes are 'X' & 'Y' only!\
    Ignoring the input scan mode, 'X' is assumed.") ;
    return;
  }

  scan_mode = in_scan_mode ;
  set_scan_mode_flage = true ;
  char tmp[100] ;
  sprintf(tmp, "Scan mode is set to '%c'.", in_scan_mode) ;
  LOG_INFO(tmp) ;
}

//--------------------------------------------------------------
char ContourFinder::GetScanMode() const
{
  return scan_mode ;
}

//--------------------------------------------------------------
void ContourFinder::FindContour(Cont2D& cont)
{
  PROFILE_FUNCTION() ;

  char tmp[150] ;
  sprintf(tmp, "==> Finding contour for c = %.2e (%s)...", cont.val,
          cont.color.name().c_str()) ;
  LOG_INFO(tmp) ;

  SetDeltas() ;

  if(scan_mode == 'X')
  {
    for (size_t j = 0; j < n_y; j++)
    {
      for (size_t i = 0; i < n_x; i++)
      {
        Cell new_cell(i, delta_x, j, delta_y, this, cont.val) ;
        new_cell.FindVerts() ;
        new_cell.GetStatus() ;
        cont.AddPts(new_cell.GetContourCoords()) ;
      }
    }
  }
  else // if(scan_mode == 'Y')
  {
    for (size_t i = 0; i < n_x; i++)
    {
      for (size_t j = 0; j < n_y; j++)
      {
        Cell new_cell(i, delta_x, j, delta_y, this, cont.val) ;
        new_cell.FindVerts() ;
        new_cell.GetStatus() ;
        cont.AddPts(new_cell.GetContourCoords()) ;
      }
    }
  }

}

//--------------------------------------------------------------
void ContourFinder::FindContourOptimal(Cont2D& cont, double* in_gridValArr)
{
  PROFILE_FUNCTION() ;

  char tmp[150] ;
  sprintf(tmp, "==> Finding contour for c = %.2e (%s)...", cont.val,
          cont.color.name().c_str()) ;
  LOG_INFO(tmp) ;

  SetDeltas() ;

  std::vector<double> corners;
  corners.reserve(n_x+1);
  double tmp_corner ;

  //============LOOP STARTS============ 
  for (size_t j = 0; j < n_y; j++)
  {
    // corners.clear() ;
    for (size_t i = 0; i < n_x; i++)
    {
      // Instantiate a cell
      Cell new_cell(i, delta_x, j, delta_y, this, cont.val) ;

      // ............................................
      // Except the first column:
      if ( i > 0)
        // Top_Left vertex: (idx = 4), i is the last element added to corners[]
        new_cell.SetVertexZ(4, corners[i]) ;
      //............................................
      // First column (except the first cell):
      if( j > 0 )
        // Bottom_Right vertex: (idx = 2), i+1 is to the right of the cell
        new_cell.SetVertexZ(2, corners[i+1]) ;
      //............................................
      // All cells except the first one at (0,0):
      if( !(i == 0 && j ==0 ) )
        // Bottom_Left vertex : (idx = 1)
        new_cell.SetVertexZ(1, tmp_corner) ;
      //............................................

      new_cell.FindVerts() ;
      new_cell.GetStatus() ;
      cont.AddPts(new_cell.GetContourCoords()) ;

      //............................................
      // Before reaching the wall on the right
      if ( i < n_x - 1)
        tmp_corner = new_cell.GetFuncVals(2) ;
      // If last column, set tmp_corner for the next row
      else
        tmp_corner = corners[0] ;
      //............................................

      if (i==0) // The first vertex in each row
        corners[0] = new_cell.GetFuncVals(4) ;
      // The rest of the row
      corners[i+1] = new_cell.GetFuncVals(3) ;
      //............................................
      if (in_gridValArr)
      {
        // saving the bottom_left vertex
        in_gridValArr[i + (n_x + 1)*j] = new_cell.GetFuncVals(1) ;

        // For the last column, we need to save the bottom-right vertex too
        if( i == n_x - 1)
          in_gridValArr[n_x + (n_x + 1)*j] = new_cell.GetFuncVals(2) ;

        // For the last row, we need to save the top-left vertex too
        if( j == n_y - 1)
          in_gridValArr[i + (n_x + 1)*n_y] = new_cell.GetFuncVals(4) ;

        // For the last top-right corner cell top-right vertex (last element)
        if ( j == n_y - 1 && i == n_x - 1 )
          in_gridValArr[(n_x+1)*(n_y+1) - 1] = new_cell.GetFuncVals(3) ;
        
      }
    }
  }
  //============END of LOOP============ 
}

//--------------------------------------------------------------
void ContourFinder::FindNextContours(double* in_gridValArr)
{
  PROFILE_FUNCTION() ;

  for (size_t k = 1 ; k < cont_set.size() ; ++k)
  {

  char tmp[150] ;
  sprintf(tmp, "==> Finding contour for c = %.2e (%s)...", cont_set[k].val, 
          cont_set[k].color.name().c_str()) ;
  LOG_INFO(tmp) ;

  SetDeltas() ;

  //============LOOP STARTS============ 
  for (size_t j = 0; j < n_y; j++)
  {
    // corners.clear() ;
    for (size_t i = 0; i < n_x; i++)
    {
      // Instantiate a cell
      Cell new_cell(i, delta_x, j, delta_y, this, cont_set[k].val) ;

      // ............................................
      new_cell.SetVertexZ(1, in_gridValArr[i    + (n_x+1) * j     ]) ;
      new_cell.SetVertexZ(2, in_gridValArr[i+1  + (n_x+1) * j     ]) ;
      new_cell.SetVertexZ(3, in_gridValArr[i+1  + (n_x+1) * (j+1) ]) ;
      new_cell.SetVertexZ(4, in_gridValArr[i    + (n_x+1) * (j+1) ]) ;
      //............................................

      new_cell.FindVerts() ;
      new_cell.GetStatus() ;
      cont_set[k].AddPts(new_cell.GetContourCoords()) ;
    }
  }
  //============END of LOOP============ 
  }
  //============END of Contour LOOP============ 
}

//--------------------------------------------------------------
//              !!! Under Construction !!!
void ContourFinder::FindContourParallel(Cont2D& cont)
{
  char tmp[150] ;
  sprintf(tmp, "==> Finding contour for c = %.2e (%s)...", cont.val,
          cont.color.name().c_str()) ;
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
        new_cell.FindVerts() ;
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
  // cont_set.clear() ;
  for (size_t i = 0; i < cont_val_in.size(); i++)
  {
    cont_set.emplace_back(cont_val_in[i]) ;
    cont_set[cont_set.size()-1].SetColor(cont_set.size()-1) ;
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
void ContourFinder::ExportContour(const std::string& f_name, const char* mode)
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
// Plot options
void ContourFinder::SetPlotXLabel(const char* const in_x_label)
{
  x_label = in_x_label;
  set_plotX_label_flag = true ;
}
//--------------------------------------------------------------
void ContourFinder::SetPlotYLabel(const char* const in_y_label)
{
  y_label = in_y_label;
  set_plotY_label_flag = true ;
}
//--------------------------------------------------------------
void ContourFinder::SetPlotLabel(const char* const in_label) 
{
  plot_label = in_label ;
  set_plot_label_flag = true ;
}
//--------------------------------------------------------------
void ContourFinder::SetPlotConnected(const bool in_connected)     
{
  connected_plot = in_connected ;
  set_plot_connected_flag = true ;
}
//--------------------------------------------------------------
TMultiGraph* ContourFinder::GetGraph() 
{
  return graph ;
}

//--------------------------------------------------------------
void ContourFinder::MakeLegend(const bool in_make_leg,
                               const char* const in_head,
                               const char* const in_option) 
{
  make_legend_flag = in_make_leg ;

  // if(in_head != NULL)
  //   legend_header = in_head ;
  
  // if(strcmp(in_option, "user") == 0)
  //   default_legend_opt = false ;
}

//--------------------------------------------------------------
TLegend* ContourFinder::GetLegend() 
{
  return legend;
}

//--------------------------------------------------------------
void ContourFinder::SetLegendLabels(std::vector<std::string> in_labels) 
{
  legend_label_set = in_labels ;
  set_leg_lab_flag = true ;
}

//--------------------------------------------------------------
void ContourFinder::Plot(const std::string& f_name, 
                         const char* const in_main_title,
                         const char* const in_x_title,
                         const char* const in_y_title)
{
  PROFILE_FUNCTION() ;

  if (! set_grid_vals_flag )
  {
    LOG_ERROR("Grid values are not set yet, use 'SetGridVals()' first!");
    return ;
  }

  TCanvas c("c", "Contour Plot", 1200, 1200) ;
  c.SetGrid();
  
  gStyle->SetOptStat(0);

  // TMultiGraph *mg = new TMultiGraph()  ;
  
  TGraph *g[cont_set.size()] ;

  std::vector<double> x_vals ;
  std::vector<double> y_vals ;

  if(make_legend_flag)
  {
    //.....................................
    // Adding Legend
    if(default_legend_opt)
    {
      // legend->SetDefaults() ;
      legend->SetX1(0.7) ;
      legend->SetX2(0.9) ;
      legend->SetY1(0.7) ;
      legend->SetY2(0.9) ;
      legend->SetTextSize(0.03);
    }

    legend->SetHeader(legend_header.c_str(), "C");
    //.....................................
  }

  for(size_t i = 0 ; i < cont_set.size() ; ++i)
  {
    cont_set[i].Sort();

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
    graph->Add(g[i]) ;

    if(make_legend_flag)
    {
      //...................................
      // Adding the legend labels
      if(set_leg_lab_flag)
        legend->AddEntry(g[i], legend_label_set[i].c_str(), "l");
      else
      {
        char tmp_label[30] ;
        sprintf(tmp_label, "%.2e", cont_set[i].val) ;
        legend->AddEntry(g[i], tmp_label, "l");
      } 
      //...................................
    }

    x_vals.clear() ;
    y_vals.clear() ;
  }

  
  if (x_scale == "Log")
    c.SetLogx() ;
  if (y_scale == "Log")
    c.SetLogy() ;

  graph->GetXaxis()->SetLimits(x_min, x_max);
  graph->GetYaxis()->SetLimits(y_min, y_max);

  //.....................................
  // Main Ttile
  //.....................................
  if(in_main_title != NULL)
    graph->SetTitle(in_main_title) ; 
  else
    graph->SetTitle(plot_label.c_str()) ;  
  //.....................................

  //.....................................
  // X Axis
  //.....................................
  // Ttile
  //......................
  if(in_x_title != NULL)
    graph->GetXaxis()->SetTitle(in_x_title) ; 
  else
    graph->GetXaxis()->SetTitle(x_label.c_str()) ; 
  //......................
  graph->GetXaxis()->SetTitleSize(0.03) ;
  graph->GetXaxis()-> SetTitleOffset(1.6);
  //.....................................

  //.....................................
  // Y Axis
  //.....................................
  // Title
  //......................
  if(in_y_title != NULL)
    graph->GetYaxis()->SetTitle(in_y_title) ; 
  else
    graph->GetYaxis()->SetTitle(y_label.c_str()) ; 
  //......................
  graph->GetYaxis()->SetTitleSize(0.03) ;
  graph->GetYaxis()-> SetTitleOffset(1.5);
  //.....................................

  // gStyle->SetTitleFontSize(.08);
  // gStyle->SetLabelSize(.005, "XY");

  if(connected_plot)
    graph->Draw("AL") ;
  else
    graph->Draw("AP") ;
  
  if(make_legend_flag)
    legend->Draw(); 

  c.Update() ;

  // Output file
  char out_file_char[150] ;    
  sprintf(out_file_char, "%s/%s.pdf", wrk_dir_str.c_str(), f_name.c_str()) ;

  c.SaveAs(out_file_char) ;

  // delete graph ;
  
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
// Will clear everything!
void ContourFinder::Clear()
{
  cont_set.clear() ;
  set_n_x_flag         = false ;
  set_n_y_flag         = false ;
  set_x_min_flag       = false ;
  set_x_scale_flag     = false ;
  set_y_scale_flag     = false ;
  set_x_max_flag       = false ;
  set_y_min_flag       = false ;
  set_y_max_flag       = false ;
  set_grid_vals_flag   = false ;
  set_func_flag        = false ;
  set_cont_val_flag    = false ;
  set_mem_func_flag    = false ;
  cpy_cons_called      = false ;

  func = NULL ;
  genFuncPtr = NULL ;
}

//==============================================================
//  ContourFinder Class ends