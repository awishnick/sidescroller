#pragma once

#include <memory>

class Interpreter {
public:
  Interpreter();
  ~Interpreter();
private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

