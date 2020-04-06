#ifndef Logger_H
#define Logger_H

#include <iostream>
#include <string>

// namespace CONFIND
// {
//==============================================================
class Logger {

  //--------------------------------------------------------------
  public:

    enum Level
    {
      LevelError = 0, LevelWarning, LevelInfo    
    } ;


    // Text style reset
    const char* Color_Off = "\033[0m"  ;  

    // Regular Colors
    // const char* Black   = "\033[0;30m" ;
    const char* Red     = "\033[0;31m" ;
    // const char* Green   = "\033[0;32m" ;
    const char* Yellow  = "\033[0;33m" ;
    // const char* Blue    = "\033[0;34m" ;
    // const char* Purple  = "\033[0;35m" ;
    const char* Cyan    = "\033[0;36m" ;
    // const char* White   = "\033[0;37m" ;

    // High Intensty
    // const char* IBlack  = "\033[0;90m" ;
    const char* IRed    = "\033[0;91m" ;
    // const char* IGreen  = "\033[0;92m" ;
    const char* IYellow = "\033[0;93m" ;
    // const char* IBlue   = "\033[0;94m" ;
    // const char* IPurple = "\033[0;95m" ;
    const char* ICyan   = "\033[0;96m" ;
    // const char* IWhite  = "\033[0;97m" ;


    const char* InfoTagColor     = Cyan   ;
    const char* InfoTextColor    = ICyan  ;

    const char* WarningTagColor  = Yellow ;
    const char* WarningTextColor = IYellow;

    const char* ErrorTagColor    = Red    ;
    const char* ErrorTextColor   = IRed   ;


  //--------------------------------------------------------------
  private:
    Level m_LogLevel = LevelInfo ;
    std::string m_UnitName = ""  ;

  //--------------------------------------------------------------
  public:
    // Constructor
    Logger()       ;

    void SetUnit(const char* ) ;
    void SetLevel(Level)      ;

    void Info(const char* , const char*  = "", int = 0)  const  ;
    void Warn(const char* , const char*  = "", int = 0)  const  ;
    void Error(const char* , const char* = "", int = 0)  const  ;
};

// }
//==============================================================
#endif /*Logger_H*/