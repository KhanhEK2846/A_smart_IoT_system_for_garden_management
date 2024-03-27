#include <stdlib.h>

enum AllStatus{
    NOTHING,
    ON,
    OFF
};

class ActuatorStatus
{
public:
    bool PumpsStatus; //Current Status Pump
    bool LightStatus; //Current Status Light
    ActuatorStatus();
    ~ActuatorStatus();
};

