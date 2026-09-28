#include <Confind/ContourFinder.hpp>
#include <atomic>
#include <array>
#include <filesystem>
#include <fstream>
#include <limits>
#include <thread>
#include <stdexcept>
using CONFIND::ContourFinder;
static void require(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
template<class F> void invalid(F f){bool caught=false;try{f();}catch(const std::invalid_argument&){caught=true;}require(caught,"expected invalid_argument");}
static auto grid(size_t nx=4,size_t ny=4){return Zaki::Math::Grid2D{{{0,4},nx,"Linear"},{{0,4},ny,"Linear"}};}
static double field(double x,double){return x;}
static auto factory(){return [](size_t){return [](double x,double){return x;};};}
static bool same(std::vector<CONFIND::Cont2D> a,std::vector<CONFIND::Cont2D> b){
 if(a.size()!=b.size())return false;
 for(size_t c=0;c<a.size();++c){if(a[c].size()!=b[c].size()||a[c].GetFound()!=b[c].GetFound())return false;
 for(size_t i=0;i<a[c].size();++i){auto x=a[c][i],y=b[c][i];if(x.x!=y.x||x.y!=y.y||x.z!=y.z)return false;}}
 return true;
}
struct Tracked:Zaki::Math::Func2D {
 static int alive;
 Tracked(){++alive;} Tracked(const Tracked&):Tracked(){} ~Tracked(){--alive;}
 double Eval(double x,double)override{return x;}
 Tracked* IClone()const override{return new Tracked(*this);}
};
int Tracked::alive=0;
struct LocalState {
 std::atomic<int> busy{0};
 std::thread::id creator;
 std::atomic<int>& destroyed;
 LocalState(std::atomic<int>& d):creator(std::this_thread::get_id()),destroyed(d){}
 ~LocalState(){if(std::this_thread::get_id()!=creator)std::terminate();++destroyed;}
};
int main(int argc,char**argv){
 require(argc==2,"output directory required");
 // Undefined historical empty conversion is now safe, including the public sorter.
 CONFIND::Cont2D empty(1);empty.SetFound();empty.SortNew();require(empty.ConvertToCurve2D().pts.empty(),"empty curve");
 ContourFinder constant;constant.SetGrid(grid());constant.SetContVal({0,1,2});
 constant.Evaluate([](size_t){return [](double,double){return 1.;};},{4});
 for(auto c:constant.GetContourSet())require(c.GetFound()&&c.ConvertToCurve2D().pts.empty(),"constant field");
 ContourFinder c;
 invalid([&]{c.SetGrid(grid(0));});invalid([&]{c.SetGrid(grid(std::numeric_limits<size_t>::max()));});
 invalid([&]{c.SetGrid(grid(std::numeric_limits<size_t>::max()/2,9));});
 invalid([&]{c.SetDeltas();});invalid([&]{c.SetGridVals();});invalid([&]{c.Evaluate(factory(),{4});});
 invalid([&]{c.SetFunc(nullptr);});invalid([&]{c.SetMemFunc(nullptr);});
 c.SetGrid(grid());c.SetContVal({1.25},{"first"});
 invalid([&]{c.SetContVal({1,2},{"one"});});invalid([&]{c.SetGridVals(static_cast<Zaki::Math::GridVals_2D*>(nullptr));});
 std::vector<double> ix,iy,z;
 for(int j=0;j<=4;++j)for(int i=0;i<=4;++i){ix.push_back(i);iy.push_back(j);z.push_back(i);}
 Zaki::Vector::DataSet ds(std::vector<Zaki::Vector::DataColumn>{{"i",ix},{"j",iy},{"z",z}});
 Zaki::Math::GridVals_2D values(ds,0,4,1,4,2);
 values.n_x=3;invalid([&]{c.SetGridVals(&values);});values.n_x=4;
 auto saved=values.m_GridValArr;values.m_GridValArr=nullptr;invalid([&]{c.SetGridVals(&values);});values.m_GridValArr=saved;
 Zaki::Math::GridVals_2D moved_values(std::move(values));invalid([&]{c.SetGridVals(&values);});
 c.SetGridVals(&moved_values);auto reference=c.GetContourSet();
 // Each index is evaluated once and each state is used by at most one worker.
 std::array<std::atomic<int>,25> visits{};std::atomic<int> destroyed{0};int created=0;
 ContourFinder parallel;parallel.SetGrid(grid());parallel.SetContVal({1.25},{"first"});
 parallel.Evaluate([&](size_t){++created;auto state=std::make_shared<LocalState>(destroyed);
   return [&,state](double x,double y){require(state->busy.fetch_add(1)==0,"shared mutable evaluator state");
     ++visits[size_t(x)+5*size_t(y)];std::this_thread::yield();--state->busy;return x;};},{4});
 require(created==4&&destroyed==4,"factory lifetime");for(auto&n:visits)require(n==1,"sample index missing or repeated");
 require(same(reference,parallel.GetContourSet()),"indexed samples differ");
 // Factory exceptions start no evaluations. Worker exceptions join and propagate;
 // no contours are assembled until every sample succeeds.
 ContourFinder failing;failing.SetGrid(grid());failing.SetContVal({1.25});
 int calls=0;bool caught=false;
 try{failing.Evaluate([&](size_t w)->ContourFinder::Evaluator{if(w==2)throw std::runtime_error("factory");return [&](double x,double){++calls;return x;};},{4});}
 catch(const std::runtime_error&e){caught=std::string(e.what())=="factory";}
 require(caught&&calls==0,"factory failure propagation");
 destroyed=0;caught=false;
 try{failing.Evaluate([&](size_t){auto state=std::make_shared<LocalState>(destroyed);return [state](double x,double y){if(x==2&&y==2)throw std::runtime_error("worker");std::this_thread::yield();return x;};},{4});}
 catch(const std::runtime_error&e){caught=std::string(e.what())=="worker";}
 require(caught&&destroyed==4,"worker cleanup/propagation");
 require(!failing.GetContourSet()[0].GetFound()&&failing.GetContourSet()[0].size()==0,"partial result published");
 failing.Evaluate(factory(),{0});require(same(reference,failing.GetContourSet()),"zero worker fallback/recovery");
 invalid([&]{failing.Evaluate({},{4});});invalid([&]{failing.Evaluate([](size_t){return ContourFinder::Evaluator{};},{4});});
 // The pool clamps to available tasks (four samples here) even for large requests.
 ContourFinder tiny;tiny.SetGrid(grid(1,1));tiny.SetContVal({1.25});created=0;
 tiny.Evaluate([&](size_t){++created;return field;},{100});require(created==4,"task count clamp");
 // Owned legacy evaluators: clone, assignment over existing ownership, null source,
 // self assignment, move, moved-from reuse, and destruction.
 {
   ContourFinder owned;owned.SetGrid(grid());owned.SetContVal({1.25});owned.SetMemFunc(std::make_unique<Tracked>());
   ContourFinder copy(owned);require(Tracked::alive==2,"clone ownership");
   ContourFinder assigned;assigned.SetMemFunc(std::make_unique<Tracked>());assigned=owned;require(Tracked::alive==3,"assignment leak");
   assigned=assigned;copy.SetGridVals();require(same(reference,copy.GetContourSet()),"cloned evaluation");
   assigned=c;require(Tracked::alive==2,"stale evaluator after assignment");
   ContourFinder moved(std::move(owned));moved.SetGridVals();require(same(reference,moved.GetContourSet()),"move construction");
   assigned=std::move(moved);require(same(reference,assigned.GetContourSet()),"move assignment");
   owned.Clear();owned.SetGrid(grid());owned.SetContVal({1.25});owned.SetFunc(field);owned.SetGridVals();owned.SetGridVals();
   require(same(reference,owned.GetContourSet()),"clear/repeated callable");
 }
 require(Tracked::alive==0,"owned evaluator leak");
 // Existing callable modes are frozen; RAII must also survive evaluator exceptions.
 ContourFinder legacy;legacy.SetGrid(grid());legacy.SetContVal({1.25,2.25});legacy.SetFunc(field);legacy.SetGridVals();
 auto before=legacy.GetContourSet();legacy.SetGridVals();require(same(before,legacy.GetContourSet()),"repeat changed contour");
 // Long paths are complete; valid output retains the historical byte format.
 auto root=std::filesystem::path(argv[1]);auto longdir=root/std::string(90,'a')/std::string(90,'b');
 c.SetWrkDir(longdir.string());c.ExportContour("contour",Zaki::File::FileMode::Write);
 require(std::filesystem::exists(longdir/"contour_first.tsv"),"truncated export path");
 ContourFinder copied(c);require(copied.GetWrkDir().Str()==c.GetWrkDir().Str(),"work directory copy");
 // Comparator semantics (including collapsing distinct equal-radius points) stay frozen.
 CONFIND::Cont2D points(0);points.SetFound();points.AddPts({{1,0,0},{0,1,0},{1,0,0},{2,0,0}});
 points.RMDuplicates();require(points.size()==2,"radius semantics changed");
 auto sum=points+points;require(sum.size()==4,"contour addition");points+=points;require(points.size()==4,"contour append");
 return 0;
}
