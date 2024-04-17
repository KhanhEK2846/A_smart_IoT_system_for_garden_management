#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <LoRa_E32.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include  <FirebaseESP32.h>
#include "addons/RTDBHelper.h"
#include <time.h>
#include <HTTPClient.h>
#include <DataPackage.h>
#include <Remember.h>
#include <Plant.h>
#include <DataSensor.h>
#include <ActuatorStatus.h>
#include <Protection.h>
#include "URL.h"
#include "html.h"
#include "CommandCode.h"
#include "CycleTime.h"
//GPIO_PORT
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
//DHT11 Variable
#define DHTTYPE DHT11 
DHT dht(DHTPIN_Port, DHTTYPE);
//Data Sensor
DataSensor dataSensor;
boolean Err = false;//Catch Error Sensor
//Plant
Plant Tree;
//Days
unsigned long Time_Passed = 0;
//Pump
unsigned long Times_Pumps=0;
unsigned long Still_Pumps = 30000; //Water in 1 minute
ActuatorStatus statusActuator;
//WIFI Variable
Protection protect;
//LoRa Variable
LoRa_E32 lora(&Serial2,AUX_Port,M1_Port,M0_Port); //16-->TX 17-->RX 4-->AUX 5-->M1 18-->M0 
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
//Ping
WiFiClient PingClient;
unsigned long Last_ping_time = 0;
boolean ping_flag = false; //Result Ping
//NTP
unsigned long timestamp;
//Firebase Variable
FirebaseData firebaseData;
unsigned long Last_datalogging_time = 0;
//MQTT Variable
WiFiClient wifiClient;
PubSubClient client(wifiClient);
boolean MQTTStatus = false; //Status Connect Broker
String MQTT_Messange = "";
//Local Server Variable
AsyncWebServer server(80); //Create a web server listening on port 80
AsyncWebSocket ws("/ws");//Create a path for web socket server
int Person = 0; // Number clients access local host
String messanger;
String MessLimit;
//Command from client
int Command_Pump = NOTHING; 
int Command_Light = NOTHING;
//flag
boolean sta_flag = false;
boolean first_sta = true;
boolean valueChange_flag = false;
//Type of server
enum GATEWAYNODE{
  DEFAULt_STATUS,
  GATEWAY_STATUS,
  NODE_STATUS
};
volatile int gateway_node = DEFAULt_STATUS; // 0:default 1: gateway 2:node
//Own & Deliver
DataPackage O_Pack;
String O_Command;
DataPackage MQTT_Data;
unsigned long own_wait_time = 0;
volatile boolean sent_RTDB = false;
//Task Delivery Data
TaskHandle_t DeliveryTask = NULL;
TaskHandle_t DatabaseTask = NULL;
TaskHandle_t CaptureTask = NULL;
TaskHandle_t ReSendTask = NULL;
QueueHandle_t Queue_Delivery = NULL;
QueueHandle_t Queue_Command = NULL;
QueueHandle_t Queue_Database = NULL;
QueueHandle_t Queue_ReSend = NULL;
const int Queue_Length = 50;
const unsigned long long Queue_item_delivery_size = sizeof(DataPackage);
const unsigned long long Queue_item_command_size = sizeof(String);
const unsigned long long Queue_item_database_size = sizeof(DataPackage);
//Sercurity
boolean sercurity_backend_key = false;
boolean tolerance_backend_key = false;
unsigned long before_reset_key = 0;
//ID
const String ID = WiFi.macAddress();
//Slove Command
String Actuator = "";
String Require = "";
// Store Recent Value
int TempData[11] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
/*-------------------------------------Get Real World Time------------------------------------------------*/
#pragma region Current Time
unsigned long getTime() // Get Timestamp
{ 
  time_t now; 
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {return(0);} //Failed to obtain time
  time(&now);
  return now;
}
void Make_Day()//Counter Day
{
  if((unsigned long) (millis()-Time_Passed) >= A_Day_millis){
    Time_Passed = millis();
    Tree.Days ++;
  }
  if(Tree.Days > 999)
    Tree.Days = 0;
}
#pragma endregion
/*-------------------------------------Ping Network-------------------------------------------------------*/
#pragma region Check Internet Connected from Wifi
void PingNetwork()// Ping to host
{
  if(gateway_node == GATEWAY_STATUS){ //If it's gateway, check Internet
    if (PingClient.connect(PINGInternet, 80)) {
      ping_flag = true;
    } else {
      ping_flag = false;
    }
    PingClient.stop();
  }else{ping_flag =  false;}
}
void Cycle_Ping()// Cycle Ping to Host // FIX:
{
  if(Last_ping_time == 0)
    Last_ping_time = millis();
  if((unsigned long)(millis()- Last_ping_time) > Five_minutes_millis){
    Last_ping_time = millis();
    PingNetwork();
  }
}
#pragma endregion
/*---------------------------------------LoRa SetUP-------------------------------------------------------*/
#pragma region LoRa
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
    //Serial.print(data.toString(true));
    /*------------------User Change Gateway--------------------*/
    if(Gateway_Changed_byUser){ //Incase that address gateway belongs to node
      String tmpIDofGateway = EnCodeAddressChannel(Gateway_AddH,Gateway_AddL,Gateway_Channel);
      Locate.RemoveRouteViaFrom(tmpIDofGateway);
      Gateway_Changed_byUser = false;
    }
    /*------------------------Response------------------------*/
    if(data.GetMode() == ACK) //Receive ACK
    {
      Serial.println("Receive ACK");
      Locate.AddACK(data.GetID(),data.GetFrom(),data.GetData());
      continue;//No mess fit ack
    }
    /*----------------------Memorize---------------------------*/
    if(data.GetMode() == Memorize)
    {
      if(data.GetData() == CommandDirect || data.GetData() == CommandNotDirect){ //Save in Friend
        Serial.println("Saving to Friend");
        Locate.RemoveRouteViaFrom(data.GetFrom());
        if(Locate.AddFriend(data.GetFrom(),CalculateChannel(data.GetFrom()))){
          Preventive_Channel = (Preventive_Channel == -1)? 0:Preventive_Channel; //Unlock Get Friend Address
          Serial.println("Save friend successfully");
        }
      }else{ //Save in Routing
        Serial.println("Saving to Routing");
        Locate.AddRoute(data.GetID(),data.GetFrom());
        if(Locate.IsFriend(data.GetID())) // Remove friend
        {
          int tmpChannel = CalculateChannel(data.GetID()); //Get Channel of this ID from receive package
          Friend_Channel = (Friend_Channel == -1)? 0:Friend_Channel; //Unlock Auto Detect Friend Node
          DeCodeAddressChannel(Locate.GetFriend(tmpChannel),DeliveryH,DeliveryL,DeliveryChan);// Get Address and Channel of friend
          Locate.RemoveFriend(tmpChannel);
          /*------------------------------If only current Gateway is used this Friend--------------------------------*/
          if(DeliveryH == Gateway_AddH && DeliveryL == Gateway_AddL && DeliveryChan == Gateway_Channel){
            Preventive_Channel = Locate.GetNextChannelFriend(Preventive_Channel); // Be locked if return -1
            if(Preventive_Channel == -1) //None Firend
            {
              Gateway_AddH = 0;
              Gateway_AddL = 0;
              Gateway_Channel = 0x17;
            }else{
              DeCodeAddressChannel(Locate.GetFriend(Preventive_Channel),DeliveryH,DeliveryL,DeliveryChan);
              Gateway_AddH = DeliveryH;
              Gateway_AddL = DeliveryL;
              Gateway_Channel = DeliveryChan;
            }
          }
        }
      }

      continue;
    }
    /*-----------------Say Hello---------------------------*/
    if(data.GetMode() == SayHello)
    {
      //Serial.print(data.toString(true));
      if(data.GetID() == ID) //Broascast Hello
      {
        lora.sendBroadcastFixedMessage(Friend_Channel,data.toString());
        Serial.println("Say Hello");
        Serial.print("Channel: ");
        Serial.println(Friend_Channel);
        Friend_Channel = Locate.GetNextChannelFriend(Friend_Channel,true); //Lock if return -1
      }else{ //Save ID and response Hi
        DeCodeAddressChannel(data.GetFrom(),DeliveryH,DeliveryL,DeliveryChan);
        Serial.println("Response Hello");
        Serial.println(data.GetID());
        Serial.println(atoi(data.GetData().c_str()));
        if(Locate.AddFriend(data.GetID(),atoi(data.GetData().c_str())))
          Preventive_Channel = (Preventive_Channel == -1)? 0:Preventive_Channel; //Unlock Get Friend Address
        data.SetDataPackage(ID,"",String(Own_Channel),SayHi);
        lora.sendFixedMessage(DeliveryH,DeliveryL,DeliveryChan,data.toString());
      }
      continue;
    }
    if(data.GetMode()== SayHi){ //Save ID
      Serial.println("Say Hi");
      Serial.println(data.GetID());
      Serial.println(atoi(data.GetData().c_str()));
      if(Locate.AddFriend(data.GetID(),atoi(data.GetData().c_str())))
        Preventive_Channel = (Preventive_Channel == -1)? 0:Preventive_Channel; //Unlock Get Friend Address
      continue;
    }
    /*-----------------Check Expired---------------------------*/  
    if(data.expired == 0)
    {
      Serial.println("Expired");
      if(data.GetMode() == CommandNotDirect) // Remove ID From Memory
      {
        Locate.RemoveRoute(data.GetID());
        continue;
      }
      if(data.GetMode() == CommandDirect)
      {
        data.NotDirect = Locate.GetRoute(data.GetID());
        if(data.NotDirect == "") // Not found
          continue; //TODO: Solution for ID not found
        if(data.NotDirect == CalculateToEncode(data.GetID())){ // From and ID belong to same node
          Locate.RemoveRoute(data.GetID());
          continue;
        }
        data.ResetExpired();
        data.SetMode(CommandNotDirect); //Direct -> Not Direct
      }
      if(data.GetMode() == LogData)
      {
        /*----------------------------------Remove if it a friend--------------------------------------*/
        if(Locate.IsFriend(Gateway_AddH,Gateway_AddL,Gateway_Channel)){
          Locate.RemoveFriend(Gateway_Channel);
          Friend_Channel = (Friend_Channel == -1)? 0:Friend_Channel; //Unlock Auto Detect Friend Node
        }
        /*-------------------------------Routing friend Router---------------------------------------------*/
        Preventive_Channel = Locate.GetNextChannelFriend(Preventive_Channel); // Be locked if return -1
        if(Preventive_Channel == -1) //None Firend
        {
          Gateway_AddH = 0;
          Gateway_AddL = 0;
          Gateway_Channel = 0x17;
        }else{
          DeCodeAddressChannel(Locate.GetFriend(Preventive_Channel),DeliveryH,DeliveryL,DeliveryChan);
          Gateway_AddH = DeliveryH;
          Gateway_AddL = DeliveryL;
          Gateway_Channel = DeliveryChan;
        }
        continue;
      }
    };
    /*--------------------------------------------------------*/
    if(data.GetMode() == Default || data.GetMode() == LogData) //Send to Gateway
    {
      if(data.GetMode() == Default && data.GetID() == ID)
        sent_RTDB = false;
      if(data.GetMode() == LogData && data.expired < 9){
        ACK_Locate = Locate.IsACK(data.GetID(),data.GetFrom(),data.GetMode());
        if(ACK_Locate != -1)
        {
          Locate.RemoveACK(ACK_Locate);
          Serial.println("Remove Message Log");
          continue;
        }
      }
      lora.sendFixedMessage(Gateway_AddH,Gateway_AddL,Gateway_Channel,data.toString());
      data.expired--;
      if(data.GetMode() == LogData){
        xQueueSend(Queue_ReSend,&data,pdMS_TO_TICKS(10));
      }
    }
    else //Send to Node
    {
      if((data.GetMode() == CommandDirect || data.GetMode() == CommandNotDirect) && data.expired < 9){
        ACK_Locate = Locate.IsACK(data.GetID(),data.GetFrom(),data.GetMode());
        if(ACK_Locate != -1)
        {
          Locate.RemoveACK(ACK_Locate);
          Serial.println("Remove Message Command");
          continue;
        }
      }
      if(data.GetMode() == PrepareACK)
      {
        data.SetMode(ACK);
        DeCodeAddressChannel(data.GetFrom(),DeliveryH,DeliveryL,DeliveryChan);
      }
        
      else if(data.GetMode() == CommandNotDirect)
      {
        DeCodeAddressChannel(data.NotDirect,DeliveryH,DeliveryL,DeliveryChan);
      }else
      {
        CalculateAddressChannel(data.GetID(),DeliveryH,DeliveryL,DeliveryChan);
      }
      lora.sendFixedMessage(DeliveryH,DeliveryL,DeliveryChan,data.toString());

      data.expired--;
      if(data.GetMode() == CommandDirect || data.GetMode() == CommandNotDirect){
        xQueueSend(Queue_ReSend,&data,pdMS_TO_TICKS(10));
      }
    }
    // Serial.print("Delivery Task: ");
    // uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    // Serial.println(uxHighWaterMark);
    // Serial.println();
    delay(3000);
  }
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
  ResponseACK.SetMode(PrepareACK);
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
      /*-------------------------Prohibit situation---------------------------*/
      if(Receive_Pack.GetID() == ID && (Receive_Pack.GetMode() == LogData || Receive_Pack.GetMode() == Default)) //Impossible to reveive a package of it which's made by it
        continue;//Ignore package to force node which send package change Gateway Channel
      /*------------------------Say Hi------------------------*/
      if(Receive_Pack.GetMode() == SayHello || Receive_Pack.GetMode() == SayHi || Receive_Pack.GetMode() == ACK){
        xQueueSendToFront(Queue_Delivery,&Receive_Pack,pdMS_TO_TICKS(10));
        continue;
      }
      if(Receive_Pack.GetMode() == CommandDirect || Receive_Pack.GetMode() == CommandNotDirect || Receive_Pack.GetMode() == LogData) //Send ACK
      {
        ResponseACK.SetDataPackage(Receive_Pack.GetID(), Receive_Pack.GetFrom(),Receive_Pack.GetMode(),"");
        Serial.println("Prepare to Send ACK");
        xQueueSendToFront(Queue_Delivery,&ResponseACK,pdMS_TO_TICKS(10));
      }
      /*-------------------------------Save for Routing Table---------------------------------*/
      Memory_Pack.SetDataPackage(Receive_Pack.GetID(),Receive_Pack.GetFrom(),Receive_Pack.GetMode(),"");
      xQueueSendToFront(Queue_Delivery,&Memory_Pack,pdMS_TO_TICKS(10));
      /*------------------------Itself or other-----------------*/  
      if(Receive_Pack.GetID() == ID) //If message for node 
      {
        if(Receive_Pack.GetMode() == CommandDirect || Receive_Pack.GetMode() == CommandNotDirect) //Receive Command 
        {
          D_Command = Receive_Pack.GetData();
          xQueueSend(Queue_Command,&D_Command,pdMS_TO_TICKS(10));
          continue;
        }
      }
      else
      {
        Receive_Pack.SetFrom(Own_Adrress);//Set Own From before Delivery
        if (Receive_Pack.GetMode() == CommandDirect || Receive_Pack.GetMode() == CommandNotDirect) //Command for the other node
        {
          Receive_Pack.SetMode(CommandDirect); //Not direct -> Direct
          xQueueSend(Queue_Delivery,&Receive_Pack,pdMS_TO_TICKS(10));    
          continue;
        }
        if(Receive_Pack.GetMode() == Default || Receive_Pack.GetMode() == LogData)//Send to gateway or database
        {
          if(gateway_node == DEFAULt_STATUS)
            continue;
          if(gateway_node == GATEWAY_STATUS)// If it's a gateway -> Send to Database
          {
            xQueueSend(Queue_Database,&Receive_Pack,pdMS_TO_TICKS(10));
            continue;
          }
          if(gateway_node == NODE_STATUS)//If it's a node -> Delivery to Gateway
          {
            xQueueSend(Queue_Delivery,&Receive_Pack,pdMS_TO_TICKS(10));
            continue;
          }
        }
      }
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
  Configuration configuration = *(Configuration*) c.data;
  CalculateAddressChannel(ID,Own_AddH,Own_AddL,Own_Channel);
  Own_address = EnCodeAddressChannel(Own_AddH,Own_AddL,Own_Channel);
  if(c.status.code == 1)
  {
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
    if(gateway_node == DEFAULt_STATUS){
      gateway_node = NODE_STATUS;
    }
      
  }
  else
  {
    lora_flag = false;
  }
  c.close();
}
#pragma endregion
/*---------------------------------------------WebSocket SetUp--------------------------------------------*/
#pragma region WebSocket Protocol
void notifyClients(const String data) //Notify all local clients with a message
{
  String predata = "{";
  predata += data;
  predata += "}"; 
  ws.textAll(predata);
}
void notifyClient(AsyncWebSocketClient *client)//Notify only one local client
{
  String data ="{";
  int Convert = 0; // DHT_Err LDR_Err Soil_Err LightStatus PumpsStatus
  data += Tree.Name;
  data += "/";
  Convert |= dataSensor.DHT_Err <<4;
  Convert |= dataSensor.LDR_Err <<3;
  Convert |= dataSensor.Soil_Err <<2;
  Convert |= statusActuator.LightStatus <<1;
  Convert |= statusActuator.PumpsStatus <<0;
  data += String(Convert);
  data += "/";
  if(dataSensor.DHT_Err)
    data += String(0);
  else
    data += String((int)dataSensor.Humidity);
  data += "/";
  if(dataSensor.DHT_Err)
    data += String(0);
  else
    data += String((int)dataSensor.Temperature);
  data += "/";
  if(dataSensor.LDR_Err)
    data += String(0);
  else
    data += String(dataSensor.lumen);
  data += "/";
  if(dataSensor.Soil_Err)
    data += String(0);
  else  
    data += String(dataSensor.soilMoist);
  data += "/";
  if(WiFi.status() != WL_CONNECTED)   
    data += "Wifi OFF";
  else if (ping_flag)
    data += "Wifi CONNECT";
  else   
    data += "Wifi ON";
  data += "}";
  ws.text(client->id(),data);
  data.clear();
}
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) //Handle messange from local clients
{
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if(String((char*)data).indexOf("Username:") >= 0){
      protect.setID(String((char*)data).substring(String((char*)data).indexOf(' ')+1,String((char*)data).length()),STA);
    }
    if(String((char*)data).indexOf("Password:") >= 0){
      protect.setPass(String((char*)data).substring(String((char*)data).indexOf(' ')+1,String((char*)data).length()),STA);
      sta_flag = true;
    }
    if(String((char*)data).indexOf("NameTree:") >= 0){
      Tree.Name = String((char*)data).substring(String((char*)data).indexOf(' ')+1,String((char*)data).length());
    }
    if(String((char*)data).indexOf("Disconnect") >= 0){
      WiFi.disconnect(true,true);
      WiFi.mode(WIFI_AP);
      gateway_node = NODE_STATUS;
      ping_flag = false;
      notifyClients("Wifi OFF");
      Reset_ConfigurationLoRa(false);
    }
    if(String((char*)data).indexOf("Pump") >= 0){
      if(statusActuator.PumpsStatus)
        Command_Pump = OFF;
      else
        Command_Pump = ON;
    }
    if(String((char*)data).indexOf("LED") >= 0){
      if(statusActuator.LightStatus)
        Command_Light = OFF;
      else
        Command_Light = ON;
    }
  }
}//Handle messange from local clients
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,void *arg, uint8_t *data, size_t len) //Handle event WebSocket
{
  switch (type) {
    case WS_EVT_CONNECT:
      Person += 1;
      notifyClient(client);
      break;
    case WS_EVT_DISCONNECT:
        Person = (Person > 0)? Person-1 : 0;
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}//Handle event WebSocket
void initWebSocket() //Initialize the WebSocket protocol
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
#pragma endregion
/*------------------------------------------MQTT SetUp----------------------------------------------------*/
#pragma region MQTT Protocol
void connect_to_broker() // Connect to the broker
{
  while (!client.connected()) {
    String clientId = "ESP32";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(MQTT_Pump_TOPIC);
      client.subscribe(MQTT_LED_TOPIC);
      MQTTStatus = true;
    } else {
      MQTTStatus = false;
      break;
    }
  }
}
void callback(char* topic, byte *payload, unsigned int length)// Receive Messange From Broker
{
  if(String((char*)payload).indexOf("{") == -1 || String((char*)payload).lastIndexOf("}") == -1) //Filter Message From Server
    return;
  MQTT_Messange = String((char*)payload).substring(String((char*)payload).indexOf("{")+1,String((char*)payload).indexOf("}"));
  String t_ID = MQTT_Messange.substring(0,MQTT_Messange.indexOf("/"));
  String t_command = MQTT_Messange.substring(MQTT_Messange.indexOf(" ")+1,MQTT_Messange.length());
  boolean flag = false;
  if(String(topic) == MQTT_Pump_TOPIC){ 
    if(t_ID == ID)
    {
      if(t_command == "N")
          Command_Pump = ON;
      else if(t_command == "F")
          Command_Pump = OFF;
    }else flag = true;
  }
  if(String(topic) == MQTT_LED_TOPIC){ 
    if(t_ID == ID)
    {
      if(t_command == "N")
          Command_Light = ON;
      else if(t_command == "F")
          Command_Light = OFF;
    }else flag = true;
  }
  if(flag)
  {    
    MQTT_Data.SetDataPackage(t_ID,"",MQTT_Messange.substring(MQTT_Messange.indexOf("/")+1,MQTT_Messange.length()),CommandDirect);
    xQueueSend(Queue_Delivery,&MQTT_Data,pdMS_TO_TICKS(10));
  }
}
#pragma endregion
/*-----------------------------------------Cloud Database SetUp-------------------------------------------*/
#pragma region Cloud Database
void DataLog(void * pvParameters)
{
  UBaseType_t uxHighWaterMark;
  DataPackage data;
  FirebaseJson json_data;
  FirebaseJson json;
  String Root;
  unsigned long time_log = 0; //Time that data was logged
  QueryFilter query;
  unsigned long time_check_before_log = 0; //Make sure that !(time < data_logging_time)
  query.orderBy("$key").limitToLast(1);
  while(true)
  {
    xQueueReceive(Queue_Database,&data,portMAX_DELAY); 
    data.DataToJson(&json_data);
    json_data.set("Status/MQTT",String(MQTTStatus));
    Root = "Realtime/";
    Root += data.GetID();
    Root += "/";
    Firebase.RTDB.updateNodeSilentAsync(&firebaseData, Root, &json_data);
    if(data.GetMode() == LogData)
    {
      Root = "DataLog/";
      Root += data.GetID();
      if(ID != data.GetID())
      {
        if(Firebase.RTDB.getJSON(&firebaseData,Root,&query))
        {
          if(firebaseData.dataType() == "json" && firebaseData.jsonString().length() > 4)
          {
            sscanf(firebaseData.jsonString().substring(2,firebaseData.jsonString().indexOf("\"",12)).c_str(),"%lu",&time_check_before_log);
          }
        }
        else
          time_check_before_log = 0;
      }
      Root += "/";
      do
      {
        time_log = getTime();
        delay(5);
      } while (time_log == 0);
      if((unsigned long)(time_log - time_check_before_log) < (unsigned long)(Three_minutes_millis /1000))
        continue;
      Root += String(time_log);
      Root += "/";
      Firebase.RTDB.setJSONAsync(&firebaseData, Root, &json_data);
    }
    Serial.print("DataLog Task: ");
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    Serial.println(uxHighWaterMark);
    Serial.println(); 
  }
}
void Setup_RTDB()//Initiate Realtime Database Firebase
{
  Firebase.begin(DATABASE_URL,Database_Secrets);
  Firebase.reconnectWiFi(true);
  firebaseData.setResponseSize(4096);
}
#pragma endregion
/*-----------------------------------------WiFi SetUp-----------------------------------------------------*/
#pragma region Network
void Setup_Server()//Initiate connection to the servers
{
  if(!first_sta)
    return;
  configTime(gmtOffset_sec, daylightOffset_sec, NTPserver1,NTPserver2,NTPserver3);
  Setup_RTDB();
  client.setServer(MQTT_SERVER, MQTT_PORT );
  client.setCallback(callback);
  connect_to_broker();
  first_sta = false;
}
void Connect_Network()//Connect to Wifi Router
{
  if(protect.getID(STA)== "")
    return;
  WiFi.begin(protect.getID(STA).c_str(), protect.getPass(STA).c_str());
  long current = millis();
  while (WiFi.status() != WL_CONNECTED && (unsigned long) (millis()- current) < Five_Seconds_millis)
  {
    delay(500);
  }

  if(WiFi.status() != WL_CONNECTED)
  {
    WiFi.mode(WIFI_AP);
    if(Person > 0)
      notifyClients("Wifi OFF");
  }
  else
  {
    gateway_node = GATEWAY_STATUS;
    Reset_ConfigurationLoRa();
    if(Person > 0)
      notifyClients("Wifi ON");
    PingNetwork();
    if(ping_flag)
      Setup_Server();
  }
}
void Reset_Key()
{
  if(before_reset_key == 0)
    return;
  if((unsigned long)(millis() - before_reset_key) > Five_minutes_millis)
  {
    sercurity_backend_key = false;
    tolerance_backend_key = false;
    before_reset_key = 0;
  }
}
void Init_Server()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(ON_STA_FILTER(request)) //Only for client from AP Mode
      return request->redirect("/NothingHereForYou");
    if(!request->authenticate(protect.getID(HTTP).c_str(), protect.getPass(HTTP).c_str()) && !request->authenticate(protect.getID(AUTH).c_str(),protect.getPass(AUTH).c_str()))
      return request->requestAuthentication();
      request->send_P(Received_Code, "text/html", main_html);
  });//Home Page Server
  server.on("/Test",HTTP_GET,[](AsyncWebServerRequest *request){
    return request->send_P(Received_Code,"text/plain",Own_address.c_str());
  });
  server.on("/Sercurity",HTTP_GET,[](AsyncWebServerRequest *request){
    if(ON_STA_FILTER(request) ) //Only for client from AP Mode
      return request->redirect("/NothingHereForYou");
    if(request->authenticate(protect.getID(AUTH).c_str(),protect.getPass(AUTH).c_str()))
    {
      sercurity_backend_key = true;
      before_reset_key = millis();
      return request->send_P(Received_Code,"text/html",Sercurity_html);
    }
    if(request->authenticate(protect.getID(HTTP).c_str(), protect.getPass(HTTP).c_str()))
      return request->send_P(Forbidden_Code,"text/html",Forbidden_html);
    return request->send_P(Not_Found_Code,"text/html",Error_html);

  });
  server.on("/BackEndSercure",HTTP_POST,[](AsyncWebServerRequest *request){},NULL,[](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!sercurity_backend_key || ON_STA_FILTER(request) || !request->authenticate(protect.getID(AUTH).c_str(),protect.getPass(AUTH).c_str()))
      return request->send(Gone_Code);
    sercurity_backend_key = false;
    before_reset_key = 0;
    String Filter = String((char*) data).substring(String((char*) data).indexOf('{')+1,String((char*) data).indexOf('}'));
    String TmpID = Filter.substring(Filter.indexOf(" ")+1,Filter.indexOf('/'));
    String TmpPass = Filter.substring(Filter.indexOf('/')+1,Filter.length());
    /*-----------------------------Address & Channel-------------------------*/
    if(String((char*) data).indexOf("Gateway: ")>=0)
    {
      int tmp0 = 0;
      int tmp1 = 0;
      sscanf(TmpID.c_str(),"%02x%02x",&tmp0,&tmp1);
      Gateway_AddH = tmp0;
      Gateway_AddL = tmp1;
      sscanf(TmpPass.c_str(),"%02x",&Gateway_Channel);
      Gateway_Changed_byUser = true;
      return request->send(No_Content_Code);
    }
    /*--------------------Username & Password---------------------------*/
    if((TmpID.length() > 63 || TmpID == NULL || TmpPass.indexOf(' ') >= 0 || ( TmpPass.length() < 8 && TmpPass != NULL)))
      return request->send(Bad_Request_Code);
    if(String((char*) data).indexOf("Authentication: ")>=0)
    {
      protect.setID(TmpID,HTTP);
      protect.setPass(TmpPass,HTTP);
      return request->send(No_Content_Code);
    }
    if(String((char*) data).indexOf("Authorization: ")>=0)
    {
      protect.setID(TmpID,AUTH);
      protect.setPass(TmpPass,AUTH);
      return request->send(No_Content_Code);
    }
    if(String((char*) data).indexOf("AP: ")>=0)
    {
      protect.setID(TmpID,AP);
      protect.setPass(TmpPass,AP);
      request->send(Network_Authentication_Required);
      Person = 0;
      WiFi.softAP(protect.getID(AP).c_str(),protect.getPass(AP).c_str());
    }
    else
      request->send(No_Response_Code);
  });
  server.on("/BackEndSercure",HTTP_GET,[](AsyncWebServerRequest *request){
    if(!sercurity_backend_key || ON_STA_FILTER(request) || !request->authenticate(protect.getID(AUTH).c_str(),protect.getPass(AUTH).c_str()))
      return request->send(Gone_Code);
      char temp0[2] = { 0 };
      char temp1[1] = {0};
      sprintf(temp0,"%02x%02x",Gateway_AddH,Gateway_AddL);
      MessLimit = String(temp0);
      MessLimit += "/";
      sprintf(temp1,"%02x",Gateway_Channel);
      MessLimit += String(temp1);
      MessLimit += "/";
      MessLimit += protect.getID(AUTH);
      MessLimit += "/";
      MessLimit += protect.getPass(AUTH);
      MessLimit += "/";
      MessLimit += protect.getID(HTTP);
      MessLimit += "/";
      MessLimit += protect.getPass(HTTP);
      MessLimit += "/";
      MessLimit += protect.getID(AP);
      MessLimit += "/";
      MessLimit += protect.getPass(AP);
      MessLimit += "/";
    return request->send_P(Received_Code,"text/plain",MessLimit.c_str());
  });
  server.on("/Tolerance",HTTP_GET,[](AsyncWebServerRequest *request){
    if(ON_STA_FILTER(request)) //Only for client from AP Mode
      return request->redirect("/NothingHereForYou");
    if(request->authenticate(protect.getID(AUTH).c_str(),protect.getPass(AUTH).c_str()))
    {
      tolerance_backend_key = true;
      before_reset_key = millis();
      return request->send_P(Received_Code,"text/html",Tolerance_html);
    }
    if(request->authenticate(protect.getID(HTTP).c_str(),protect.getPass(HTTP).c_str()))
      return request->send_P(Forbidden_Code,"text/html",Forbidden_html);
    return request->send_P(Not_Found_Code,"text/html",Error_html);
  });
  server.on("/BackEndTolerance",HTTP_POST,[](AsyncWebServerRequest *request){},NULL,[](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!tolerance_backend_key || ON_STA_FILTER(request) || !request->authenticate(protect.getID(AUTH).c_str(),protect.getPass(AUTH).c_str()))
      return request->send(Gone_Code);
    String Filter = String((char*) data).substring(String((char*) data).indexOf('{')+1,String((char*) data).indexOf('}'));
    Tree.Danger_Temp = atof(Filter.substring(0,Filter.indexOf('/')).c_str());
    Filter = Filter.substring(Filter.indexOf('/')+1);
    Tree.Save_Temp = atof(Filter.substring(0,Filter.indexOf('/')).c_str());
    Filter = Filter.substring(Filter.indexOf('/')+1);
    Tree.Danger_Humi = atof(Filter.substring(0,Filter.indexOf('/')).c_str());
    Filter = Filter.substring(Filter.indexOf('/')+1);
    Tree.Save_Humi = atof(Filter.substring(0,Filter.indexOf('/')).c_str());
    Filter = Filter.substring(Filter.indexOf('/')+1);
    Tree.WET_SOIL = atof(Filter.substring(0,Filter.indexOf('/')).c_str());
    Filter = Filter.substring(Filter.indexOf('/')+1);
    Tree.DRY_SOIL = atof(Filter.substring(0,Filter.indexOf('/')).c_str());
    Filter = Filter.substring(Filter.indexOf('/')+1);
    Tree.DARK_LIGHT = atof(Filter.c_str());
    request->send(Received_Code);
    tolerance_backend_key = false;
    before_reset_key = 0;
  });
  server.on("/BackEndTolerance",HTTP_GET,[](AsyncWebServerRequest *request){
    if(!request->authenticate(protect.getID(AUTH).c_str(),protect.getPass(AUTH).c_str()) || !tolerance_backend_key)
      return request->send_P(Forbidden_Code,"text/html",Forbidden_html);
    MessLimit = String(Tree.Danger_Temp);
    MessLimit += "/";
    MessLimit += String(Tree.Save_Temp);
    MessLimit += "/";
    MessLimit += String(Tree.Danger_Humi);
    MessLimit += "/";
    MessLimit += String(Tree.Save_Humi);
    MessLimit += "/";
    MessLimit += String(Tree.WET_SOIL);
    MessLimit += "/";
    MessLimit += String(Tree.DRY_SOIL);
    MessLimit += "/";
    MessLimit += String(Tree.DARK_LIGHT);
    MessLimit += "/";
    return request->send_P(Received_Code,"text/plain",MessLimit.c_str());
  });
  server.on("/logout",HTTP_GET,[](AsyncWebServerRequest *request){
    request->send(Unauthorized_Code);
  });
  server.onNotFound([](AsyncWebServerRequest *request){
    if(ON_STA_FILTER(request)) //Only for client from AP Mode
      return request->redirect("https://youtu.be/dQw4w9WgXcQ");
    request->send_P(Not_Found_Code,"text/html",Error_html);
  });
  // Start server
  server.begin();
}
#pragma endregion
/*-----------------------------------------------Message Prepare------------------------------------------*/
#pragma region Send Message
void InitPackage()
{
    O_Pack.SetDataPackage(ID,Own_address,"","");
    MQTT_Data.SetFrom("000017"); //MQTT work only when it's gateway
    Hello_Message.SetDataPackage(ID,"",String(Own_Channel),SayHello);
}
void PrepareMess() //Decide what to send
{
  messanger.clear();
  messanger = Tree.Name;
  messanger += "/";
  messanger += BoolToInt(dataSensor.DHT_Err,dataSensor.LDR_Err,dataSensor.Soil_Err,statusActuator.LightStatus,statusActuator.PumpsStatus);
  messanger += "/";
  if(dataSensor.DHT_Err)
    messanger += String(0);
  else
    messanger += String((int)dataSensor.Humidity);
  messanger += "/";
  if(dataSensor.DHT_Err)
    messanger += String(0);
  else
    messanger += String((int)dataSensor.Temperature);
  messanger += "/";
  if(dataSensor.LDR_Err)
    messanger += String(0);
  else
    messanger += String(dataSensor.lumen);
  messanger += "/";
  if(dataSensor.Soil_Err)
    messanger += String(0);
  else
    messanger += String(dataSensor.soilMoist);
  messanger += "/";
  messanger += String(Tree.Days);
  if(TempData[0] != (int)dataSensor.DHT_Err)
  {
    valueChange_flag = true;
    TempData[0] = (int)dataSensor.DHT_Err;
  }
  if(TempData[1] != (int)dataSensor.LDR_Err )
  {
    valueChange_flag = true;
    TempData[1] = (int)dataSensor.LDR_Err;
  }
  if(TempData[2] != (int)dataSensor.Soil_Err)
  {
    valueChange_flag = true;
    TempData[2] = (int)dataSensor.Soil_Err;
  }
  if(TempData[3] != (int)Tree.Days )
  {
    valueChange_flag = true;
    TempData[3] = (int)Tree.Days;
  }
  if(TempData[4] != (int)dataSensor.Humidity)
  {
    valueChange_flag = true;
    TempData[4] = (int)dataSensor.Humidity;
  }
  if(TempData[5] != (int)dataSensor.Temperature)
  {
    valueChange_flag = true;
    TempData[5] = (int)dataSensor.Temperature;
  }
  if(TempData[6] != (int)dataSensor.lumen)
  {
    valueChange_flag = true;
    TempData[6] = (int)dataSensor.lumen;
  }
  if(TempData[7] != (int)dataSensor.soilMoist)
  {
    valueChange_flag = true;
    TempData[7] = (int)dataSensor.soilMoist;
  }
  if(TempData[8] != (int)statusActuator.LightStatus)
  {
    valueChange_flag = true;
    TempData[8] = (int)statusActuator.LightStatus;
  }
  if(TempData[9] != (int)statusActuator.PumpsStatus)
  {
    valueChange_flag = true;
    TempData[9] = (int)statusActuator.PumpsStatus;
  }
  if(TempData[10] != (int)MQTTStatus )
  {
    valueChange_flag = true;
    TempData[10] = (int)MQTTStatus;
  }
  
}
void SendMess() //Send mess prepared to who
{
  if(Person>0 && valueChange_flag) //Send thourgh WebSocket
    notifyClients(messanger);
  
  if(((unsigned long)(millis()- Last_datalogging_time)>Three_minutes_millis)||Last_datalogging_time == 0)
  {
    if(((!ping_flag || first_sta) && (gateway_node != NODE_STATUS)))
      return;
    if(Firebase.ready() || gateway_node == NODE_STATUS)
    {
      Last_datalogging_time = millis();
      own_wait_time = millis(); //Renew realtime send
      O_Pack.SetDataPackage(ID,"",messanger,LogData);
      switch (gateway_node)
      {
      case GATEWAY_STATUS: // Gateway -> Database
        xQueueSend(Queue_Database,&O_Pack,pdMS_TO_TICKS(10));
        break;
      case NODE_STATUS: //Node -> Gateway
        xQueueSend(Queue_Delivery,&O_Pack,pdMS_TO_TICKS(10));
        break;
      default:
        break;
      }
    }
  }
  else
  {
    if(valueChange_flag)
    {
      O_Pack.SetDataPackage(ID,"",messanger,Default);
      if(!sent_RTDB && gateway_node == NODE_STATUS && ((unsigned long)(millis() - own_wait_time)>Five_Seconds_millis )) //Send to Gateway only if It's a node
      {
        own_wait_time = millis();
        xQueueSend(Queue_Delivery,&O_Pack,pdMS_TO_TICKS(10));
        valueChange_flag = false;
        sent_RTDB = true;
      }
      if(WiFi.status() == WL_CONNECTED && ping_flag && !first_sta && Firebase.ready()) //Send to database
      {
        xQueueSend(Queue_Database,&O_Pack,pdMS_TO_TICKS(10));
        valueChange_flag = false;
      }  
    }
  }
}
void Hello_Around()
{
  if(Wait_to_Hello == 0){
    Wait_to_Hello = millis();
    return;
  }
  if( Friend_Channel != -1 && ((unsigned long)(millis()-Wait_to_Hello)> A_minutes_millis))
  {
    if(Friend_Channel >= 0 && Friend_Channel <= 31){
      if(gateway_node == GATEWAY_STATUS)
        Hello_Message.SetDataPackage("","000017","","");
      else
        Hello_Message.SetDataPackage("",Own_address,"","");
      xQueueSend(Queue_Delivery,&Hello_Message,pdMS_TO_TICKS(10));
    }
    Wait_to_Hello = millis();
  }
  
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
#pragma endregion
/*------------------------------------------Read Environment----------------------------------------------*/
#pragma region Sensor Data Read
int Get_Sensor(int anaPin)// Get Data From Light Sensor & Soild Sensor
{
  int value = 0;
  value = analogRead(anaPin);
  value = map(value,4095,0,0,100);
  return value;
} 
void Check()// Check error sensor
{
  if(isnan(dataSensor.Humidity) || isnan(dataSensor.Temperature) || dataSensor.Humidity > 90 || dataSensor.Humidity < 20|| dataSensor.Temperature > 50 || dataSensor.Temperature < 0 ){
      dataSensor.DHT_Err = true;
    }
  else{
      dataSensor.DHT_Err = false;
    }
  if(dataSensor.lumen < 0 || dataSensor.lumen > 100){
      dataSensor.LDR_Err = true;
    }
  else{
      dataSensor.LDR_Err = false;
    }
  if(dataSensor.soilMoist < 0 || dataSensor.soilMoist > 100){
      dataSensor.Soil_Err = true;
    }
  else{
      dataSensor.Soil_Err = false;
    }
  if(dataSensor.DHT_Err || dataSensor.LDR_Err || dataSensor.Soil_Err)
    Err = true;
  else if(Err)
    Err = false;
}
void Read_Sensor()//Get Data from All Sensors
{
  dataSensor.lumen = Get_Sensor(LDR_Port);
  delay(500);
  dataSensor.soilMoist = Get_Sensor(Soil_Moisture_Port);
  delay(500);
  if(!statusActuator.PumpsStatus)
  {
    dataSensor.Humidity = dht.readHumidity();
    dataSensor.Temperature = dht.readTemperature();
  }

  Check();
}
#pragma endregion
/*--------------------------------------------Controling--------------------------------------------------*/
#pragma region Controlled Things
void Condition_Pump()//Check watering conditions
{
  //Condition & Error || Time until Pump || Current Command
  if((((dataSensor.soilMoist < Tree.DRY_SOIL || dataSensor.Humidity < Tree.Save_Humi ) && !Err ||(unsigned long)(millis()-Times_Pumps) >= Half_Day_millis) || Command_Pump == ON) && !statusActuator.PumpsStatus) //Đang tắt
  {
    Times_Pumps = millis();
    statusActuator.PumpsStatus = true;
    if(Command_Pump == OFF)
      Command_Pump = NOTHING;
    if(Command_Pump == ON)
      return;
  }
  //Condition & Error || Time until Stop || Current Command
  if((((dataSensor.Temperature >= Tree.Danger_Temp || dataSensor.Temperature <= Tree.Save_Temp || dataSensor.Humidity >= Tree.Danger_Humi  || dataSensor.soilMoist > Tree.WET_SOIL) && !Err || ((unsigned long)(millis()- Times_Pumps) >= Still_Pumps)) || Command_Pump == OFF) && statusActuator.PumpsStatus) //Đang bật
  {
    Times_Pumps = millis(); 
    statusActuator.PumpsStatus = false;
    if(Command_Pump == ON)
      Command_Pump = NOTHING;
    if(Command_Pump ==OFF)
      return;
  }
}
void Pump()//Pump Choice
{
  if(Err && Command_Pump == NOTHING)
    statusActuator.PumpsStatus = false;
  else
    Condition_Pump();
  if(statusActuator.PumpsStatus)
    digitalWrite(Pumps_Port,HIGH);
  else 
    digitalWrite(Pumps_Port,LOW);
}
void Condition_Light()//Check lighting up conditions
{
  if((dataSensor.lumen <= Tree.DARK_LIGHT && !Err ||  Command_Light == ON )&& !statusActuator.LightStatus) //Đang tắt
    {
      statusActuator.LightStatus = true;
      if(Command_Light == OFF)
        Command_Light = NOTHING;
      if(Command_Light == ON)
        return;
    }
  if((dataSensor.lumen > Tree.DARK_LIGHT && !Err || Command_Light == OFF) && statusActuator.LightStatus) // Đang bật
    {
      statusActuator.LightStatus = false;
      if(Command_Light == ON)
        Command_Light = NOTHING;
      if(Command_Light ==OFF)
        return;
    }
}
void Light_Up()//Light up choice
{
  if(Err && Command_Light == NOTHING)
    statusActuator.LightStatus = false;
  else
    Condition_Light();
  if(statusActuator.LightStatus)
    digitalWrite(Light_Port,HIGH);
  else 
    digitalWrite(Light_Port,LOW);
}
#pragma endregion
/*----------------------------------------------Main Task-------------------------------------------------*/
#pragma region Main System
void Solve_Command()
{
  if(xQueueReceive(Queue_Command,&O_Command,0) == pdPASS)
  {
    Actuator = O_Command.substring(0,O_Command.indexOf(" "));
    Require = O_Command.substring(O_Command.indexOf(" ")+1,O_Command.length());
    if(Actuator == "L")
    {
      if(Require == "N")
          Command_Light = ON;
      else if(Require == "F")
          Command_Light = OFF;
      return;
    }
    if(Actuator == "P")
    {
      if(Require == "N")
          Command_Pump = ON;
      else if(Require == "F")
          Command_Pump = OFF;
      return;
    }
  }
  O_Command.clear();
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
    DataLog,
    "DataLog",
    8000,//1964B left
    NULL,
    0,
    &DatabaseTask
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
void Network()// Netword Part
{
  ws.cleanupClients();
  PrepareMess();
  SendMess();
  Hello_Around();
  if(sta_flag)
  {
    WiFi.mode(WIFI_AP_STA);
    Connect_Network();
    sta_flag = false;
  } 
  Reset_Key(); 
  if(WiFi.status() != WL_CONNECTED){
    if(Last_ping_time != 0) Last_ping_time = 0;
    if(ping_flag) ping_flag = false;
    return;
  }
  Cycle_Ping();
  if(!ping_flag)
    return;
  Setup_Server();// If it doesn't setup
  client.loop();
  if (!client.connected()) {
    connect_to_broker();
  }
}
void Auto()//Auto Part
{
  Make_Day();
  Solve_Command();
  Read_Sensor();
  Check();
  Pump();
  Light_Up();
}
#pragma endregion
/*------------------------------------Arduino Platform----------------------------------------------------*/
#pragma region Arduino program structure
void setup() 
{
  Serial.begin(9600);
  Serial.println();
  Init_LoRa();
  Init_Task();
  initWebSocket();
  pinMode(Pumps_Port,OUTPUT);
  pinMode(Light_Port,OUTPUT);
  digitalWrite(Pumps_Port,LOW);
  digitalWrite(Light_Port,LOW);
  dht.begin();
  Time_Passed = millis();
  WiFi.mode(WIFI_AP_STA);
  Init_Server();
  protect.AppendIDAP(ID);
  WiFi.softAP(protect.getID(AP).c_str(), protect.getPass(AP).c_str());
  InitPackage();
  Connect_Network();
}
void loop() 
{
  Auto();
  Network();
}
#pragma endregion