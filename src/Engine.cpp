#include "Engine.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <utility>

SDLError::SDLError(std::string error) :
  error_(std::move(error)) {
}
const char* SDLError::what() const throw() {
  return error_.c_str();
}
SDLError::~SDLError() throw() {}

struct Engine::Impl {
  SDL_Surface* screen;

  Impl() :
    screen(nullptr) {
  }
};

Engine::Engine() :
  impl_(new Impl) {

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw SDLError(SDL_GetError());
  }
}

void Engine::SetVideoMode(unsigned width,
                          unsigned height,
                          unsigned bit_depth) {
  SDL_Surface* screen = SDL_SetVideoMode(width,
                                         height,
                                         bit_depth,
                                         SDL_DOUBLEBUF);
  if (!screen) {
    throw SDLError(SDL_GetError());
  }

  impl_->screen = screen;
}

Engine::~Engine() {
  SDL_Quit();
}

