# Install Bos Wars

These instructions have been validated on Linux distributions and
should work on most Posix systems. If you have another system, we'd
love hearing from you how it worked and what adaptations were needed.


## Software Requirements

* [SDL 2](http://www.libsdl.org/index.php) (required)
* [libpng](http://www.libpng.org/pub/png/) (required)
* [zlib](http://www.gzip.org/zlib/) (required)
* [Lua 5.1](http://www.lua.org) (required)
* [libvorbis](http://www.xiph.org/downloads) (recommended)
* [libtheora](http://www.theora.org/) (recommended)
* [libogg](http://www.xiph.org/downloads/) (recommended)
* [tolua++](http://www.codenix.com/~tolua/) if you plan to change the Lua API

On Debian, the runtime dependencies can be installed with:

    sudo apt install libsdl2-2.0 liblua5.1 libtheora libogg libpng12 libvorbis

And the build dependencies can be installed with:

    sudo apt install liblua5.1-dev libtolua++5.1-dev libtheora-dev libogg-dev \
      libpng-dev libsdl2-dev libvorbis-dev build-essential

On Fedora, these packages should help: lua-devel tolua++-devel
libtheora-devel libogg-devel libpng-devel SDL2-devel
libvorbis-devel


## Sources

Get a package from the Bos Wars website or clone from the Bos Wars git
repository.


## Build

* Run 'make.py' for the release build. This is equivalent to './make.py release'
* Run 'make.py debug' for the debug build

The build results are stored in the *fbuild/* directory.

To adjust compile time options, add them as key=value to the
commandline after the buildname. Currently, *make.py* supports the
following options for "release" build:

* builddir: Build directory. The directory where compiled objects and the
  final executable will be written. Defaults to "fbuild/release/"
* cflag: Compiler and preprocessor flags.
* ldflags: Linker flags.
* cc: Compiler. Default "g++".

The BosWars engine is compiled in C++. Thus 'cc' should point to a C++
compiler and not a C compiler. If using traditional environment
variables, cc can get the value of CXX, but should not get CC. The
cflags variable contains options for the preprocessor and the C++
compiler. If using traditional environment variables, cflags can be
assigned to the content of CPPFLAGS and CXXFLAGS.

Example:

    ./make.py release cflags="$CFLAGS $CXXFLAGS" ldflags="$LDFLAGS"


At startup, the engine needs the path to the BosWars data. By default
it uses the current working directory. When creating a package for a
distribution, that directory is probably somewhere else. You can
override DEFAULT_DATA_PATH to change the default path in the
executable, for example:

    ./make.py release cflags=-DDEFAULT_DATA_PATH=/usr/lib/share/boswars


## Packaging

When packaging for a distribution, be sure to use to make a `release`
build.

To help packaging, BosWars ./make.py can copy the Boswars data into a
target directory:

    ./make.py install_data datadir=./packagedata/

Or install Boswars and data with the executable:

    ./make.py install cflags=-DDEFAULT_DATA_PATH=/usr/lib/share/boswars bindir=./packaging/usr/bin datadir=./packaging/usr/share

This will build a `release` version before installing data and executable.
You will likely adjust DEFAULT_DATA_PATH depending on packaging needs.

DEFAULT_DATA_PATH supports different types of paths:
* absolute paths (typically starting with "/")
* paths relative to the current working directory (typically starting with "./" or "../")
* path relative to the location of the executable (starting with "~~/" which will be replaced by the executable path)

A man page for Bos Wars can be generated from the executable with
"help2man". The make.py tool automates building a "release" build and
after that generation of a man page with:

    ./make.py manpage
