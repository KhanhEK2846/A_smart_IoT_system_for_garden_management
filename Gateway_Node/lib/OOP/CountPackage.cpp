#include "CountPackage.h"

CountPackage::CountPackage()
{
    for(flag = 0;flag<10;flag++){
        ID[flag] = "";
        total[flag] = 0;
    }
}

CountPackage::~CountPackage()
{
    for(flag = 0;flag<10;flag++){
        ID[flag].remove(0);
        total[flag] = 0;
    }
    flag = 0;
}

int CountPackage::Count(const String id){
    if(id == "")
        return 0;
    for(flag = 0;flag<10;flag++){
        if(ID[flag] == "" || ID[flag] == id)
            break;
    }
    if(flag == 10)
        return 1;
    ID[flag] = id;
    total[flag] += 1;
    return 2;
}

String CountPackage::Export(){
    String tmpData = "/--------------------------------------/\n";
    for(flag = 0;flag<10;flag++){
        if(ID[flag] == "")
            break;
    tmpData += "ID: " + ID[flag] + "\n" "Receive: "+ String(total[flag]) +"\n";
    
    }
    tmpData += "/--------------------------------------/";
    return tmpData;

}