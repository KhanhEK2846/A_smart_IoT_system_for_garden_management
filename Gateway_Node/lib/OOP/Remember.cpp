#include "Remember.h"

dataRemeber::dataRemeber()
{
    NodeAdrress = "";
    ID= "";
}

dataRemeber::~dataRemeber()
{
    NodeAdrress.remove(0);
    ID.remove(0);
}

Remember::Remember()
{
    count = 0;
    flag = 0;
}

Remember::~Remember(){}

bool Remember::Add(const String ID, const String From)
{
    if(ID == "" || From == "")
        return false;
    for(flag = 0; flag<10;flag++)
    {
        if(data[flag].ID == ID) //Update from
        {
            data[flag].NodeAdrress = From;
            return true;
        }
        if(data[flag].ID == "")
            break;
    }
    if(count == 10)
        return false;
    data[flag].ID = ID;
    data[flag].NodeAdrress = From;
    count ++;
    return true;
}

String Remember::GetAddrress(const String ID)
{
    if(ID == "")
        return "";
    else
    {
        for(flag = 0;flag <10; flag++)
        {
            if(data[flag].ID == ID)
                return data[flag].NodeAdrress;
            if(data[flag].ID == "")
                break;
        }
    }
    return "";
}
