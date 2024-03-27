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

class Remember
{
private:
    dataRemeber data[10];
    FriendAround friends[32];
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
};
