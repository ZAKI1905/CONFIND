#include <unordered_set>
#include <typeinfo>

// Root
// #include <TGraph.h>
// #include <TMultiGraph.h>
// #include <TAxis.h>
// #include <TCanvas.h>
// #include <TStyle.h>
// #include <TLegend.h>

#include <Zaki/Util/Logger.hpp>
#include <Zaki/Util/Profile_Timer.hpp>
#include <Zaki/Util/ObjObserver.hpp>
#include <Zaki/Vector/Vector_Basic.hpp>


// Local headers
#include "Confind/ContourFinder.hpp"
#include "Confind/Bundle.hpp"
#include "Confind/Common.hpp"

//==============================================================
//  ContourFinder Class begins
//--------------------------------------------------------------
// Default Constructor
CONFIND::ContourFinder::ContourFinder() 
  : Base("ContourFinder", true)
{
  Z_LOG_NOTE("ContourFinder constructor called from " 
              + PtrStr() + ".") ;

  // These two are automatically deleted by root:
  // graph = new TMultiGraph()  ; 
  // legend = new TLegend(0.7, 0.9, 0.7, 0.9) ;
} 

//--------------------------------------------------------------
// Destructor
CONFIND::ContourFinder::~ContourFinder() 
{ 
  Z_LOG_NOTE("ContourFinder destructor called from " 
              + PtrStr() + ".") ;
  // delete graph ;
  // delete legend ;
  // if (cpy_cons_called)  delete genFuncPtr; 
} 

//--------------------------------------------------------------
// Assignment operator
CONFIND::ContourFinder&
CONFIND::ContourFinder::operator=(const ContourFinder &other) 
{
  Z_LOG_NOTE("ContourFinder '=' operator called: " +
               PtrStr() + " <-- " + other.PtrStr() + ".") ;

  if(this == &other) return *this ;
  else
  {
  set_height_flag = other.set_height_flag ;
  set_width_flag = other.set_width_flag ;
  set_grid_flag = other.set_grid_flag ;
  set_grid_vals_flag = other.set_grid_vals_flag ;
  set_func_flag = other.set_func_flag ;
  set_cont_val_flag = other.set_cont_val_flag ;
  set_mem_func_flag = other.set_mem_func_flag ;
  set_plotX_label_flag = other.set_plotX_label_flag ;
  set_plotY_label_flag = other.set_plotY_label_flag ;
  set_plot_label_flag = other.set_plot_label_flag ;
  set_plot_connected_flag = other.set_plot_connected_flag ;
  cpy_cons_called = other.cpy_cons_called ;
  set_scan_mode_flage = other.set_scan_mode_flage ;
  set_leg_lab_flag = other.set_leg_lab_flag ;
  make_legend_flag = other.make_legend_flag ;
  scan_mode = other.scan_mode ;
  algorithm = other.algorithm ;
  func = other.func ;
  legend_label_set = other.legend_label_set ;
  legend_header = other.legend_header ;
  default_legend_opt = other.default_legend_opt ;
  optimization_trials = other.optimization_trials ;
  random_engine = other.random_engine ;
  grid = other.grid ; width = other.width ; height = other.height ;
  delta_x = other.delta_x ; delta_y = other.delta_y ; 
  x_label = other.x_label ; y_label = other.y_label ;
  plot_label = other.plot_label ; connected_plot = other.connected_plot ;
  cont_set = other.cont_set ; req_threads = other.req_threads ;
  unfound_contours = other.unfound_contours ;

  // Pointer member variables
  if(other.genFuncPtr)
    genFuncPtr = other.genFuncPtr->Clone() ;

  // if(other.graph)
  //   graph = dynamic_cast<TMultiGraph*> (other.graph->Clone());

  // if(other.legend)
  //   legend = dynamic_cast<TLegend*>(other.legend->Clone()) ;

  return *this ;
  }
}

