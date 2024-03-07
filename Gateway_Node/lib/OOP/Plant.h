#include <stdlib.h>
#include <WString.h>

class Plant
{
public:
    String Name;
    int Days;
    float Save_Temp; //Limit Temperature Low
    float Danger_Temp; //Limit Temperature High
    float Save_Humi;
    float Danger_Humi;
    int DARK_LIGHT; //Change Point
    int DRY_SOIL; //Limit soilMoist Low
    int WET_SOIL; //Limit soilMoist High
    Plant();
    ~Plant();
};
