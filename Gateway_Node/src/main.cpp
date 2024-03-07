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
#include "URL.h"
#include "html.h"
#include "CommandCode.h"
//GPIO_PORT
#define DHTPIN_Port 21 //Read DHT22 Sensor
#define LDR_Port 34 //Read Light Sensor
#define Soil_Moisture_Port 35 //Read Soild Sensor
#define Pumps_Port 22 // Control Pump
#define Light_Port 23 //Control Light
//DHT11 Variable
#define DHTTYPE DHT11 
DHT dht(DHTPIN_Port, DHTTYPE);
float Humidity = 0; 
float Temperature = 0;
//Light Sensor Variable
int lumen = 0; //Store value from LDR
//Soild Sensor Variable
int soilMoist = 0; //Store value from Moisure
//Sensor Check
boolean Soil_Err = false;
boolean Err = false;//Catch Error Sensor
boolean DHT_Err = false;
boolean LDR_Err = false;
//Plant
Plant Tree;
//Days
unsigned long Time_Passed = 0;
const unsigned long A_Day_milis = 86400000;//24 hours
const unsigned long A_Day_timestamp = 86400;//24 hours
//Pump
unsigned long Times_Pumps=0;
const unsigned long Next_Pump = 43200000; //12 hours
unsigned long Still_Pumps = 30000; //Water in 1 minute
boolean PumpsStatus = false; //Current Status Pump
//Light
boolean LightStatus = false; //Current Status Light
//Bits -> int
int ConvertToInt = 0; //DHT_Err LDR_Err Soil_Err LightStatus PumpsStatus
//WIFI Variable
String sta_ssid = ""; 
String sta_password = "" ;
String ap_ssid = "ESP_";
String ap_password = "123456789";
const unsigned long Network_TimeOut = 5000;// Wait 5 seconds to Connect Wifi
//LoRa Variable
LoRa_E32 lora(&Serial2,4,5,18); //16-->TX 17-->RX 4-->AUX 5-->M1 18-->M0 
volatile boolean lora_flag = false;
uint8_t Own_AddH;
uint8_t Own_AddL;
uint8_t Own_Channel;
String Own_address = "";
volatile uint8_t Gateway_AddH = 0;
volatile uint8_t Gateway_AddL = 0;
volatile uint8_t Gateway_Channel = 0x17;
//Ping
WiFiClient PingClient;
const unsigned long time_delay_to_ping = 300000; // 5 minutes/ping
unsigned long Last_ping_time = 0;
boolean ping_flag = false; //Result Ping
//NTP
unsigned long timestamp;
const long  gmtOffset_sec = 7 * 60 * 60; // UTC +7
const int daylightOffset_sec = 0; //Daylight saving time
//Firebase Variable
FirebaseData firebaseData;
const unsigned long time_delay_send_datalogging = 180000; //3 minutes/Send
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
int Command_Pump = 0; // 0: Nothing, 1:ON, 2:OFF
int Command_Light = 0; // 0: Nothing, 1:ON, 2:OFF
//flag
boolean sta_flag = false;
boolean first_sta = true;
boolean valueChange_flag = false;
boolean contingency_flag = false;
//Type of server
volatile int gateway_node = 0; // 0:default 1: gateway 2:node
//Own & Deliver
DataPackage O_Pack;
String O_Command;
String D_Command;
DataPackage MQTT_Data;
const unsigned long own_delay_send = 5000; // 5 seconds/send
unsigned long own_wait_time = 0;
volatile boolean sent_RTDB = false;
//Task Delivery Data
TaskHandle_t DeliveryTask = NULL;
TaskHandle_t DatabaseTask = NULL;
TaskHandle_t CaptureTask = NULL;
QueueHandle_t Queue_Delivery = NULL;
QueueHandle_t Queue_Command = NULL;
QueueHandle_t Queue_Database = NULL;
const int Queue_Length = 10;
const unsigned long long Queue_item_delivery_size = sizeof(DataPackage);
const unsigned long long Queue_item_command_size = sizeof(String);
const unsigned long long Queue_item_database_size = sizeof(DataPackage);
//Sercurity
String http_username = "admin";
String http_password = "admin";;
String auth_username = "owner";
String auth_password = "owner";
boolean sercurity_backend_key = false;
boolean tolerance_backend_key = false;
const unsigned long reset_key_time = 300000; // 5 minutes to reset
unsigned long before_reset_key = 0;
//ID
const String ID = WiFi.macAddress();
//Slove Command
String Actuator = "";
String Require = "";
//Loop variable
int i;
// Store Recent Value
int Temp[11] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
#pragma region Common Function
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
  if((unsigned long) (millis()-Time_Passed) >= A_Day_milis){
    Time_Passed = millis();
    Tree.Days ++;
  }
  if(Tree.Days > 999)
    Tree.Days = 0;
}
#pragma endregion
#pragma region Check Internet Connected from Wifi
void PingNetwork()// Ping to host
{
  if(gateway_node == 1){ //If it's gateway, check Internet
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
  if((unsigned long)(millis()- Last_ping_time) > time_delay_to_ping){
    Last_ping_time = millis();
    PingNetwork();
  }
}
#pragma endregion
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
  UBaseType_t uxHighWaterMark;
  uint8_t DeliveryH;
  uint8_t DeliveryL;
  uint8_t DeliveryChan;
  Remember Locate;
  while(true)
  {
    xQueueReceive(Queue_Delivery,&data,portMAX_DELAY);
    /*----------------------Memorize---------------------------*/
    if(data.GetMode() == Memorize)
    {
      Locate.Add(data.GetID(),data.GetFrom());
      continue;
    }
    /*-----------------Check Expired---------------------------*/  
    if(data.expired == 0)
    {
      if(data.GetMode() == CommandDirect)
      {
        data.NotDirect = Locate.GetAddrress(data.GetID());
        if(data.NotDirect == "")
          continue; //TODO: Solution for ID not found
        data.ResetExpired();
        data.SetMode(CommandNotDirect); //Direct -> Not Direct
      }
      if(data.GetMode() == CommandNotDirect)
        continue;
      if(data.GetMode() == LogData)
      {
        Gateway_AddH = 0;
        Gateway_AddL = 0;
        Gateway_Channel = 0x17;
        continue;
      }
    };
    /*--------------------------------------------------------*/
    if(data.GetMode() == Default || data.GetMode() == LogData) //Send to Gateway
    {
      if(data.GetMode() == Default && data.GetID() == ID)
        sent_RTDB = false;
      lora.sendFixedMessage(Gateway_AddH,Gateway_AddL,Gateway_Channel,data.toString());
      data.expired--;
      if(data.GetMode() == LogData)
        xQueueSendToBack(Queue_Delivery,&data,pdMS_TO_TICKS(10));
    }
    else //Send to Node
    {
      if(data.GetMode() == ACK)
        DeCodeAddressChannel(data.GetID(),DeliveryH,DeliveryL,DeliveryChan);
      else if(data.GetMode() == CommandNotDirect)
      {
        DeCodeAddressChannel(data.NotDirect,DeliveryH,DeliveryL,DeliveryChan);
      }else
      {
        CalculateAddressChannel(data.GetID(),DeliveryH,DeliveryL,DeliveryChan);
      }
      lora.sendFixedMessage(DeliveryH,DeliveryL,DeliveryChan,data.toString());

      data.expired--;
      if(data.GetMode() == CommandDirect || data.GetMode() == CommandNotDirect)
        xQueueSendToBack(Queue_Delivery,&data,pdMS_TO_TICKS(10));
    }
    Serial.print("Delivery Task: ");
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    Serial.println(uxHighWaterMark);
    Serial.println();
    delay(3000);
  }
}
void Capture(void * pvParameters)
{
  ResponseContainer mess;
  UBaseType_t uxHighWaterMark;
  DataPackage ResponseACK;
  DataPackage tempData;
  String TempAddress = "";
  DataPackage Receive_Pack;
  DataPackage Memory_Pack;
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
      Serial.print(ID);
      Serial.println(" receive:");
      Serial.println(Receive_Pack.toString(true));
      /*------------------------Response------------------------*/
      if(Receive_Pack.GetMode() == ACK) //Receive ACK
      {
        Serial.println("Receive ACK");
        for(int i = 0; i<Queue_Length;i++) //Find the data for that ACK
        {
          if(xQueueReceive(Queue_Delivery,&tempData,0) == pdPASS)
          {
            if(Receive_Pack.GetID() == tempData.GetFrom() && Receive_Pack.GetData() == tempData.GetMode())
            {
              if(Receive_Pack.GetData() == LogData)
                TempAddress = EnCodeAddressChannel(Gateway_AddH,Gateway_AddL,Gateway_Channel);
              else
                TempAddress = CalculateToEncode(tempData.GetID());
              if(Receive_Pack.GetFrom() == TempAddress)
              {
                break;
              }
            }
            xQueueSend(Queue_Delivery,&tempData,0);
          }
          else break; //Empty Queue
        }
        continue;//No mess fit ack
      }
      if(Receive_Pack.GetFrom() != CalculateToEncode(Receive_Pack.GetID())) //Ignore Send From Direct
      {
        Memory_Pack.SetDataPackage(Receive_Pack.GetID(),Receive_Pack.GetFrom(),"","");
        xQueueSendToFront(Queue_Delivery,&Memory_Pack,pdMS_TO_TICKS(10));
      }
      if(Receive_Pack.GetMode() == CommandDirect || Receive_Pack.GetMode() == CommandNotDirect || Receive_Pack.GetMode() == LogData) //Send ACK
      {
        if(gateway_node == 1)
          ResponseACK.SetDataPackage(Receive_Pack.GetFrom(),"000017",Receive_Pack.GetMode(),"");
        if(gateway_node == 2)
          ResponseACK.SetDataPackage(Receive_Pack.GetFrom(),Own_Adrress,Receive_Pack.GetMode(),"");
        Serial.println("Prepare to Send ACK");
        xQueueSendToFront(Queue_Delivery,&ResponseACK,pdMS_TO_TICKS(10));
      }
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
          if(gateway_node == 0)
            continue;
          if(gateway_node == 1)// If it's a gateway -> Send to Database
          {
            xQueueSend(Queue_Database,&Receive_Pack,pdMS_TO_TICKS(10));
            continue;
          }
          if(gateway_node ==2)//If it's a node -> Delivery to Gateway
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
  O_Pack.SetFrom(Own_address);
  MQTT_Data.SetFrom("000017"); //MQTT work only when it's gateway
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
    if(gateway_node == 0)
      gateway_node = 2;
  }
  else
  {
    lora_flag = false;
  }
  c.close();
}
#pragma endregion LoRa
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
  Convert |= DHT_Err <<4;
  Convert |= LDR_Err <<3;
  Convert |= Soil_Err <<2;
  Convert |= LightStatus <<1;
  Convert |= PumpsStatus <<0;
  data += String(Convert);
  data += "/";
  if(DHT_Err)
    data += String(0);
  else
    data += String((int)Humidity);
  data += "/";
  if(DHT_Err)
    data += String(0);
  else
    data += String((int)Temperature);
  data += "/";
  if(LDR_Err)
    data += String(0);
  else
    data += String(lumen);
  data += "/";
  if(Soil_Err)
    data += String(0);
  else  
    data += String(soilMoist);
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
      sta_ssid = String((char*)data).substring(String((char*)data).indexOf(' ')+1,String((char*)data).length());
    }
    if(String((char*)data).indexOf("Password:") >= 0){
      sta_password = String((char*)data).substring(String((char*)data).indexOf(' ')+1,String((char*)data).length());
      sta_flag = true;
    }
    if(String((char*)data).indexOf("NameTree:") >= 0){
      Tree.Name = String((char*)data).substring(String((char*)data).indexOf(' ')+1,String((char*)data).length());
    }
    if(String((char*)data).indexOf("Disconnect") >= 0){
      WiFi.disconnect(true,true);
      WiFi.mode(WIFI_AP);
      gateway_node = 2;
      ping_flag = false;
      notifyClients("Wifi OFF");
      Reset_ConfigurationLoRa(false);
    }
    if(String((char*)data).indexOf("Pump") >= 0){
      if(PumpsStatus)
        Command_Pump = 2;
      else
        Command_Pump = 1;
    }
    if(String((char*)data).indexOf("LED") >= 0){
      if(LightStatus)
        Command_Light = 2;
      else
        Command_Light = 1;
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
    MQTT_Messange = String((char*)payload).substring(String((char*)payload).indexOf("{")+1,String((char*)payload).indexOf("}"));
    String t_ID = MQTT_Messange.substring(0,MQTT_Messange.indexOf("/"));
    String t_command = MQTT_Messange.substring(MQTT_Messange.indexOf(" ")+1,MQTT_Messange.length());
    boolean flag = false;
    if(String(topic) == MQTT_Pump_TOPIC){ 
      if(t_ID == ID)
      {
        if(t_command == "N")
            Command_Pump = 1;
        else if(t_command == "F")
            Command_Pump = 2;
      }else flag = true;
    }
    if(String(topic) == MQTT_LED_TOPIC){ 
      if(t_ID == ID)
      {
        if(t_command == "N")
            Command_Light = 1;
        else if(t_command == "F")
            Command_Light = 2;
      }else flag = true;
    }
    if(flag)
    {    
      MQTT_Data.SetDataPackage(t_ID,"",MQTT_Messange.substring(MQTT_Messange.indexOf("/")+1,MQTT_Messange.length()),CommandDirect);
      xQueueSend(Queue_Delivery,&MQTT_Data,pdMS_TO_TICKS(10));
    }
}
#pragma endregion
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
      if((unsigned long)(time_log - time_check_before_log) < (unsigned long)(time_delay_send_datalogging /1000))
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
  if(sta_ssid == "")
    return;
  WiFi.begin(sta_ssid.c_str(), sta_password.c_str());
  long current = millis();
  while (WiFi.status() != WL_CONNECTED && (unsigned long) (millis()- current) < Network_TimeOut)
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
    gateway_node = 1;
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
  if((unsigned long)(millis() - before_reset_key) > reset_key_time)
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
    if(!request->authenticate(http_username.c_str(), http_password.c_str()) && !request->authenticate(auth_username.c_str(),auth_password.c_str()))
      return request->requestAuthentication();
      request->send_P(Received_Code, "text/html", main_html);
  });//Home Page Server
  server.on("/Test",HTTP_GET,[](AsyncWebServerRequest *request){
    return request->send_P(Received_Code,"text/plain",Own_address.c_str());
  });
  server.on("/Sercurity",HTTP_GET,[](AsyncWebServerRequest *request){
    if(ON_STA_FILTER(request) ) //Only for client from AP Mode
      return request->redirect("/NothingHereForYou");
    if(request->authenticate(auth_username.c_str(), auth_password.c_str()))
    {
      sercurity_backend_key = true;
      before_reset_key = millis();
      return request->send_P(Received_Code,"text/html",Sercurity_html);
    }
    if(request->authenticate(http_username.c_str(),http_password.c_str()))
      return request->send_P(Forbidden_Code,"text/html",Forbidden_html);
    return request->send_P(Not_Found_Code,"text/html",Error_html);

  });
  server.on("/BackEndSercure",HTTP_POST,[](AsyncWebServerRequest *request){},NULL,[](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!sercurity_backend_key || ON_STA_FILTER(request) || !request->authenticate(auth_username.c_str(), auth_password.c_str()))
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
      return request->send(No_Content_Code);
    }
    /*--------------------Username & Password---------------------------*/
    if((TmpID.length() > 63 || TmpID == NULL || TmpPass.indexOf(' ') >= 0 || ( TmpPass.length() < 8 && TmpPass != NULL)))
      return request->send(Bad_Request_Code);
    if(String((char*) data).indexOf("Authentication: ")>=0)
    {
      http_username = TmpID;
      http_password = TmpPass;
      return request->send(No_Content_Code);
    }
    if(String((char*) data).indexOf("Authorization: ")>=0)
    {
      auth_username = TmpID;
      auth_password = TmpPass;
      return request->send(No_Content_Code);
    }
    if(String((char*) data).indexOf("AP: ")>=0)
    {
      ap_ssid = TmpID;
      ap_password = TmpPass;
      request->send(Network_Authentication_Required);
      Person = 0;
      WiFi.softAP(ap_ssid.c_str(),ap_password.c_str());
    }
    else
      request->send(No_Response_Code);
  });
  server.on("/BackEndSercure",HTTP_GET,[](AsyncWebServerRequest *request){
    if(!sercurity_backend_key || ON_STA_FILTER(request) || !request->authenticate(auth_username.c_str(), auth_password.c_str()))
      return request->send(Gone_Code);
      char temp0[2] = { 0 };
      char temp1[1] = {0};
      sprintf(temp0,"%02x%02x",Gateway_AddH,Gateway_AddL);
      MessLimit = String(temp0);
      MessLimit += "/";
      sprintf(temp1,"%02x",Gateway_Channel);
      MessLimit += String(temp1);
      MessLimit += "/";
      MessLimit += auth_username;
      MessLimit += "/";
      MessLimit += auth_password;
      MessLimit += "/";
      MessLimit += http_username;
      MessLimit += "/";
      MessLimit += http_password;
      MessLimit += "/";
      MessLimit += ap_ssid;
      MessLimit += "/";
      MessLimit += ap_password;
      MessLimit += "/";
    return request->send_P(Received_Code,"text/plain",MessLimit.c_str());
  });
  server.on("/Tolerance",HTTP_GET,[](AsyncWebServerRequest *request){
    if(ON_STA_FILTER(request)) //Only for client from AP Mode
      return request->redirect("/NothingHereForYou");
    if(request->authenticate(auth_username.c_str(), auth_password.c_str()))
    {
      tolerance_backend_key = true;
      before_reset_key = millis();
      return request->send_P(Received_Code,"text/html",Tolerance_html);
    }
    if(request->authenticate(http_username.c_str(),http_password.c_str()))
      return request->send_P(Forbidden_Code,"text/html",Forbidden_html);
    return request->send_P(Not_Found_Code,"text/html",Error_html);
  });
  server.on("/BackEndTolerance",HTTP_POST,[](AsyncWebServerRequest *request){},NULL,[](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!tolerance_backend_key || ON_STA_FILTER(request) || !request->authenticate(auth_username.c_str(), auth_password.c_str()))
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
    if(!request->authenticate(auth_username.c_str(), auth_password.c_str()) || !tolerance_backend_key)
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
#pragma region Send Message
void PrepareMess() //Decide what to send
{
  messanger.clear();
  messanger = Tree.Name;
  messanger += "/";
  ConvertToInt = 0;
  ConvertToInt |= DHT_Err <<4;
  ConvertToInt |= LDR_Err <<3;
  ConvertToInt |= Soil_Err <<2;
  ConvertToInt |= LightStatus <<1;
  ConvertToInt |= PumpsStatus <<0;
  messanger += String(ConvertToInt);
  messanger += "/";
  if(DHT_Err)
    messanger += String(0);
  else
    messanger += String((int)Humidity);
  messanger += "/";
  if(DHT_Err)
    messanger += String(0);
  else
    messanger += String((int)Temperature);
  messanger += "/";
  if(LDR_Err)
    messanger += String(0);
  else
    messanger += String(lumen);
  messanger += "/";
  if(Soil_Err)
    messanger += String(0);
  else
    messanger += String(soilMoist);
  messanger += "/";
  messanger += String(Tree.Days);
  if(Temp[0] != (int)DHT_Err)
  {
    valueChange_flag = true;
    Temp[0] = (int)DHT_Err;
  }
  if(Temp[1] != (int)LDR_Err )
  {
    valueChange_flag = true;
    Temp[1] = (int)LDR_Err;
  }
  if(Temp[2] != (int)Soil_Err)
  {
    valueChange_flag = true;
    Temp[2] = (int)Soil_Err;
  }
  if(Temp[3] != (int)Tree.Days )
  {
    valueChange_flag = true;
    Temp[3] = (int)Tree.Days;
  }
  if(Temp[4] != (int)Humidity)
  {
    valueChange_flag = true;
    Temp[4] = (int)Humidity;
  }
  if(Temp[5] != (int)Temperature)
  {
    valueChange_flag = true;
    Temp[5] = (int)Temperature;
  }
  if(Temp[6] != (int)lumen)
  {
    valueChange_flag = true;
    Temp[6] = (int)lumen;
  }
  if(Temp[7] != (int)soilMoist)
  {
    valueChange_flag = true;
    Temp[7] = (int)soilMoist;
  }
  if(Temp[8] != (int)LightStatus)
  {
    valueChange_flag = true;
    Temp[8] = (int)LightStatus;
  }
  if(Temp[9] != (int)PumpsStatus)
  {
    valueChange_flag = true;
    Temp[9] = (int)PumpsStatus;
  }
  if(Temp[10] != (int)MQTTStatus )
  {
    valueChange_flag = true;
    Temp[10] = (int)MQTTStatus;
  }
  
}
void SendMess() //Send mess prepared to who
{
  if(Person>0 && valueChange_flag) //Send thourgh WebSocket
    notifyClients(messanger);
  
  if(((unsigned long)(millis()- Last_datalogging_time)>time_delay_send_datalogging)||Last_datalogging_time == 0)
  {
    if(((!ping_flag || first_sta) && (gateway_node != 2)))
      return;
    if(Firebase.ready() || gateway_node == 2)
    {
      Last_datalogging_time = millis();
      own_wait_time = millis(); //Renew realtime send
      O_Pack.SetDataPackage(ID,"",messanger,LogData);
      switch (gateway_node)
      {
      case 1: // Gateway -> Database
        xQueueSend(Queue_Database,&O_Pack,pdMS_TO_TICKS(10));
        break;
      case 2: //Node -> Gateway
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
      if(!sent_RTDB && gateway_node == 2 && ((unsigned long)(millis() - own_wait_time)>own_delay_send )) //Send to Gateway only if It's a node
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
#pragma endregion Send Message
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
  if(isnan(Humidity) || isnan(Temperature) || Humidity > 90 || Humidity < 20|| Temperature > 50 || Temperature < 0 ){
      DHT_Err = true;
    }
  else{
      DHT_Err = false;
    }
  if(lumen < 0 || lumen > 100){
      LDR_Err = true;
    }
  else{
      LDR_Err = false;
    }
  if(soilMoist < 0 || soilMoist > 100){
      Soil_Err = true;
    }
  else{
      Soil_Err = false;
    }
  if(DHT_Err || LDR_Err || Soil_Err)
    Err = true;
  else if(Err)
    Err = false;
}
void Read_Sensor()//Get Data from All Sensors
{
  lumen = Get_Sensor(LDR_Port);
  delay(500);
  soilMoist = Get_Sensor(Soil_Moisture_Port);
  delay(500);
  if(!PumpsStatus)
  {
    Humidity = dht.readHumidity();
    Temperature = dht.readTemperature();
  }

  Check();
}
#pragma endregion
#pragma region Controlled Things
void Condition_Pump()//Check watering conditions
{
  if(((soilMoist < Tree.DRY_SOIL && !Err ||(unsigned long)(millis()-Times_Pumps) >= Next_Pump) || Command_Pump == 1) && !PumpsStatus) //Đang tắt
  {
    Times_Pumps = millis();
    PumpsStatus = true;
    if(Command_Pump == 2)
      Command_Pump = 0;
    if(Command_Pump == 1)
      return;
  }
  if((((Temperature >= Tree.Danger_Temp || Temperature <= Tree.Save_Temp || Humidity >= Tree.Danger_Humi  || soilMoist > Tree.WET_SOIL) && !Err || ((unsigned long)(millis()- Times_Pumps) >= Still_Pumps)) || Command_Pump == 2) && PumpsStatus) //Đang bật
  {
    Times_Pumps = millis(); 
    PumpsStatus = false;
    if(Command_Pump == 1)
      Command_Pump = 0;
    if(Command_Pump ==2)
      return;
  }
}
void Pump()//Pump Choice
{
  if(Err && Command_Pump == 0)
    PumpsStatus = false;
  else
    Condition_Pump();
  if(PumpsStatus)
    digitalWrite(Pumps_Port,HIGH);
  else 
    digitalWrite(Pumps_Port,LOW);
}
void Condition_Light()//Check lighting up conditions
{
  if((lumen <= Tree.DARK_LIGHT && !Err ||  Command_Light == 1 )&& !LightStatus) //Đang tắt
    {
      LightStatus = true;
      if(Command_Light == 2)
        Command_Light = 0;
      if(Command_Light ==1)
        return;
    }
  if((lumen > Tree.DARK_LIGHT && !Err || Command_Light == 2) && LightStatus) // Đang bật
    {
      LightStatus = false;
      if(Command_Light == 1)
        Command_Light = 0;
      if(Command_Light ==2)
        return;
    }
}
void Light_Up()//Light up choice
{
  if(Err && Command_Light == 0)
    LightStatus = false;
  else
    Condition_Light();
  if(LightStatus)
    digitalWrite(Light_Port,HIGH);
  else 
    digitalWrite(Light_Port,LOW);
}
#pragma endregion
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
          Command_Light = 1;
      else if(Require == "F")
          Command_Light = 2;
      return;
    }
    if(Actuator == "P")
    {
      if(Require == "N")
          Command_Pump = 1;
      else if(Require == "F")
          Command_Pump = 2;
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
  xTaskCreate(
    Delivery,
    "Delivery",
    5000, //3634B left
    NULL,
    0,
    &DeliveryTask
  );
  xTaskCreate(
    DataLog,
    "DataLog",
    8000,//1972B left
    NULL,
    0,
    &DatabaseTask
  );
  xTaskCreate(
    Capture,
    "Capture",
    5000, //?B left
    (void*)&Own_address,
    0,
    &CaptureTask
  );
}
void Network()// Netword Part
{
  ws.cleanupClients();
  PrepareMess();
  SendMess();
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
  ap_ssid += ID;
  WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());
  Connect_Network();
}
void loop() 
{
  Auto();
  Network();
}
#pragma endregion
