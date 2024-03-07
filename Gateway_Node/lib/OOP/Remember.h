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


class Remember
{
private:
    dataRemeber data[10];
    int count;
    int flag;
public:
    Remember();
    ~Remember();
    bool Add(const String ID = "", const String From = "");
    String GetAddrress(const String ID = "");
};
