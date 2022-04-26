// http://www.science-and-fiction.org/rendering/noise.html
// I use their noise in like every homework so it's about time I just
// actually make a file of a bunch of noise
// I made it finally, but then didn't even use it :/
#include "mylib.h"

float rand2D(float x, float y) {
    float ret = x * 12.9898 + y * 78.233;
    ret = sin(ret) * 43758.5453;
    ret = ret - floor(ret);
    return ret;
}
float rand2Dvec(float coord[2]) {
    float ret = coord[0] * 12.9898 + coord[1] * 78.233;
    ret = sin(ret) * 43758.5453;
    ret = ret - floor(ret);
    return ret;
}
float rand3D(float x, float y, float z) {
    float ret = x * 12.9898 + y * 78.233 + z * 144.7272;
    ret = sin(ret) * 43758.5453;
    ret = ret - floor(ret);
    return ret;
}
float rand3Dvec(float coord[3]) {
    float ret = coord[0] * 12.9898 + coord[1] * 78.233 + coord[2] * 144.7272;
    ret = sin(ret) * 43758.5453;
    ret = ret - floor(ret);
    return ret;
}