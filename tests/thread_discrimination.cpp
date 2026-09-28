#include <Confind/ContourFinder.hpp>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <set>
#include <stdexcept>
#include <thread>
#include <vector>
using CONFIND::ContourFinder;
static void require(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
static uint64_t bits(double x){uint64_t u;std::memcpy(&u,&x,8);return u;}
static void setup(ContourFinder& c){c.SetGrid({{{0,64},64,"Linear"},{{0,64},64,"Linear"}});c.SetContVal({13.25});}
// Bounded waits are deadlock diagnostics, never the evidence for concurrency.
// The evidence is distinct executing thread IDs admitted under a mutex.
static void concurrency(size_t workers){
 ContourFinder c;setup(c);std::mutex m;std::condition_variable cv;std::set<std::thread::id> ids;
 c.Evaluate([&](size_t){return [&](double x,double){
  {std::unique_lock<std::mutex> lock(m);ids.insert(std::this_thread::get_id());cv.notify_all();
   require(cv.wait_for(lock,std::chrono::seconds(5),[&]{return ids.size()>=2;}),"no concurrent evaluator workers");}
  std::this_thread::yield();return x;};},{workers});
 require(ids.size()>=2,"silently serial executor");std::printf("concurrency requested=%zu observed=%zu\n",workers,ids.size());
}
struct State {std::mutex mutex;std::condition_variable cv;size_t entered=0;bool thrower_exited=false;};
struct ExitNotice {std::shared_ptr<State> state;~ExitNotice(){if(state){std::lock_guard<std::mutex> lock(state->mutex);state->thrower_exited=true;state->cv.notify_all();}}};
static void cancellation(size_t workers){
 auto state=std::make_shared<State>();ContourFinder c;setup(c);bool caught=false;
 try{c.Evaluate([=](size_t){return [=](double x,double y){
  std::unique_lock<std::mutex> lock(state->mutex);++state->entered;state->cv.notify_all();
  require(state->cv.wait_for(lock,std::chrono::seconds(5),[&]{return state->entered>=workers;}),"initial claims did not overlap");
  if(x==0&&y==0){
   // TLS destruction occurs on thread exit AFTER RunIndexed's catch stores
   // cancellation. Its mutex release makes that store visible to held workers.
   thread_local ExitNotice notice;notice.state=state;lock.unlock();throw std::runtime_error("first worker error");
  }
  require(state->cv.wait_for(lock,std::chrono::seconds(5),[&]{return state->thrower_exited;}),"thrower did not exit");
  return x;};},{workers});}
 catch(const std::runtime_error& e){caught=std::string(e.what())=="first worker error";}
 require(caught,"first exception was not propagated");
 // This controlled schedule has exactly one claim per worker before failure.
 // No general race-dependent production count is asserted: every other worker
 // was held inside its already-claimed task until cancellation became visible.
 require(state->entered==workers,"new unclaimed tasks began after visible cancellation");
 auto cs=c.GetContourSet();require(!cs[0].GetFound()&&cs[0].size()==0,"partial contour published");
 std::printf("cancellation workers=%zu already_claimed=%zu new_after_visible=0\n",workers,state->entered);
}
static void coordinates(const char* filename){
 FILE*f=std::fopen(filename,"r");require(f,"node reference missing");
 for(int log=0;log<2;++log){
  std::vector<std::pair<uint64_t,uint64_t>> expected;
  for(size_t k=0;k<56;++k){int mode;size_t i,j;double x,y;unsigned long long bx,by;
   require(std::fscanf(f,"%d %zu %zu %la %la %llx %llx",&mode,&i,&j,&x,&y,&bx,&by)==7,"invalid node reference");
   require(mode==log&&i==k%8&&j==k/8&&bits(x)==bx&&bits(y)==by,"node provenance encoding");expected.push_back({bx,by});}
  ContourFinder c;c.SetGrid({{{0.13,3.71},7,log?"Log":"Linear"},{{0.23,2.67},6,log?"Log":"Linear"}});c.SetContVal({1.31});size_t k=0;
  c.Evaluate([&](size_t){return [&](double x,double y){require(k<expected.size()&&bits(x)==expected[k].first&&bits(y)==expected[k].second,"Evaluate node-coordinate convention changed");++k;return x+2*y;};},{1});
  require(k==56,"node count");std::printf("coordinates %s nodes=56 exact\n",log?"log":"linear");
 }
 std::fclose(f);
}
int main(int argc,char**argv){require(argc==2,"node reference argument");for(size_t w:{2,3,4,6,8}){concurrency(w);cancellation(w);}coordinates(argv[1]);}
