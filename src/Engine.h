#pragma once

#include <exception>
#include <memory>
#include <string>

class SDLError : public std::exception {
public:
  SDLError(std::string error);
  virtual const char* what() const throw();
  virtual ~SDLError() throw();
private:
  const std::string error_;
};

class Engine {
public:
  // Initialize, throws SDLError if initialization fails.
  Engine();
  ~Engine();

  // Sets the video mode. On failure, this throws SDLError and
  // does not set the video mode.
  void SetVideoMode(unsigned width, unsigned height, unsigned bit_depth);
private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};
