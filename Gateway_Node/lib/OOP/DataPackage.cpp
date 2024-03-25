#include "DataPackage.h"

DataPackage ::DataPackage()
{
    data = "";
    ID = "";
    NotDirect = "";
    From = "";
    Mode = "";
    expired = 4;
}

String DataPackage:: GetID() const
{
    return this->ID;
}

String DataPackage::GetData() const
{
    return this->data;
}

String DataPackage::GetMode() const
{
    return this->Mode;
}

String DataPackage::GetFrom() const
{
    return this->From;
}

void DataPackage::SetDataPackage(const String t_id,const String t_from, const String t_data, const String t_mode)
{
    if(t_id != "")
        this->ID = t_id;
    if(t_data != "")
        this->data = t_data;
    if(t_from != "")
        this->From = t_from;
    if(t_mode != "")
        this->Mode = t_mode;
}

void DataPackage::SetMode(const String mode)
{
    this->Mode = mode;
}

void DataPackage::SetFrom(const String address)
{
    this->From = address;
}

DataPackage ::~DataPackage()
{
    data.remove(0);
    ID.remove(0);
    From.remove(0);
    Mode.remove(0);
    NotDirect.remove(0);
}

DataPackage& DataPackage::operator=(const DataPackage temp)
{
    data = temp.data;
    ID = temp.ID;
    From = temp.From;
    Mode = temp.Mode;
    expired = temp.expired;
    NotDirect = temp.NotDirect;
    return *this;
}

String DataPackage::toString(boolean prettier) const
{
    String temp;
    if(prettier)
        temp = "{\nID: ";
    else
        temp = "{";
    temp += this->ID;
    if(prettier)
        temp += "\nFrom: ";
    else
        temp += ",";
    temp += this->From;
    if(prettier)
        temp += "\nMode: ";
    else
        temp += ",";
    temp += this->Mode;
    if(prettier)
        temp += "\nData: ";
    else
        temp += "," ;
    temp += this->data ;
    if(prettier) //Not Send expired
    {
        temp += "\nExpired: ";
        temp += this->expired;
        temp += "\nNot Direct: ";
        temp += this->NotDirect;
        temp += "\n}";
    }  
    else
        temp += "}";
    return temp;
};

void DataPackage::DataToJson(FirebaseJson* slave)
{
    String t_data = this->data;
    slave->set("Name",t_data.substring(0,t_data.indexOf("/")));//Name
    t_data = t_data.substring(t_data.indexOf("/")+1);
    int tempNumber = atof(t_data.substring(0,t_data.indexOf("/")).c_str());
    slave->set("Error/DHT11",String((tempNumber>>4)&1)); //Status DHT
    slave->set("Error/LDR",String((tempNumber>>3)&1));//Status Light Sensor
    slave->set("Error/Soil",String((tempNumber>>2)&1)); //Soid moisure sensor
    slave->set("Status/Led",String((tempNumber>>1)&1));//Light
    slave->set("Status/Pump",String((tempNumber>>0)&1));//Pump
    t_data = t_data.substring(t_data.indexOf("/")+1);
    slave->set("Sensor/DHT11/Humi",t_data.substring(0,t_data.indexOf("/")));//Humi
    t_data = t_data.substring(t_data.indexOf("/")+1);
    slave->set("Sensor/DHT11/Temp",t_data.substring(0,t_data.indexOf("/")));//Temperture
    t_data = t_data.substring(t_data.indexOf("/")+1);
    slave->set("Sensor/Light",t_data.substring(0,t_data.indexOf("/")));//Bright
    t_data = t_data.substring(t_data.indexOf("/")+1);
    slave->set("Sensor/Solid",t_data.substring(0,t_data.indexOf("/")));//Mois
    t_data = t_data.substring(t_data.indexOf("/")+1);
    slave->set("Plant/Days",t_data); // Day pass
    //slave->toString(t_data,true);
    //Serial.println(t_data);
}

boolean DataPackage::CheckFormat(){
    if(this->Mode == "0"){ //If ACK package
        if(this->ID.length() == 6 && this->From.length() == 6) return true;
    }
    else{
        if(this->ID.length() == 17 && this->From.length() == 6) return true;
    }
    return false;
}

boolean DataPackage::fromString(const String data)
{
    if(data.indexOf("{") == -1 || data.lastIndexOf("}") == -1)
        return false;
    String temp = data.substring(data.indexOf("{") + 1, data.lastIndexOf("}"));
    this->ID = temp.substring(0,temp.indexOf(","));
    temp = temp.substring(temp.indexOf(",")+1);
    this->From = temp.substring(0,temp.indexOf(","));
    temp = temp.substring(temp.indexOf(",")+1);
    this->Mode = temp.substring(0,temp.indexOf(","));
    this->data = temp.substring(temp.indexOf(",")+1);
    return CheckFormat();
}

void DataPackage::ResetExpired()
{
    this->expired = 4;
}