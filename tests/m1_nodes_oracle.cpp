// Authority recipe: vendored GetDeltas(), then the predeclared sampled-node
// expression min+i*delta (NOT legacy cell-local callable rounding).
#include <Confind/ContourFinder.hpp>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstring>
static double (*volatile historical_pow)(double,double)=std::pow;
static uint64_t bits(double x){uint64_t u;std::memcpy(&u,&x,8);return u;}
int main(int,char**argv){FILE*f=std::fopen(argv[1],"w");if(!f)return 1;
 for(int log=0;log<2;++log){
  Zaki::Math::Grid2D g{{{0.13,3.71},7,log?"Log":"Linear"},{{0.23,2.67},6,log?"Log":"Linear"}};
  CONFIND::ContourFinder c;c.SetGrid(g);auto d=c.GetDeltas();
  double x0=log?std::log10(g.xAxis.Min()):g.xAxis.Min(),y0=log?std::log10(g.yAxis.Min()):g.yAxis.Min();
  for(size_t j=0;j<=6;++j)for(size_t i=0;i<=7;++i){double x=x0+i*d.first,y=y0+j*d.second;if(log){x=historical_pow(10.,x);y=historical_pow(10.,y);}
   std::fprintf(f,"%d\t%zu\t%zu\t%a\t%a\t%016llx\t%016llx\n",log,i,j,x,y,(unsigned long long)bits(x),(unsigned long long)bits(y));}}
 std::fclose(f);
}
