#include <unordered_set>

// Root
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>

#include <zaki/Util/Logger.h>
#include <zaki/Util/Profile_Timer.h>

#include <zaki/File/SaveVec.h>
#include <zaki/Vector/Vector_Basic.h>


// Local headers
#include "../include/ContourFinder.h"
#include "../include/Common.h"

//==============================================================
// Cont2D struct begins
//--------------------------------------------------------------
// Constructors
CONFIND::Cont2D::Cont2D(double in_val) : val(in_val) { } 
//--------------------------------------------------------------
size_t CONFIND::Cont2D::size() const 
{
  return pts.size() ;
}

//--------------------------------------------------------------
void CONFIND::Cont2D::SetColor(const CONFIND::Color in_color)
{
  color = in_color ;

  char tmp[100] ;
  sprintf(tmp, "Contour color set to '%s'.", color.name().c_str()) ;
  Z_LOG_INFO(tmp) ;
}

//--------------------------------------------------------------
void CONFIND::Cont2D::SetColor(const size_t in_idx)
{
  color.idx = in_idx ;

  char tmp[100] ;
  sprintf(tmp, "Contour color set to '%s'.", color.name().c_str()) ;
  Z_LOG_INFO(tmp) ;
}

//--------------------------------------------------------------    
CONFIND::Color CONFIND::Cont2D::GetColor() const 
{
  return color;
}

//--------------------------------------------------------------
bool CONFIND::Cont2D::GetFound() const 
{
  return is_found_flag ;
}

//--------------------------------------------------------------
void CONFIND::Cont2D::SetFound(const bool in_flag) 
{
  is_found_flag = in_flag;
}

//--------------------------------------------------------------
std::ostream& CONFIND::operator << ( std::ostream &output, const CONFIND::Cont2D& c)
{ 
  output << "\n*        c = " << c.val << "        *\n" ;

  for (size_t i = 0 ; i < c.pts.size() ; i++)
  {
    output << "(" << c[i].x << ", " << c[i].y << ", " << c[i].z << ") ";
  }
  
  return output;            
}

//--------------------------------------------------------------
Zaki::Physics::Coord3D CONFIND::Cont2D::operator[](size_t idx_in) const
{ 
  return pts[idx_in] ;
}

//--------------------------------------------------------------
//  Addition operator
CONFIND::Cont2D CONFIND::Cont2D::operator+(const Cont2D& in_c) const 
{
  if(in_c.val != val)
    Z_LOG_ERROR("Contours with different values cannot be added!") ;
  
  Cont2D out_c(val) ;
  out_c.pts.reserve(size() + in_c.size()) ;

  out_c.pts.insert(out_c.pts.end(), pts.begin(), pts.end())  ;

  out_c.pts.insert(out_c.pts.end(), in_c.pts.begin(), in_c.pts.end())  ;

  return out_c ;
}

//--------------------------------------------------------------
//  += operator overloading
void CONFIND::Cont2D::operator+=(const Cont2D& in_c) 
{
  if(in_c.val != val)
    Z_LOG_ERROR("Contours with different values cannot be added!") ;
  
  pts.reserve(size() + in_c.size()) ;

  pts.insert(pts.end(), in_c.pts.begin(), in_c.pts.end())  ;

}
//--------------------------------------------------------------
void CONFIND::Cont2D::AddPts(const std::vector<Zaki::Physics::Coord3D>& in_pts) 
{
  for (size_t i = 0; i < in_pts.size(); i++)
  {
    pts.push_back(in_pts[i]) ;
  }
}

//--------------------------------------------------------------
void CONFIND::Cont2D::Export(const std::string& f_name, const char* mode)
{
  if (pts.size() == 0)
  {
    char tmp[150] ;
    sprintf(tmp, "Contour '%.2e' has no points within the specified range.", val) ;
    Z_LOG_ERROR(tmp) ;
    return ;
  }  

  char tmp_char[100] ;
  sprintf(tmp_char, "%s_%.2e", f_name.c_str(), val ) ;
  // Sort() ;
  Zaki::File::SaveVec(pts, tmp_char, mode) ;
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
int CONFIND::Cont2D::Orientation(const Zaki::Physics::Coord3D& p, const Zaki::Physics::Coord3D& q, const Zaki::Physics::Coord3D& r) const
{ 
  double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y); 

  if (val == 0) return 0;  // colinear 

  if (0 < val && val < 5*(r.x - q.x)*(q.x - p.x)) return 3;
  if (0 > val && val > -5*(r.x - q.x)*(q.x - p.x)) return 4;

  return (val > 0)? 1: 2; // clockwise or counterclock wise 
} 

