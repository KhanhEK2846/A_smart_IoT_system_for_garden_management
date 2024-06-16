#include <Arduino.h>
#include <WiFi.h>
#include <LoRa_E32.h>
#include <DataPackage.h>
#include <Remember.h>
#include <CountPackage.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "CommandCode.h"
#include "CycleTime.h"

AsyncWebServer server(80); //Create a web server listening on port 80
AsyncWebSocket ws("/ws");//Create a path for web socket server

enum PORT{
  DHTPIN_Port = 21,
  LDR_Port = 34,
  Soil_Moisture_Port = 35,
  Pumps_Port = 22,
  Light_Port = 23,
  AUX_Port = 4,
  M1_Port = 5,
  M0_Port = 18
};

CountPackage packageCount;

const String id = "";
const String pass = "";
const String ap_id = WiFi.macAddress();
const String ap_pass = "1234567890";

//Define LoRa
LoRa_E32 lora(&Serial2,AUX_Port,M1_Port,M0_Port);
volatile boolean lora_flag = false;
uint8_t Own_AddH;
uint8_t Own_AddL;
uint8_t Own_Channel;
String Own_address = "";
volatile uint8_t Gateway_AddH = 0;
volatile uint8_t Gateway_AddL = 0;
volatile uint8_t Gateway_Channel = 0x17;
volatile boolean Gateway_Changed_byUser = false;
volatile int Friend_Channel = 0;
unsigned long Wait_to_Hello = 0;
DataPackage Hello_Message;

//Task Delivery Data
TaskHandle_t DeliveryTask = NULL;
TaskHandle_t DatabaseTask = NULL;
TaskHandle_t CaptureTask = NULL;
TaskHandle_t ReSendTask = NULL;
QueueHandle_t Queue_Delivery = NULL;
QueueHandle_t Queue_Command = NULL;
QueueHandle_t Queue_Database = NULL;
QueueHandle_t Queue_ReSend = NULL;
const int Queue_Length = 200;
const unsigned long long Queue_item_delivery_size = sizeof(DataPackage);
const unsigned long long Queue_item_command_size = sizeof(String);
const unsigned long long Queue_item_database_size = sizeof(DataPackage);

//ID
const String ID = WiFi.macAddress();

//Own & Deliver
DataPackage O_Pack;
String O_Command;
DataPackage MQTT_Data;

//Time
unsigned long timeSend= 0;
unsigned long timeExport = 0;

//count
volatile int count = 0;
DataPackage predata;

/*------------------------------------------------------------------------------------------*/
void Reset_ConfigurationLoRa(boolean gateway = true)
{
  if(!lora_flag)
    return;
  ResponseStructContainer c = lora.getConfiguration();
  Configuration configuration = *(Configuration*) c.data;
  if(gateway)
  {
    configuration.ADDH = 0;
    configuration.ADDL = 0;
    configuration.CHAN = 0x17;
  }
  else
  {
    configuration.ADDH = Own_AddH;
    configuration.ADDL = Own_AddL;
    configuration.CHAN = Own_Channel;
  }
  configuration.SPED.uartParity = 0;
  configuration.SPED.uartBaudRate = 0b11;
  configuration.SPED.airDataRate = 0b10;
  configuration.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;
  configuration.OPTION.ioDriveMode = 0b1;
  configuration.OPTION.wirelessWakeupTime = 0;
  configuration.OPTION.fec = 0b1;
  configuration.OPTION.transmissionPower = 0;
  lora.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
  c.close(); 
}
void Delivery(void * pvParameters)
{
  DataPackage data;
  DataPackage tempData;
  UBaseType_t uxHighWaterMark;
  uint8_t DeliveryH;
  uint8_t DeliveryL;
  uint8_t DeliveryChan;
  Remember Locate;
  int Preventive_Channel = 0;
  int ACK_Locate = -1;
  while(true)
  {
    xQueueReceive(Queue_Delivery,&data,portMAX_DELAY);

if(data.GetMode() == ACK) { //Send Back
DeCodeAddressChannel(data.GetFrom(),DeliveryH,DeliveryL,DeliveryChan);
lora.sendFixedMessage(DeliveryH,DeliveryL,DeliveryChan,data.toString());
}else{
lora.sendFixedMessage(Gateway_AddH,Gateway_AddL,Gateway_Channel,data.toString());}
    
  }
    // Serial.print("Delivery Task: ");
    // uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    // Serial.println(uxHighWaterMark);
    // Serial.println();
    delay(3000);
}

