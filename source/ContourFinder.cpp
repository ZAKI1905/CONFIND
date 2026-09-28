#include <unordered_set>
#include <typeinfo>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include "IndexedExecutor.hpp"


#include <Zaki/Vector/Vector_Basic.hpp>


// Local headers
#include "Confind/ContourFinder.hpp"
#include "Confind/Bundle.hpp"

//==============================================================
//  ContourFinder Class begins
//--------------------------------------------------------------
// Default Constructor
CONFIND::ContourFinder::ContourFinder() 
  : Base("ContourFinder")
{
  (void)0 ;

} 

//--------------------------------------------------------------
// Destructor
CONFIND::ContourFinder::~ContourFinder() 
{ 
  (void)0 ;
  // if (cpy_cons_called)  delete genFuncPtr; 
} 

//--------------------------------------------------------------
// Assignment operator
CONFIND::ContourFinder&
CONFIND::ContourFinder::operator=(const ContourFinder &other) 
{
  (void)0 ;

  if(this == &other) return *this ;
  else
  {
  set_grid_flag = other.set_grid_flag ;
  set_grid_vals_flag = other.set_grid_vals_flag ;
  set_func_flag = other.set_func_flag ;
  set_cont_val_flag = other.set_cont_val_flag ;
  set_mem_func_flag = other.set_mem_func_flag ;
  cpy_cons_called = other.cpy_cons_called ;
  set_scan_mode_flage = other.set_scan_mode_flage ;
  scan_mode = other.scan_mode ;
  algorithm = other.algorithm ;
  func = other.func ;
  grid = other.grid ;
  delta_x = other.delta_x ; delta_y = other.delta_y ;
  cont_set = other.cont_set ;
  unfound_contours = other.unfound_contours ;

  // Pointer member variables
  if(other.genFuncPtr)
    genFuncPtr = other.genFuncPtr->Clone() ;



  return *this ;
  }
}

//--------------------------------------------------------------
// Copy constructor
// Copies everything!
CONFIND::ContourFinder::ContourFinder(const ContourFinder &zc2) 
  : Base("ContourFinder"),
  set_grid_flag(zc2.set_grid_flag),
  set_grid_vals_flag(zc2.set_grid_vals_flag),
  set_func_flag(zc2.set_func_flag),
  set_cont_val_flag(zc2.set_cont_val_flag),
  set_mem_func_flag(zc2.set_mem_func_flag),
  cpy_cons_called(zc2.cpy_cons_called),
  set_scan_mode_flage(zc2.set_scan_mode_flage),
  scan_mode(zc2.scan_mode),
  algorithm(zc2.algorithm),
  func(zc2.func),
  grid(zc2.grid),
  delta_x(zc2.delta_x), delta_y(zc2.delta_y),
  cont_set(zc2.cont_set),
  unfound_contours(zc2.unfound_contours)
{
  (void)0 ;
    
    
  if(zc2.genFuncPtr)
    genFuncPtr = zc2.genFuncPtr->Clone() ;


}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetGrid(const Zaki::Math::Grid2D& g) 
{
  grid = g ;
  set_grid_flag = true ; 
  SetDeltas() ;
}

//--------------------------------------------------------------


//--------------------------------------------------------------


//--------------------------------------------------------------
size_t CONFIND::ContourFinder::GetN_X() const
{
  if(! set_grid_flag)
  {
    (void)0 ;
    return 0 ;
  }

  return grid.xAxis.res;
}

//--------------------------------------------------------------
size_t CONFIND::ContourFinder::GetN_Y() const 
{
  if(! set_grid_flag)
  {
    (void)0 ;
    return 0 ;
  }

  return grid.yAxis.res;
}

//--------------------------------------------------------------
double CONFIND::ContourFinder::GetX_Min() const
{
  if(! set_grid_flag)
  {
    (void)0 ;
    return 0 ;
  }

  return grid.xAxis.Min();
}

//--------------------------------------------------------------
double CONFIND::ContourFinder::GetX_Max() const
{  
  if(! set_grid_flag)
  {
    (void)0 ;
    return 0 ;
  }

  return grid.xAxis.Max();
}

//--------------------------------------------------------------
double CONFIND::ContourFinder::GetY_Min() const
{
  if(! set_grid_flag)
  {
    (void)0 ;
    return 0 ;
  }

  return grid.yAxis.Min();
}

