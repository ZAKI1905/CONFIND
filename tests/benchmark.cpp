#include <Confind/ContourFinder.hpp>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <set>
#include <thread>
#include <vector>
static uint64_t bits(double x){uint64_t v;std::memcpy(&v,&x,8);return v;}
static std::vector<uint64_t> snapshot(const std::vector<CONFIND::Cont2D>& contours){
 std::vector<uint64_t> out;
 for(const auto& c:contours){out.push_back(c.size());out.push_back(c.GetFound());out.push_back(bits(c.GetVal()));
 for(size_t i=0;i<c.size();++i){auto p=c[i];out.push_back(bits(p.x));out.push_back(bits(p.y));out.push_back(bits(p.z));}}
 return out;
}
struct State{uint64_t checksum=0;};
int main(int argc,char**argv){
 const size_t cost=argc>1?std::stoul(argv[1]):40000;
 const size_t max_workers=std::min(8u,std::max(1u,std::thread::hardware_concurrency()));
 const size_t tasks=65*65;
 std::vector<uint64_t> baseline;uint64_t reference_checksum=0;
 std::cout<<"workers\ttasks\tcost\tseconds\ttasks_per_second\tchecksum\n";
 for(size_t workers:std::set<size_t>{1,2,4,max_workers}){
   if(workers>std::max(1u,std::thread::hardware_concurrency()))continue;
   std::vector<std::shared_ptr<State>> states;
   CONFIND::ContourFinder c;c.SetGrid({{{0,64},64,"Linear"},{{0,64},64,"Linear"}});c.SetContVal({15.25,60.25,90.25});
   auto start=std::chrono::steady_clock::now();
   c.Evaluate([&](size_t){auto state=std::make_shared<State>();states.push_back(state);
     return [=](double x,double y){
       uint64_t v=static_cast<uint64_t>(x)+65*static_cast<uint64_t>(y)+1;
       const size_t work=cost+(v%7)*(cost/4); // Deterministic variable CPU cost, no sleeps.
       for(size_t n=0;n<work;++n){v^=v>>12;v^=v<<25;v^=v>>27;v*=2685821657736338717ULL;}
       state->checksum^=v;return x+2*y;
     };},{workers});
   double seconds=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
   uint64_t checksum=0;for(const auto&s:states)checksum^=s->checksum;
   auto result=snapshot(c.GetContourSet());
   if(workers==1){baseline=result;reference_checksum=checksum;}
   else if(result!=baseline||checksum!=reference_checksum)return 2;
   std::cout<<workers<<'\t'<<tasks<<'\t'<<cost<<'\t'<<std::setprecision(9)<<seconds<<'\t'<<tasks/seconds<<'\t'<<std::hex<<checksum<<std::dec<<'\n';
 }
}
