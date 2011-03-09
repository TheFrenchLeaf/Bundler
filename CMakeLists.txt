# CMake powered build system for Bundler
# Create by Pierre Moulon
# First draft the 26 june 2010

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(BUNDLER C CXX)

IF(WIN32)
INCLUDE_DIRECTORIES(
./include
./lib/zlib/include
./lib/ann_1.1_char/include
./lib/sfm-driver
./lib/matrix
./lib/imagelib
./lib/jpeg
./lib/zlib
./lib/5point
./lib/getopt
)
ELSE(WIN32)
INCLUDE_DIRECTORIES(
./include
./lib/zlib/include
./lib/ann_1.1_char/include
./lib/sfm-driver
./lib/matrix
./lib/imagelib
./lib/zlib
./lib/5point
./lib/getopt
)
ENDIF(WIN32)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(lib)

