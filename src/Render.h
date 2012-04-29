#pragma once

#include <memory>
struct SDL_Surface;

class Render {
public:
  Render();
  ~Render();

  void SetScreen(SDL_Surface* screen);
  void SetClouds(std::shared_ptr<SDL_Surface> clouds);

  void Draw();
private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};
