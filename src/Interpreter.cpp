#include "common.h"
#include "Interpreter.h"
#include "ResourceError.h"
#include <fstream>
#include <string>
PUSH_ALL_WARNINGS
#include <boost/python.hpp>
POP_ALL_WARNINGS

struct Interpreter::Impl {
};

std::string loadScript(const std::string& filename) {
  using namespace std;
  std::ifstream file(filename, std::ifstream::in);
  if (file.fail()) {
    throw ResourceError(std::string("Could not open script") + filename);
  }

  return std::string(std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>());
}

Interpreter::Interpreter() :
  impl_(new Impl) {

  const std::string init_script = loadScript("resources/scripts/init.py");

  Py_Initialize();

  using namespace boost::python;
  object main_module = import("__main__");
  object main_namespace = main_module.attr("__dict__");
  try {
    exec(init_script.c_str(), main_namespace);
    object init_fn = main_namespace["init"];
    init_fn();
  } catch(const error_already_set&) {
    PyErr_Print();
  }
}

Interpreter::~Interpreter() {
  Py_Finalize();
}

