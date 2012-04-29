#include "Interpreter.h"
#include <Python.h>

struct Interpreter::Impl {
};

Interpreter::Interpreter() :
  impl_(new Impl) {

  Py_Initialize();
}

Interpreter::~Interpreter() {
  Py_Finalize();
}

