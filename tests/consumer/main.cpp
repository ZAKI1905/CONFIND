#include <Confind/ContourFinder.hpp>
#include <Confind/ConfindConfig.h>
int main() {
 static_assert(CONFIND_VERSION_MAJOR == 2);
 CONFIND::ContourFinder finder;
 finder.SetGrid({{{0,4},4,"Linear"},{{0,4},4,"Linear"}});
 finder.SetContVal({1.25});
 finder.Evaluate([](std::size_t) { return [](double x,double) { return x; }; }, {2});
 auto contours=finder.GetContourSet();
 if(contours.size()!=1 || !contours[0].GetFound() || contours[0].size()!=24) return 1;
 for(size_t i=0;i<contours[0].size();++i) if(contours[0][i].x!=1.25) return 2;
 return 0;
}
