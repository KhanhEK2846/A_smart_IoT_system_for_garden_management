#include "Protection.h"

Protection::Protection(){
    sta_ssid = ""; 
    sta_password = "" ;
    ap_ssid = "ESP_";
    ap_password = "123456789";
    http_username = "admin";
    http_password = "admin";
    auth_username = "owner";
    auth_password = "owner";
}

Protection::~Protection(){
    sta_ssid.remove(0); 
    sta_password.remove(0);
    ap_ssid.remove(0);
    ap_password.remove(0);
    http_username.remove(0);
    http_password.remove(0);
    auth_username.remove(0);
    auth_password.remove(0);
}

String Protection::getID(int id) const{
    switch (id)
    {
    case TypeProtect::STA:
        return sta_ssid;
    case TypeProtect::AP:
        return ap_ssid;
    case TypeProtect::HTTP:
        return http_username;
    case TypeProtect::AUTH:
        return auth_username;
    
    default:
        return "";
    }

}

void Protection::setID(const String data, int id){
    switch (id)
    {
        case TypeProtect::STA:
            sta_ssid = data;
            break;
        case TypeProtect::AP:
            ap_ssid = data;
            break;
        case TypeProtect::HTTP:
            http_username = data;
            break;
        case TypeProtect::AUTH:
            auth_username = data;
            break;
        default:
            break;
    }
}

String Protection::getPass(int id) const{
    switch (id)
    {
    case TypeProtect::STA:
        return sta_password;
    case TypeProtect::AP:
        return ap_password;
    case TypeProtect::HTTP:
        return http_password;
    case TypeProtect::AUTH:
        return auth_password;
    
    default:
        return "";
    }

}

void Protection::setPass(const String data, int id){
    switch (id)
    {
        case TypeProtect::STA:
            sta_password = data;
            break;
        case TypeProtect::AP:
            ap_password = data;
            break;
        case TypeProtect::HTTP:
            http_password = data;
            break;
        case TypeProtect::AUTH:
            auth_password = data;
            break;
        default:
            break;
    }
}

void Protection::AppendIDAP(const String append_id){
    ap_ssid += append_id;
}