INCLUDE_DIRECTORIES("../sba-1.5" "../matrix")

# list sources files
FILE(GLOB LIBRARY_FILES_C "./*.c")
FILE(GLOB LIBRARY_FILES_H "./*.h")

ADD_LIBRARY(sfm-driver
${LIBRARY_FILES_C}
${LIBRARY_FILES_H}
)