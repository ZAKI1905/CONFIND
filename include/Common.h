#ifndef Common_H
#define Common_H

#include <iostream>
#include <string>
// #include <vector>
// #include <chrono>
// #include <cmath>

// Root
#include <TColor.h>

// Dependencies
#include <zaki/Util/Logger.h>

namespace CONFIND
{

//==============================================================
struct Color
{
    size_t idx ;
    std::string name() ;
};
// ........................................................
EColor RColorMap(size_t) ;
//==============================================================

}
//......................CONFIND namespace ends.........................

//==============================================================
#endif /*Common_H*/
