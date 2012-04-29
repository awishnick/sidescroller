#include <SDL.h>
#include "Engine.h"
#include <iostream>

int main(int, char**) {
  std::unique_ptr<Engine> engine;
  try {
    engine.reset(new Engine);
  } catch(const SDLError& e) {
    std::cerr << "Error initializing engine: " << e.what() << std::endl;
    return -1;
  }

  const unsigned window_width = 800,
                 window_height = 600,
                 bit_depth = 16;
  try {
    engine->SetVideoMode(window_width, window_height, bit_depth);
  } catch(const SDLError& e) {
    std::cerr << "Error setting video mode: " << e.what() << std::endl;
    return -1;
  }

  try {
    engine->LoadResources();
  } catch(const ResourceError& e) {
    std::cerr << "Error loading resources: " << e.what() << std::endl;
    return -1;
  }

  engine->EventLoop();

  return 0;
}