//--------------------------------------------------------------
bool CONFIND::Cont2D::comp_Orient(const Zaki::Physics::Coord3D &a, const Zaki::Physics::Coord3D &b) const
{
  // Find orientation 
  int o = Orientation(bottom_left, a, b); 
  // std::cout << " --> Orientation: " << o << ", a: " << a << ", b: " << b<< "\n"<< std::flush ;

  float dis = 6;
  Zaki::Physics::Coord3D test_pt = {2, 2, 0} ;
  if (o == 0) 
    // std::cout << " --> Orientation: " << o << ", a: " << a << ", b: " << b<< "\n"<< std::flush ;
    return (bottom_left.XYDist2(a) < bottom_left.XYDist2(b) )? true : false; 

  if (o == 3 && sort_cw) 
    return (test_pt.XYDist2(a) - test_pt.XYDist2(b) > dis )? false : true; 

  if (o == 4 && sort_cw) 
    return (test_pt.XYDist2(b) - test_pt.XYDist2(a) > dis )? true : false;

  if (o == 3 && !sort_cw) 
    return (test_pt.XYDist2(a) - test_pt.XYDist2(b) > dis )? true : false; 

  if (o == 4 && !sort_cw) 
    return (test_pt.XYDist2(b) - test_pt.XYDist2(a) > dis )? false : true;


  // CW
  if(sort_cw)
    return (o == 2) ? false: true; 
  else
  // CCW
    return (o == 2) ? true: false; 
}

//--------------------------------------------------------------
void CONFIND::Cont2D::RMDuplicates()
{
  std::set<Zaki::Physics::Coord3D> tmp_set(pts.begin(), pts.end());

  pts.clear() ;
  pts.insert(pts.end(), tmp_set.begin(), tmp_set.end());
}

//--------------------------------------------------------------

void CONFIND::Cont2D::SortNew(const std::pair<double, double> del) 
{
  PROFILE_FUNCTION() ;

  if (already_sorted)
    return;

  if (!is_found_flag)
  {
    Z_LOG_ERROR("Contour values hasn't been found yet!") ;
    return ; 
  }

  Z_LOG_ERROR("==> Sorting the points...") ;

  RMDuplicates() ;

  std::vector<Zaki::Physics::Coord3D> out ;
  out.reserve(pts.size());


  // Top
  // double ymax = pts[0].y ; size_t max = 0; 
  // for (size_t i = 1; i < pts.size() ; i++) 
  // { 
  //   if ( (pts[i].y > ymax) || (ymax == pts[i].y && pts[i].x < pts[max].x)) 
  //     ymax = pts[i].y, max = i; 
  // } 

  // Left 
  double xmin = pts[0].x ; size_t min = 0; 
  for (size_t i = 1; i < pts.size() ; i++) 
  { 
    if ( (pts[i].x < xmin) || (xmin == pts[i].x && pts[i].y > pts[min].y)) 
      xmin = pts[i].x, min = i; 
  } 


  size_t j = min ;
  out.push_back(pts[j]) ;

  while(out.size() < pts.size())
  {
    double d_min = INFINITY ;
    size_t idx = 0 ;
    for(size_t i = 0 ; i< pts.size() ; i++)
    {
      if( pts[j].XYDist2(pts[i]) < d_min && !Zaki::Vector::Exists(pts[i], out))
        {
          idx = i ; d_min = pts[j].XYDist2(pts[i]) ;
        }
    }
    out.push_back(pts[idx]) ;
    j = idx ;
  }

  already_sorted = true ;
  pts = out ;

}

//--------------------------------------------------------------
void CONFIND::Cont2D::Sort()
{
  PROFILE_FUNCTION() ;

  if (already_sorted)
    return;

  if (!is_found_flag)
  {
    Z_LOG_ERROR("Contour values hasn't been found yet!") ;
    return ; 
  }

  Z_LOG_INFO("==> Sorting the points...");

  
  RMDuplicates() ;

  // Find the upper_left point 
    double ymax = pts[0].y ; size_t max = 0; 
   for (size_t i = 1; i < pts.size() ; i++) 
   { 
    // Pick the upper_left. In case of tie, choose the 
    // left most point 
    if ( (pts[i].y > ymax) || (ymax == pts[i].y && pts[i].x < pts[max].x)) 
      ymax = pts[i].y, max = i; 
   } 
  
   // Place the upper_left point at first position 
   std::swap(pts[0], pts[max]); 
  //.......xXXXXXXXXX

  bottom_left =  pts[0];

  std::sort(pts.begin() + 1, pts.end(), [this] (const Zaki::Physics::Coord3D& a, const Zaki::Physics::Coord3D& b) {
    return comp_Orient(a, b); }) ;

  // Checking if the orientation was correct:
  if ( pts[0].XYDist2(pts[1]) >  pts[0].XYDist2(pts[pts.size() - 1]) )
  {
    sort_cw = !sort_cw ;
    std::sort(pts.begin() + 1, pts.end(), [this] (const Zaki::Physics::Coord3D& a, const Zaki::Physics::Coord3D& b) 
      { return comp_Orient(a, b); }) ;
  }
}

//--------------------------------------------------------------
void CONFIND::Cont2D::Clear()
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
CONFIND::ContourFinder::ContourFinder() 
{
  graph = new TMultiGraph()  ; 
  legend = new TLegend(0.7, 0.9, 0.7, 0.9) ;
} 

