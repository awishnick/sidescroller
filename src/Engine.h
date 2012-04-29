#pragma once

#include <exception>
#include <memory>
#include <string>

class Engine {
public:
  class InitializationError : public std::exception {
  public:
    InitializationError(std::string error);
    virtual const char* what() const throw();
    virtual ~InitializationError() throw();
  private:
    const std::string error_;
  };

  Engine();
  ~Engine();
private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};
