/*
  Logger class

*/

#include "../include/Logger.h"

//==============================================================
//--------------------------------------------------------------
// Constructor
Logger::Logger() { }

//--------------------------------------------------------------
// Setting the unit name
void Logger::SetUnit(const char* unit_input) 
{
  m_UnitName = unit_input ;
}

//--------------------------------------------------------------
void Logger::SetLevel(Logger::Level level)    
{
  m_LogLevel = level ;
}

//--------------------------------------------------------------
void Logger::Info(const char* msg, const char* func, int line)  const
{
  if ( m_LogLevel >= LevelInfo )
  {
    char tmp[200] ;
    sprintf(tmp, "%sInfo in [%s:%s:%d] %s %s %s\n", 
            InfoTagColor, m_UnitName.c_str(), func, line, 
            InfoTextColor, msg, Color_Off) ;
    std::cout<< tmp << std::flush ; 
  }
}

//--------------------------------------------------------------
void Logger::Warn(const char*  msg, const char* func, int line) const
{
  if ( m_LogLevel >= LevelWarning )
  {
    char tmp[200] ;
    sprintf(tmp, "%sWarning in [%s:%s:%d] %s %s %s\n",
            WarningTagColor, m_UnitName.c_str(), func, line,
            WarningTextColor, msg, Color_Off) ;
    std::cout<< tmp << std::flush ; 
  }
}

//--------------------------------------------------------------
void Logger::Error(const char* msg, const char* func, int line) const
{
  if ( m_LogLevel >= LevelError )
  {
    char tmp[200] ;
    sprintf(tmp, "%sError in [%s:%s:%d] %s %s %s\n", 
            ErrorTagColor, m_UnitName.c_str(), func, line,
            ErrorTextColor, msg, Color_Off) ;
    std::cout<< tmp << std::flush ; 
  }
}

//--------------------------------------------------------------
//==============================================================