//--------------------------------------------------------------
// default Destructor
CONFIND::ContourFinder::~ContourFinder() 
{ 
  delete graph ;
  delete legend ;
  if (cpy_cons_called)  delete genFuncPtr; 
} 
//--------------------------------------------------------------
// Copy constructor
// It will set the graph & legend pointers to NULL
CONFIND::ContourFinder::ContourFinder(const ContourFinder &zc2) :
  set_grid_flag(zc2.set_grid_flag),
  set_grid_vals_flag(zc2.set_grid_vals_flag),
  set_func_flag(zc2.set_func_flag),
  set_cont_val_flag(zc2.set_cont_val_flag),
  set_mem_func_flag(zc2.set_mem_func_flag), func(zc2.func),
  delta_x(zc2.delta_x), delta_y(zc2.delta_y), 
  cont_set(zc2.cont_set)
{
  genFuncPtr = zc2.genFuncPtr->Clone() ;
  graph = NULL;
  legend = NULL ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetGrid(const Zaki::Math::Grid2D& g) 
{
  grid = g ;
  set_grid_flag = true ; 
  SetDeltas() ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetWidth(const size_t width_in)  
{
  width = width_in ;
  set_width_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetHeight(const size_t height_in)  
{
  height = height_in ;
  set_height_flag = true ;
}

//--------------------------------------------------------------
size_t CONFIND::ContourFinder::GetN_X() const
{
  if(! set_grid_flag)
  {
    Z_LOG_ERROR("Grid not set!") ;
    return 0 ;
  }

  return grid.xAxis.res;
}

//--------------------------------------------------------------
size_t CONFIND::ContourFinder::GetN_Y() const 
{
  if(! set_grid_flag)
  {
    Z_LOG_ERROR("Grid not set!") ;
    return 0 ;
  }

  return grid.yAxis.res;
}

//--------------------------------------------------------------
double CONFIND::ContourFinder::GetX_Min() const
{
  if(! set_grid_flag)
  {
    Z_LOG_ERROR("Grid not set!") ;
    return 0 ;
  }

  return grid.xAxis.Min();
}

//--------------------------------------------------------------
double CONFIND::ContourFinder::GetX_Max() const
{  
  if(! set_grid_flag)
  {
    Z_LOG_ERROR("Grid not set!") ;
    return 0 ;
  }

  return grid.xAxis.Max();
}

//--------------------------------------------------------------
double CONFIND::ContourFinder::GetY_Min() const
{
  if(! set_grid_flag)
  {
    Z_LOG_ERROR("Grid not set!") ;
    return 0 ;
  }

  return grid.yAxis.Min();
}

//--------------------------------------------------------------
double CONFIND::ContourFinder::GetY_Max() const
{
  if(! set_grid_flag)
  {
    Z_LOG_ERROR("Grid not set!") ;
    return 0 ;
  }

  return grid.yAxis.Max();
}

//--------------------------------------------------------------
std::pair<double, double> CONFIND::ContourFinder::ij_2_xy(size_t i, size_t j) const
{
  // double delta_x = (x_max - x_min) / n_x ;
  // double delta_y = (y_max - y_min) / n_y ;

  double x = grid.xAxis.Min() + i*delta_x ;
  double y = grid.yAxis.Min() + j*delta_y ;

  std::pair<double, double> coordinate = {x, y} ;

  return coordinate ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetGridVals(const Mode& in_mode)
{
  if(!set_func_flag && !set_mem_func_flag)
  {
    Z_LOG_ERROR("Function not set!");
    return ;
  }

  Z_LOG_INFO("==> Setting grid values ...");

  algorithm = in_mode ;

  if (algorithm == Optimal)
    FindOptimalMode() ; // will replace algorithm with the best mode

  // Fast Mode
  if (algorithm == Fast)
  {
    double* m_GridValArr = NULL;
    //..........................................
    // if (cont_set.size() > 1)
    if (unfound_contours > 1)
    {
      // # of total corners: (n_x+1)*(n_y+1)
      m_GridValArr = new double[(grid.xAxis.res+1)*(grid.yAxis.res+1)] ;
    }
    //..........................................
    // Finding the first 'unfound' contour
    FindContourFast(cont_set[cont_set.size()-unfound_contours], m_GridValArr) ;

    if (unfound_contours > 0)
      // Now the function values are all set inside  'm_GridValArr'
      FindNextContours(m_GridValArr) ;

    if(m_GridValArr) delete m_GridValArr;
    set_grid_vals_flag = true ;
    return ;
  }
  //..........................................
  //  Ludicrous Mode
  else if(algorithm == Ludicrous)
  {
    FindContourLudicrous() ;
    set_grid_vals_flag = true ;
    return ;
  }

  //..........................................
  // Normal & parallel mode
  for (size_t i = 0; i < cont_set.size(); i++)
  {
    // If the contour is already found
    if (cont_set[i].GetFound())
    {
      // skip to the next contour
      continue ;
    }
    
    if (algorithm == Normal)
      FindContour(cont_set[i]) ;
    else if (algorithm == Parallel)
      FindContourParallel(cont_set[i]) ;
  }

  set_grid_vals_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetDeltas()
{
  if (grid.xAxis.scale == "Linear")
  {
    delta_x = (grid.xAxis.Max() - grid.xAxis.Min()) / grid.xAxis.res ;
  }
  else if (grid.xAxis.scale == "Log")
  {
    delta_x = (log10(grid.xAxis.Max()) - log10(grid.xAxis.Min())) / grid.xAxis.res ;
  }

  if (grid.yAxis.scale == "Linear")
  {
    delta_y = (grid.yAxis.Max() - grid.yAxis.Min()) / grid.yAxis.res ;
  }
  else if (grid.yAxis.scale == "Log")
  {
    delta_y = (log10(grid.yAxis.Max()) - log10(grid.yAxis.Min())) / grid.yAxis.res ;
  }
}

//--------------------------------------------------------------
std::pair<double, double> CONFIND::ContourFinder::GetDeltas() const
{
  if (!set_grid_flag)
    {
      Z_LOG_ERROR("Grid is not set!") ;
      return {-1, -1};
    }
  return {delta_x, delta_y} ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetScanMode(const char in_scan_mode)
{
  if(in_scan_mode != 'X' || in_scan_mode != 'Y')
  {
    Z_LOG_ERROR("Valid scan modes are 'X' & 'Y' only!\
    Ignoring the input scan mode, 'X' is assumed.") ;
    return;
  }

  scan_mode = in_scan_mode ;
  set_scan_mode_flage = true ;
  char tmp[100] ;
  sprintf(tmp, "Scan mode is set to '%c'.", in_scan_mode) ;
  Z_LOG_INFO(tmp) ;
}

//--------------------------------------------------------------
char CONFIND::ContourFinder::GetScanMode() const
{
  return scan_mode ;
}

//--------------------------------------------------------------
// Finding the optimal mode:
//  ContourFinder will find the best mode depending on the 
//  average function call time. By default, '50' points are
//  randomly chosen on the grid to ensure a more precise
//  decision. 'SetOptimizationTrials' can be used to change 
//  this number for cases where some areas of the grid
//  might be unusually fast or slow.
void CONFIND::ContourFinder::FindOptimalMode()
{
  Z_LOG_INFO("Finding the optimal mode...") ;
  
  //seed
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  random_engine.seed(seed); // seeding the engine

  double x_low_bound, x_up_bound, y_low_bound, y_up_bound;

  // X axis
  if(grid.xAxis.scale == "Linear")
  {
    x_low_bound = grid.xAxis.Min() ;
    x_up_bound = grid.xAxis.Max() ;
  }
  else // Log scale
  {
    x_low_bound = log10(grid.xAxis.Min()) ;
    x_up_bound = log10(grid.xAxis.Max()) ;
  }
  
  // Y-axis
  if(grid.yAxis.scale == "Linear")
  {
    y_low_bound = grid.yAxis.Min() ;
    y_up_bound = grid.yAxis.Max() ;
  }
  else // Log scale
  {
    y_low_bound = log10(grid.yAxis.Min()) ;
    y_up_bound = log10(grid.yAxis.Max()) ;
  }
  
  // distribution for x values
  std::uniform_real_distribution<double> x_di(x_low_bound,x_up_bound);

  // distribution for y values
  std::uniform_real_distribution<double> y_di(y_low_bound,y_up_bound);

  algorithm = TimeFunc(x_di, y_di) ;
  Z_LOG_INFO("Optimal algorithm is set.") ;

}

//--------------------------------------------------------------
// Sets the optimization_trials value in 'TimeFunc'
// Also see: 'FindOptimalMode'
void CONFIND::ContourFinder::SetOptimizationTrials(int in_val)
{
  optimization_trials = in_val ;
}

//--------------------------------------------------------------
// Timing the function or member-function
CONFIND::ContourFinder::Mode CONFIND::ContourFinder::TimeFunc(
  std::uniform_real_distribution<double>& in_x_dis,
  std::uniform_real_distribution<double>& in_y_dis ) 
{
  
  Zaki::Util::TimeProfileManager::BeginSpecialSession("FuncTiming") ;

  double x=0, y = 0; 
  for (size_t i = 0; i < optimization_trials; i++)
  {
    x = in_x_dis(random_engine) ;
    y = in_y_dis(random_engine) ;

    if (grid.xAxis.scale == "Log" && grid.yAxis.scale == "Log")
     { x = pow(10, x) ; y = pow(10, y) ; }

    else if(grid.xAxis.scale == "Linear" && grid.yAxis.scale == "Log")
      y = pow(10, y) ;

    else if(grid.xAxis.scale == "Log" && grid.yAxis.scale == "Linear")
      x =pow(10, x) ;

    if(func)
    { 
      // Timing scope
      Z_TIME_PROFILE_SIMPLE("Func_"+std::to_string(i)) ;
      func(x,y) ;
    }
    else if(genFuncPtr)
    { 
      // Timing scope
      Z_TIME_PROFILE_SIMPLE("Mem_Func_"+std::to_string(i)) ;
      genFuncPtr->Eval(x,y) ;
    }
    else
    {
    Z_LOG_WARNING(
      "Optimal mode wasn't found because neither a function or a"
      "member function is set. Use 'SetFunc' or 'SetMemFunc' first"
      " and try again.") ;
      break;
    }
  }

  std::vector<double> timing_results =
  Zaki::Util::TimeProfileManager::GetSpecialValues() ;

  Zaki::Util::TimeProfileManager::EndSpecialSession() ;

  // Analyzing the results
  double mean = 0, min = timing_results[0], max = timing_results[0] ;
  for (size_t i = 0; i < timing_results.size(); i++)
  {
    mean += timing_results[i] / timing_results.size() ;

    if (timing_results[i] < min)
      min = timing_results[i] ;

    if (timing_results[i] > max)
      max = timing_results[i] ;
  }

  // Change of beahviour happens around this time
  double threshold_time = 2e-2 ; // in ms

  // For now we only use the mean value
  // If grid size is small the threshold time is a bit higher
  if( grid.xAxis.res < 50 && grid.yAxis.res < 50 )
    threshold_time = 4e-2;

  if(mean > threshold_time)
  {
    // Ludicrous mode is the optimal mode
    Z_LOG_INFO("Ludicrous mode is the optimal mode!") ;
    return Mode::Ludicrous ;
  }
  else
  {
    // Fast mode is the optimal mode
    Z_LOG_INFO("Fast mode is the optimal mode!") ;
    return Mode::Fast ;
  }
    
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::FindContour(Cont2D& cont)
{
  PROFILE_FUNCTION() ;

  char tmp[150] ;
  sprintf(tmp, "==> Finding contour for c = %.2e (%s)...", cont.val,
          cont.color.name().c_str()) ;
  Z_LOG_INFO(tmp) ;

  // SetDeltas() ;

  Bundle b(grid, cont, genFuncPtr, func) ;

  if(scan_mode == 'X')
  {
    for (size_t j = 0; j < grid.yAxis.res; j++)
    {
      for (size_t i = 0; i < grid.xAxis.res; i++)
      {
        Cell new_cell(i, delta_x, j, delta_y, &b, cont.val) ;
        new_cell.FindVerts() ;
        new_cell.GetStatus() ;
        cont.AddPts(new_cell.GetContourCoords()) ;
      }
    }
  }
  else // if(scan_mode == 'Y')
  {
    for (size_t i = 0; i < grid.xAxis.res; i++)
    {
      for (size_t j = 0; j < grid.yAxis.res; j++)
      {
        Cell new_cell(i, delta_x, j, delta_y, &b, cont.val) ;
        new_cell.FindVerts() ;
        new_cell.GetStatus() ;
        cont.AddPts(new_cell.GetContourCoords()) ;
      }
    }
  }

  // Marking the contour as found
  cont.SetFound();
  unfound_contours--;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::FindContourFast(Cont2D& cont, double* in_gridValArr)
{
  PROFILE_FUNCTION() ;

  char tmp[150] ;
  sprintf(tmp, "==> Finding contour for c = %.2e (%s)...", cont.val,
          cont.color.name().c_str()) ;
  Z_LOG_INFO(tmp) ;

  // SetDeltas() ;

  std::vector<double> corners;
  corners.reserve(grid.xAxis.res+1);
  double tmp_corner ;

  Bundle b(grid, cont, genFuncPtr, func) ;

  int n_x = grid.xAxis.res ;
  int n_y = grid.yAxis.res ;
  //============LOOP STARTS============ 
  for (size_t j = 0; j < n_y; j++)
  {
    // corners.clear() ;
    for (size_t i = 0; i < n_x; i++)
    {
      // Instantiate a cell
      Cell new_cell(i, delta_x, j, delta_y, &b, cont.val) ;

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

  // Marking the contour as found
  cont.SetFound();
  unfound_contours--;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::FindNextContours(double* in_gridValArr)
{
  PROFILE_FUNCTION() ;

  for (size_t k = 1 ; k < cont_set.size() ; ++k)
  {

    // Skip the contours that are already found
    if (cont_set[k].GetFound())
    {
      continue ;
    }
    
    char tmp[150] ;
    sprintf(tmp, "==> Finding contour for c = %.2e (%s)...", cont_set[k].val, 
            cont_set[k].color.name().c_str()) ;
    Z_LOG_INFO(tmp) ;

    // SetDeltas() ;

    Bundle b(grid, cont_set[k], genFuncPtr, func) ;
    int n_x = grid.xAxis.res ;
    int n_y = grid.yAxis.res ;
    //============LOOP STARTS============ 
    for (size_t j = 0; j < n_y; j++)
    {
      // corners.clear() ;
      for (size_t i = 0; i < n_x; i++)
      {
        // Instantiate a cell
        Cell new_cell(i, delta_x, j, delta_y, &b, cont_set[k].val) ;

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
    // Marking the contour as found
    cont_set[k].SetFound();
    unfound_contours--;
  }
  //============END of Contour LOOP============ 
}

//--------------------------------------------------------------
//  Ludicrous mode: Combination of fast & parallel modes
void CONFIND::ContourFinder::FindContourLudicrous() 
{
  PROFILE_FUNCTION() ;

  // Including only the unfound contours
  std::vector<Cont2D> 
  curr_cont_set(cont_set.end() - unfound_contours, cont_set.end()) ; 

  int n_t = 1 ;
  omp_set_num_threads(req_threads) ;
  Z_LOG_INFO( ("Threads requested: " + std::to_string(req_threads)).c_str()) ;

  // ..............................
  // Parallel Region
  // ..............................
  #pragma omp parallel
  {
    //.......................................
    // Figuring out the thread id & numbers
    #pragma omp single
    {
      n_t = omp_get_num_threads() ;
      Z_LOG_INFO( ("Working Threads: " + std::to_string(n_t)).c_str()) ;
    }

    int t_n = omp_get_thread_num() ;
    //.......................................
    std::vector<Cont2D> loc_con_set ;
    #pragma omp critical
      loc_con_set = curr_cont_set;

    //.......................................
    // Defining local sub_grids:
    double y_min = grid.yAxis.Min() + t_n * (grid.yAxis.Max() - grid.yAxis.Min())/(1.0*n_t) ;
    double y_max = grid.yAxis.Min() + (t_n+1) * (grid.yAxis.Max() - grid.yAxis.Min())/(1.0*n_t) ;
    int res      = grid.yAxis.res / n_t + 1  ; 

    Zaki::Math::Grid2D loc_grid = {grid.xAxis, 
                    {{y_min, y_max}, res, grid.yAxis.scale}};

    CONFIND::Bundle b(loc_grid, genFuncPtr, func);

    std::vector<double> loc_gridVal;
    // If more than one contour
    if (unfound_contours > 1)
    {
      loc_gridVal.resize((loc_grid.xAxis.res+1)*(loc_grid.yAxis.res+1)) ;
    }
    //.......................................
    // Contour set loop begins
    //.......................................
    for (size_t i = 0; i < loc_con_set.size(); i++)
    {
      size_t tmp_first = 0 ;
      // Skip the contours that are already found
      if (loc_con_set[i].GetFound())
        { continue ; tmp_first++ ;}

      char tmp[150] ;
      sprintf(tmp, "(Thread: %d) ==> Finding contour for c = %.2e (%s)...",
        t_n, loc_con_set[i].val,
        loc_con_set[i].color.name().c_str()) ;
      Z_LOG_INFO(tmp) ;
      // .............................

      b.AddCont(loc_con_set[i]) ;
      // This should be intensive
      if (i==tmp_first)
      {  
        ThreadTaskLudicrous(b, loc_gridVal) ;
      }
      else  // Not the first contour 
      {
        ThreadNextTaskLudicrous(b, loc_gridVal) ;
      }
      // .............................


      loc_con_set[i] += b.GetCont() ;
      loc_con_set[i].SetFound();
    }
    //.......................................
    // Contour set loop ends
    //.......................................


    #pragma omp critical
    {
      // Need to make sure we add the points 
      // only to the unfound contours
      //  unfound contours = loc_con_set.size()
      for (size_t i = 0; i < loc_con_set.size(); i++)
      {
        cont_set[ cont_set.size() - loc_con_set.size() + i] += loc_con_set[i] ;
      }     
    }
  }
  // ..............................
  // Parallel Region Ends

  for (size_t i = 0; i < cont_set.size(); i++)
  {
    // Marking the contour as found
    cont_set[i].SetFound();
    unfound_contours--;
  }

  return;
}

//--------------------------------------------------------------
//  Ludicrous mode: Combination of fast & parallel modes
void CONFIND::ContourFinder::ThreadNextTaskLudicrous(Bundle& in_b,
 std::vector<double>& in_vec) 
{
  PROFILE_FUNCTION() ;


  int n_x = in_b.Grid.xAxis.res ;
  int n_y = in_b.Grid.yAxis.res ;
  //============LOOP STARTS============ 
  for (size_t j = 0; j < n_y; j++)
  {
    // corners.clear() ;
    for (size_t i = 0; i < n_x; i++)
    {
      // Instantiate a cell
      Cell new_cell(i, delta_x, j, delta_y, &in_b, in_b.Con.val) ;

      // ............................................
      new_cell.SetVertexZ(1, in_vec[i    + (n_x+1) * j     ]) ;
      new_cell.SetVertexZ(2, in_vec[i+1  + (n_x+1) * j     ]) ;
      new_cell.SetVertexZ(3, in_vec[i+1  + (n_x+1) * (j+1) ]) ;
      new_cell.SetVertexZ(4, in_vec[i    + (n_x+1) * (j+1) ]) ;
      //............................................

      new_cell.FindVerts() ;
      new_cell.GetStatus() ;
      in_b.Con.AddPts(new_cell.GetContourCoords()) ;
    }
  }
  //============END of LOOP============ 
}

//--------------------------------------------------------------
// Task for each thread
void CONFIND::ContourFinder::ThreadTaskLudicrous(Bundle& in_b, std::vector<double>& in_vec)
{
  std::vector<double> corners;
  corners.reserve(grid.xAxis.res+1);
  double tmp_corner ;

  int n_x = in_b.Grid.xAxis.res ;
  int n_y = in_b.Grid.yAxis.res ;
  //============LOOP STARTS============ 
  for (size_t j = 0; j < n_y; j++)
  {
    // corners.clear() ;
    for (size_t i = 0; i < n_x; i++)
    {
      // Instantiate a cell
      Cell new_cell(i, delta_x, j, delta_y, &in_b, in_b.Con.val) ;

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
      in_b.Con.AddPts(new_cell.GetContourCoords()) ;

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
      // double* in_gridValArr  = NULL ;
      if (in_vec.size() != 0)
      {
        // saving the bottom_left vertex
        in_vec[i + (n_x + 1)*j] = new_cell.GetFuncVals(1) ;

        // For the last column, we need to save the bottom-right vertex too
        if( i == n_x - 1)
          in_vec[n_x + (n_x + 1)*j] = new_cell.GetFuncVals(2) ;

        // For the last row, we need to save the top-left vertex too
        if( j == n_y - 1)
          in_vec[i + (n_x + 1)*n_y] = new_cell.GetFuncVals(4) ;

        // For the last top-right corner cell top-right vertex (last element)
        if ( j == n_y - 1 && i == n_x - 1 )
          in_vec[(n_x+1)*(n_y+1) - 1] = new_cell.GetFuncVals(3) ;
      }
    }
  }
  //============END of LOOP============ 

}

//--------------------------------------------------------------
// Setting the number of threads
void CONFIND::ContourFinder::SetThreads(int in_num) 
{
  req_threads = in_num ;
}

//--------------------------------------------------------------
// Parallel evaluation of the contours
void CONFIND::ContourFinder::FindContourParallel(Cont2D& cont)
{
  PROFILE_FUNCTION() ;

  char tmp[150] ;
  sprintf(tmp, "==> Finding contour for c = %.2e (%s)...", cont.val,
          cont.color.name().c_str()) ;
  Z_LOG_INFO(tmp) ;

  int n_t = 1 ;
  omp_set_num_threads(req_threads) ;
  Z_LOG_INFO( ("Threads requested: " + std::to_string(req_threads)).c_str()) ;

  // ..............................
  // Parallel Region
  // ..............................
  #pragma omp parallel
  {
    #pragma omp single
    {
      n_t = omp_get_num_threads() ;
      Z_LOG_INFO( ("Working Threads: " + std::to_string(n_t)).c_str()) ;
    }

    int t_n = omp_get_thread_num() ;

    double y_min = grid.yAxis.Min() + t_n * (grid.yAxis.Max() - grid.yAxis.Min())/(1.0*n_t) ;
    double y_max = grid.yAxis.Min() + (t_n+1) * (grid.yAxis.Max() - grid.yAxis.Min())/(1.0*n_t) ;
    int res      = grid.yAxis.res / n_t + 1  ; 

    Zaki::Math::Grid2D loc_grid = {grid.xAxis, 
                    {{y_min, y_max}, res, grid.yAxis.scale}};

    // takes the contour level (value)
    Cont2D loc_cont(cont.val);
    CONFIND::Bundle b(loc_grid, loc_cont, genFuncPtr, func);

    // This should be intensive
    ThreadTask(b) ;

    #pragma omp critical
    cont += b.GetCont() ;
  }
  // ..............................

  // Marking the contour as found
  cont.SetFound();
  unfound_contours--;

}

//--------------------------------------------------------------
// Task for each thread
void CONFIND::ContourFinder::ThreadTask(Bundle& in_b)
{

  for (size_t j = 0; j < in_b.Grid.yAxis.res; j++)
  {
    for (size_t i = 0; i < in_b.Grid.xAxis.res; i++)
    {
      Cell new_cell(i, delta_x, j, delta_y, &in_b, in_b.Con.val) ;
      new_cell.FindVerts() ;
      new_cell.GetStatus() ;
      in_b.Con.AddPts(new_cell.GetContourCoords()) ;
    }
  }

}

//--------------------------------------------------------------
void CONFIND::ContourFinder::Print() const
{
  if (! set_grid_vals_flag )
    {
      Z_LOG_ERROR("Grid values are not set yet, use 'SetGridVals()' first!");
      return ;
    }
  
  for (size_t i = 0; i < cont_set.size(); i++)
  {
    char tmp[100] ;
    sprintf(tmp, "==> Printing Contour = %f ...", cont_set[i].val) ;
    Z_LOG_INFO(tmp);

    std::cout << cont_set[i] ;
  }
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetFunc(double (*f)(double, double) ) 
{
  func = f;

  if(func) 
  {
    Z_LOG_INFO("Function is set.") ;
    set_func_flag = true ;

    // Reseting 'genFuncPtr'
    if(genFuncPtr)
    {
      genFuncPtr = NULL ;
      set_mem_func_flag = false ; 
    }
  }
  else
    Z_LOG_INFO("Function is not set, because the input is a NULL pointer.") ;

}

//--------------------------------------------------------------
// Non-static mem-funcs
void CONFIND::ContourFinder::SetMemFunc(Zaki::Math::Func2D* gen_Fun) 
{
  genFuncPtr = gen_Fun ;

  if(genFuncPtr) 
  {
    Z_LOG_INFO("Member function is set.") ;
    set_mem_func_flag = true ;

    // Reseting 'func'
    if(func)
    {
      func = NULL ;
      set_func_flag = false ; 
    }
  }
  else
    Z_LOG_INFO("Member function is not set, because the input is a NULL pointer.") ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetContVal(const std::vector<double>& cont_val_in) 
{
  // cont_set.clear() ;
  for (size_t i = 0; i < cont_val_in.size(); i++)
  {
    cont_set.emplace_back(cont_val_in[i]) ;
    cont_set[cont_set.size()-1].SetColor(cont_set.size()-1) ;
  }

  // Kepping track of contours
  unfound_contours += cont_val_in.size() ;
  set_cont_val_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::ExportContour(const std::string& f_name, const char* mode)
{
  if (! set_grid_vals_flag )
  {
    Z_LOG_ERROR("Grid values are not set yet, use 'SetGridVals()' first!");
    return ;
  }

  for(size_t i = 0 ; i < cont_set.size() ; ++i)
  {
    if (!cont_set[i].GetFound())
    {
      char tmp[150] ;
      sprintf(tmp, "Contour '%.2e' hasn't been found yet, skipping to the next one.", cont_set[i].val) ;
      Z_LOG_WARNING(tmp) ;
      continue ;
    }  
    cont_set[i].Export(wrk_dir_str + "/" + f_name, mode) ;
  }

  char tmp_char[150] ;
  sprintf(tmp_char, "Contours exported to %s_[CONT].dat.", f_name.c_str() ) ;
  Z_LOG_INFO(tmp_char) ;
}
//--------------------------------------------------------------
// Plot options
void CONFIND::ContourFinder::SetPlotXLabel(const char* const in_x_label)
{
  x_label = in_x_label;
  set_plotX_label_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetPlotYLabel(const char* const in_y_label)
{
  y_label = in_y_label;
  set_plotY_label_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetPlotLabel(const char* const in_label) 
{
  plot_label = in_label ;
  set_plot_label_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetPlotConnected(const bool in_connected)     
{
  connected_plot = in_connected ;
  set_plot_connected_flag = true ;
}

//--------------------------------------------------------------
TMultiGraph* CONFIND::ContourFinder::GetGraph() 
{
  return graph ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::MakeLegend(const bool in_make_leg,
                               const char* const in_head,
                               const char* const in_option) 
{
  make_legend_flag = in_make_leg ;

  if(in_head != NULL)
    legend_header = in_head ;
  
  if(strcmp(in_option, "user") == 0)
    default_legend_opt = false ;
}

//--------------------------------------------------------------
TLegend* CONFIND::ContourFinder::GetLegend() 
{
  return legend;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetLegendLabels(const std::vector<std::string>& in_labels) 
{
  legend_label_set = in_labels ;
  set_leg_lab_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::Plot(const std::string& f_name, 
                         const char* const in_main_title,
                         const char* const in_x_title,
                         const char* const in_y_title)
{
  PROFILE_FUNCTION() ;

  if (! set_grid_vals_flag )
  {
    Z_LOG_ERROR("Grid values are not set yet, use 'SetGridVals()' first!");
    return ;
  }

  Z_LOG_INFO("==> Plotting contours...");

  TCanvas c("c", "Contour Plot", width, height) ;
  c.SetGrid();
  
  gStyle->SetOptStat(0);

  // .....................................
  // Main Ttile
  //.....................................
  if(in_main_title != NULL)
    graph->SetTitle(in_main_title) ; 
  else
    graph->SetTitle(plot_label.c_str()) ;  
  //.....................................

  // gStyle->SetTitleAlign(33)  ;
  // gStyle->SetTitleX(.99)     ;
  gStyle->SetTitleOffset(10) ;

  TGraph *g[cont_set.size()] ;

  std::vector<double> x_vals ;
  std::vector<double> y_vals ;

  if(make_legend_flag)
  {

    //.....................................
    // Adding Legend
    if(default_legend_opt)
    {
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

    if (!cont_set[i].GetFound())
    {
      char tmp[150] ;
      sprintf(tmp, "Contour '%.2e' hasn't been found yet, skipping to the next one.", cont_set[i].val) ;
      Z_LOG_WARNING(tmp) ;
      continue ;
    }

    if (cont_set[i].pts.size() == 0)
    {
      char tmp[150] ;
      sprintf(tmp, "Contour '%.2e' has no points within the specified range.", cont_set[i].val) ;
      Z_LOG_ERROR(tmp) ;
      continue ;
    }  

    // cont_set[i].Sort();
    if(connected_plot)
      cont_set[i].SortNew(GetDeltas());


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

  // .....................................
  // Axis Scale  & Limits
  //.....................................
  if (grid.xAxis.scale == "Log")
    c.SetLogx() ;
  if (grid.yAxis.scale == "Log")
    c.SetLogy() ;
  //...............
  graph->GetXaxis()->SetLimits(grid.xAxis.Min(), grid.xAxis.Max());
  graph->GetYaxis()->SetLimits(grid.yAxis.Min(), grid.yAxis.Max());
  // .....................................

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
std::string CONFIND::ContourFinder::GetXScale()  const
{
  return grid.xAxis.scale;
}

//--------------------------------------------------------------
std::string CONFIND::ContourFinder::GetYScale()  const
{
  return grid.yAxis.scale;
}

//--------------------------------------------------------------
// Will clear everything!
void CONFIND::ContourFinder::Clear()
{
  cont_set.clear() ;
  set_grid_flag         = false ;
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