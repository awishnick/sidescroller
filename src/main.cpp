#include <SDL.h>
#include <SDL_opengl.h>
#include "Engine.h"
#include <iostream>

int main(int, char**) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Could not initialize SDL: " << SDL_GetError() << std::endl;
  }

  std::unique_ptr<Engine> engine;
  try {
    engine.reset(new Engine);
  } catch(const Engine::InitializationError& e) {
    std::cerr << "Error initializing engine: " << e.what() << std::endl;
    return -1;
  }

  const unsigned window_width = 800,
                 window_height = 600,
                 bit_depth = 16;
  SDL_Surface* screen = SDL_SetVideoMode(window_width,
                                         window_height,
                                         bit_depth,
                                         SDL_DOUBLEBUF);
  (void)screen;

  SDL_Delay(2500);

  return 0;
}
