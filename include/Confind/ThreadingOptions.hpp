#pragma once
#include <cstddef>
namespace CONFIND {
struct ThreadingOptions {
  // Zero falls back to one. No global state and no automatic oversubscription.
  std::size_t worker_count = 1;
};
}
