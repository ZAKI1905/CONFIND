#include <iterator>
#include <algorithm>

#include "Confind/Cell.hpp"
#include "Confind/ContourFinder.hpp"
#include "Confind/Bundle.hpp"

//==============================================================
// Default Constructor
CONFIND::Cell::Cell() 
  : Base("Cell")
{
} 

//--------------------------------------------------------------
// Destructor
CONFIND::Cell::~Cell() 
{
} 

//--------------------------------------------------------------
// Constructor 2
CONFIND::Cell::Cell(const size_t& i_in, const double& lx, 
                    const size_t& j_in, const double& ly) 
  : Base("Cell")
{

  SetSize(lx, ly) ;
  SetIdx(i_in, j_in) ;
}

//--------------------------------------------------------------
// Constructor 3
CONFIND::Cell::Cell(const size_t& i_in, const double& lx,
                    const size_t& j_in, const double& ly, Bundle* bun_in,
                    const double& cont_val_in) 
  :  Base("Cell"),
l_x(lx), l_y(ly), idx(i_in, j_in), contour_val(cont_val_in), BundlePtr(bun_in),
set_idx_flag(true), set_bundle_ptr_flag(true), set_size_flag(true),
set_contour_val_flag(true), set_full_constructor(true)
{

  // FindVerts() ;
  BundlePtr->Grid.xAxis.scale == "Log" ? G_x_min = log10(BundlePtr->Grid.xAxis.Min()) : G_x_min = BundlePtr->Grid.xAxis.Min() ;
  BundlePtr->Grid.yAxis.scale == "Log" ? G_y_min = log10(BundlePtr->Grid.yAxis.Min()) : G_y_min = BundlePtr->Grid.yAxis.Min() ;

  x_min = G_x_min + i_in*l_x ;
  y_min = G_y_min + j_in*l_y ;
}

//--------------------------------------------------------------
void CONFIND::Cell::SetSize(const double& lx_in, const double& ly_in)  
{
  l_x = lx_in ;
  l_y = ly_in ;

  set_size_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::Cell::SetIdx(const size_t& i_in, const size_t& j_in)  
{
  idx = {i_in, j_in} ;

  set_idx_flag = true ;

  x_min = G_x_min + i_in*l_x ;
  y_min = G_y_min + j_in*l_y ;

  triangle_set.clear() ;
  contour_coords.clear();
  set_vertexZ_flag = {{false, false, false, false}} ;
  found_verts_flag = false ;
}

//--------------------------------------------------------------
void CONFIND::Cell::SetVertexZ(const size_t& idx_in, const double& val_z)   
{
  if (idx_in > 4 || idx_in < 1)
  {
    (void)0 ;
    return ;
  }
  
  verts_set[idx_in].xyz.z = val_z ;

  // There is an offset between the definitions of verts_set & set_vertexZ_flag
  // because we are not including the center vertex anymore
  set_vertexZ_flag[idx_in - 1] = true ;

}

//--------------------------------------------------------------
void CONFIND::Cell::SetVertex(const size_t& idx_in, const double& val_x, 
                     const double& val_y, const double& val_z)
{
  if (idx_in > 4)
  {
    (void)0 ;
    return ;
  }
  verts_set[idx_in].xyz.x = val_x ;
  verts_set[idx_in].xyz.y = val_y ;
  verts_set[idx_in].xyz.z = val_z ;

  set_vertex_flag[idx_in] = true ;
}

//--------------------------------------------------------------
void CONFIND::Cell::SetVertex(const size_t& idx_in, 
                              const Zaki::Physics::Coord3D& val)
{
  (void)0 ;
  if (idx_in > 4)
  {
    (void)0 ;
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
    (void)0 ;
    return {-1, -1};
  }

  return idx;
}

//--------------------------------------------------------------
CONFIND::vertex CONFIND::Cell::operator[](const size_t idx_in) const
{
  if( set_vertex_flag[idx_in] )
  {
    (void)0 ;
  }
  
  return verts_set[idx_in] ;
}

//--------------------------------------------------------------
// Used for optimizing the process
double CONFIND::Cell::GetFuncVals(const size_t& i) const 
{
  // Returning the function values at vertex i
  return verts_set[i].xyz.z ;
}

//--------------------------------------------------------------
void CONFIND::Cell::EvalCenter()
{ 
  (void)0 ;
  double cen_x  = (verts_set[B_Left].xyz.x + verts_set[B_Right].xyz.x) / 2 ;
  double cen_y  = (verts_set[B_Left].xyz.y + verts_set[T_Left].xyz.y) / 2 ;
  double cen_z  = (  verts_set[B_Left].xyz.z + verts_set[B_Right].xyz.z
                   + verts_set[T_Right].xyz.z + verts_set[T_Left].xyz.z) / 4 ;

  // idx = 0 is for center
  SetVertex(Center, {cen_x, cen_y, cen_z}) ;
}

//--------------------------------------------------------------
void CONFIND::Cell::SetBundlePtr(Bundle* bun_in)
{
  BundlePtr = bun_in ;
  set_bundle_ptr_flag = true ;
  BundlePtr->Grid.xAxis.scale == "Log" ? G_x_min = log10(BundlePtr->Grid.xAxis.Min()) : G_x_min = BundlePtr->Grid.xAxis.Min() ;
  BundlePtr->Grid.yAxis.scale == "Log" ? G_y_min = log10(BundlePtr->Grid.yAxis.Min()) : G_y_min = BundlePtr->Grid.yAxis.Min() ;
}

//--------------------------------------------------------------
double CONFIND::Cell::GetLX()  const
{
  if ( !set_size_flag )
  {
    (void)0 ;
    return -1;
  }  

  return l_x;
}

//--------------------------------------------------------------
double CONFIND::Cell::GetLY()  const
{
  if ( !set_size_flag )
  {
    (void)0 ;
    return -1;
  }  

  return l_y;
}

//--------------------------------------------------------------
double CONFIND::Cell::EvalFunc(const double& x, const double& y)
{
  (void)0 ;
  if ( BundlePtr->Func )
    return EvalSimpleFunc(x, y) ;
  else
    return EvalMemFunc(x, y) ;
}

//--------------------------------------------------------------
double CONFIND::Cell::EvalMemFunc(const double& x, const double& y)
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
   (void)0 ;
    return -1 ;
  } 
}

