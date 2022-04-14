// managing my stuff better
// making my own library
#ifndef mylib
#define mylib

#include "X239lib/CSCIx239.h"
#include <time.h> 
#include <stdbool.h>

// basic random functions
float rand2D(float x, float y);
float rand2Dvec(float coord[2]);
float rand3D(float x, float y, float z);
float rand3Dvec(float coord[3]);
float frand(float min, float max);

// some helper functions
float frand(float min, float max);

#endif