/*
  Base class

*/

// Creating directory
#include <sys/stat.h>

#include <Zaki/Util/Logger.hpp>
#include <Zaki/Util/MemoryManager.hpp>
#include <Zaki/Util/ObjObserver.hpp>

#include <Zaki/String/Banner.hpp>
#include <Zaki/String/String_Basic.hpp>

#include "Confind/Base.hpp"

//==============================================================
//  Base Class
//==============================================================
std::atomic<size_t> CONFIND::Base::counter = 0 ;
//--------------------------------------------------------------
/// Default Constructor for faster object creation
///  -> No name assignment or object tracking
CONFIND::Base::Base() 
{
//  Z_OBJ_CTR(this, name) ;
  if(counter==0)
    ShowBanner() ;

  counter++ ;
}

//--------------------------------------------------------------
/// Constructor via a name and no tracking options
/// -> Name is set but no object tracking
CONFIND::Base::Base(const std::string& in_name)
{
  name          = in_name;
  set_name_flag = true ;

//  Z_OBJ_CTR(this, name) ;
    
  if(counter==0)
    ShowBanner() ;

  counter++ ;
}

//--------------------------------------------------------------
/// Constructor via a name and tracking options
/// This is slower and shouldn't be used for objects
/// that are created thousands of time, i.e. particles, etc.
/// -> Name is set and object tracking is on
///
/// NOTE:  CONFIND_BASE_DEBUG_MODE macro overrides everything
///  It's useful for the fastest runtime, because then
///  tracking is not checked in the destructor
CONFIND::Base::Base(const std::string& in_name, const bool& tracking)
: track_objs_flag(tracking)
{
    name          = in_name;
    set_name_flag = true ;
    
#if CONFIND_BASE_DEBUG_MODE
    if(track_objs_flag)
        Z_OBJ_CTR(this, name) ;
#endif
    
    if(counter==0)
        ShowBanner() ;
    
    counter++ ;
}

//--------------------------------------------------------------
void CONFIND::Base::ShowBanner()
{
  using namespace Zaki::String ;

  Banner banner ;

  ProgramName p_name("CONFIND", 1) ;
  banner.AddContent(&p_name) ;

  Author auth("Mohammadreza", "Zakeri", 4) ;
  banner.AddContent(&auth) ;

  Version ver(CONFIND_VERSION_STR, CONFIND_RELEASE_DATE, 2);
  banner.AddContent(&ver) ;

  Website web("GitHub", "github.com/Zaki1905/CONFIND", 5) ;
  banner.AddContent(&web) ;

  Misc misc("A Contour Finding & Plotting Package", 3); 
  banner.AddContent(&misc) ;

  banner.GetTextBox()->SetTextColor({FGColor::LCyan, BGColor::BlackBg}) ;
  banner.GetTextBox()->SetFrameColor({FGColor::Blue, BGColor::BlackBg}) ;
  banner.GetTextBox()->SetPadColor({FGColor::LGreen, BGColor::BlackBg}) ;

  banner.GetTextBox()->SetAlignment(TextBox::center) ;
  banner.GetTextBox()->SetPadding(5) ;

  banner.GetTextBox()->EnableClearScreen() ;
  banner.Show() ;

}

//--------------------------------------------------------------
CONFIND::Base::~Base() 
{
#if CONFIND_BASE_DEBUG_MODE
    if(track_objs_flag)
        Z_OBJ_DTR(this, name) ;
#endif
}

//--------------------------------------------------------------
void CONFIND::Base::SetWrkDir(const Zaki::String::Directory& input) 
{
  wrk_dir = input;
  set_wrk_dir_flag = true ;

  // ............ Creating a directory ............
  if (mkdir(wrk_dir.Str().c_str(), ACCESSPERMS) == -1) 
  {
    Z_LOG_NOTE("Directory '"+input.Str()+"' wasn't created, because: "+strerror(errno)+".") ; 
  }
  else
    Z_LOG_INFO("Directory '" + wrk_dir.Str() + "' created."); 
  // .................................................

  Z_LOG_INFO("Work directory set to '" + input.Str() + "'.") ;
}

//--------------------------------------------------------------
void CONFIND::Base::SetName(const std::string& input) 
{
  name          = input;
  set_name_flag = true ;

  Z_LOG_INFO("Name set to '" + input + "'.") ;
}

//--------------------------------------------------------------
std::string CONFIND::Base::GetName() const 
{
  if (!set_name_flag)
    Z_LOG_ERROR("Name not set!") ;
  return name;
}

//--------------------------------------------------------------
Zaki::String::Directory CONFIND::Base::GetWrkDir() const 
{
  if (!set_wrk_dir_flag)
    Z_LOG_ERROR("Work directory not set!") ;

  return wrk_dir;
}

//--------------------------------------------------------------
void CONFIND::Base::Print() const
{
  std::cout<<"\n --------------------------------------------------------------\n" ;
  std::cout<<"|         Name: "<<GetName() << "\n";
  std::cout<<" --------------------------------------------------------------\n" ;
}

//--------------------------------------------------------------
// Retrns the pointer address in string form
std::string CONFIND::Base::PtrStr() const 
{
  std::stringstream ss;
  ss << static_cast<const void*>(this);

  return ss.str() ;
}

//--------------------------------------------------------------
// Overloading Class specific new operator
void* CONFIND::Base::operator new(size_t sz)
{
  // if > 10 KBytes send a note to the user
  if(sz > 10000)
    Z_LOG_NOTE("Large size of memory requested: "
                + std::to_string(sz) + " bytes.") ;
  void* m = malloc(sz);
  
  Z_NEW(m, sz) ;

  return m;
}
//--------------------------------------------------------------
// Overloading CLass specific delete operator
void CONFIND::Base::operator delete(void* m)
{
  Z_DELETE(m) ;
}
//--------------------------------------------------------------

//==============================================================
