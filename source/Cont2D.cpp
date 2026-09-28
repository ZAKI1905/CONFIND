#include <cstdio>
#include <set>

#include <Zaki/Vector/Vector_Basic.hpp>

#include <Zaki/Math/Math_Core.hpp>

// Local headers
#include "Confind/Cont2D.hpp"

//==============================================================
// Cont2D class begins
//--------------------------------------------------------------
// Constructors
CONFIND::Cont2D::Cont2D(double in_val)
  : Base("Cont2D"), val(in_val) { }

//--------------------------------------------------------------
// Copy Constructor
CONFIND::Cont2D::Cont2D(const Cont2D& other)
  : Base("Cont2D")
{
  *this = other ;

}

//--------------------------------------------------------------
size_t CONFIND::Cont2D::size() const
{
  return pts.size() ;
}

//--------------------------------------------------------------
void CONFIND::Cont2D::SetLabel(const std::string& in_label)
{
  label = in_label ;
  set_label_flag = true ;
}

//--------------------------------------------------------------

//--------------------------------------------------------------

//--------------------------------------------------------------

//--------------------------------------------------------------
double CONFIND::Cont2D::GetVal() const
{
  return val ;
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
Zaki::Math::Curve2D CONFIND::Cont2D::ConvertToCurve2D()
{
  SortNew() ;

  Zaki::Math::Curve2D tmp_curve ;
  tmp_curve.Reserve(pts.size()) ;

  for (size_t i = 0; i < pts.size(); i++)
  {
    tmp_curve.Append({pts[i].x, pts[i].y}) ;
  }

  return tmp_curve ;
}

//--------------------------------------------------------------
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
Zaki::Physics::Coord3D CONFIND::Cont2D::operator[](const size_t& idx_in) const
{
  return pts[idx_in] ;
}

//--------------------------------------------------------------
//  Addition operator
CONFIND::Cont2D CONFIND::Cont2D::operator+(const Cont2D& in_c) const
{

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
  if (this == &in_c) { Cont2D copy(in_c); *this += copy; return; }

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
void CONFIND::Cont2D::Export(const Zaki::String::Directory& f_name,
                             const Zaki::File::FileMode& mode)
{
  if (pts.size() == 0)
  {
    char tmp[150] ;
    snprintf(tmp, sizeof(tmp), "Contour '%.2e' has no points within the specified range.", val) ;
    return ;
  }

  std::string suffix;
  if (set_label_flag) suffix=label;
  else { char level[64]; std::snprintf(level,sizeof(level),"%.2e",val); suffix=level; }
  const std::string output_name=f_name.Str()+"_"+suffix+".tsv";

  SortNew() ;
  Zaki::File::VecSaver my_saver(output_name, mode);
  my_saver.Export1D(pts) ;
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
// The new sorting algorithm (slow)
// void CONFIND::Cont2D::SortNew(const std::pair<double, double>& del)
void CONFIND::Cont2D::SortNew()
{

  if (already_sorted || pts.empty())
    return;

  if (!is_found_flag)
  {
    return ;
  }

  RMDuplicates() ;

  std::vector<Zaki::Physics::Coord3D> out ;
  out.reserve(pts.size());

  // Top
  // double ymax = pts[0].y ; size_t max = 0;
  // for (size_t i = 1; i < pts.size() ; i++)
  // {
  //   if ( (pts[i].y > ymax) || (ymax == pts[i].y && pts[i].x < pts[max].x))
  //     {ymax = pts[i].y; max = i;}
  // }

  // Left
  double xmin = pts[0].x ; size_t min = 0;
  for (size_t i = 1; i < pts.size() ; i++)
  {
    if ( (pts[i].x < xmin) || (xmin == pts[i].x && pts[i].y > pts[min].y))
      { xmin = pts[i].x; min = i; }
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

  if (already_sorted)
    return;

  if (!is_found_flag)
  {
    return ;
  }

  RMDuplicates() ;

  // Find the upper_left point
    double ymax = pts[0].y ; size_t max = 0;
   for (size_t i = 1; i < pts.size() ; i++)
   {
    // Pick the upper_left. In case of tie, choose the
    // left most point
    if ( (pts[i].y > ymax) || (ymax == pts[i].y && pts[i].x < pts[max].x))
    {   ymax = pts[i].y; max = i;   }
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

//--------------------------------------------------------------
//                Cont2D class ends
//==============================================================
