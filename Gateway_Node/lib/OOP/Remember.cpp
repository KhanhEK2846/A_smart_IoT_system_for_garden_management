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

bool Remember::AddAddress(const String ID, const String From)
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

void Remember::RemoveAddress(const String ID){
    if(ID == "")
        return;
    bool del = false;
    for(flag =0;flag<10;flag++){
        if(data[flag].ID == ID){
            del = true;
            count--;
        }
        if(del){
            if(data[flag].ID == "" && data[flag].NodeAdrress == "")
                return;
            if(flag == 9)//End of Memory
            {
                data[flag].ID = "";
                data[flag].NodeAdrress = "";
            }else{
                data[flag].ID = data[flag+1].ID;
                data[flag].NodeAdrress = data[flag+1].NodeAdrress;    
            }
        }
    }
}

String Remember::GetAddress(const String ID)
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
