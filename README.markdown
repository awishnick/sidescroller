## Sidescroller
I think I'm making a sidescroller!
## Requirements
* CMake
## Dependencies
* SDL
* SDL_image
## To build SDL
    $ cd lib/SDL-1.2.15
    $ ./configure --prefix=`pwd`/local
    $ make && make install
## To build SDL_image
    $ cd lib/SDL-1.2.15/SDL_image-1.2.12
    $ ./configure --prefix=`pwd`/../local
    $ make && make install
## To build sidescroller
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
