#include "DataSensor.h"

DataSensor::DataSensor(){
    Humidity = 0;
    Temperature = 0;
    lumen = 0;
    soilMoist = 0;

    Soil_Err = false;
    DHT_Err = false;
    LDR_Err = false;
}

DataSensor::~DataSensor(){}