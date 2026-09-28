// Historical characterization adapted from the hash-verified reconnaissance probe
// behavioural probe of the vendored CompactStar arm64 libConfind.a.
// Not part of any repository. Uses only the authenticated vendored public headers.
// Deliberately does NOT use Coord3D::operator< / operator== / XYDist2 so that this
// translation unit emits no competing weak definitions of the comparator.
#include <Confind/ContourFinder.hpp>
#include <Zaki/Util/Logger.hpp>
#include <Zaki/Vector/DataSet.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <limits>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <array>
#include <stdexcept>

static FILE* OUT = nullptr;

static uint64_t bits(double d) { uint64_t u; std::memcpy(&u, &d, 8); return u; }

// FNV-1a over the exact bit patterns of a point sequence (order-sensitive)
static uint64_t seqhash(const std::vector<std::pair<double, double>>& v)
{
  uint64_t h = 1469598103934665603ULL;
  for (auto& p : v)
    for (uint64_t w : {bits(p.first), bits(p.second)})
      for (int k = 0; k < 8; ++k) { h ^= (w >> (8 * k)) & 0xff; h *= 1099511628211ULL; }
  return h;
}

// r^2 exactly as the vendored (unfused, -O0) Coord3D::XYDist2({0,0,0}) computes it.
// This TU is compiled with -ffp-contract=off so the expression is not fused.
static double r2_unfused(double x, double y)
{
  volatile double xx = (x - 0.0) * (x - 0.0);
  volatile double yy = (y - 0.0) * (y - 0.0);
  return xx + yy;
}

struct Summary
{
  size_t raw = 0, exact_distinct = 0, r2_classes = 0, curve = 0;
  uint64_t raw_hash = 0, curve_hash = 0;
};

static void dump_points(const char* tag, const std::vector<std::pair<double, double>>& v, size_t maxn)
{
  for (size_t i = 0; i < v.size() && i < maxn; ++i)
    std::fprintf(OUT, "    %s[%zu] = (%.17g, %.17g)  [%a, %a]\n", tag, i, v[i].first, v[i].second,
                 v[i].first, v[i].second);
  if (v.size() > maxn) std::fprintf(OUT, "    %s ... (%zu more)\n", tag, v.size() - maxn);
}

// Complete deterministic records: raw order, z, bit identity and curve order.
static Summary analyse(const char* name, std::vector<CONFIND::Cont2D> cs, size_t, bool curve_ok = true)
{
  Summary total;
  std::fprintf(OUT, "FIXTURE\t%s\t%zu\n", name, cs.size());
  for (size_t c = 0; c < cs.size(); ++c)
  {
    std::set<std::array<uint64_t,3>> ex;
    std::set<uint64_t> radii;
    size_t exact = 0;
    for (size_t i=0; i<cs[c].size(); ++i) {
      auto p=cs[c][i]; ex.insert({bits(p.x),bits(p.y),bits(p.z)});
      radii.insert(bits(r2_unfused(p.x,p.y)));
      bool prior=false;
      for(size_t j=0;j<i;++j) { auto q=cs[c][j]; if(p.x==q.x && p.y==q.y && p.z==q.z) {prior=true;break;} }
      if(!prior) ++exact;
    }
    std::fprintf(OUT,"CONTOUR\t%zu\t%a\t%016llx\t%d\t%zu\t%zu\t%zu\t%zu\n",c,cs[c].GetVal(),
      (unsigned long long)bits(cs[c].GetVal()),int(cs[c].GetFound()),cs[c].size(),exact,ex.size(),radii.size());
    for (size_t i=0; i<cs[c].size(); ++i) {
      auto p=cs[c][i]; std::fprintf(OUT,"RAW\t%zu\t%a\t%a\t%a\t%016llx\t%016llx\t%016llx\n",i,p.x,p.y,p.z,
        (unsigned long long)bits(p.x),(unsigned long long)bits(p.y),(unsigned long long)bits(p.z));
    }
    if(curve_ok && cs[c].size()) {
      auto cv=cs[c].ConvertToCurve2D(); std::fprintf(OUT,"CURVE\t%zu\n",cv.pts.size());
      for(size_t i=0;i<cv.pts.size();++i) {auto p=cv.pts[i];std::fprintf(OUT,"POINT\t%zu\t%a\t%a\t%016llx\t%016llx\n",i,p.x,p.y,
        (unsigned long long)bits(p.x),(unsigned long long)bits(p.y));}
    } else std::fprintf(OUT,"CURVE\tHISTORICAL_DEFECT_EMPTY_CONVERSION_NOT_INVOKED\n");
  }
  return total;
}

