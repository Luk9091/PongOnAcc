#ifndef __AXIS_H__
#define __AXIS_H__

#include <pico/stdlib.h>

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} axis16_t;

typedef struct{
    float x;
    float y;
    float z;
} axisf_t;



#endif