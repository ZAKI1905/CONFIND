#include <Confind/ContourFinder.hpp>
#include <Zaki/Vector/DataSet.hpp>
#include <vector>
int main(int argc,char** argv) {
  if(argc!=2)return 2;
  std::vector<double> i,j,z;
  for(int y=0;y<=4;++y)for(int x=0;x<=4;++x){i.push_back(x);j.push_back(y);z.push_back(x);}
  Zaki::Vector::DataSet ds(std::vector<Zaki::Vector::DataColumn>{{"i",i},{"j",j},{"z",z}});
  Zaki::Math::GridVals_2D values(ds,0,4,1,4,2);
  CONFIND::ContourFinder c;c.SetGrid({{{0,4},4,"Linear"},{{0,4},4,"Linear"}});
  c.SetContVal({1.25,2.25},{"first","second"});c.SetGridVals(&values);
  c.SetWrkDir(argv[1]);c.ExportContour("contour",Zaki::File::FileMode::Write);
}
