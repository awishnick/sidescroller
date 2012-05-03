#pragma once

#include "ResourceError.h"
#include <memory>
#include <string>
struct SDL_Surface;

class Image {
public:
  // Throws ResourceError if the image cannot be loaded
  explicit Image(std::string filename);
  Image(const Image& other);
  Image(Image&& other);
  Image& operator=(const Image& other);
  Image& operator=(Image&& other);

  const SDL_Surface* GetSDLSurface() const;

  ~Image();
  
private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  // No default constructor
  Image();
};

