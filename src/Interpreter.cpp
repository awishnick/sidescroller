#include "common.h"
#include "Interpreter.h"
PUSH_ALL_WARNINGS
#include <boost/python.hpp>
POP_ALL_WARNINGS

struct Interpreter::Impl {
};

Interpreter::Interpreter() :
  impl_(new Impl) {

  Py_Initialize();
}

Interpreter::~Interpreter() {
  Py_Finalize();
}