//--------------------------------------------------------------
double CONFIND::ContourFinder::GetY_Max() const
{
  if(! set_grid_flag)
  {
    (void)0 ;
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
  if (!set_func_flag && !set_mem_func_flag)
    throw std::invalid_argument("CONFIND: function not set");
  algorithm = in_mode;
  if (algorithm == Fast)
  {
    double* m_GridValArr = nullptr;
    if (unfound_contours > 1)
      m_GridValArr = new double[(grid.xAxis.res+1)*(grid.yAxis.res+1)];
    FindContourFast(cont_set[cont_set.size()-unfound_contours], m_GridValArr);
    if (unfound_contours > 0) FindNextContours(m_GridValArr);
    if (m_GridValArr) delete m_GridValArr;
  }
  else if (algorithm == Normal)
  {
    for (auto& contour : cont_set)
      if (!contour.GetFound()) FindContour(contour);
  }
  else throw std::invalid_argument("CONFIND: invalid serial mode");
  set_grid_vals_flag = true;
}


//--------------------------------------------------------------
void CONFIND::ContourFinder::SetGridVals(Zaki::Math::GridVals_2D* in_grid_v_2d)
{
  (void)0;

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
      (void)0 ;
      return {-1, -1};
    }
  return {delta_x, delta_y} ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetScanMode(const char in_scan_mode)
{
  if(in_scan_mode != 'X' || in_scan_mode != 'Y')
  {
    (void)0 ;
    return;
  }

  scan_mode = in_scan_mode ;
  set_scan_mode_flage = true ;
  char tmp[100] ;
  snprintf(tmp, sizeof(tmp), "Scan mode is set to '%c'.", in_scan_mode) ;
  (void)0 ;
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


//--------------------------------------------------------------
// Sets the optimization_trials value in 'TimeFunc'
// Also see: 'FindOptimalMode'


//--------------------------------------------------------------
// Timing the function or member-function


//--------------------------------------------------------------
void CONFIND::ContourFinder::FindContour(Cont2D& cont)
{
  (void)0 ;

  (void)0 ;

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
  (void)0 ;

  char tmp[150] ;
  snprintf(tmp, sizeof(tmp), "%.2e", cont.val) ;
    

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
  (void)0 ;

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
    
    (void)0 ;

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


//--------------------------------------------------------------


//--------------------------------------------------------------


//--------------------------------------------------------------


//--------------------------------------------------------------
// Parallel evaluation of the contours


//--------------------------------------------------------------

                 
//--------------------------------------------------------------
void CONFIND::ContourFinder::Print() const
{
  if (! set_grid_vals_flag )
    {
      (void)0;
      return ;
    }
  
  for (size_t i = 0; i < cont_set.size(); i++)
  {
    char tmp[100] ;
    snprintf(tmp, sizeof(tmp), "==> Printing Contour = %f ...", cont_set[i].val) ;
    (void)0;

    std::cout << cont_set[i] ;
  }
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetFunc(double (*f)(double, double) ) 
{
  func = f;

  if(func) 
  {
    (void)0 ;
    set_func_flag = true ;

    // Reseting 'genFuncPtr'
    if(genFuncPtr)
    {
      genFuncPtr.reset() ;
      set_mem_func_flag = false ; 
    }
  }
  else
    (void)0 ;

}

//--------------------------------------------------------------
// Non-static mem-funcs
void CONFIND::ContourFinder::SetMemFunc(Zaki::Math::Func2D* gen_Fun) 
{
  genFuncPtr = std::unique_ptr<Zaki::Math::Func2D>(gen_Fun) ;

  if(genFuncPtr) 
  {
    (void)0 ;
    set_mem_func_flag = true ;

    // Reseting 'func'
    if(func)
    {
      func = nullptr ;
      set_func_flag = false ; 
    }
  }
  else
    (void)0 ;
}

//--------------------------------------------------------------
void CONFIND::ContourFinder::SetContVal(const std::vector<double>& cont_val_in) 
{
  // cont_set.clear() ;
  for (size_t i = 0; i < cont_val_in.size(); i++)
  {
    cont_set.emplace_back(cont_val_in[i]) ;
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
    (void)0;
    return ;
  }

  for(size_t i = 0 ; i < cont_set.size() ; ++i)
  {
    if (!cont_set[i].GetFound())
    {
      char tmp[150] ;
      snprintf(tmp, sizeof(tmp), "Contour '%.2e' hasn't been found yet, skipping to the next one.", cont_set[i].val) ;
      (void)0 ;
      continue ;
    }  
    cont_set[i].Export(wrk_dir + f_name, mode) ;
  }

  (void)0 ;
}
//--------------------------------------------------------------


//--------------------------------------------------------------


//--------------------------------------------------------------

//--------------------------------------------------------------

//--------------------------------------------------------------


//--------------------------------------------------------------


//--------------------------------------------------------------
// {
// }

//--------------------------------------------------------------


//--------------------------------------------------------------
// {
// }

//--------------------------------------------------------------


//--------------------------------------------------------------
// Commented on May 1, 2023:


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

// Samples are independent indexed tasks; contour assembly starts only after join.
void CONFIND::ContourFinder::Evaluate(const EvaluatorFactory& factory, ThreadingOptions options)
{
  if (!factory || !set_grid_flag || !set_cont_val_flag)
    throw std::invalid_argument("CONFIND: grid, levels and evaluator factory required");
  const size_t nx = grid.xAxis.res, ny = grid.yAxis.res;
  const size_t limit = std::numeric_limits<size_t>::max();
  if (!nx || !ny || nx == limit || ny == limit || nx+1 > limit/(ny+1))
    throw std::invalid_argument("CONFIND: invalid sample dimensions");
  const size_t tasks = (nx+1)*(ny+1);
  if (tasks > std::vector<double>().max_size())
    throw std::invalid_argument("CONFIND: sample dimensions exceed storage limit");
  const size_t workers = detail::WorkerCount(tasks, options.worker_count);
  std::vector<Evaluator> evaluators;
  evaluators.reserve(workers);
  // Creation/destruction happen on the caller, including factory failure cleanup.
  for (size_t w=0; w<workers; ++w) {
    evaluators.push_back(factory(w));
    if (!evaluators.back()) throw std::invalid_argument("CONFIND: empty evaluator");
  }
  std::vector<double> values(tasks);
  const bool log_x = grid.xAxis.scale == "Log", log_y = grid.yAxis.scale == "Log";
  const double x0 = log_x ? log10(grid.xAxis.Min()) : grid.xAxis.Min();
  const double y0 = log_y ? log10(grid.yAxis.Min()) : grid.yAxis.Min();
  detail::RunIndexed(tasks, workers, [&](size_t worker, size_t k) {
    const size_t i = k % (nx+1), j = k / (nx+1);
    const double x = x0 + i*delta_x, y = y0 + j*delta_y;
    values[k] = evaluators[worker](log_x ? pow(10,x) : x, log_y ? pow(10,y) : y);
  });
  FindNextContours(values.data());
  set_grid_vals_flag = true;
}
