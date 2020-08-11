#include "simple.h"
#include <math.h>

static struct Sine sine = {
    .amplitude = 7200,
    .period = 7200,
    .x = 0,
};
static struct Triangle triangle = {
    .sign = 1,
    .amplitude = 7200,
    .period = 7200,
    .x = 0,
};

void measure(Sine* self){
    self->x += 1;
    float w = (float)self->x*M_PI/self->period;
    float value = self->amplitude*sinf(w);
}

void measure(Triangle* self){
    self->x += 1;
    float slope = (float)self->amplitude/(2.*(float)self->period);
    float value = self->sign*slope*self->x;
    if (value > self->amplitude) {self->sign *= -1;}
}