## Sidescroller
I think I'm making a sidescroller!
## Requirements
* CMake
## Dependencies
* SDL
## To build SDL
    $ cd lib/SDL-1.2.15
    $ ./configure --prefix=`pwd`/local
    $ make && make install
## To build sidescroller
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
