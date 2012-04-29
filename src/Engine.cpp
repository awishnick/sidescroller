#include "Engine.h"
#include "Render.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <utility>
using std::shared_ptr; using std::unique_ptr;
using std::move;

SDLError::SDLError(std::string error) :
  error_(move(error)) {
}
const char* SDLError::what() const throw() {
  return error_.c_str();
}
SDLError::~SDLError() throw() {}

ResourceError::ResourceError(std::string error) :
  error_(move(error)) {
}
const char* ResourceError::what() const throw() {
  return error_.c_str();
}
ResourceError::~ResourceError() throw() {}

struct Engine::Impl {
  SDL_Surface* screen;
  unique_ptr<Render> render;

  Impl() :
    screen(nullptr),
    render(new Render) {
  }
};

Engine::Engine() :
  impl_(new Impl) {

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw SDLError(SDL_GetError());
  }
}

void Engine::LoadResources() {
  shared_ptr<SDL_Surface> clouds(IMG_Load("resources/clouds.jpg"),
                                 SDL_FreeSurface);
  if (!clouds) {
    throw ResourceError(SDL_GetError());
  }

  impl_->render->SetClouds(move(clouds));
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
  impl_->render->SetScreen(screen);
}

void Engine::EventLoop() throw() {
  bool exit = false;
  while (!exit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          exit = true;
          break;
        default:
          break;
      }
    }

    impl_->render->Draw();
  }
}

Engine::~Engine() {
  SDL_Quit();
}

