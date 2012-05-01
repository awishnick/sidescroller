#include "ResourceError.h"

ResourceError::ResourceError(std::string error) :
  error_(move(error)) {
}
const char* ResourceError::what() const throw() {
  return error_.c_str();
}
ResourceError::~ResourceError() throw() {}

