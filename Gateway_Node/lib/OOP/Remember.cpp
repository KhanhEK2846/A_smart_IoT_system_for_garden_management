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

FriendAround::FriendAround(){
    friendID = "";
}

FriendAround::~FriendAround(){
    friendID.remove(0);
}

Remember::Remember()
{
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
    if(flag == 10)
        return false;
    data[flag].ID = ID;
    data[flag].NodeAdrress = From;
    return true;
}

void Remember::RemoveAddress(const String ID){
    if(ID == "")
        return;
    bool del = false;
    for(flag =0;flag<10;flag++){
        if(data[flag].ID == ID){
            del = true;
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

const String Remember::GetAddress(const String ID)
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

bool Remember::AddFriend(const String ID,const int channel){
    if(ID == "" || channel == -1 || channel > 31)
        return false;
    if(friends[channel].friendID == ""){
        friends[channel].friendID = ID;
        return true;
    }
    return false;
}

void Remember::RemoveFriend(const int channel){
    if(channel == -1 || channel > 31)
        return;
    friends[channel].friendID = "";
}

String Remember::GetFriend(const int channel) const{
    if(channel == -1 || channel > 31)
        return "";
    return friends[channel].friendID;
}

const int Remember::GetNextChannelFriend(const int CurrentChannel, bool freeRoom){
    if (CurrentChannel == -1 || CurrentChannel > 31)
        return -1;
    flag = CurrentChannel;
    while(true){
        flag = (flag < 31)? flag+1:0;
        
        if(freeRoom && friends[flag].friendID == "")
            return flag;
        if(!freeRoom && friends[flag].friendID != "")
            return flag;
        if(flag == CurrentChannel)
            return -1;
    }
}

const bool Remember::IsFriend(const uint8_t H,const uint8_t L,const uint8_t chan){
    if(chan < 0 || chan >31)
        return false;
    if(friends[chan].friendID == "")//Empty
        return false;
    uint8_t tmp_H = 0;
    uint8_t tmp_L = 0;
    uint8_t tmp_chan = 0;
    CalculateAddressChannel(friends[chan].friendID,tmp_H,tmp_L,tmp_chan);
    if(tmp_H==H && tmp_L==L && tmp_chan==chan)
        return true;
    return false;
}

const bool Remember::IsFriend(const String ID){
    if(ID== "")
        return false;
    if(friends[CalculateChannel(ID)].friendID == ID)
        return true;
    return false;
}

const bool Remember::IsOnAddress(const String friendID){
    if(GetAddress(friendID) == "")
        return false;
    return true;
}