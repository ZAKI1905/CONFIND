/*
  Base class

*/

// Creating directory
#include <sys/stat.h>

#include <zaki/Util/Logger.h>
#include <zaki/String/Banner.h>
#include <zaki/String/String_Basic.h>

#include "../include/Base.h"

//==============================================================
//  Base Class
//==============================================================
size_t Base::counter = 0 ;
//--------------------------------------------------------------
// Constructor
Base::Base() 
{
  if(counter==0)
    ShowBanner() ;

  counter++ ;
}

//--------------------------------------------------------------
void Base::ShowBanner()
{
  using namespace Zaki::String ;

  Banner banner ;

  ProgramName p_name("CONFIND", 1) ;
  banner.AddContent(&p_name) ;

  Author auth("Mohammadreza", "Zakeri", 4) ;
  banner.AddContent(&auth) ;

  Version ver("1.0.0", "Apr 6, 2020", 2);
  banner.AddContent(&ver) ;

  Website web("GitHub", "github.com/ZAKI1905/CONFIND", 5) ;
  banner.AddContent(&web) ;

  Misc misc("A Contour Finding & Plotting Package", 3); 
  banner.AddContent(&misc) ;

  banner.GetTextBox()->SetTextColor({FGColor::LCyan, BGColor::BlackBg}) ;
  banner.GetTextBox()->SetFrameColor({FGColor::Blue, BGColor::BlackBg}) ;
  banner.GetTextBox()->SetPadColor({FGColor::LGreen, BGColor::BlackBg}) ;

  banner.GetTextBox()->SetAlignment(TextBox::center) ;
  banner.GetTextBox()->SetPadding(5) ;

  banner.Show() ;

}

//--------------------------------------------------------------
Base::~Base() {}

//--------------------------------------------------------------
void Base::SetWrkDir(const char* input) 
{
  wrk_dir_str = input;
  set_wrk_dir_flag = true ;

  // ............ Creating a directory ............
  if (mkdir(wrk_dir_str.c_str(), ACCESSPERMS) == -1) 
  {
    char tmp[75] ;
    sprintf(tmp, "Directory '%s' wasn't created, because: %s.", wrk_dir_str.c_str(), strerror(errno)) ;
    Z_LOG_WARNING(tmp) ; 
  }
  else
    Z_LOG_INFO(("Directory '" + wrk_dir_str + "' created.").c_str()); 
  // .................................................

  char tmp[75];
  sprintf(tmp, "Work directory set to '%s'.", input) ;
  Z_LOG_INFO(tmp) ;
}

//--------------------------------------------------------------
void Base::SetName(const char* input) 
{
  name          = input;
  set_name_flag = true ;
  char tmp[75];
  sprintf(tmp, "Name set to '%s'.", input) ;
  Z_LOG_INFO(tmp) ;
}

//--------------------------------------------------------------
std::string Base::GetName() const 
{
  if (!set_name_flag)
    Z_LOG_ERROR("Name not set!") ;
  return name;
}

//--------------------------------------------------------------
std::string Base::GetWrkDir() const 
{
  if (!set_wrk_dir_flag)
    Z_LOG_ERROR("Work directory not set!") ;

  return wrk_dir_str;
}

//--------------------------------------------------------------
void Base::Print() const
{
  std::cout<<"\n --------------------------------------------------------------\n" ;
  std::cout<<"|         Name: "<<GetName() << "\n";
  std::cout<<" --------------------------------------------------------------\n" ;
}

//==============================================================