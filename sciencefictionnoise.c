// http://www.science-and-fiction.org/rendering/noise.html
// I use their noise in like every homework so it's about time I just
// actually make a file of a bunch of noise
#include "mylib.h"

/* basic noise functions
shader implementation
float rand2D(in vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
float rand3D(in vec3 co){
    return fract(sin(dot(co.xyz ,vec3(12.9898,78.233,144.7272))) * 43758.5453);
}
*/
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
float rnad3Dvec(float coord[3]) {
    float ret = coord[0] * 12.9898 + coord[1] * 78.233 + coord[2] * 144.7272;
    ret = sin(ret) * 43758.5453;
    ret = ret - floor(ret);
    return ret;
}