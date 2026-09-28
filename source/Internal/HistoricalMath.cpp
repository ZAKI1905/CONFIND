#include "HistoricalMath.hpp"
#include <cmath>

namespace CONFIND::detail {
// This TU alone has -fno-builtin and -fno-lto. Preserve the historical libm
// pow call: optimized AppleClang otherwise substitutes a non-identical exp10.
#if defined(__clang__) || defined(__GNUC__)
__attribute__((noinline))
#endif
double HistoricalPow10(double x) {
  return std::pow(10.0, x);
}
}
