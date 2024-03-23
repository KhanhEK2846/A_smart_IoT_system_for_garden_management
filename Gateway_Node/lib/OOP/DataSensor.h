#include <stdlib.h>

class DataSensor
{
public:
    float Humidity; //Humidity data
    float Temperature; // Temperature data
    int lumen; //Light Sensor Variable
    int soilMoist; //Soild Sensor Variable

    bool Soil_Err;
    bool DHT_Err;
    bool LDR_Err;

    DataSensor();
    ~DataSensor();
};

