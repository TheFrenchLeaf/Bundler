Authors : 
[main author] 		Noah Snavely snavely[AT]cs.cornell.edu
[Windows Porting] Pierre moulon pmoulon[AT]gmail.com
[Mac Porting] Kai Ni http://www.kaini.org/

Date : 4 Sept 2010

--------------------
- Web ressources : - 
--------------------
[Bundler] http://phototour.cs.washington.edu/bundler/
[windows porting] http://francemapping.free.fr/Portfolio/Prog3D/BUNDLER.html

--------------------
-- Compilation  --
--------------------

Windows => compiled with VS2008.
How To :
  Use the CMake build system.

Linux => use makefile in program/main or Cmake.
      => Note the fact that the compilation will use the following resident libraries :
         - jpeg
         - lapack blas gfortran (included in most lapack distibution)
         - sometimes atlas.

Mac => use Cmake.

=> CMAKE USAGE :
Go to the extracted archive path. 
$ mkdir Output
$ cd Output
$ cmake -G "CodeBlocks - Unix Makefiles" ..
$ make
=> Your binaries will be in Output/src !

--------------------
----  Notes :   ----
--------------------
Add CMake based build system for BUNDLER.
  Tested on windows and linux.

Update minpack from 1.0 to 1.0.3

=> External contributor :
  Ólafur Haraldsson: Add "Canon Canon EOS-1Ds Mark III" and "Canon Canon EOS 5D Mark II" to
    extract_focal.pl
