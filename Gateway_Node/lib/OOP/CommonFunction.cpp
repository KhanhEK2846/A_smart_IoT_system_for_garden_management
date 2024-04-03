#include "CommonFunction.h"

void CalculateAddressChannel(const String id, uint8_t &H, uint8_t &L, uint8_t &chan)
{
  int tempid[6];
  sscanf(id.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &tempid[0], &tempid[1], &tempid[2], &tempid[3], &tempid[4], &tempid[5]);
	H = tempid[0] + tempid[1] + tempid[2];
	L = tempid[3] + tempid[4] + tempid[5];
	chan = tempid[0] + tempid[1] + tempid[2] + tempid[3] + tempid[4] + tempid[5];
  while(H > 0xFF) H -= 0x100;
  while(L > 0xFF) L -= 0x100;
  while(chan > 0x1F) chan -= 0x20;
}
uint8_t CalculateChannel(const String id)
{
  int tempid[6];
  sscanf(id.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &tempid[0], &tempid[1], &tempid[2], &tempid[3], &tempid[4], &tempid[5]);
  uint8_t chan = tempid[0] + tempid[1] + tempid[2] + tempid[3] + tempid[4] + tempid[5];
  while(chan > 0x1F) chan -= 0x20;
  return chan;
}
String EnCodeAddressChannel(const uint8_t H,const uint8_t L,const uint8_t chan)
{
  char macStr[3] = { 0 };
  sprintf(macStr,"%02x%02x%02x", H,L,chan);
  return String(macStr);
}
void DeCodeAddressChannel(const String address, uint8_t &H, uint8_t &L, uint8_t &chan)
{
  sscanf(address.c_str(),"%02x%02x%02x",&H,&L,&chan);
}
String CalculateToEncode(const String id)
{
  uint8_t H = 0;
  uint8_t L = 0;
  uint8_t chan = 0;
  CalculateAddressChannel(id,H,L,chan);
  return EnCodeAddressChannel(H,L,chan);
}

String BoolToInt(bool DHT,bool LDR, bool Soil, bool Light, bool Pump)
{
  int ConvertToInt = 0; //DHT_Err LDR_Err Soil_Err LightStatus PumpsStatus
  ConvertToInt |= DHT <<4;
  ConvertToInt |= LDR <<3;
  ConvertToInt |= Soil <<2;
  ConvertToInt |= Light <<1;
  ConvertToInt |= Pump <<0;
  return String(ConvertToInt);
}