void Capture(void * pvParameters)
{
  ResponseContainer mess;
  UBaseType_t uxHighWaterMark;
  DataPackage ResponseACK;
  String TempAddress = "";
  DataPackage Receive_Pack;
  DataPackage Memory_Pack;
  String D_Command;
  Memory_Pack.SetMode(Memorize);
  ResponseACK.SetMode(ACK);
  const String Own_Adrress = *((String*)pvParameters);
  while (true)
  {
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    if(lora.available()>1)
    {
      mess = lora.receiveMessageUntil();
      if(!Receive_Pack.fromString(mess.data))
         continue;
      Serial.println("Receive: ");
      Serial.println(Receive_Pack.toString(true));
      D_Command = Receive_Pack.GetData();
      xQueueSend(Queue_Command,&D_Command,pdMS_TO_TICKS(10));

if (Receive_Pack.GetMode() == Default ) {
ResponseACK.SetDataPackage(Receive_Pack.GetID(),Receive_Pack.GetFrom(),ID,"");
xQueueSend(Queue_Delivery,&Receive_Pack,pdMS_TO_TICKS(10));}
 
      
    } else delay(10);

    // Serial.print("Capture Task: ");
    // uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    // Serial.println(uxHighWaterMark);
    // Serial.println();
  }
}
void Init_LoRa()
{
  lora.begin();
  ResponseStructContainer c = lora.getConfiguration(); 
  if(c.status.code == 1)
  {
    Configuration configuration = *(Configuration*) c.data;
    CalculateAddressChannel(ID,Own_AddH,Own_AddL,Own_Channel);
    Own_address = EnCodeAddressChannel(Own_AddH,Own_AddL,Own_Channel);
    configuration.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;
    configuration.ADDH = Own_AddH;
    configuration.ADDL = Own_AddL;
    configuration.CHAN = Own_Channel;
    lora.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
    configuration.SPED.uartParity = 0;
    configuration.SPED.uartBaudRate = 0b11;
    configuration.SPED.airDataRate = 0b10;
    configuration.OPTION.ioDriveMode = 0b1;
    configuration.OPTION.wirelessWakeupTime = 0;
    configuration.OPTION.fec = 0b1;
    configuration.OPTION.transmissionPower = 0;
    lora_flag = true;
      
  }
  else
  {
    lora_flag = false;
  }
  c.close();
}

void PostponeSend(void * pvParameters)
{
  DataPackage data;
  while(true){
    xQueueReceive(Queue_ReSend,&data,portMAX_DELAY);
    xQueueSend(Queue_Delivery,&data,pdMS_TO_TICKS(10));
    delay(7500);
  }
}

void initWebSocket() //Initialize the WebSocket protocol
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void Init_Server(){
server.on("/Test",HTTP_GET,[](AsyncWebServerRequest *request){
    return request->send_P(Received_Code,"text/plain",packageCount.Export());
  });
server.begin();
}

void Init_Task()
{
  Queue_Delivery = xQueueCreate(Queue_Length,Queue_item_delivery_size+1);
  Queue_Command = xQueueCreate(Queue_Length,Queue_item_command_size+1);
  Queue_Database = xQueueCreate(Queue_Length,Queue_item_database_size+1);
  Queue_ReSend = xQueueCreate(Queue_Length,Queue_item_delivery_size+1);
  xTaskCreate(
    Delivery,
    "Delivery",
    6000, //4100B left
    NULL,
    0,
    &DeliveryTask
  );
  xTaskCreate(
    Capture,
    "Capture",
    6000, //4628B left
    (void*)&Own_address,
    0,
    &CaptureTask
  );
  xTaskCreate(
    PostponeSend,
    "PostponeSend",
    6000,
    NULL,
    0,
    &ReSendTask
  );
}

void setup(){
  Serial.begin(9600);
  Serial.println();
  Init_LoRa();
  Init_Task();
initWebSocket();
WiFi.mode(WIFI_AP_STA);
WiFi.softAP(ap_id.c_str(), ap_pass.c_str());
Init_Server();
  timeSend = millis();
  timeExport = millis();
  O_Pack.SetDataPackage(ID,Own_address,ID,Default);
  predata.SetDataPackage(ID,Own_address,String(count),Default);
  if(id != ""){
    WiFi.begin(id,pass);
    long current = millis();
    while (WiFi.status() != WL_CONNECTED && (unsigned long) (millis()- current) < Five_Seconds_millis){
      delay(500);
    }
  }
  if( WiFi.status() == WL_CONNECTED){
    Reset_ConfigurationLoRa();
  }

}

void loop(){
  if(xQueueReceive(Queue_Command,&O_Command,0) == pdPASS){
    Serial.println(O_Command);
    int r = packageCount.Count(O_Command);
    Serial.println(String(r));
    Serial.println("Receive Mess");
  }

  if( WiFi.status() != WL_CONNECTED && ((unsigned long)(millis() - timeSend) >= Five_Seconds_millis) && count < 200)
  {
    timeSend = millis();
    xQueueSend(Queue_Delivery,&O_Pack,pdMS_TO_TICKS(10));
    count++;
  }
  if(((unsigned long)(millis() - timeExport) >= A_minutes_millis)){
    timeExport = millis();
    Serial.println(packageCount.Export());
  }

}