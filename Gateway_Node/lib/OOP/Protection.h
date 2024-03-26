#include <stdlib.h>
#include <WString.h>

enum TypeProtect{
    STA,
    AP,
    HTTP,
    AUTH
};

class Protection
{
private:
    //WiFi
    String sta_ssid; 
    String sta_password ;
    String ap_ssid;
    String ap_password;
    //Authentication
    String http_username;
    String http_password;
    String auth_username;
    String auth_password;
public:
    Protection();
    ~Protection();
    String getID(int id) const;
    void setID(const String data, int id);
    String getPass(int id) const;
    void setPass(const String data, int id);
    void AppendIDAP(const String append_id);
};

