#include "Engine.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <utility>

Engine::InitializationError::InitializationError(std::string error) :
  error_(std::move(error)) {
}
const char* Engine::InitializationError::what() const throw() {
  return error_.c_str();
}
Engine::InitializationError::~InitializationError() throw() {}

struct Engine::Impl {
};

Engine::Engine() :
  impl_(new Impl) {

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw InitializationError(SDL_GetError());
  }
}

Engine::~Engine() {
  SDL_Quit();
}

