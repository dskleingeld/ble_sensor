#include "slow_data.hpp"

SlowMeasurents::SlowMeasurents(){
    last_measurment_started = 0;//millis(); //TODO fixme
}

void SlowMeasurents::reset(){
    //reset all sensors
    setup();
}

void SlowMeasurents::poll(PackageQueue& queue){
    auto now =  millis();
    switch(blocked_on) {
        case BlockedOn::StartMeasurment: 
            if (now-last_measurment_started > SLOW_MEASURE_PERIOD){
                start();
                last_measurment_started = now;
                blocked_on = BlockedOn::ReadResults;
            }
            break;
        case BlockedOn::ReadResults:
            if (now-last_measurment_started > SLOW_MEASURE_DURATION){
                finish(queue);
                blocked_on = BlockedOn::StartMeasurment;
            }
            break;
    }
}

void SlowMeasurents::setup(){
    //init hardware for sensors
    last_measurment_started = millis() - SLOW_MEASURE_PERIOD;
    fakesensor = 0.0;
}

void SlowMeasurents::start(){

}

void SlowMeasurents::finish(PackageQueue& queue){
    //finish sensor readout
    fakesensor += 1.0;

    //create a package and encode content into 
    //its buffer starting at byte
    Package package(20, TYPE_SLOW_DATA);
    //uint8_t* buffer = package.get_bufferpointer();
    //*((uint32_t*)buffer) = (uint32_t)fakesensor;
    package.finalise();

    queue.enqueue(std::move(package));
}