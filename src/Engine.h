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

class ResourceError : public std::exception {
public:
  ResourceError(std::string error);
  virtual const char* what() const throw();
  virtual ~ResourceError() throw();
private:
  const std::string error_;
};

class Engine {
public:
  // Initialize, throws SDLError if initialization fails.
  Engine();
  ~Engine();

  // Loads resources from disk. If there is an error, resources
  // are not loaded and ResourceError is thrown.
  void LoadResources();

  // Sets the video mode. On failure, this throws SDLError and
  // does not set the video mode.
  void SetVideoMode(unsigned width, unsigned height, unsigned bit_depth);

  void EventLoop() throw();
private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};
