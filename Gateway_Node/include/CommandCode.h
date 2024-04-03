/*----------------------------HTTP Code-----------------------------------*/
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

/*----------------------------ACK-----------------------------------------*/
#define ACK "0" // Send ACK
#define PrepareACK "8"
/*-----------------------Node -> Gateway----------------------------------*/
#define Default "1" //Node -> Gateway
#define LogData "4" //Log to Database
/*-------------------Gateway -> Node--------------------------------------*/
#define CommandDirect "2" // Handle command & Broadcast
#define CommandNotDirect "3" //Not Direct Command
/*---------------------None Command Send----------------------------------*/
#define Memorize "5" // Remember ID & From
/*-------------------Detect Node Around Command---------------------------*/
#define SayHello "6" // Send Hello to a channel
#define SayHi "7" // Response Hello received
