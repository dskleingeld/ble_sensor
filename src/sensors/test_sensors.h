#pragma once

#include <stdint.h>

struct Triangle {
    int8_t sign;
    const float amplitude;
    const float period;
    int16_t x;
};

float measure_triangle();

struct Sine {
    const float amplitude;
    const float period;
    int16_t x;
};

float measure_sine();
