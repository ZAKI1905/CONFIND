/*
  Common functions between different files

*/

// #include <algorithm>

#include "../include/Common.h"

//==============================================================
std::string CONFIND::Color::name()
{
  int base  = idx % 13 ;
  int shade = idx / 13 ;
  std::string out_name ;

  switch (base)
  {
  case 0:
    out_name = "Red" ;
    break;
  case 1:
    out_name = "Green" ;
    break;
  case 2:
    out_name = "Blue" ;
    break;
  case 3:
    out_name = "Magenta" ;
    break;
  case 4:
    out_name = "Teal" ;
    break;
  case 5:
    out_name = "Orange" ;
    break;
  case 6:
    out_name = "Cyan" ;
    break;
  case 7:
    out_name = "Violet" ;
    break;
  case 8:
    out_name = "Gray" ;
    break;
  case 9:
    out_name = "Pink" ;
    break;
  case 10:
    out_name = "Azure" ;
    break;
  case 11:
    out_name = "Yellow" ;
    break;
  case 12:
    out_name = "Spring" ;
    break;
  default:
    out_name = "color" ;
    break;
  }
  
  switch (shade)
  {
  case 0:
    break;
  case 1:
    out_name = "Dark" + out_name ; 
    break;
  case 2:
    out_name = "Darker" + out_name ; 
    break;
  default:
    out_name = "shade" + out_name ;
    break;
  }

  return out_name;

}
// ............................................
// Root color mapping 
// Ref: https://root.cern.ch/doc/master/classTColor.html
std::array<EColor, 13> RColor = {kRed, kGreen, kBlue, kMagenta,
                                 kTeal, kOrange, kCyan, kViolet,
                                kGray, kPink, kAzure, kYellow, kSpring} ;

EColor CONFIND::RColorMap(size_t i) 
{ 
  return static_cast<EColor>(RColor[i % 13] + 2*(i/13)) ;
}

//==============================================================