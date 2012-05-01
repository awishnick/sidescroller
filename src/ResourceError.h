#pragma once

#include <exception>
#include <string>

class ResourceError : public std::exception {
public:
  ResourceError(std::string error);
  virtual const char* what() const throw();
  virtual ~ResourceError() throw();
private:
  const std::string error_;
};

