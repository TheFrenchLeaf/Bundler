

# list sources files
FILE(GLOB LIBRARY_FILES_CPP "src/*.cpp")
FILE(GLOB LIBRARY_FILES_H "include/*.h")

ADD_LIBRARY(ann_1.1_char
${LIBRARY_FILES_CPP}
${LIBRARY_FILES_H}
)