//--------------------------------------------------------------
double CONFIND::Cell::EvalSimpleFunc(const double& x, const double& y)
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
   (void)0 ;
    return -1 ;
  } 
}

//--------------------------------------------------------------
void CONFIND::Cell::FindVerts() 
{
  (void)0 ;
  // If made from the full constructor no need to check
  //  other conditions
  if(!set_full_constructor)
  {
    if (!set_bundle_ptr_flag)
    {
      (void)0 ;
      return ;
    }
    if(!set_idx_flag)
    {
      (void)0 ;
      return ;
    }
    if(!set_size_flag)
    {
      (void)0 ;
      return ;
    }
  }
  //.................................................

  //.......................................
  // There is an offset between 'set_vertexZ_flag' & 'verts_set' definitions
  //.......................................
  if (set_vertexZ_flag[B_Left-1])
  SetVertex(B_Left, {x_min,  y_min, verts_set[B_Left].xyz.z}) ;
  else
  SetVertex(B_Left, {x_min,  y_min,
                   EvalFunc(x_min, y_min)}) ;
  //.......................................
  if (set_vertexZ_flag[B_Right-1])
  SetVertex(B_Right, {x_min + l_x, y_min, verts_set[B_Right].xyz.z} ) ;
  else
  SetVertex(B_Right, {x_min + l_x, y_min, 
                   EvalFunc(x_min + l_x, y_min)} ) ;
  //.......................................
  if (set_vertexZ_flag[T_Right-1])
  SetVertex(T_Right, {x_min +l_x, y_min + l_y, verts_set[T_Right].xyz.z} ) ;
  else
  SetVertex(T_Right, {x_min + l_x, y_min + l_y, 
                   EvalFunc(x_min + l_x, y_min + l_y)} ) ;
  //.......................................
  if (set_vertexZ_flag[T_Left-1])
  SetVertex(T_Left, {x_min, y_min + l_y,  verts_set[T_Left].xyz.z} ) ;
  else
  SetVertex(T_Left, {x_min, y_min + l_y, 
                   EvalFunc(x_min, y_min + l_y)} ) ;
  //.......................................

  EvalCenter()         ; // Center of the cell

  SetTriangles() ;

  found_verts_flag = true ;
}

//--------------------------------------------------------------
void CONFIND::Cell::SetTriangles()
{
  (void)0 ;
  triangle_set.reserve(4) ;

  // 0: Bottom triangle
  triangle_set.emplace_back(verts_set[Center], verts_set[B_Left], verts_set[B_Right]) ;

  // 1: Right triangle
  triangle_set.emplace_back(verts_set[Center], verts_set[B_Right], verts_set[T_Right]) ;

  // 2: Top triangle
  triangle_set.emplace_back(verts_set[Center], verts_set[T_Right], verts_set[T_Left]) ;

  // 3: Left triangle
  triangle_set.emplace_back(verts_set[Center], verts_set[T_Left], verts_set[B_Left]) ;
}

//--------------------------------------------------------------
void CONFIND::Cell::SetContourValue(const double& cont_in)
{
  contour_val = cont_in ;
  set_contour_val_flag = true ;
}

//--------------------------------------------------------------
double CONFIND::Cell::GetContourValue() const
{
  if (!set_contour_val_flag)
  {
    (void)0 ;
    return -1;
  }
  return contour_val  ;
}

//--------------------------------------------------------------
int CONFIND::Cell::GetStatus()
{
  (void)0 ;
  if (!found_verts_flag)
  {
    (void)0 ;
    return -1;
  }

  if (!set_contour_val_flag)
  {
    (void)0 ;
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
void CONFIND::Cell::case36(const triangle& tri, const int& odd_sign)
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