//--------------------------------------------------------------
// Copy constructor
// Copies everything!
CONFIND::ContourFinder::ContourFinder(const ContourFinder &zc2) 
  : Base("ContourFinder", true),
  set_height_flag(zc2.set_height_flag),
  set_width_flag(zc2.set_width_flag),
  set_grid_flag(zc2.set_grid_flag),
  set_grid_vals_flag(zc2.set_grid_vals_flag),
  set_func_flag(zc2.set_func_flag),
  set_cont_val_flag(zc2.set_cont_val_flag),
  set_mem_func_flag(zc2.set_mem_func_flag),
  set_plotX_label_flag(zc2.set_plotX_label_flag),
  set_plotY_label_flag(zc2.set_plotY_label_flag),
  set_plot_label_flag(zc2.set_plot_label_flag),
  set_plot_connected_flag(zc2.set_plot_connected_flag),
  cpy_cons_called(zc2.cpy_cons_called),
  set_scan_mode_flage(zc2.set_scan_mode_flage),
  set_leg_lab_flag(zc2.set_leg_lab_flag),
  make_legend_flag(zc2.make_legend_flag),
  scan_mode(zc2.scan_mode),
  algorithm(zc2.algorithm),
  func(zc2.func),
  legend_label_set(zc2.legend_label_set),
  legend_header(zc2.legend_header),
  default_legend_opt(zc2.default_legend_opt),
  optimization_trials(zc2.optimization_trials),
  random_engine(zc2.random_engine),
  grid(zc2.grid), width(zc2.width), height(zc2.height),
  delta_x(zc2.delta_x), delta_y(zc2.delta_y), 
  x_label(zc2.x_label), y_label(zc2.y_label),
  plot_label(zc2.plot_label), connected_plot(zc2.connected_plot),
  cont_set(zc2.cont_set), req_threads(zc2.req_threads),
  unfound_contours(zc2.unfound_contours)
{
  Z_LOG_NOTE("ContourFinder copy constructor: from " 
              + zc2.PtrStr() + " --> " + PtrStr() + ".") ;
    
#if CONFIND_BASE_DEBUG_MODE
  Z_OBJ_CCTR(this, (void*)(&zc2), "ContourFinder", "ContourFinder") ;
#endif
    
  if(zc2.genFuncPtr)
    genFuncPtr = zc2.genFuncPtr->Clone() ;

  // if(zc2.graph)
  //   graph = dynamic_cast<TMultiGraph*> (zc2.graph->Clone());

  // if(zc2.legend)
  //   legend = dynamic_cast<TLegend*>(zc2.legend->Clone()) ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetGrid(const Zaki::Math::Grid2D& g) 
{
  grid = g ;
  set_grid_flag = true ; 
  SetDeltas() ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetWidth(const size_t& width_in)  
{
  width = width_in ;
  set_width_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetHeight(const size_t& height_in)  
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

  // If one of the parallel algorithms is requested, but no omp support
  if((algorithm == Mode::Parallel || algorithm == Mode::Ludicrous) && !Z_OMP)
  {
    algorithm = Mode::Fast ;
    Z_LOG_WARNING("Parallel & Ludicrous modes are not available since:\n'omp.h' header files not found, using 'Mode::Fast'(serial) instead.");
  }

  if (algorithm == Optimal)
    FindOptimalMode() ; // will replace algorithm with the best mode

  // Fast Mode
  if (algorithm == Fast)
  {
    double* m_GridValArr = nullptr;
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
void CONFIND::ContourFinder::SetGridVals(Zaki::Math::GridVals_2D* in_grid_v_2d)
{
  Z_LOG_INFO("==> Setting grid values ...");

  FindNextContours(in_grid_v_2d->m_GridValArr) ;

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
  snprintf(tmp, sizeof(tmp), "Scan mode is set to '%c'.", in_scan_mode) ;
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
  
  // If no omp support, fast mode is the best
  if(!Z_OMP)
  {
    algorithm = Mode::Fast ;
    Z_LOG_INFO("'omp.h' header files not found, setting the mode to 'Mode::Fast'(serial).");
    return ;
  }

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
  // algorithm =  Mode::Ludicrous ;
  Z_LOG_INFO("Optimal algorithm is set.") ;

}

//--------------------------------------------------------------
// Sets the optimization_trials value in 'TimeFunc'
// Also see: 'FindOptimalMode'
void CONFIND::ContourFinder::SetOptimizationTrials(const int& in_val)
{
  optimization_trials = in_val ;
}

//--------------------------------------------------------------
// Timing the function or member-function
CONFIND::ContourFinder::Mode CONFIND::ContourFinder::TimeFunc(
  std::uniform_real_distribution<double>& in_x_dis,
  std::uniform_real_distribution<double>& in_y_dis ) 
{
  
  // Zaki::Util::TimeProfileManager::BeginSpecialSession("FuncTiming") ;

  // double x=0, y = 0; 
  // for (unsigned int i = 0; i < optimization_trials; i++)
  // {
  //   x = in_x_dis(random_engine) ;
  //   y = in_y_dis(random_engine) ;

  //   if (grid.xAxis.scale == "Log" && grid.yAxis.scale == "Log")
  //    { x = pow(10, x) ; y = pow(10, y) ; }

  //   else if(grid.xAxis.scale == "Linear" && grid.yAxis.scale == "Log")
  //     y = pow(10, y) ;

  //   else if(grid.xAxis.scale == "Log" && grid.yAxis.scale == "Linear")
  //     x =pow(10, x) ;

  //   if(func)
  //   { 
  //     // Timing scope
  //     Z_TIME_PROFILE_SIMPLE("Func_"+std::to_string(i)) ;
  //     func(x,y) ;
  //   }
  //   else if(genFuncPtr)
  //   { 
  //     // Timing scope
  //     Z_TIME_PROFILE_SIMPLE("Mem_Func_"+std::to_string(i)) ;
  //     genFuncPtr->Eval(x,y) ;
  //   }
  //   else
  //   {
  //   Z_LOG_WARNING(
  //     "Optimal mode wasn't found because neither a function or a"
  //     "member function is set. Use 'SetFunc' or 'SetMemFunc' first"
  //     " and try again.") ;
  //     break;
  //   }
  // }

  // std::vector<double> timing_results =
  // Zaki::Util::TimeProfileManager::GetSpecialValues() ;

  // Zaki::Util::TimeProfileManager::EndSpecialSession() ;

  // // Analyzing the results
  // double mean=0, min=0, max=0;

  // //............................
  // // Safety check
  // if(timing_results.size() > 0)
  // {
  //   min = timing_results[0]; max = timing_results[0] ;
  // }
  // else
  // {
  //   Z_LOG_ERROR("Special timing returned an empty set!") ;
  //   Z_LOG_ERROR("Automatic optimization failed, choosing Ludicrous mode.") ;
  //   return Mode::Ludicrous ;
  // }
  // //............................

  // for (size_t i = 0; i < timing_results.size(); i++)
  // {
  //   mean += timing_results[i] / timing_results.size() ;

  //   if (timing_results[i] < min)
  //     min = timing_results[i] ;

  //   if (timing_results[i] > max)
  //     max = timing_results[i] ;
  // }

  // // Change of beahviour happens around this time
  // double threshold_time = 2e-2 ; // in ms

  // // For now we only use the mean value
  // // If grid size is small the threshold time is a bit higher
  // if( grid.xAxis.res < 50 && grid.yAxis.res < 50 )
  //   threshold_time = 4e-2;

  // if(mean > threshold_time)
  // {
  //   // Ludicrous mode is the optimal mode
  //   Z_LOG_INFO("Ludicrous mode is the optimal mode!") ;
  //   return Mode::Ludicrous ;
  // }
  // else
  // {
  //   // Fast mode is the optimal mode
  //   Z_LOG_INFO("Fast mode is the optimal mode!") ;
  //   return Mode::Fast ;
  // }
  return Mode::Ludicrous ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::FindContour(Cont2D& cont)
{
  PROFILE_FUNCTION() ;

  char tmp[150] ;
  snprintf(tmp, sizeof(tmp), "==> Finding contour for c = %.2e (%s)...", cont.val,
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
  snprintf(tmp, sizeof(tmp), "%.2e", cont.val) ;
    
  Z_LOG_INFO("==> Finding contour for c= " + std::string(tmp) + " ("+cont.color.name()+")...") ;

  // SetDeltas() ;

  std::vector<double> corners;
  corners.resize(grid.xAxis.res+1);
  double tmp_corner ;

  Bundle b(grid, cont, genFuncPtr, func) ;

  size_t n_x = grid.xAxis.res ;
  size_t n_y = grid.yAxis.res ;
    
  // Instantiate a cell
  Cell new_cell(0, delta_x, 0, delta_y, &b, cont.val) ;
  //============LOOP STARTS============ 
  for (size_t j = 0; j < n_y; j++)
  {
    // corners.clear() ;
    for (size_t i = 0; i < n_x; i++)
    {
//      // Instantiate a cell
//      Cell new_cell(i, delta_x, j, delta_y, &b, cont.val) ;
        new_cell.SetIdx(i,j) ;
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

  // ......................
  // May 17, 2022: 
  // I changed the initial k from '1' to '0'
  // so I can use FindNextContours for the first contour too.
  // if the first contour is found, it will be skipped anyways!
  // ......................
  for (size_t k = 0 ; k < cont_set.size() ; ++k)
  {

    // Skip the contours that are already found
    if (cont_set[k].GetFound())
    {
      continue ;
    }
    
    char tmp[150] ;
    snprintf(tmp, sizeof(tmp), "==> Finding contour for c = %.2e (%s)...", cont_set[k].val, 
            cont_set[k].color.name().c_str()) ;
    Z_LOG_INFO(tmp) ;

    // SetDeltas() ;

    Bundle b(grid, cont_set[k], genFuncPtr, func) ;
    size_t n_x = grid.xAxis.res ;
    size_t n_y = grid.yAxis.res ;
      
    // Instantiate a cell
    Cell new_cell(0, delta_x, 0, delta_y, &b, cont_set[k].val) ;
    //============LOOP STARTS============ 
    for (size_t j = 0; j < n_y; j++)
    {
      // corners.clear() ;
      for (size_t i = 0; i < n_x; i++)
      {
//        // Instantiate a cell
//        Cell new_cell(i, delta_x, j, delta_y, &b, cont_set[k].val) ;
        new_cell.SetIdx(i,j) ;
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

  /// Including only the unfound contours
//  std::vector<Cont2D>
//  curr_cont_set(cont_set.end() - unfound_contours, cont_set.end()) ;

  int n_t = 1 ;
  omp_set_num_threads(req_threads) ;
  Z_LOG_INFO("Threads requested: " + std::to_string(req_threads)) ;

  // std::vector<Zaki::Math::Func2D*> loc_MemFuncs ;
  // for (size_t i = 0; i < req_threads; i++)
  // {
  //   loc_MemFuncs.push_back(genFuncPtr->Clone()) ;
  // }
  
  // ..............................
  /// Parallel Region
  // ..............................
  #pragma omp parallel
  {
    //.......................................
    /// Figuring out the thread id & numbers
    #pragma omp single
    {
      n_t = omp_get_num_threads() ;
      Z_LOG_INFO("Working Threads: " + std::to_string(n_t)) ;
    }

    int t_n = omp_get_thread_num() ;
    //.......................................
    std::vector<Cont2D> loc_con_set ;
    loc_con_set.reserve(unfound_contours) ;
    #pragma omp critical
      std::copy(cont_set.end() - unfound_contours, cont_set.end(), std::back_inserter(loc_con_set));

    //.......................................
    /// Defining local sub_grids:
    double x_min, x_max, y_min, y_max ;
    size_t res_x, res_y ;
    std::string scale_x, scale_y ;

    #pragma omp critical
    {
        x_min = grid.xAxis.Min() ;
        x_max = grid.xAxis.Max() ;
        res_x = grid.yAxis.res ;
        scale_x = grid.xAxis.scale ;
        
        y_min = grid.yAxis.Min() + t_n * (grid.yAxis.Max() - grid.yAxis.Min())/(1.0*n_t) ;
        y_max = grid.yAxis.Min() + (t_n+1) * (grid.yAxis.Max() - grid.yAxis.Min())/(1.0*n_t) ;
        res_y = grid.yAxis.res / n_t + 1  ;
        scale_y = grid.yAxis.scale ;
    }
      
    Zaki::Math::Grid2D loc_grid = {
        {{x_min, x_max}, res_x, scale_x},
                    {{y_min, y_max}, res_y, scale_y}};

    /// Thread-Safety
    CONFIND::Bundle b ;
    b.AddGrid(loc_grid) ;

    #pragma omp critical
    {  
      b.AddMemFunc(genFuncPtr);
      b.AddFunc(func) ;
    }

    std::vector<double> loc_gridVal;
    /// If more than one contour
    if (unfound_contours > 1)
    {
      loc_gridVal.resize((loc_grid.xAxis.res+1)*(loc_grid.yAxis.res+1)) ;
    }
    //.......................................
    /// Contour set loop begins
    //.......................................
    for (size_t i = 0; i < loc_con_set.size(); i++)
    {
      size_t tmp_first = 0 ;
      /// Skip the contours that are already found
      if (loc_con_set[i].GetFound())
        { tmp_first++ ; continue ; }

      char tmp[150] ;
      snprintf(tmp, sizeof(tmp), "Thread-%d: Finding contour for c = %.2e (%s)...",
        t_n, loc_con_set[i].val,
        loc_con_set[i].color.name().c_str()) ;
      Z_LOG_INFO(tmp) ;
      // .............................

      b.AddCont(loc_con_set[i]) ;
      /// This should be intensive
      if (i==tmp_first)
      {  
        ThreadTaskLudicrous(b, loc_gridVal) ;
      }
      else  /// Not the first contour
      {
        ThreadNextTaskLudicrous(b, loc_gridVal) ;
      }
      // .............................


      loc_con_set[i] += b.GetCont() ;
      loc_con_set[i].SetFound();
    }
    //.......................................
    /// Contour set loop ends
    //.......................................


    #pragma omp critical
    {
      /**
           Need to make sure we add the points
           only to the unfound contours
           unfound contours = loc_con_set.size()
       */
      for (size_t i = 0; i < loc_con_set.size(); i++)
      {
        cont_set[ cont_set.size() - loc_con_set.size() + i] += loc_con_set[i] ;
      }     
    }
  }
  // ..............................
  /// Parallel Region Ends

    
    /**
        Need to make sure we only count the unfound contours
    */
  for (size_t i = cont_set.size() - unfound_contours; i < cont_set.size(); i++)
  {
    /// Marking the contour as found
    cont_set[i].SetFound();
    unfound_contours--;
  }

  return;
}

//--------------------------------------------------------------
//  Ludicrous mode: Combination of fast & parallel modes
void CONFIND::ContourFinder::ThreadNextTaskLudicrous(Bundle& in_b,
 const std::vector<double>& in_vec) 
{
  PROFILE_FUNCTION() ;


  size_t n_x = in_b.Grid.xAxis.res ;
  size_t n_y = in_b.Grid.yAxis.res ;

  // Instantiate a cell
  Cell new_cell(0, delta_x, 0, delta_y, &in_b, in_b.Con.val) ;
  //============LOOP STARTS============ 
  for (size_t j = 0; j < n_y; j++)
  {
    // corners.clear() ;
    for (size_t i = 0; i < n_x; i++)
    {
      // Instantiate a cell
      // Cell new_cell(i, delta_x, j, delta_y, &in_b, in_b.Con.val) ;
      new_cell.SetIdx(i,j) ;
      // ............................................
      new_cell.SetVertexZ(Cell::B_Left , in_vec[i    + (n_x+1) * j     ]) ;
      new_cell.SetVertexZ(Cell::B_Right, in_vec[i+1  + (n_x+1) * j     ]) ;
      new_cell.SetVertexZ(Cell::T_Right, in_vec[i+1  + (n_x+1) * (j+1) ]) ;
      new_cell.SetVertexZ(Cell::T_Left , in_vec[i    + (n_x+1) * (j+1) ]) ;
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
void CONFIND::ContourFinder::ThreadTaskLudicrous(Bundle& in_b,
  std::vector<double>& in_vec)
{
  PROFILE_FUNCTION() ;

  std::vector<double> corners;
  corners.resize(grid.xAxis.res+1);
  double tmp_corner ;

  size_t n_x = in_b.Grid.xAxis.res ;
  size_t n_y = in_b.Grid.yAxis.res ;

  // Instantiate a cell
  Cell new_cell(0, delta_x, 0, delta_y, &in_b, in_b.Con.val) ;
  //============LOOP STARTS============ 
  for (size_t j = 0; j < n_y; j++)
  {
    // corners.clear() ;
    for (size_t i = 0; i < n_x; i++)
    {
      // Instantiate a cell
      // Cell new_cell(i, delta_x, j, delta_y, &in_b, in_b.Con.val) ;
      new_cell.SetIdx(i,j) ;
      // ............................................
      // Except the first column:
      if ( i > 0)
        // Top_Left vertex: (idx = 4), i is the last element added to corners[]
        new_cell.SetVertexZ(Cell::T_Left, corners[i]) ;
      //............................................
      // First column (except the first cell):
      if( j > 0 )
        // Bottom_Right vertex: (idx = 2), i+1 is to the right of the cell
        new_cell.SetVertexZ(Cell::B_Right, corners[i+1]) ;
      //............................................
      // All cells except the first one at (0,0):
      if( !(i == 0 && j ==0 ) )
        // Bottom_Left vertex : (idx = 1)
        new_cell.SetVertexZ(Cell::B_Left, tmp_corner) ;
      //............................................

      new_cell.FindVerts() ;
      new_cell.GetStatus() ;
      in_b.Con.AddPts(new_cell.GetContourCoords()) ;

      //............................................
      // Before reaching the wall on the right
      if ( i < n_x - 1)
        tmp_corner = new_cell.GetFuncVals(Cell::B_Right) ;
      // If last column, set tmp_corner for the next row
      else
        tmp_corner = corners[0] ;
      //............................................

      if (i==0) // The first vertex in each row
        corners[0] = new_cell.GetFuncVals(Cell::T_Left) ;
      // The rest of the row
      corners[i+1] = new_cell.GetFuncVals(Cell::T_Right) ;
      //............................................
      // double* in_gridValArr  = nullptr ;
      if (in_vec.size() != 0)
      {
        // saving the bottom_left vertex
        in_vec[i + (n_x + 1)*j] = new_cell.GetFuncVals(Cell::B_Left) ;

        // For the last column, we need to save the bottom-right vertex too
        if( i == n_x - 1)
          in_vec[n_x + (n_x + 1)*j] = new_cell.GetFuncVals(Cell::B_Right) ;

        // For the last row, we need to save the top-left vertex too
        if( j == n_y - 1)
          in_vec[i + (n_x + 1)*n_y] = new_cell.GetFuncVals(Cell::T_Left) ;

        // For the last top-right corner cell top-right vertex (last element)
        if ( j == n_y - 1 && i == n_x - 1 )
          in_vec[(n_x+1)*(n_y+1) - 1] = new_cell.GetFuncVals(Cell::T_Right) ;
      }
    }
  }
  //============END of LOOP============ 

}

//--------------------------------------------------------------
// Setting the number of threads
void CONFIND::ContourFinder::SetThreads(const int& in_num) 
{
  req_threads = in_num ;
  Z_LOG_NOTE("Number of requested threads set to: " 
                + std::to_string(in_num)) ;
}

//--------------------------------------------------------------
// Parallel evaluation of the contours
void CONFIND::ContourFinder::FindContourParallel(Cont2D& cont)
{
  PROFILE_FUNCTION() ;

  char tmp[150] ;
  snprintf(tmp, sizeof(tmp), "==> Finding contour for c = %.2e (%s)...", cont.val,
          cont.color.name().c_str()) ;
  Z_LOG_INFO(tmp) ;

  int n_t = 1 ;
  omp_set_num_threads(req_threads) ;
  // Z_LOG_INFO("Threads requested: " + std::to_string(req_threads)) ;

  // ..............................
  // Parallel Region
  // ..............................
  #pragma omp parallel
  {
    #pragma omp single
    {
      n_t = omp_get_num_threads() ;
      Z_LOG_INFO("Working Threads: " + std::to_string(n_t)) ;
    }

    int t_n = omp_get_thread_num() ;

      
      double x_min, x_max, y_min, y_max ;
      size_t res_x, res_y ;
      std::string scale_x, scale_y ;
      
#pragma omp critical
      {
          x_min = grid.xAxis.Min() ;
          x_max = grid.xAxis.Max() ;
          res_x = grid.yAxis.res ;
          scale_x = grid.xAxis.scale ;
          
          y_min = grid.yAxis.Min() + t_n * (grid.yAxis.Max() - grid.yAxis.Min())/(1.0*n_t) ;
          y_max = grid.yAxis.Min() + (t_n+1) * (grid.yAxis.Max() - grid.yAxis.Min())/(1.0*n_t) ;
          res_y = grid.yAxis.res / n_t + 1  ;
          scale_y = grid.yAxis.scale ;
      }
      
      Zaki::Math::Grid2D loc_grid = {
          {{x_min, x_max}, res_x, scale_x},
          {{y_min, y_max}, res_y, scale_y}};

    // takes the contour level (value)
    // Cont2D loc_cont(cont.val);

    // Thread-Safety
    CONFIND::Bundle b ;
    b.AddGrid(loc_grid) ;
    
    #pragma omp critical
    {  
      b.AddCont(cont) ;
      b.AddMemFunc(genFuncPtr);
      b.AddFunc(func) ;
    }
    // #pragma omp critical
    //   CONFIND::Bundle b(loc_grid, cont, genFuncPtr, func);

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
    snprintf(tmp, sizeof(tmp), "==> Printing Contour = %f ...", cont_set[i].val) ;
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
      genFuncPtr.reset() ;
      set_mem_func_flag = false ; 
    }
  }
  else
    Z_LOG_INFO("Function is not set, because the input is a nullptr.") ;

}

//--------------------------------------------------------------
// Non-static mem-funcs
void CONFIND::ContourFinder::SetMemFunc(Zaki::Math::Func2D* gen_Fun) 
{
  genFuncPtr = std::unique_ptr<Zaki::Math::Func2D>(gen_Fun) ;

  if(genFuncPtr) 
  {
    Z_LOG_INFO("Member function is set.") ;
    set_mem_func_flag = true ;

    // Reseting 'func'
    if(func)
    {
      func = nullptr ;
      set_func_flag = false ; 
    }
  }
  else
    Z_LOG_INFO("Member function is not set, because the input is a nullptr.") ;
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
void CONFIND::ContourFinder::SetContVal(const std::vector<double>& cont_val_in,
                                        const std::vector<std::string>& in_label) 
{
  // cont_set.clear() ;
  for (size_t i = 0; i < cont_val_in.size(); i++)
  {
    cont_set.emplace_back(cont_val_in[i]) ;
    cont_set[cont_set.size()-1].SetColor(cont_set.size()-1) ;
    cont_set[cont_set.size()-1].SetLabel(in_label[i]) ;
  }

  // Kepping track of contours
  unfound_contours += cont_val_in.size() ;
  set_cont_val_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::ExportContour(const Zaki::String::Directory& f_name,
                                    const Zaki::File::FileMode& mode)
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
      snprintf(tmp, sizeof(tmp), "Contour '%.2e' hasn't been found yet, skipping to the next one.", cont_set[i].val) ;
      Z_LOG_WARNING(tmp) ;
      continue ;
    }  
    cont_set[i].Export(wrk_dir + f_name, mode) ;
  }

  Z_LOG_INFO("Contours exported to '"+f_name.Str()+"_[CONT].dat'.") ;
}
//--------------------------------------------------------------
// Plot options
void CONFIND::ContourFinder::SetPlotXLabel(const std::string& in_x_label)
{
  x_label = in_x_label;
  set_plotX_label_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetPlotYLabel(const std::string& in_y_label)
{
  y_label = in_y_label;
  set_plotY_label_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetPlotXRange(
  const Zaki::Math::Range<double>& in_x_range) 
{
  plot_x_range = in_x_range ;
  set_plotX_range_flag = true ;
}
//--------------------------------------------------------------
void CONFIND::ContourFinder::SetPlotYRange(
  const Zaki::Math::Range<double>& in_y_range) 
{
  plot_y_range = in_y_range ;
  set_plotY_range_flag = true ;
}
//--------------------------------------------------------------
void CONFIND::ContourFinder::SetPlotLabel(const std::string& in_label) 
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
// TMultiGraph* CONFIND::ContourFinder::GetGraph() 
// {
//   return graph ;
// }

//--------------------------------------------------------------
void CONFIND::ContourFinder::MakeLegend(const bool in_make_leg,
                               const char* const in_head,
                               const char* const in_option) 
{
  make_legend_flag = in_make_leg ;

  if(in_head != nullptr)
    legend_header = in_head ;
  
  if(strcmp(in_option, "user") == 0)
    default_legend_opt = false ;
}

//--------------------------------------------------------------
// TLegend* CONFIND::ContourFinder::GetLegend() 
// {
//   return legend;
// }

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetLegendLabels(const std::vector<std::string>& in_labels) 
{
  legend_label_set = in_labels ;
  set_leg_lab_flag = true ;
}

//--------------------------------------------------------------
// Commented on May 1, 2023:
// Root needs to be replaced by matplotlib
void CONFIND::ContourFinder::Plot(const Zaki::String::Directory& f_name, 
                         const char* const in_main_title,
                         const char* const in_x_title,
                         const char* const in_y_title)
{}

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

  func = nullptr ;
  genFuncPtr = nullptr ;
}

//--------------------------------------------------------------
/// Returns 'cont_set'
std::vector<CONFIND::Cont2D> CONFIND::ContourFinder::GetContourSet() const 
{
  return cont_set ;
}
//==============================================================
//  ContourFinder Class ends
