//HTTP Response Code
#define Continue_Code 100
#define Received_Code 200
#define Init_Gateway_Code 202
#define No_Content_Code 204
#define Bad_Request_Code 400
#define Unauthorized_Code 401
#define Forbidden_Code 403
#define Not_Found_Code 404
#define Gone_Code 410
#define No_Response_Code 444
#define Network_Authentication_Required 511
//Mode
#define ACK "0" // Send ACK
#define Default "1" //Node -> Gateway
#define Broadcast "2" // Send both except from sender
#define CommandDirect "3" // Handle command & Broadcast
#define CommandNotDirect "4" //Not Direct Command
#define LogData "5" //Log to Database
#define Memorize "6" // Remember ID & From
#define SayHello "7" // Send Hello to a channel
#define Response "8" // Response Hello received
