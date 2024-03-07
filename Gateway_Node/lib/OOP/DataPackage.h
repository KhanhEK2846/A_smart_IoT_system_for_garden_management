#include <stdlib.h>
#include <WString.h>
#include <FirebaseESP32.h>
class DataPackage
{
private:
    String Mode;
    String ID;
    String From;
    String data;
public:
    int expired;
    String NotDirect;
    DataPackage();
    String GetID() const;
    String GetFrom() const;
    String GetData() const;
    String GetMode() const;
    void SetDataPackage(const String t_id,const String t_from, const String t_data, const String t_mode); // "" for nothing change
    void SetMode(const String mode);
    void SetFrom(const String address);
    ~DataPackage();
    DataPackage& operator=(const DataPackage temp);
    String toString(boolean prettier = false) const;
    void DataToJson(FirebaseJson * slave);
    boolean fromString(const String data);
    void ResetExpired();
};
