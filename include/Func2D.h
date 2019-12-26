#ifndef Func2D_H
#define Func2D_H

#include "Base.h"

//==============================================================
// namespace CONFIND
// {
//==============================================================
class Func2D : public Base
{

  public:
    Func2D() ;

    virtual ~Func2D() ;

    virtual double Eval(double x, double y) = 0 ;
    virtual Func2D* Clone() = 0 ;
};

//==============================================================
// } // Namespace "CONFIND" ends.
//==============================================================
#endif /*Func2D_H*/