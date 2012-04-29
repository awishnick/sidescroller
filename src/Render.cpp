#include "Render.h"
#include <SDL.h>
#include <utility>

struct Render::Impl {
  SDL_Surface* screen;
  std::shared_ptr<SDL_Surface> clouds;

  Impl() :
    screen(nullptr),
    clouds(nullptr) {
  }
};

Render::Render() :
  impl_(new Impl) {
}

void Render::SetScreen(SDL_Surface* screen) {
  impl_->screen = screen;
}

void Render::SetClouds(std::shared_ptr<SDL_Surface> clouds) {
  impl_->clouds = std::move(clouds);
}

void Render::Draw() {
  SDL_Surface* const screen = impl_->screen;
  if (!screen) return;

  const std::shared_ptr<SDL_Surface>& clouds = impl_->clouds;
  if (clouds) {
    SDL_BlitSurface(clouds.get(), NULL, screen, NULL);
  }

  SDL_Flip(screen);
}

Render::~Render() {
}

