// Compile this same harness against vendored or candidate headers/archives.
// No Coord3D comparators are instantiated here. Samples deliberately call libm.
#include <Confind/ContourFinder.hpp>
#include <Zaki/Vector/DataSet.hpp>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>
static double (*volatile historical_pow)(double,double)=std::pow;
static uint64_t bits(double x){uint64_t u;std::memcpy(&u,&x,8);return u;}
static void dump(FILE* f,const char* name,std::vector<CONFIND::Cont2D> cs){
  std::fprintf(f,"FIXTURE\t%s\t%zu\n",name,cs.size());
  for(size_t c=0;c<cs.size();++c){
    std::fprintf(f,"CONTOUR\t%zu\t%a\t%016llx\t%d\t%zu\n",c,cs[c].GetVal(),(unsigned long long)bits(cs[c].GetVal()),int(cs[c].GetFound()),cs[c].size());
    for(size_t i=0;i<cs[c].size();++i){auto p=cs[c][i];std::fprintf(f,"RAW\t%zu\t%a\t%a\t%a\t%016llx\t%016llx\t%016llx\n",i,p.x,p.y,p.z,(unsigned long long)bits(p.x),(unsigned long long)bits(p.y),(unsigned long long)bits(p.z));}
    if(cs[c].size()) {auto cv=cs[c].ConvertToCurve2D();std::fprintf(f,"CURVE\t%zu\n",cv.pts.size());
      for(size_t i=0;i<cv.pts.size();++i){auto p=cv.pts[i];std::fprintf(f,"POINT\t%zu\t%a\t%a\t%016llx\t%016llx\n",i,p.x,p.y,(unsigned long long)bits(p.x),(unsigned long long)bits(p.y));}}
  }
}
int main(int argc,char**argv){
  if(argc!=5)throw std::runtime_error("usage: output small|stellar workers perturb(0|1)");
  FILE* out=std::fopen(argv[1],"w");if(!out)throw std::runtime_error("output");
  bool small=std::string(argv[2])=="small";size_t workers=std::stoul(argv[3]);bool perturb=std::string(argv[4])=="1";
  size_t nx=small?1:99,ny=small?1:150;
  Zaki::Math::Grid2D g=small?Zaki::Math::Grid2D{{{1,100},nx,"Log"},{{0,1},ny,"Linear"}}:
    Zaki::Math::Grid2D{{{8e14,5e15},nx,"Log"},{{5e13,5e16},ny,"Log"}};
  auto field=[=](double x,double y){if(small)return std::log10(x);double u=std::log10(x)-15.3,v=std::log10(y)-15.0;return std::exp(-(u*u*3+v*v))+0.1*u*v;};
  const double x0=std::log10(g.xAxis.Min()),dx=(std::log10(g.xAxis.Max())-x0)/nx;
  const double y0=small?0:std::log10(g.yAxis.Min()),dy=((small?1:std::log10(g.yAxis.Max()))-y0)/ny;
  std::vector<double> ii,jj,zz,levels;
  // Known libm split: pow=0x1.00a7b7380b140p+0, exp10=0x1.00a7b7380b13fp+0.
  if(small)levels={0x1.22fad6cb53501p-10};else for(double f=0.70;f<0.96;f+=0.005)levels.push_back(f);
  for(size_t j=0;j<=ny;++j)for(size_t i=0;i<=nx;++i){ii.push_back(i);jj.push_back(j);zz.push_back(field(historical_pow(10.,x0+i*dx),small?y0+j*dy:historical_pow(10.,y0+j*dy)));}
  Zaki::Vector::DataSet ds(std::vector<Zaki::Vector::DataColumn>{{"i",ii},{"j",jj},{"z",zz}});
  Zaki::Math::GridVals_2D gv(ds,0,nx,1,ny,2);
  CONFIND::ContourFinder con;con.SetGrid(g);con.SetContVal(levels);
#ifdef CONFIND_MODERN
  if(workers)con.Evaluate([=](size_t){return [=](double x,double y){if(perturb){std::this_thread::yield();if((bits(x)^bits(y))%7==0)std::this_thread::sleep_for(std::chrono::microseconds(1));}return field(x,y);};},{workers});
  else
#else
  if(workers)throw std::runtime_error("historical oracle has no Evaluate API");
#endif
  con.SetGridVals(&gv);
  dump(out,small?"M1_small_pow_split":"M1_TaskManager_99x150_52levels",con.GetContourSet());
  std::fclose(out);
}
