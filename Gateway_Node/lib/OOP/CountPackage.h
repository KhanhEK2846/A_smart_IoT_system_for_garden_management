#include <stdlib.h>
#include <WString.h>

class CountPackage
{
private:
    String ID[10];
    int total[10];
    int flag;
public:
    CountPackage(/* args */);
    ~CountPackage();
    int Count(const String id = "");
    String Export();
};

