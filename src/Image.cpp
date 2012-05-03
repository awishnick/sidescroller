#include "Image.h"
#include <SDL_image.h>
#include <utility>

struct Image::Impl {
  std::shared_ptr<SDL_Surface> surface;
};

Image::Image(std::string filename) :
  impl_(new Impl) {

  impl_->surface.reset(IMG_Load(filename.c_str()), SDL_FreeSurface);
  if (!impl_->surface) {
    impl_->surface.reset();
    throw ResourceError(SDL_GetError());
  }
}

Image::Image(const Image& other) :
  impl_(new Impl(*other.impl_)) {
}

Image::Image(Image&& other) :
  impl_(new Impl(std::move(*other.impl_))) {
}

Image& Image::operator=(const Image& other) {
  if (this != &other) {
    *impl_ = *other.impl_;
  }
  return *this;
}

Image& Image::operator=(Image&& other) {
  if (this != &other) {
    std::swap(impl_, other.impl_);
  }
  return *this;
}

const SDL_Surface* Image::GetSDLSurface() const {
  return impl_->surface.get();
}

Image::~Image() {
}