// ---------------------------------------------------------------------------
using Field = std::function<double(double, double)>;

static Zaki::Math::Grid2D grid(double x0, double x1, size_t nx, double y0, double y1, size_t ny)
{
  return {{{x0, x1}, nx, "Linear"}, {{y0, y1}, ny, "Linear"}};
}

// Sampled-grid path (the one CompactStar uses): SetGrid + SetContVal + SetGridVals(GridVals_2D*)
static std::vector<CONFIND::Cont2D> run_sampled(const Zaki::Math::Grid2D& g, const Field& f,
                                                const std::vector<double>& lv,
                                                const std::vector<std::pair<size_t, size_t>>& drop = {})
{
  const size_t nx = g.xAxis.res, ny = g.yAxis.res;
  const double x0 = g.xAxis.scale=="Log" ? std::log10(g.xAxis.Min()) : g.xAxis.Min();
  const double x1 = g.xAxis.scale=="Log" ? std::log10(g.xAxis.Max()) : g.xAxis.Max();
  const double y0 = g.yAxis.scale=="Log" ? std::log10(g.yAxis.Min()) : g.yAxis.Min();
  const double y1 = g.yAxis.scale=="Log" ? std::log10(g.yAxis.Max()) : g.yAxis.Max();
  const double dx = (x1 - x0) / nx;  // same expression as CONFIND::SetDeltas
  const double dy = (y1 - y0) / ny;
  std::vector<double> iv, jv, zv;
  for (size_t j = 0; j <= ny; ++j)
    for (size_t i = 0; i <= nx; ++i)
    {
      bool skip = false;
      for (auto& d : drop) if (d.first == i && d.second == j) skip = true;
      if (skip) continue;
      iv.push_back(i); jv.push_back(j);
      double x=x0+i*dx, y=y0+j*dy;
      zv.push_back(f(g.xAxis.scale=="Log" ? std::pow(10,x) : x, g.yAxis.scale=="Log" ? std::pow(10,y) : y));
    }
  Zaki::Vector::DataSet ds(std::vector<Zaki::Vector::DataColumn>{
      Zaki::Vector::DataColumn("i", iv), Zaki::Vector::DataColumn("j", jv), Zaki::Vector::DataColumn("z", zv)});
  Zaki::Math::GridVals_2D gv(ds, 0, nx, 1, ny, 2);
  CONFIND::ContourFinder con;
  con.SetGrid(g);
  con.SetContVal(lv);
  con.SetGridVals(&gv);
  return con.GetContourSet();
}

static Field g_callable;
static double callable_trampoline(double x, double y) { return g_callable(x, y); }

static std::vector<CONFIND::Cont2D> run_callable(const Zaki::Math::Grid2D& g, const Field& f,
                                                 const std::vector<double>& lv,
                                                 CONFIND::ContourFinder::Mode m, int threads = 1)
{
  g_callable = f;
  CONFIND::ContourFinder con;
  con.SetGrid(g);
  con.SetContVal(lv);
  con.SetFunc(&callable_trampoline);
#ifndef CONFIND_MODERN
  con.SetThreads(threads);
#else
  (void)threads;
#endif
  con.SetGridVals(m);
  return con.GetContourSet();
}

