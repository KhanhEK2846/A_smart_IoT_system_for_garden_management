#include "Plant.h"

Plant::Plant()
{
    Name = "Tree";
    Days = 0;
    Save_Temp=25; //Limit Temperature Low
    Danger_Temp=40; //Limit Temperature High
    Save_Humi=50;
    Danger_Humi=70;
    DARK_LIGHT=65; //Change Point
    DRY_SOIL = 20; //Limit soilMoist Low
    WET_SOIL = 80; //Limit soilMoist High
}

Plant::~Plant()
{
    Name.remove(0);
}

