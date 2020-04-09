#include <iterator>
#include <algorithm>

#include "../include/Cell.h"
#include "../include/ContourFinder.h"

//==============================================================
// default Constructor
CONFIND::Cell::Cell() {} 

//--------------------------------------------------------------
// default Destructor
CONFIND::Cell::~Cell() {} 

//--------------------------------------------------------------
// Constructor 2
CONFIND::Cell::Cell(size_t i_in, double lx, size_t j_in, double ly) 
{
  SetSize(lx, ly) ;
  SetIdx(i_in, j_in) ;
}

//--------------------------------------------------------------
// Constructor 3
CONFIND::Cell::Cell(size_t i_in, double lx, size_t j_in, double ly, Bundle* bun_in, double cont_val_in) :
l_x(lx), l_y(ly), idx(i_in, j_in), contour_val(cont_val_in), BundlePtr(bun_in),
set_idx_flag(true), set_bundle_ptr_flag(true), set_size_flag(true), set_contour_val_flag(true)
{
  // FindVerts() ;
}

//--------------------------------------------------------------
void CONFIND::Cell::SetSize(const double lx_in, const double ly_in)  
{
  l_x = lx_in ;
  l_y = ly_in ;

  set_size_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::Cell::SetIdx(const size_t i_in, const size_t j_in)  
{
  // idx_x = i_in ;
  // idx_y = j_in ;
  idx = {i_in, j_in} ;

  set_idx_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::Cell::SetVertexZ(const size_t idx_in, const double val_z)   
{
  if (idx_in > 4 || idx_in < 1)
  {
    Z_LOG_ERROR("Index out of bound, must be less than 5 and greater than 0!") ;
    return ;
  }
  
  verts_set[idx_in].xyz.z = val_z ;

  // There is an offset between the defenitions of verts_set & set_vertexZ_flag
  // because we are nit including the center vertex anymore
  set_vertexZ_flag[idx_in - 1] = true ;

}

//--------------------------------------------------------------
void CONFIND::Cell::SetVertex(const size_t idx_in, const double val_x, 
                     const double val_y, const double val_z)
{
  if (idx_in > 4)
  {
    Z_LOG_ERROR("Index out of bound, must be less than 5!") ;
    return ;
  }
  verts_set[idx_in].xyz.x = val_x ;
  verts_set[idx_in].xyz.y = val_y ;
  verts_set[idx_in].xyz.z = val_z ;

  set_vertex_flag[idx_in] = true ;
}

//--------------------------------------------------------------
void CONFIND::Cell::SetVertex(const size_t idx_in, const Zaki::Physics::Coord3D& val)
{
  PROFILE_FUNCTION() ;
  if (idx_in > 4)
  {
    Z_LOG_ERROR("Index out of bound, must be less than 5!") ;
    return ;
  }

  verts_set[idx_in].xyz.x = val.x ;
  verts_set[idx_in].xyz.y = val.y ;
  verts_set[idx_in].xyz.z = val.z ;

  set_vertex_flag[idx_in] = true  ;
}

//--------------------------------------------------------------
std::pair<size_t, size_t> CONFIND::Cell::GetIdx() const
{
  if(!set_idx_flag)
  {
    Z_LOG_ERROR("variable not set!") ;
    return {-1, -1};
  }

  return idx;
}

//--------------------------------------------------------------
CONFIND::vertex CONFIND::Cell::operator[](size_t idx_in) const
{
  if( set_vertex_flag[idx_in] )
  {
    Z_LOG_ERROR("vertex not set!") ;
  }
  
  return verts_set[idx_in] ;
}

//--------------------------------------------------------------
// Used for optimizing the process
double CONFIND::Cell::GetFuncVals(const size_t i) const 
{
  // Returning the function values at vertex i
  return verts_set[i].xyz.z ;
}

//--------------------------------------------------------------
void CONFIND::Cell::EvalCenter()
{ 
  PROFILE_FUNCTION() ;
  double cen_x  = (verts_set[1].xyz.x + verts_set[2].xyz.x) / 2 ;
  double cen_y  = (verts_set[1].xyz.y + verts_set[4].xyz.y) / 2 ;
  double cen_z  = (  verts_set[1].xyz.z + verts_set[2].xyz.z
                   + verts_set[3].xyz.z + verts_set[4].xyz.z) / 4 ;

  // idx = 0 is for center
  SetVertex(0, {cen_x, cen_y, cen_z}) ;
}

//--------------------------------------------------------------
void CONFIND::Cell::SetBundlePtr(Bundle* bun_in)
{
  BundlePtr = bun_in ;
  set_bundle_ptr_flag = true ;
}

//--------------------------------------------------------------
double CONFIND::Cell::GetLX()  const
{
  if ( !set_size_flag )
  {
    Z_LOG_ERROR("Cell size not set!") ;
    return -1;
  }  

  return l_x;
}

//--------------------------------------------------------------
double CONFIND::Cell::GetLY()  const
{
  if ( !set_size_flag )
  {
    Z_LOG_ERROR("Cell size not set!") ;
    return -1;
  }  

  return l_y;
}

//--------------------------------------------------------------
double CONFIND::Cell::EvalFunc(const double x, const double y)
{
  PROFILE_FUNCTION() ;
  if ( BundlePtr->Func )
    return EvalSimpleFunc(x, y) ;
  else
    return EvalMemFunc(x, y) ;
}

//--------------------------------------------------------------
double CONFIND::Cell::EvalMemFunc(const double x, const double y)
{
  if (BundlePtr->Grid.xAxis.scale == "Linear" && BundlePtr->Grid.yAxis.scale == "Linear")
    return BundlePtr->MemFunc->Eval(x, y) ;

  else if (BundlePtr->Grid.xAxis.scale == "Log" && BundlePtr->Grid.yAxis.scale == "Log")
    return BundlePtr->MemFunc->Eval(pow(10, x), pow(10, y)) ;

  else if(BundlePtr->Grid.xAxis.scale == "Linear" && BundlePtr->Grid.yAxis.scale == "Log")
    return BundlePtr->MemFunc->Eval(x, pow(10, y)) ;

  else if(BundlePtr->Grid.xAxis.scale == "Log" && BundlePtr->Grid.yAxis.scale == "Linear")
    return BundlePtr->MemFunc->Eval(pow(10, x), y) ;
  
  else
  {
   Z_LOG_ERROR("Invalid axis scale!") ;
    return -1 ;
  } 
}

//--------------------------------------------------------------
double CONFIND::Cell::EvalSimpleFunc(const double x, const double y)
{
  if (BundlePtr->Grid.xAxis.scale == "Linear" && BundlePtr->Grid.yAxis.scale == "Linear")
    return BundlePtr->Func(x, y) ;

  else if (BundlePtr->Grid.xAxis.scale == "Log" && BundlePtr->Grid.yAxis.scale == "Log")
    return BundlePtr->Func(pow(10, x), pow(10, y)) ;

  else if(BundlePtr->Grid.xAxis.scale == "Linear" && BundlePtr->Grid.yAxis.scale == "Log")
    return BundlePtr->Func(x, pow(10, y)) ;

  else if(BundlePtr->Grid.xAxis.scale == "Log" && BundlePtr->Grid.yAxis.scale == "Linear")
    return BundlePtr->Func(pow(10, x), y) ;
  
  else
  {
   Z_LOG_ERROR("Invalid axis scale!") ;
    return -1 ;
  } 
}

//--------------------------------------------------------------
void CONFIND::Cell::FindVerts() 
{
  PROFILE_FUNCTION() ;
  if (!set_bundle_ptr_flag)
  {
    Z_LOG_ERROR("ContourFinder pointer not set!") ;
    return ;
  }
  if(!set_idx_flag)
  {
    Z_LOG_ERROR("Cell index is not set!") ;
    return ;
  }
  if(!set_size_flag)
  {
    Z_LOG_ERROR("Cell size is not set!") ;
    return ;
  }

  BundlePtr->Grid.xAxis.scale == "Log" ? x_min = log10(BundlePtr->Grid.xAxis.Min()) : x_min = BundlePtr->Grid.xAxis.Min() ;
  BundlePtr->Grid.yAxis.scale == "Log" ? y_min = log10(BundlePtr->Grid.yAxis.Min()) : y_min = BundlePtr->Grid.yAxis.Min() ;


  //.......................................
  if (set_vertexZ_flag[0])
  SetVertex(1, {x_min + idx.first*l_x,  y_min + idx.second*l_y, verts_set[1].xyz.z}) ;
  else
  SetVertex(1, {x_min + idx.first*l_x,  y_min + idx.second*l_y,
                   EvalFunc(x_min + idx.first*l_x, y_min + idx.second*l_y)}) ;
  //.......................................
  if (set_vertexZ_flag[1])
  SetVertex(2, {x_min + (idx.first+1)*l_x, y_min + idx.second*l_y, verts_set[2].xyz.z} ) ;
  else
  SetVertex(2, {x_min + (idx.first+1)*l_x, y_min + idx.second*l_y, 
                   EvalFunc(x_min + (idx.first+1)*l_x, y_min + idx.second*l_y)} ) ;
  //.......................................
  if (set_vertexZ_flag[2])
  SetVertex(3, {x_min + (idx.first+1)*l_x, y_min + (idx.second+1)*l_y, verts_set[3].xyz.z} ) ;
  else
  SetVertex(3, {x_min + (idx.first+1)*l_x, y_min + (idx.second+1)*l_y, 
                   EvalFunc(x_min + (idx.first+1)*l_x, y_min + (idx.second+1)*l_y)} ) ;
  //.......................................
  if (set_vertexZ_flag[3])
  SetVertex(4, {x_min + idx.first*l_x, y_min + (idx.second+1)*l_y,  verts_set[4].xyz.z} ) ;
  else
  SetVertex(4, {x_min + idx.first*l_x, y_min + (idx.second+1)*l_y, 
                   EvalFunc(x_min + idx.first*l_x, y_min + (idx.second+1)*l_y)} ) ;
  //.......................................

  EvalCenter()         ; // Center of the cell

  SetTriangles() ;

  found_verts_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::Cell::SetTriangles()
{
  PROFILE_FUNCTION() ;
  triangle_set.reserve(4) ;

  // 0: Bottom triangle
  triangle_set.emplace_back(verts_set[0], verts_set[1], verts_set[2]) ;

  // 1: Right triangle
  triangle_set.emplace_back(verts_set[0], verts_set[2], verts_set[3]) ;

  // 2: Top triangle
  triangle_set.emplace_back(verts_set[0], verts_set[3], verts_set[4]) ;

  // 3: Left triangle
  triangle_set.emplace_back(verts_set[0], verts_set[4], verts_set[1]) ;
}

//--------------------------------------------------------------
void CONFIND::Cell::SetContourValue(double cont_in)
{
  contour_val = cont_in ;
  set_contour_val_flag = true ;
}

//--------------------------------------------------------------
double CONFIND::Cell::GetContourValue() const
{
  if (!set_contour_val_flag)
  {
    Z_LOG_ERROR("Contour value not set!") ;
    return -1;
  }
  return contour_val  ;
}

//--------------------------------------------------------------
int CONFIND::Cell::GetStatus()
{
  PROFILE_FUNCTION() ;
  if (!found_verts_flag)
  {
    Z_LOG_ERROR("FindVerts() first!") ;
    return -1;
  }

  if (!set_contour_val_flag)
  {
    Z_LOG_ERROR("Contour value not set!") ;
    return -1;
  }

  int above_counter = 0 ;
  int below_counter = 0 ;

  // double cont = GetContourValue() ;
  // Setting the status of vertices
  for (size_t i = 0; i < 5; i++)
  {
    if (  verts_set[i].status(contour_val) == 1)
      above_counter++ ;

    else if ( verts_set[i].status(contour_val) == -1)
      below_counter++ ;
  }
  
  // Checking the status of the vertices
  if (below_counter == 5)
    return -50;
  else if (above_counter == 5)
    return +50;

  for (size_t i = 0; i < triangle_set.size() ; i++)
  {
    switch (triangle_set[i].status(contour_val))
    {
    case 3: //  c) Two vertices lie below and one above the contour level.
      case36(triangle_set[i], +1) ;
      break;
    case 6: // f) One vertex lies below and two above the contour level.
      case36(triangle_set[i], -1) ;
      break;

    case 4: // d) One vertex lies below and two on the contour level.
      case48(triangle_set[i]) ;
      break;
    case 8: // h) Two vertices lie on and one above the contour level.
      case48(triangle_set[i]) ;
      break;

    case 5: // e) One vertex lies below, one on and one above the contour level.
      case5(triangle_set[i]) ;
      break;
    
    default:
      break;
    }
    
  }
  return 0;
}

//--------------------------------------------------------------
// (-1, -1, odd_sign--> +1) & ( odd_sign --> -1, +1, +1)
void CONFIND::Cell::case36(const triangle& tri, const int odd_sign)
{
  // double c = GetContourValue() ;
  Zaki::Physics::Coord3D p_top, p_1, p_2 ;

  if (tri.v[0].status(contour_val) == odd_sign)
    { p_top = tri.v[0].xyz ; p_1 = tri.v[1].xyz ; p_2 = tri.v[2].xyz; }

  else if (tri.v[1].status(contour_val) == odd_sign)
    { p_top = tri.v[1].xyz ; p_1 = tri.v[0].xyz ; p_2 = tri.v[2].xyz; }

  else
    { p_top = tri.v[2].xyz ; p_1 = tri.v[0].xyz ; p_2 = tri.v[1].xyz; }
  
  double ratio_1 = (contour_val - p_1.z) / ( p_top.z - p_1.z) ;
  double ratio_2 = (contour_val - p_2.z) / ( p_top.z - p_2.z) ;

  Zaki::Physics::Coord3D o_1 = 
  {
    ratio_1 * (p_top.x - p_1.x) + p_1.x ,
    ratio_1 * (p_top.y - p_1.y) + p_1.y ,
    contour_val
  } ;

  Zaki::Physics::Coord3D o_2 = 
  {
    ratio_2 * (p_top.x - p_2.x) + p_2.x ,
    ratio_2 * (p_top.y - p_2.y) + p_2.y ,
    contour_val
  } ;

  if (BundlePtr->Grid.xAxis.scale == "Log")
    {o_1.x = pow(10, o_1.x) ; o_2.x = pow(10, o_2.x) ;}

  if (BundlePtr->Grid.yAxis.scale == "Log")
    {o_1.y = pow(10, o_1.y) ; o_2.y = pow(10, o_2.y) ;}
  
  contour_coords.emplace_back(o_1.x, o_1.y, o_1.z) ;
  contour_coords.emplace_back(o_2.x, o_2.y, o_2.z) ;
}

//--------------------------------------------------------------
// (-1, +1, 0)
void CONFIND::Cell::case5(const triangle& tri)
{
  // double c = GetContourValue() ;
  Zaki::Physics::Coord3D o_on, p_1, p_2 ;

  if (tri.v[0].status(contour_val) == 0)
    { o_on = tri.v[0].xyz ; p_1 = tri.v[1].xyz; p_2 = tri.v[2].xyz; }

  else if (tri.v[1].status(contour_val) == 0)
    { o_on = tri.v[1].xyz ; p_1 = tri.v[0].xyz; p_2 = tri.v[2].xyz; }

  else
    { o_on = tri.v[2].xyz ; p_1 = tri.v[0].xyz; p_2 = tri.v[1].xyz; }

  double ratio = (contour_val - p_1.z) / ( p_2.z - p_1.z) ;

  
  Zaki::Physics::Coord3D o_other = 
  {
    ratio * (p_2.x - p_1.x) + p_1.x ,
    ratio * (p_2.y - p_1.y) + p_1.y ,
    contour_val
  } ;
  
  if (BundlePtr->Grid.xAxis.scale == "Log")
    {o_on.x = pow(10, o_on.x) ; o_other.x = pow(10, o_other.x) ;}

  if (BundlePtr->Grid.yAxis.scale == "Log")
    {o_on.y = pow(10, o_on.y) ; o_other.y = pow(10, o_other.y) ;}

  contour_coords.emplace_back(o_on.x, o_on.y, o_on.z) ;
  contour_coords.emplace_back(o_other.x, o_other.y, o_other.z) ;
}
//--------------------------------------------------------------
// (-1, 0, 0) & (+1, 0, 0)
void CONFIND::Cell::case48(const triangle& tri)
{
  // double c = GetContourValue() ;
  Zaki::Physics::Coord3D o_1, o_2 ;

  if (abs(tri.v[0].status(contour_val)) == 1)
    { o_1 = tri.v[1].xyz ; o_2 = tri.v[2].xyz; }

  else if (abs(tri.v[1].status(contour_val)) == 1)
    { o_1 = tri.v[0].xyz ; o_2 = tri.v[2].xyz; }

  else
    { o_1 = tri.v[0].xyz ; o_2 = tri.v[1].xyz; }

  if (BundlePtr->Grid.xAxis.scale == "Log")
    {o_1.x = pow(10, o_1.x) ; o_2.x = pow(10, o_2.x) ;}

  if (BundlePtr->Grid.yAxis.scale == "Log")
    {o_1.y = pow(10, o_1.y) ; o_2.y = pow(10, o_2.y) ;}

  contour_coords.emplace_back(o_1.x, o_1.y, o_1.z) ;
  contour_coords.emplace_back(o_2.x, o_2.y, o_2.z) ;
}

//--------------------------------------------------------------
const std::vector<Zaki::Physics::Coord3D>& CONFIND::Cell::GetContourCoords() const
{
  return contour_coords;
}

//==============================================================