
## Unoficial build using CodeBlocks

### "History"

Old BosWars code base has also Microsoft VS Solution file.
Code::Blocks IDE can import such files - I create project file this .sln file.

### Goals

(1) Make possible to build BosWars on Windows XP.

Legacy MSys system not provide all needed build dependencies so
__scons__ build system can not automatically find needed packages.
Even more: it is tricky to install python3 on Windows XP with
integrated with MSys/MinGW . 

But Code::Blocks 20.03 works fine on Windows XP.

(2) Provide alternative engine build system for experiments
- with different compilers;
- libraries (for example LuaJIT);
- build options.

### Custom build dependecies

Code::Blocks project files are placed in engine folder. This place allows to run BosWars from IDE (for Windows You need to copy your custom dll-files near executable build by IDE).

__stratagus.cbp__ use only Linux (or other similar OS) build system.

__stratagus_win32.cbp__ has also 
- "../win32_dep/include" and "../win32_dep/include/SDL2" as compiler includes
- "../win32_dep/lib" as linker search paths.

__stratagus_win64.cbp__ has also 
- "../win64_dep/include" and "../win64_dep/include/SDL2" as compiler includes
- "../win64_dep/lib" as linker search paths.

I provide my custom build dependencies in similar way:
- liblua, libogg, libvorbis, libtheora, libpng build myself from source;
- libSDL2 from SDL project (I was not able to build this on old MSys, 
but my build on MSys2 was about 10Mb)

But You may prefer Msys2 provided liblua, libogg, libvorbis, 
libtheora, libpng, libSDL2.

### Making portable aplication

#### Preface 

Making windows applications portable is a little bit tricky:
you should find .dll files from your MSys/ MSsys2 system.
On windowsXP (and if use wine) not found .dll is in crash report,
but on win64 systens sometimes you have to use ldd to track this files.

#### Tracking used .dll files

Simpliest way to do so:
- copy boswars to simple path (for example on C:/).
- open MSYS2 MinGW 64-bit console
- cd /c/boswars/bin/win64
- ldd boswars.exe

You should copy all dll-s (but only), opened from .... /mingw64/bin to /bin/win64
(installation prefix is different on Your system).

#### Copied file  list on MSYS2 MinGW 64-bit

I had to copy:
- libgcc_s_seh-1.dll
- libstdc++-6.dll
- libwinpthread-1.dll
- zlib1.dll

All overs was provided in win64_dep/dll folder

#### Copied file  list on Windows XP MSYS 

In similar way I copied files from WindowsXP MinGW:
- libgcc_s_dw2-1.dll
- libstdc++-6.dll
- libwinpthread-1.dll
- zlib1.dll

But find what to copy on WindowsXP is simple (not need to use ldd).

