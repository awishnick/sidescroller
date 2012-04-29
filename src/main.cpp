#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>
#include <tuple>

int main(int, char**) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Could not initialize SDL: " << SDL_GetError() << std::endl;
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

  SDL_Quit();

  return 0;
}
