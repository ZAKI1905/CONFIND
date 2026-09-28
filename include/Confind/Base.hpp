#pragma once
#include <cmath>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <Zaki/String/Directory.hpp>

namespace CONFIND {
class Base {
protected:
  Zaki::String::Directory wrk_dir{""};
  std::string name;
  bool set_name_flag = false;
  bool set_wrk_dir_flag = false;
public:
  Base() = default;
  explicit Base(const std::string& value) : name(value), set_name_flag(true) {}
  virtual ~Base() = default;
  virtual void SetWrkDir(const Zaki::String::Directory&);
  virtual void SetName(const std::string& value) { name=value; set_name_flag=true; }
  virtual Zaki::String::Directory GetWrkDir() const { return wrk_dir; }
  virtual std::string GetName() const { return name; }
  virtual void Print() const;
};
}
