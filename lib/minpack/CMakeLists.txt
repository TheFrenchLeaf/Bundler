
# Extra special variables
set(ENV{DISPLAY}             "")
if(CTEST_CMAKE_GENERATOR MATCHES Makefiles)
  set(ENV{CC}                  "${c_compiler}")
  set(ENV{FC}                  "${f_compiler}")
  set(ENV{CXX}                 "${full_compiler}")
endif(CTEST_CMAKE_GENERATOR MATCHES Makefiles)

if (UNIX)
   if ( "${CMAKE_Fortran_COMPILER}" MATCHES "ifort" )
  set( CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -fltconsistency -fp_port" )
   endif ()
endif ()

PROJECT(minpack FORTRAN)
# list sources files
FILE(GLOB LIBRARY_FILES_F "./*.f")

ADD_LIBRARY(minpack
${LIBRARY_FILES_F}
)
