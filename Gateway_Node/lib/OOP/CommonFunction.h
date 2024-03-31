#include <stdlib.h>
#include <stdio.h>
#include <WString.h>

void CalculateAddressChannel(const String id, uint8_t &H, uint8_t &L, uint8_t &chan);
String EnCodeAddressChannel(const uint8_t H,const uint8_t L,const uint8_t chan);
void DeCodeAddressChannel(const String address, uint8_t &H, uint8_t &L, uint8_t &chan);
String CalculateToEncode(const String id);
String BoolToInt(bool DHT,bool LDR, bool Soil, bool Light, bool Pump);