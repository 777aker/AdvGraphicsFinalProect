#include "mylib.h"

float frand(float min, float max) {
    return rand() * (max - min) / RAND_MAX + min;
}