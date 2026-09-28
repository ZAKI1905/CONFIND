#include "Confind/Base.hpp"
#include <filesystem>
namespace CONFIND {
void Base::SetWrkDir(const Zaki::String::Directory& value) {
  std::filesystem::create_directories(value.Str());
  wrk_dir = value;
  set_wrk_dir_flag = true;
}
void Base::Print() const { std::cout << name << '\n'; }
}
