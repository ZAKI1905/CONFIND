#ifndef Base_H
#define Base_H

#include <zaki/Util/Instrumentor.h>

#include "Common.h"

// namespace CONFIND
// {
//==============================================================
class Base {

  //--------------------------------------------------------------
  protected:
    void ShowBanner();

    static size_t counter;

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