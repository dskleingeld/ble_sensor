#pragma once

struct Triangle {
    int8_t sign;
    const int16_t amplitude;
    const int16_t period;
    int16_t x;
};

void measure(Triangle* self);

struct Sine {
    const int16_t amplitude;
    const int16_t period;
    int16_t x;
};

void measure(Sine* self);