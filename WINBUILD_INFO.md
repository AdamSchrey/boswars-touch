
## Unoficial build using CodeBlocks


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

