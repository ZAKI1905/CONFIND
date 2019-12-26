#ifndef Func2D_H
#define Func2D_H

//==============================================================
class Func2D 
{

  public:
    Func2D() ;

    virtual ~Func2D() ;

    virtual double Eval(double x, double y) = 0 ;
    virtual Func2D* Clone() = 0 ;
};

//==============================================================
#endif /*Func2D_H*/