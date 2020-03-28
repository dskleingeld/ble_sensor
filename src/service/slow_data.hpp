#pragma once

#include "package.hpp"
#include "UUIDS.hpp"
#include <utility>
#include <cstdint>

constexpr int SLOW_MEASURE_PERIOD = 1000; //ms

class SlowMeasurents {
  public:
    SlowMeasurents();
    void setup();
    void poll(PackageQueue& queue);
    void reset();
  private:
    void start();
    void finish(PackageQueue& queue);

    uint32_t last_measurment_started;
    bool waiting = false;
    enum class BlockedOn {
        StartMeasurment,
        ReadResults,
    } blocked_on;

    //sensor specific data
    float fakesensor;
};