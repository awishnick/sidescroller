## Sidescroller
I think I'm making a sidescroller!

## Requirements
* CMake

## Dependencies
### SDL
    $ cd lib/SDL-1.2.15
    $ ./configure --prefix=`pwd`/local --disable-stdio-redirect
    $ make && make install
### SDL_image
    $ cd lib/SDL-1.2.15/SDL_image-1.2.12
    $ ./configure --prefix=`pwd`/../local
    $ make && make install
### Python
    $ cd lib/Python-2.7.3
    $ ./configure --prefix=`pwd`/local
    $ make && make install
### Boost
    $ cd lib/boost_1_49_0
    $ ./bootstrap.sh
    $ ./b2 toolset=clang cxxflags="-stdlib=libc++ -U__STRICT_ANSI__ -D__STDC_FORMAT_MACROS" linkflags="-stdlib=libc++"
## To build sidescroller
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
