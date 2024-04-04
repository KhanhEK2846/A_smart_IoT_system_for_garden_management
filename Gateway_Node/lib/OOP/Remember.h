#include <CommonFunction.h>

class dataRemeber
{
private:
    friend class Remember;
protected:
    String NodeAdrress;
    String ID;
public:
    dataRemeber();
    ~dataRemeber();
};

class FriendAround
{
private:
    friend class Remember;
protected:
    String friendID;
public:
    FriendAround();
    ~FriendAround();
};

class ACKstorage
{
private:
    friend class Remember;
protected:
    String ID;
    String From;
    String Mode;
public:
    ACKstorage();
    ~ACKstorage();
};
class Remember
{
private:
    dataRemeber data[10];
    FriendAround friends[32];
    ACKstorage ACK[20];
    int flag;
public:
    Remember();
    ~Remember();
    bool AddAddress(const String ID = "", const String From = "");
    void RemoveAddress(const String ID = "");
    const String GetAddress(const String ID = "");
    bool AddFriend(const String ID = "",const int channel = -1);
    void RemoveFriend(const int channel = -1);
    String GetFriend(const int channel = -1) const;
    const int GetNextChannelFriend(const int CurrentChannel = -1, bool freeRoom = false);
    const bool IsFriend(const uint8_t H,const uint8_t L,const uint8_t chan);
    const bool IsFriend(const String ID = "");
    const bool IsOnAddress(const String friendID = "");
    bool AddACK(const String ID = "", const String From = "", const String Mode = "");
    int IsACK(const String ID = "", const String From = "", const String Mode = "");
    void RemoveACK(int Location = -1);
};
