#include "mylib.h"
// I wanted a better random function that makes more sense to me
float frand(float min, float max) {
    return rand() * (max - min) / RAND_MAX + min;
}