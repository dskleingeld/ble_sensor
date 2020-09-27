#include "test_sensors.h"
#include "nrf_delay.h"
#include <math.h>
#include <stdint.h>

static struct Sine sine = {
    .amplitude = 5000.,
    .period = 7200,
    .x = 0,
};
static struct Triangle triangle = {
    .sign = 1,
    .amplitude = 10.,
    .period = 7200,
    .x = 0,
};

float measure_sine(){
    nrf_delay_ms(100); //simulate this taking a while
    sine.x += 1;
    float w = (float)sine.x*M_PI/sine.period*2;
    float value = sine.amplitude*sinf(w);
    return value;
}

float measure_triangle(){
    nrf_delay_ms(100); //simulate this taking a while
    triangle.x += 1;
    float slope = (float)triangle.amplitude/(2.*(float)triangle.period);
    float value = triangle.sign*slope*triangle.x;
    if (value > triangle.amplitude) {triangle.sign *= -1;}
    return value;
}