int main(int argc, char** argv)
{
  OUT = std::fopen(argc > 1 ? argv[1] : "oracle_out.txt", "w");
  if(!OUT) throw std::runtime_error("cannot open output");
  Zaki::Util::LogManager::SetLogLevels(Zaki::Util::LogLevel::Error);

  // F01 constant field (ConvertToCurve2D deliberately skipped: SortNew reads pts[0] on empty -> UB)
  analyse("F01_constant", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double, double) { return 1.0; }, {0.5, 1.0, 2.0}), 4, false);

  // F02..F04 linear crossings
  analyse("F02_vertical_x=1.25", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double) { return x; }, {1.25}), 40);
  analyse("F03_vertical_centre_on_level_x=1.5", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double) { return x; }, {1.5}), 40);
  analyse("F04_horizontal_y=2.25", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double, double y) { return y; }, {2.25}), 40);
  analyse("F05a_diagonal_x+y=2.5", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double y) { return x + y; }, {2.5}), 60);
  analyse("F05b_diagonal_through_vertices_x+y=3", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double y) { return x + y; }, {3.0}), 60);

  // F06..F08 circles (origin-centred = Coord3D radius-ordering stress; off-centre = control)
  analyse("F06a_circle_origin_r2=1_(vertices_on_level)", run_sampled(grid(-2, 2, 8, -2, 2, 8), [](double x, double y) { return x * x + y * y; }, {1.0}), 80);
  analyse("F06b_circle_origin_r2=1.7", run_sampled(grid(-2, 2, 8, -2, 2, 8), [](double x, double y) { return x * x + y * y; }, {1.7}), 80);
  analyse("F07_circle_offcentre_r2=1.7", run_sampled(grid(-2, 2, 8, -2, 2, 8), [](double x, double y) { return (x - 0.3) * (x - 0.3) + (y - 0.2) * (y - 0.2); }, {1.7}), 80);

  // F09 saddle z = x*y ; origin at a cell centre (n odd) and at a vertex (n even)
  analyse("F09a_saddle_centre_cell_level0", run_sampled(grid(-1.5, 1.5, 3, -1.5, 1.5, 3), [](double x, double y) { return x * y; }, {0.0}), 60);
  analyse("F09b_saddle_centre_cell_level+0.1", run_sampled(grid(-1.5, 1.5, 3, -1.5, 1.5, 3), [](double x, double y) { return x * y; }, {0.1}), 60);
  analyse("F09c_saddle_centre_cell_level-0.1", run_sampled(grid(-1.5, 1.5, 3, -1.5, 1.5, 3), [](double x, double y) { return x * y; }, {-0.1}), 60);
  analyse("F09d_saddle_vertex_level0", run_sampled(grid(-2, 2, 4, -2, 2, 4), [](double x, double y) { return x * y; }, {0.0}), 60);
  // pure checkerboard 2x2 samples: corners (+1,-1,+1,-1): centre avg 0
  analyse("F09e_checkerboard_level0", run_sampled(grid(0, 1, 1, 0, 1, 1), [](double x, double y) { return (x == y) ? 1.0 : -1.0; }, {0.0}), 40);
  analyse("F09f_checkerboard_level+0.5", run_sampled(grid(0, 1, 1, 0, 1, 1), [](double x, double y) { return (x == y) ? 1.0 : -1.0; }, {0.5}), 40);
  analyse("F09g_checkerboard_level-0.5", run_sampled(grid(0, 1, 1, 0, 1, 1), [](double x, double y) { return (x == y) ? 1.0 : -1.0; }, {-0.5}), 40);

  // F10 two disjoint closed contours
  analyse("F10_two_circles", run_sampled(grid(-3, 3, 12, -1.5, 1.5, 6), [](double x, double y) { return std::min((x - 1.5) * (x - 1.5) + y * y, (x + 1.5) * (x + 1.5) + y * y); }, {0.5}), 0);

  // F11/F12 boundary & grid-edge coincidence
  analyse("F11a_on_left_boundary_x=0", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double) { return x; }, {0.0}), 40);
  analyse("F11b_on_right_boundary_x=4", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double) { return x; }, {4.0}), 40);
  analyse("F12_along_interior_grid_line_x=2", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double) { return x; }, {2.0}), 60);

  // F13 two nearby branches
  analyse("F13_near_branches", run_sampled(grid(0, 4, 16, 0, 1, 2), [](double x, double) { return (x - 2) * (x - 2); }, {0.01}), 40);

  // F14 missing samples -> def_val = -1 (GridVals_2D second ctor)
  analyse("F14_missing_sample_defval", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double) { return x + 10.0; }, {11.5}, {{2, 2}}), 60);

  // F15 callable (Fast / Normal) vs sampled equivalence on exact dyadic grids
  {
    Field circ = [](double x, double y) { return x * x + y * y; };
    Field sadl = [](double x, double y) { return x * y; };
    auto g1 = grid(-2, 2, 8, -2, 2, 8);
    analyse("F15a_sampled_circle", run_sampled(g1, circ, {1.7, 1.0}), 0);
    analyse("F15b_callable_Fast_circle", run_callable(g1, circ, {1.7, 1.0}, CONFIND::ContourFinder::Fast), 0);
    analyse("F15c_callable_Normal_circle", run_callable(g1, circ, {1.7, 1.0}, CONFIND::ContourFinder::Normal), 0);
    auto g2 = grid(-1.5, 1.5, 3, -1.5, 1.5, 3);
    analyse("F15d_sampled_saddle", run_sampled(g2, sadl, {0.1}), 0);
    analyse("F15e_callable_Fast_saddle", run_callable(g2, sadl, {0.1}, CONFIND::ContourFinder::Fast), 0);
  }

  // F17 CompactStar-scale coordinates (~1e15), smooth field, 50x50
  {
    auto g = grid(1e15, 4e15, 50, 3e13, 1.4e15, 50);
    Field f = [](double x, double y) { double u = x / 1e15 - 2.3, v = y / 1e15 - 0.6; return std::exp(-(u * u + 2 * v * v)); };
    analyse("F17_CompactStar_scale", run_sampled(g, f, {0.3, 0.6, 0.9}), 0);
  }

  // F18 NaN sample in the field (status() treats NaN as 'on' the level)
  analyse("F18_nan_sample", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double y) { return (x == 2 && y == 2) ? std::numeric_limits<double>::quiet_NaN() : x; }, {1.25}), 60);

  // F19 Plot() side-effect probe: raw order before and after Plot()
  {
    auto g = grid(-2, 2, 8, -2, 2, 8);
    Field f = [](double x, double y) { return (x - 0.3) * (x - 0.3) + (y - 0.2) * (y - 0.2); };
    std::vector<double> iv, jv, zv; const size_t n = 8; const double d = 0.5;
    for (size_t j = 0; j <= n; ++j) for (size_t i = 0; i <= n; ++i) { iv.push_back(i); jv.push_back(j); zv.push_back(f(-2 + i * d, -2 + j * d)); }
    Zaki::Vector::DataSet ds(std::vector<Zaki::Vector::DataColumn>{Zaki::Vector::DataColumn("i", iv), Zaki::Vector::DataColumn("j", jv), Zaki::Vector::DataColumn("z", zv)});
    Zaki::Math::GridVals_2D gv(ds, 0, n, 1, n, 2);
    CONFIND::ContourFinder con; con.SetGrid(g); con.SetContVal({1.7}); con.SetGridVals(&gv);
    analyse("F19a_before_Plot", con.GetContourSet(), 0);
#ifndef CONFIND_MODERN
    con.SetPlotConnected();
    con.Plot("scratch_plot_probe");
#endif
    analyse("F19b_after_Plot", con.GetContourSet(), 0);
  }


  // C13: explicit exact duplicate, radius classes, and one-ulp twins.
  {
    CONFIND::Cont2D c(0.5); c.SetFound();
    c.AddPts({{1,0,0.5},{0,1,0.5},{-1,0,0.5},{1,0,0.5},
      {1,std::nextafter(0.0,1.0),0.5},{2,0,0.5},{std::nextafter(2.0,3.0),0,0.5}});
    analyse("C13_explicit_radius_exact_ulp",{c},0);
  }
  analyse("C14_reverse_x",run_sampled(grid(4,0,4,0,4,4),[](double x,double){return x;},{1.25}),0);
  analyse("C14_reverse_both",run_sampled(grid(4,0,4,4,0,4),[](double x,double y){return x+y;},{2.5}),0);
  // C21: append levels after a first scan; existing found levels are skipped.
  {
    auto g=grid(0,4,4,0,4,4); std::vector<double> ii,jj,zz;
    for(size_t j=0;j<=4;++j) for(size_t i=0;i<=4;++i) {ii.push_back(i);jj.push_back(j);zz.push_back(i);}
    Zaki::Vector::DataSet ds(std::vector<Zaki::Vector::DataColumn>{{"i",ii},{"j",jj},{"z",zz}});
    Zaki::Math::GridVals_2D gv(ds,0,4,1,4,2);
    CONFIND::ContourFinder con; con.SetGrid(g); con.SetContVal({2.25,1.25,2.25});
    analyse("C21_before",con.GetContourSet(),0,false);
    con.SetGridVals(&gv); analyse("C21_first",con.GetContourSet(),0);
    con.SetContVal({3.25}); analyse("C21_append_before",con.GetContourSet(),0);
    con.SetGridVals(&gv); analyse("C21_append_after",con.GetContourSet(),0);
    con.SetGridVals(&gv); analyse("C21_repeat",con.GetContourSet(),0);
  }
  // C22: each log-axis combination, plus serial callable paths.
  for(int mask=1;mask<=3;++mask) {
    auto g=grid(1,100,4,1,100,4);
    if(mask&1)g.xAxis.scale="Log"; if(mask&2)g.yAxis.scale="Log";
    Field f=[](double x,double y){return std::log10(x)+2*std::log10(y);};
    auto n="C22_log_"+std::to_string(mask);
    analyse((n+"_sampled").c_str(),run_sampled(g,f,{1.3,2.7}),0);
    analyse((n+"_Fast").c_str(),run_callable(g,f,{1.3,2.7},CONFIND::ContourFinder::Fast),0);
    analyse((n+"_Normal").c_str(),run_callable(g,f,{1.3,2.7},CONFIND::ContourFinder::Normal),0);
  }
  // Additional non-dyadic callable coordinates; preserved as their own historical path.
  {
    auto g=grid(0.13,3.71,7,-0.23,2.67,6); Field f=[](double x,double y){return x*x+2*y;};
    analyse("C15_nondyadic_sampled",run_sampled(g,f,{1.31,2.43}),0);
    analyse("C15_nondyadic_Fast",run_callable(g,f,{1.31,2.43},CONFIND::ContourFinder::Fast),0);
    analyse("C15_nondyadic_Normal",run_callable(g,f,{1.31,2.43},CONFIND::ContourFinder::Normal),0);
  }
  // C23 bounded analogues of TaskManager consumer operations, no stellar solves.
  {
    auto cs=run_sampled(grid(0,4,4,0,4,4),[](double x,double){return x;},{1.25});
    analyse("C23_critical_raw",cs,0);
    size_t selected=0; double maximum=-INFINITY;
    for(size_t p=0;p+1<cs[0].size();++p) {auto q=cs[0][p];double v=q.x+2*q.y; if(v>maximum){maximum=v;selected=p;}}
    auto mass=cs[0].ConvertToCurve2D();
    Zaki::Math::Curve2D critical(std::vector<Zaki::Math::Coord2D>{{0,2.125},{4,2.125}});
    auto intersections=critical.Intersection(mass);
    std::fprintf(OUT,"CONSUMER\tcritical_index\t%zu\tmaximum\t%a\nmass_curve_0\t%a\t%a\nintersections\t%zu\n",selected,maximum,mass[0].x,mass[0].y,intersections.size());
    for(auto p:intersections) {
      std::fprintf(OUT,"INTERSECTION\t%a\t%a\n",p.x,p.y);
      auto halves=mass.Bisect(p);
      std::fprintf(OUT,"BISECT\t%zu\t%zu\t%zu\n",mass.GetIdx(p),halves.first.pts.size(),halves.second.pts.size());
      for(auto q:halves.first.pts)std::fprintf(OUT,"BISECT_FIRST\t%a\t%a\n",q.x,q.y);
    }
  }

  std::fclose(OUT);
  return 0;
}
