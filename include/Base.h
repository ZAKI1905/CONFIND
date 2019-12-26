#ifndef Base_H
#define Base_H

#include "Common.h"
// #include "Logger.h"
// #include "SaveVectorToFile.h"
// #include "../pch.h"
// #include "Instrumentor.h"

//==============================================================
//                    Profiling
// #define PROFILING 1
// #if PROFILING
//   #define PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
//   #define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
// #else
//   #define PROFILE_SCOPE(name)
//   #define PROFILE_FUNCTION()
// #endif

#define LOG_INFO(message)  logger.Info(message, __func__)
#define LOG_ERROR(message)  logger.Error(message, __func__)
#define LOG_WARNING(message)  logger.Warn(message, __func__)
//==============================================================

// namespace CONFIND
// {
//==============================================================
class Base {

  //--------------------------------------------------------------
  protected:
    Logger logger;
    std::string wrk_dir_str        = ""    ;
    std::string name               = ""    ;

    // Flags
    bool set_name_flag = false    ;
    bool set_wrk_dir_flag = false ;

  //--------------------------------------------------------------
  public:
    // Constructor
    Base() ;

    // Destructor
    virtual ~Base() ;

    // Setters
    virtual void SetWrkDir(const char*) ;
    virtual void SetName(const char*) ;

    // Getters
    virtual std::string GetWrkDir() const ;
    virtual std::string GetName() const   ;
    virtual void Print() const            ;
};

//==============================================================
// } // Namespace "CONFIND" ends.
//==============================================================
#endif /*Base_H*/