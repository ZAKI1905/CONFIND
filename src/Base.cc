/*
  Base class

*/

#include "../include/Base.h"

//==============================================================
//  Base Class
//--------------------------------------------------------------
// Constructor
Base::Base() 
{
  logger.SetLevel(Logger::LevelInfo) ;
}

//--------------------------------------------------------------
Base::~Base() {}

//--------------------------------------------------------------
void Base::SetWrkDir(const char* input) 
{
  wrk_dir_str = input;
  set_wrk_dir_flag = true ;
  char tmp[75];
  sprintf(tmp, "Work directory set to '%s'.", input) ;
  LOG_INFO(tmp) ;
}

//--------------------------------------------------------------
void Base::SetName(const char* input) 
{
  name          = input;
  set_name_flag = true ;
  char tmp[75];
  sprintf(tmp, "Name set to '%s'.", input) ;
  LOG_INFO(tmp) ;
}

//--------------------------------------------------------------
std::string Base::GetName() const 
{
  if (!set_name_flag)
    LOG_ERROR("Name not set!") ;
  return name;
}

//--------------------------------------------------------------
std::string Base::GetWrkDir() const 
{
  if (!set_wrk_dir_flag)
    LOG_ERROR("Work directory not set!") ;

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