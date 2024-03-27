#include "Plant.h"

Plant::Plant()
{
    Name = "Tree";
    Days = 0;
    Save_Temp=22; //Limit Temperature Low
    Danger_Temp=27; //Limit Temperature High
    Save_Humi=60;
    Danger_Humi=80;
    DARK_LIGHT=65; //Change Point
    DRY_SOIL = 60; //Limit soilMoist Low
    WET_SOIL = 80; //Limit soilMoist High
}

Plant::~Plant()
{
    Name.remove(0);
}

