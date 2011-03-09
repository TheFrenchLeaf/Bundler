
INCLUDE_DIRECTORIES( "../matrix")

ADD_LIBRARY(imagelib
affine.c bmp.c canny.c color.c fileio.c filter.c fit.c
fmatrix.c homography.c horn.c image.c lerp.c morphology.c
pgm.c poly.c qsort.c resample.c tps.c transform.c
triangulate.c util.c
)