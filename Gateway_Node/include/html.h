#include <stdlib.h>
#include <pgmspace.h>

#pragma region main_default_html
const char main_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html> 
<head>
  <title>ESP Web Server</title>
  <style>
    *{
      padding: 0;
      margin: 0;
      box-sizing: border-box;
    }
    body{
      overflow: hidden;
      text-align: center;
      height: 100vh;
      width: 100vw;
      background: linear-gradient(to right, #12c2e9, #c471ed, #f64f59);
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-direction: column;
      align-content: center;
      flex-wrap: wrap;
    }
    button{
      background-color: azure;
      border: none;
      outline: none;
      border-radius: 100px;
      font-weight: bolder;
      transition-duration: 0.3s;
      transition-property: color,background-color ;
      -webkit-box-shadow: 0px 11px 22px -9px #000000; 
      box-shadow: 0px 11px 22px -9px #000000;
      cursor: pointer;
      margin: 5px 25px;
    }
    button:hover{
      color: whitesmoke;
      background-color: black;
    }
    #overlay{
      position: fixed;
      z-index: 1;
      width: 100vw;
      height: 100vh;
      top: 0;
      left: 0;
      background: transparent;
      backdrop-filter: blur(5px);
    }
    #Wifi{
      background-color: transparent;
      border: 2px solid rgba(255, 255, 255, 0.5);
      backdrop-filter: blur(15px);
      width: 900px;
      border-radius: 25px;
      display: none;
      justify-content: center;
      align-items: center;
      flex-direction: column;
      padding: 20px;
      position: fixed;
      transform: scale(0.8);
      transition: 0.5s;
      font-size: xx-large;
      z-index: 3;
      top: 25%;
    }
    #Wifi button{
      width: 440px;
      padding: 12px 0 12px 0;
      font-size: x-large;
    }
    #menu{
      background-color: transparent;
      border: 2px solid rgba(255, 255, 255, 0.5);
      display: flex;
      justify-content:space-around;
      width: 100vw;
      border-radius: 50px;
      z-index: 2;
    }
    #menu button{
      width: 200px;
      padding: 7px 0 7px 0;
    }
    #Router{
      display: flex;
      align-items: center;
      justify-content: center;
    }
    #Action button{
      margin-bottom: 20px;
      border: 0;
      display: block;
      width: 300px;
      padding: 15px 0 15px 0;
    }
    #status{
      width: 10px;
      height: 10px;
      background-color: gray;
      border-radius: 50%;
      margin-left: 5px;
    }
    .hide{
        display: none;
    }
    .outer{
      width: 160px;
      height: 160px;
      border-radius: 50%;
      box-shadow: rgba(0, 0, 0, 0.16) 0px 3px 6px, rgba(0, 0, 0, 0.23) 0px 3px 6px;
      padding: 20px;
    }
    .inner{
      height: 120px;
      width: 120px;
      display: flex;
      align-items: center;
      justify-content: center;
      border-radius: 50%;
      box-shadow: rgba(50, 50, 93, 0.25) 0px 30px 60px -12px inset, rgba(0, 0, 0, 0.3) 0px 18px 36px -18px inset;
    }
    .number{
      display: flex;
      align-items: center;
      justify-content: center;
      font-weight: bolder;
      color: #555;
    }
    circle{
      fill: none;
      stroke: url(#GradientColor);
      stroke-width: 20px;
      stroke-dasharray: 472; /*Exact circle*/
      stroke-dashoffset: 200;
      transition: 0.5s;
    }
    svg{
      margin-top: 22px;
      position: absolute;
      top: 0;
      left: 0;
    }
    .container_gauge{
      display: flex;
      justify-content: space-around;
      width: 50%;
    }
    .Sensor_Value{
      display: flex;
      height: 200px;
      width: 100vw;
      justify-content: space-around;
      flex-wrap: wrap;
    }
    .gauge{
      width: 160px;
      height: 160px;
      position: relative;
      margin: 0 10px 100px 10px;
    }
    .gauge h3{
      text-align: center;
    }
    #Bool{
      margin: 200px 0 0 25px;
      display: flex;
      justify-content: space-between;
      width: 100vw;
      height: 300px;
      align-items: center;
      flex-wrap: wrap;
    }
    #NameTree{
      font-size: xx-large;
      font-weight: bolder;
      text-align: center; 
    }
    .Error_Sensor{
      display: flex;
    }
    input, .input_box input{
      border: none;
      outline: none;
      background: transparent;
    }
    .input_box{
      position: relative;
      width: 800px;
      margin: 40px 0;
      border-bottom: 2px solid #fff;
    }
    .input_box label{
      position: absolute;
      top: 50%;
      left: 5px;
      margin-left: 5px;
      transform: translateY(-50%);
      transition: 0.5s;
      font-weight: bolder;
      color: azure;
      pointer-events: none;
    }
    .input_box input:focus~label, .input_box input:valid~label{
      top: -15px; 
    }
    .input_box input{
      font-size: 1em;
      width: 100%;
      height: 50px;
      color: aliceblue;
      padding: 0 35px 0 5px;
    }
    .loading{
      width: 80px;
      height: 80px;
      position: relative;
    }
    .first_circle,.last_circle{
      border-radius: 50%;
    }
    .first_circle{
      width: 100%;
      height: 100%;
      border: 10px solid rgba(255, 255, 255, 0.5);
      border-left: 5px solid transparent;
      border-right: 5px solid transparent;
      animation: spinner 1.5s infinite linear;
    }
    .last_circle{
      width: 75%;
      height: 75%;
      position: absolute;
      top: 12.5%;
      left: 12.5%;
      border: 7px solid rgba(255, 255, 255, 0.25);
      border-top: 2.5px solid transparent;
      border-bottom: 2.5px solid transparent;
      animation: spinner 1.5s infinite linear reverse;
    }
    @keyframes spinner{
      from{
        transform: rotate(-360deg);
      }
      to{
        transform: rotate(360deg);
      }
    }
    @media(max-width: 62em)
    {
      body{
        height: 100vw;
      }
      .container_gauge{
        flex-direction: column;
        align-items: center;
      }
    }
  </style>
</head>
<body>
  <div id="overlay" class="hide"></div>
  <div id="menu">
    <button id="Router">Gateway <div id="status"></div></button>
    <button id = "Bound">Bound</button>
    <button id = "Security">Security</button>
    <button id ="LogOut">Log Out</button>
  </div>
  <div>
    <input type="text" id="NameTree" value="No Name" maxlength="4" name="NameTree">
  </div>
<div class="Sensor_Value">
  <div class="container_gauge">
    <!--Độ ẩm không khí-->
    <div class="gauge">
      <h3>Humidity</h3>
      <div class="outer">
        <div class="inner">
          <div id="humi" class="number">
            %HUMIDITY%
          </div>
        </div>
      </div>
      <svg width="160px" height="160px">
        <defs>
          <linearGradient id="GradientColor">
              <stop offset="0%" stop-color="#e91e63" stop-opacity="1"/>
              <stop offset="50%" stop-color="#f503e7" stop-opacity="1"/>
              <stop offset="100%" stop-color="#673ab7" stop-opacity="1"/>
          </linearGradient>
        </defs>
        <circle id="c_humi" cx="80" cy="80" r="70" stroke-linecap="round">
      </svg>
    </div>
    <!--Độ ẩm đất-->
    <div class="gauge">
      <h3>Moisture</h3>
      <div class="outer">
        <div class="inner">
          <div class="number" id="mois">
            %MOISTURE%
          </div>
        </div>
      </div>
      <svg width="160px" height="160px">
        <circle id="c_mois" cx="80" cy="80" r="70" stroke-linecap="round">
      </svg>
    </div>
  </div>
  <div class="container_gauge">
    <!--Nhiệt độ-->
    <div class="gauge">
      <h3>Temperature</h3>
      <div class="outer">
        <div class="inner">
          <div class="number" id="temp">
            %TEMPERATURE%
          </div>
        </div>
      </div>
      <svg width="160px" height="160px">
        <circle id="c_temp" cx="80" cy="80" r="70" stroke-linecap="round">
      </svg>
    </div>
    <!--ÁNh sáng-->
    <div class="gauge">
      <h3>Light</h3>
      <div class="outer">
        <div class="inner">
          <div class="number" id="ligh">
            %LIGHT%
          </div>
        </div>
      </div>
      <svg width="160px" height="160px">
        <circle id="c_ligh" cx="80" cy="80" r="70" stroke-linecap="round">
      </svg>
    </div>
  </div>
</div>
<div id="Bool">
  <div id="All_Status">
    <h2>Status Sensor</h2>
    <div class="Error_Sensor">
      <h3>DHT11: </h3><h3 id="DHT11_Status">%dht11_S%</h3>
    </div>
    <div class="Error_Sensor">
      <h3>Soil Moisture: </h3><h3 id="Mois_Status">%mois_S%</h3>
    </div>
    <div class="Error_Sensor">
      <h3>Light Sensor: </h3><h3 id="Ligh_Status">%Ligh_S%</h3>
    </div>
  </div>
  <div id="Action">
    <h2>Command</h2>
    <button id="Pump">Pump: %Pump%</button>
    <button id="Led">Led: %LED%</button>
  </div>
</div>
<div id="Wifi" class="show"> <!--Login Wifi--> 
  <h1>GATEWAY SERVER</h1>
  <h3 id = "Info">%INFO%</h3>
  <div id="wait" class="loading hide">
    <div class="first_circle"></div>
    <div class="last_circle"></div>
  </div>
  <form  id = "LogIn" autocomplete="off">
    <div class="input_box">
      <input type="text" id="ssid" name="ssid" required/><br>
      <label for="ssid">Name Wifi: </label>
    </div>
    <div class="input_box">
      <input type="password" id="password" name="password" required/><br>
      <label for="password">Password: </label>
    </div>
  </form>    
  <button id="Submit">Connect</button>
  <button id = "Disconnect" class="hide">Disconnect</button>
</div>  
  <script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    
    document.getElementById("LogOut").addEventListener("click",()=>{
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/logout", true);
      xhr.send();
      setTimeout(function(){ window.open("/","_self"); }, 1000);
    })
    document.getElementById("Router").addEventListener("click",()=>{
      document.getElementById("Wifi").style.display = "flex";
      document.getElementById("overlay").classList.remove("hide");
    })
    document.getElementById("overlay").addEventListener("click",OverLay)
    document.getElementById("Bound").addEventListener("click",()=>{
      window.open(location.href+"Tolerance","_self","")
    })
    document.getElementById("Security").addEventListener("click",()=>{
      window.open(location.href+"Sercurity","_self","")
    })
    document.getElementById("NameTree").addEventListener("change",(e)=>{
      websocket.send("NameTree: " + document.getElementById("NameTree").value);
    })
    window.addEventListener('load', onLoad);
    function initWebSocket() {
      console.log('Trying to open a WebSocket connection...');
      websocket = new WebSocket(gateway);
      websocket.onopen    = onOpen;
      websocket.onclose   = onClose;
      websocket.onmessage = onMessage; // <-- add this line
    }
    function onOpen(event) {
      console.log('Connection opened');
    }
    function onClose(event) {
      console.log('Connection closed');
      document.getElementById('status').style.backgroundColor = "gray";
      TheEnd()
    }
    function onMessage(event){
      var pre_data = String(event.data).substring(String(event.data).indexOf("{")+1,String(event.data).indexOf("}"))
      if(pre_data.length > 15)
      {
        document.getElementById("NameTree").defaultValue = String(pre_data).split("/")[0];
        if((Number(String(pre_data).split("/")[1])>>4)&1 == 1)
          document.getElementById("DHT11_Status").innerHTML = "Error";
        else
          document.getElementById("DHT11_Status").innerHTML = "Ok";
        if((Number(String(pre_data).split("/")[1])>>3)&1 == 1)
          document.getElementById("Ligh_Status").innerHTML = "Error";
        else
          document.getElementById("Ligh_Status").innerHTML = "Ok";
        if((Number(String(pre_data).split("/")[1])>>2)&1 == 1)
          document.getElementById("Mois_Status").innerHTML = "Error";
        else
          document.getElementById("Mois_Status").innerHTML = "Ok";
        if((Number(String(pre_data).split("/")[1])>>1)&1 == 1)
          document.getElementById("Led").innerHTML = "Led: ON";
        else
          document.getElementById("Led").innerHTML = "Led: OFF";
        if((Number(String(pre_data).split("/")[1])>>0)&1 == 1)
          document.getElementById("Pump").innerHTML = "Pump: ON";
        else
          document.getElementById("Pump").innerHTML = "Pump: OFF";
        Gauge2("humi","c_humi",Number(String(pre_data).split("/")[2]));
        Gauge2("temp","c_temp",Number(String(pre_data).split("/")[3]));
        Gauge1("ligh","c_ligh",Number(String(pre_data).split("/")[4]));
        Gauge1("mois","c_mois",Number(String(pre_data).split("/")[5]));
        if(String(pre_data).split("/")[6].indexOf("Wifi") >= 0)
        {
          if(String(pre_data).split("/")[6].indexOf("OFF") >= 0)
          {
            document.getElementById("Submit").classList.remove("hide");
            document.getElementById('LogIn').classList.remove("hide");
            document.getElementById("wait").classList.add("hide");
            document.getElementById('Disconnect').classList.add("hide");
            document.getElementById('Info').innerHTML = "Status: Not connected";
            document.getElementById('status').style.backgroundColor = "red";
          }
          if(String(pre_data).split("/")[6].indexOf("ON") >= 0 || String(pre_data).split("/")[6].indexOf("CONNECT") >= 0)
          {
            document.getElementById("Submit").classList.add("hide");
            document.getElementById('LogIn').classList.add("hide");
            document.getElementById('Info').innerHTML = "Status: Connected";
            document.getElementById('Disconnect').classList.remove("hide");
            document.getElementById("wait").classList.add("hide");
            if(String(pre_data).split("/")[6].indexOf("ON") >= 0)
              document.getElementById('status').style.backgroundColor = "orange";
            else
              document.getElementById('status').style.backgroundColor = "green";
          }
        }
      }else{
        if(String(pre_data).indexOf("Wifi") >= 0)
        {
          if(String(pre_data).indexOf("OFF") >= 0)
          {
            document.getElementById("Submit").classList.remove("hide");
            document.getElementById('LogIn').classList.remove("hide");
            document.getElementById("wait").classList.add("hide");
            document.getElementById('Disconnect').classList.add("hide");
            document.getElementById('Info').innerHTML = "Status: Not connected";
            document.getElementById('status').style.backgroundColor = "red";
          }
          if(String(pre_data).indexOf("ON") >= 0 || String(pre_data).indexOf("CONNECT") >= 0)
          {
            document.getElementById("Submit").classList.add("hide");
            document.getElementById('LogIn').classList.add("hide");
            document.getElementById('Info').innerHTML = "Status: Connected";
            document.getElementById('Disconnect').classList.remove("hide");
            document.getElementById("wait").classList.add("hide");
            if(String(pre_data).indexOf("ON") >= 0)
              document.getElementById('status').style.backgroundColor = "orange";
            else
              document.getElementById('status').style.backgroundColor = "green";
          }
        }
      }

    }
    function onLoad(event) {
      initWebSocket();
      initButton();
    }
    function initButton() {
      document.getElementById("Submit").addEventListener("click",Log);
      document.getElementById("Disconnect").addEventListener("click",Dis);
      document.getElementById("Pump").addEventListener("click",ActiveP);
      document.getElementById("Led").addEventListener("click",ActiveL);
    }
    function ActiveL(){
      websocket.send("LED");
    }
    function ActiveP(){
      websocket.send("Pump");
    }
    function Dis(){
        websocket.send("Disconnect");
    }
    function Log(){
      websocket.send("Username: " + document.getElementById('ssid').value);
      websocket.send("Password: " + document.getElementById('password').value);
      document.getElementById('ssid').value = "";
      document.getElementById('password').value = "";
      document.getElementById("wait").classList.remove("hide");
      document.getElementById("Submit").classList.add("hide");
      document.getElementById('LogIn').classList.add("hide");
      document.getElementById('status').style.backgroundColor = "yellow";
      document.getElementById('Info').innerHTML = "Waiting....";
    }
    function Gauge1(name1,name2,value){
      document.getElementById(name1).innerHTML = value+ "%";
      var temp = Math.floor(504 * (1 - (value/100)));
      document.getElementById(name2).style.strokeDashoffset = temp;
    }
    function Gauge2(name1,name2,value){
      document.getElementById(name1).innerHTML = value;
      var temp = Math.floor(504 * (1 - (value/100)));
      document.getElementById(name2).style.strokeDashoffset = temp;
    }
    function OverLay()
    {
        document.getElementById("Wifi").style.display = "none";
        document.getElementById("overlay").classList.add("hide");
    }
    function TheEnd()
    {
      document.getElementById("overlay").classList.remove("hide");
      document.getElementById("overlay").style.zIndex = 3;
      document.getElementById("overlay").removeEventListener("click",OverLay);
      document.getElementById("Wifi").style.display = "flex";
      document.getElementById("Wifi").style.zIndex = 4;
      document.getElementById("Wifi").innerHTML = "<b>Lost Connection to Server<b>";
    }

    /*----------------------Detect DevTool------------------------------------*/
    function OnDevToolIsOpen(){
        document.querySelector('html').textContent = ''
    }
    class DevToolChecker extends Error{
        toString(){}
        get message(){OnDevToolIsOpen();}
    }
    console.log(new DevToolChecker())
  </script>
</body>
</html>
)rawliteral";
#pragma endregion main_default_html
#pragma region 404_Error
const char Error_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>404 NOT FOUND</title>
    <style>
@charset "UTF-8";
* {
  margin: 0;
  padding: 0;
  -webkit-text-size-adjust: none;
}
html, body {
  height: 100%;
  overflow: hidden;
}
body {
  padding: 0;
  margin: 0;
  background: #181828;
  font-size: 14px;
  line-height: 1;
}
label {
  cursor: pointer;
}
a {
  margin: 0;
  padding: 0;
  vertical-align: baseline;
  background: transparent;
  text-decoration: none;
  color: #000;
}
input, select, button, textarea {
  margin: 0;
  font-size: 100%;
}
html, div, span, applet, object, iframe, h1, h2, h3, h4, h5, h6, p, blockquote, pre,
a, abbr, acronym, address, big, cite, code, del, dfn, em, font, img, ins, kbd, q, s, samp,
small, strike, strong, sub, sup, tt, var, b, u, i, center, dl, dt, dd, ol, ul, li,
fieldset, form, label, legend, table, caption, tbody, tfoot, thead, tr, th, td, input {
  border: 0;
  outline: 0;
  font-size: 100%;
  vertical-align: baseline;
  background: transparent;
}
.top-header:before {
    background-image: url(https://1.bp.blogspot.com/-gxsOcYWghHA/Xp_izTh4sFI/AAAAAAAAU8s/y637Fwg99qAuzW9na_NT_uApny8Vce95gCEwYBhgL/s1600/header-footer-gradient-bg.png);
}
.top-header:before {
    content: '';
    display: block;
    width: 100%;
    height: 4px;
    background-repeat: repeat-x;
    background-size: contain;
    position: absolute;
    top: 0;
    left: 0;
  opacity:0.5;
}
.starsec{
  content: " ";
    position: absolute;
    width: 3px;
    height: 3px;
    background: transparent;
   box-shadow: 571px 173px #00BCD4, 1732px 143px #00BCD4, 1745px 454px #FF5722, 234px 784px #00BCD4, 1793px 1123px #FF9800, 1076px 504px #03A9F4, 633px 601px #FF5722, 350px 630px #FFEB3B, 1164px 782px #00BCD4, 76px 690px #3F51B5, 1825px 701px #CDDC39, 1646px 578px #FFEB3B, 544px 293px #2196F3, 445px 1061px #673AB7, 928px 47px #00BCD4, 168px 1410px #8BC34A, 777px 782px #9C27B0, 1235px 1941px #9C27B0, 104px 1690px #8BC34A, 1167px 1338px #E91E63, 345px 1652px #009688, 1682px 1196px #F44336, 1995px 494px #8BC34A, 428px 798px #FF5722, 340px 1623px #F44336, 605px 349px #9C27B0, 1339px 1344px #673AB7, 1102px 1745px #3F51B5, 1592px 1676px #2196F3, 419px 1024px #FF9800, 630px 1033px #4CAF50, 1995px 1644px #00BCD4, 1092px 712px #9C27B0, 1355px 606px #F44336, 622px 1881px #CDDC39, 1481px 621px #9E9E9E, 19px 1348px #8BC34A, 864px 1780px #E91E63, 442px 1136px #2196F3, 67px 712px #FF5722, 89px 1406px #F44336, 275px 321px #009688, 592px 630px #E91E63, 1012px 1690px #9C27B0, 1749px 23px #673AB7, 94px 1542px #FFEB3B, 1201px 1657px #3F51B5, 1505px 692px #2196F3, 1799px 601px #03A9F4, 656px 811px #00BCD4, 701px 597px #00BCD4, 1202px 46px #FF5722, 890px 569px #FF5722, 1613px 813px #2196F3, 223px 252px #FF9800, 983px 1093px #F44336, 726px 1029px #FFC107, 1764px 778px #CDDC39, 622px 1643px #F44336, 174px 1559px #673AB7, 212px 517px #00BCD4, 340px 505px #FFF, 1700px 39px #FFF, 1768px 516px #F44336, 849px 391px #FF9800, 228px 1824px #FFF, 1119px 1680px #FFC107, 812px 1480px #3F51B5, 1438px 1585px #CDDC39, 137px 1397px #FFF, 1080px 456px #673AB7, 1208px 1437px #03A9F4, 857px 281px #F44336, 1254px 1306px #CDDC39, 987px 990px #4CAF50, 1655px 911px #00BCD4, 1102px 1216px #FF5722, 1807px 1044px #FFF, 660px 435px #03A9F4, 299px 678px #4CAF50, 1193px 115px #FF9800, 918px 290px #CDDC39, 1447px 1422px #FFEB3B, 91px 1273px #9C27B0, 108px 223px #FFEB3B, 146px 754px #00BCD4, 461px 1446px #FF5722, 1004px 391px #673AB7, 1529px 516px #F44336, 1206px 845px #CDDC39, 347px 583px #009688, 1102px 1332px #F44336, 709px 1756px #00BCD4, 1972px 248px #FFF, 1669px 1344px #FF5722, 1132px 406px #F44336, 320px 1076px #CDDC39, 126px 943px #FFEB3B, 263px 604px #FF5722, 1546px 692px #F44336;
  animation: animStar 150s linear infinite;
}
.starthird
{
  content: " ";
    position: absolute;
    width: 3px;
    height: 3px;
    background: transparent;
   box-shadow: 571px 173px #00BCD4, 1732px 143px #00BCD4, 1745px 454px #FF5722, 234px 784px #00BCD4, 1793px 1123px #FF9800, 1076px 504px #03A9F4, 633px 601px #FF5722, 350px 630px #FFEB3B, 1164px 782px #00BCD4, 76px 690px #3F51B5, 1825px 701px #CDDC39, 1646px 578px #FFEB3B, 544px 293px #2196F3, 445px 1061px #673AB7, 928px 47px #00BCD4, 168px 1410px #8BC34A, 777px 782px #9C27B0, 1235px 1941px #9C27B0, 104px 1690px #8BC34A, 1167px 1338px #E91E63, 345px 1652px #009688, 1682px 1196px #F44336, 1995px 494px #8BC34A, 428px 798px #FF5722, 340px 1623px #F44336, 605px 349px #9C27B0, 1339px 1344px #673AB7, 1102px 1745px #3F51B5, 1592px 1676px #2196F3, 419px 1024px #FF9800, 630px 1033px #4CAF50, 1995px 1644px #00BCD4, 1092px 712px #9C27B0, 1355px 606px #F44336, 622px 1881px #CDDC39, 1481px 621px #9E9E9E, 19px 1348px #8BC34A, 864px 1780px #E91E63, 442px 1136px #2196F3, 67px 712px #FF5722, 89px 1406px #F44336, 275px 321px #009688, 592px 630px #E91E63, 1012px 1690px #9C27B0, 1749px 23px #673AB7, 94px 1542px #FFEB3B, 1201px 1657px #3F51B5, 1505px 692px #2196F3, 1799px 601px #03A9F4, 656px 811px #00BCD4, 701px 597px #00BCD4, 1202px 46px #FF5722, 890px 569px #FF5722, 1613px 813px #2196F3, 223px 252px #FF9800, 983px 1093px #F44336, 726px 1029px #FFC107, 1764px 778px #CDDC39, 622px 1643px #F44336, 174px 1559px #673AB7, 212px 517px #00BCD4, 340px 505px #FFF, 1700px 39px #FFF, 1768px 516px #F44336, 849px 391px #FF9800, 228px 1824px #FFF, 1119px 1680px #FFC107, 812px 1480px #3F51B5, 1438px 1585px #CDDC39, 137px 1397px #FFF, 1080px 456px #673AB7, 1208px 1437px #03A9F4, 857px 281px #F44336, 1254px 1306px #CDDC39, 987px 990px #4CAF50, 1655px 911px #00BCD4, 1102px 1216px #FF5722, 1807px 1044px #FFF, 660px 435px #03A9F4, 299px 678px #4CAF50, 1193px 115px #FF9800, 918px 290px #CDDC39, 1447px 1422px #FFEB3B, 91px 1273px #9C27B0, 108px 223px #FFEB3B, 146px 754px #00BCD4, 461px 1446px #FF5722, 1004px 391px #673AB7, 1529px 516px #F44336, 1206px 845px #CDDC39, 347px 583px #009688, 1102px 1332px #F44336, 709px 1756px #00BCD4, 1972px 248px #FFF, 1669px 1344px #FF5722, 1132px 406px #F44336, 320px 1076px #CDDC39, 126px 943px #FFEB3B, 263px 604px #FF5722, 1546px 692px #F44336;
  animation: animStar 10s linear infinite;
}
.starfourth
{
  content: " ";
    position: absolute;
    width: 2px;
    height: 2px;
    background: transparent;
   box-shadow: 571px 173px #00BCD4, 1732px 143px #00BCD4, 1745px 454px #FF5722, 234px 784px #00BCD4, 1793px 1123px #FF9800, 1076px 504px #03A9F4, 633px 601px #FF5722, 350px 630px #FFEB3B, 1164px 782px #00BCD4, 76px 690px #3F51B5, 1825px 701px #CDDC39, 1646px 578px #FFEB3B, 544px 293px #2196F3, 445px 1061px #673AB7, 928px 47px #00BCD4, 168px 1410px #8BC34A, 777px 782px #9C27B0, 1235px 1941px #9C27B0, 104px 1690px #8BC34A, 1167px 1338px #E91E63, 345px 1652px #009688, 1682px 1196px #F44336, 1995px 494px #8BC34A, 428px 798px #FF5722, 340px 1623px #F44336, 605px 349px #9C27B0, 1339px 1344px #673AB7, 1102px 1745px #3F51B5, 1592px 1676px #2196F3, 419px 1024px #FF9800, 630px 1033px #4CAF50, 1995px 1644px #00BCD4, 1092px 712px #9C27B0, 1355px 606px #F44336, 622px 1881px #CDDC39, 1481px 621px #9E9E9E, 19px 1348px #8BC34A, 864px 1780px #E91E63, 442px 1136px #2196F3, 67px 712px #FF5722, 89px 1406px #F44336, 275px 321px #009688, 592px 630px #E91E63, 1012px 1690px #9C27B0, 1749px 23px #673AB7, 94px 1542px #FFEB3B, 1201px 1657px #3F51B5, 1505px 692px #2196F3, 1799px 601px #03A9F4, 656px 811px #00BCD4, 701px 597px #00BCD4, 1202px 46px #FF5722, 890px 569px #FF5722, 1613px 813px #2196F3, 223px 252px #FF9800, 983px 1093px #F44336, 726px 1029px #FFC107, 1764px 778px #CDDC39, 622px 1643px #F44336, 174px 1559px #673AB7, 212px 517px #00BCD4, 340px 505px #FFF, 1700px 39px #FFF, 1768px 516px #F44336, 849px 391px #FF9800, 228px 1824px #FFF, 1119px 1680px #FFC107, 812px 1480px #3F51B5, 1438px 1585px #CDDC39, 137px 1397px #FFF, 1080px 456px #673AB7, 1208px 1437px #03A9F4, 857px 281px #F44336, 1254px 1306px #CDDC39, 987px 990px #4CAF50, 1655px 911px #00BCD4, 1102px 1216px #FF5722, 1807px 1044px #FFF, 660px 435px #03A9F4, 299px 678px #4CAF50, 1193px 115px #FF9800, 918px 290px #CDDC39, 1447px 1422px #FFEB3B, 91px 1273px #9C27B0, 108px 223px #FFEB3B, 146px 754px #00BCD4, 461px 1446px #FF5722, 1004px 391px #673AB7, 1529px 516px #F44336, 1206px 845px #CDDC39, 347px 583px #009688, 1102px 1332px #F44336, 709px 1756px #00BCD4, 1972px 248px #FFF, 1669px 1344px #FF5722, 1132px 406px #F44336, 320px 1076px #CDDC39, 126px 943px #FFEB3B, 263px 604px #FF5722, 1546px 692px #F44336;
  animation: animStar 50s linear infinite;
}
.starfifth
{
  content: " ";
    position: absolute;
    width: 1px;
    height: 1px;
    background: transparent;
   box-shadow: 571px 173px #00BCD4, 1732px 143px #00BCD4, 1745px 454px #FF5722, 234px 784px #00BCD4, 1793px 1123px #FF9800, 1076px 504px #03A9F4, 633px 601px #FF5722, 350px 630px #FFEB3B, 1164px 782px #00BCD4, 76px 690px #3F51B5, 1825px 701px #CDDC39, 1646px 578px #FFEB3B, 544px 293px #2196F3, 445px 1061px #673AB7, 928px 47px #00BCD4, 168px 1410px #8BC34A, 777px 782px #9C27B0, 1235px 1941px #9C27B0, 104px 1690px #8BC34A, 1167px 1338px #E91E63, 345px 1652px #009688, 1682px 1196px #F44336, 1995px 494px #8BC34A, 428px 798px #FF5722, 340px 1623px #F44336, 605px 349px #9C27B0, 1339px 1344px #673AB7, 1102px 1745px #3F51B5, 1592px 1676px #2196F3, 419px 1024px #FF9800, 630px 1033px #4CAF50, 1995px 1644px #00BCD4, 1092px 712px #9C27B0, 1355px 606px #F44336, 622px 1881px #CDDC39, 1481px 621px #9E9E9E, 19px 1348px #8BC34A, 864px 1780px #E91E63, 442px 1136px #2196F3, 67px 712px #FF5722, 89px 1406px #F44336, 275px 321px #009688, 592px 630px #E91E63, 1012px 1690px #9C27B0, 1749px 23px #673AB7, 94px 1542px #FFEB3B, 1201px 1657px #3F51B5, 1505px 692px #2196F3, 1799px 601px #03A9F4, 656px 811px #00BCD4, 701px 597px #00BCD4, 1202px 46px #FF5722, 890px 569px #FF5722, 1613px 813px #2196F3, 223px 252px #FF9800, 983px 1093px #F44336, 726px 1029px #FFC107, 1764px 778px #CDDC39, 622px 1643px #F44336, 174px 1559px #673AB7, 212px 517px #00BCD4, 340px 505px #FFF, 1700px 39px #FFF, 1768px 516px #F44336, 849px 391px #FF9800, 228px 1824px #FFF, 1119px 1680px #FFC107, 812px 1480px #3F51B5, 1438px 1585px #CDDC39, 137px 1397px #FFF, 1080px 456px #673AB7, 1208px 1437px #03A9F4, 857px 281px #F44336, 1254px 1306px #CDDC39, 987px 990px #4CAF50, 1655px 911px #00BCD4, 1102px 1216px #FF5722, 1807px 1044px #FFF, 660px 435px #03A9F4, 299px 678px #4CAF50, 1193px 115px #FF9800, 918px 290px #CDDC39, 1447px 1422px #FFEB3B, 91px 1273px #9C27B0, 108px 223px #FFEB3B, 146px 754px #00BCD4, 461px 1446px #FF5722, 1004px 391px #673AB7, 1529px 516px #F44336, 1206px 845px #CDDC39, 347px 583px #009688, 1102px 1332px #F44336, 709px 1756px #00BCD4, 1972px 248px #FFF, 1669px 1344px #FF5722, 1132px 406px #F44336, 320px 1076px #CDDC39, 126px 943px #FFEB3B, 263px 604px #FF5722, 1546px 692px #F44336;
  animation: animStar 80s linear infinite;
}
@keyframes animStar
{
  0% {
    transform: translateY(0px);
}
  100% {
    transform: translateY(-2000px);
}
}
button {
  border: none;
  padding: 0;
  font-size: 0;
  line-height: 0;
  background: none;
  cursor: pointer;
}
:focus {
  outline: 0;
}
.clearfix:before, .clearfix:after {
  content: "\0020";
  display: block;
  height: 0;
  visibility: hidden;
}
.clearfix:after {
  clear: both;
}
.clearfix {
  zoom: 1;
}
.error {
  min-height: 100vh;
  position: relative;
  padding: 240px 0;
  box-sizing: border-box;
  width: 100%;
  height: 100%;
  text-align: center;
  margin-top: 70px;
}
.error__overlay {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  overflow: hidden;
}
.error__content {
  position: absolute;
  top: 50%;
  left: 50%;
  width: 100%;
  -webkit-transform: translate(-50%, -50%);
          transform: translate(-50%, -50%);
}
.error__message {
  text-align: center;
  color: #181828;
}
.message__title {
  font-family: 'Montserrat', sans-serif;
  font-weight: 900;
  text-transform: uppercase;
  letter-spacing: 5px;
   font-size: 5.6rem;
  padding-bottom: 40px;
  max-width: 960px;
  margin: 0 auto;
}
.message__text {
  font-family: 'Montserrat', sans-serif;
  line-height: 42px;
  font-size: 18px;
  padding: 0 60px;
  max-width: 680px;
  margin: auto;
}
.error__nav {
  max-width: 600px;
  margin: 40px auto 0;
  text-align: center;
}
.e-nav__form {
  position: relative;
  height: 45px;
  overflow: hidden;
  width: 170px;
  display: inline-block;
  vertical-align: top;
  border: 1px solid #212121;
  padding-left: 10px;
  padding-right: 46px;
}
.e-nav__icon {
  position: absolute;
  right: 15px;
  top: 50%;
  -webkit-transform: translateY(-50%);
          transform: translateY(-50%);
  color: #212121;
  -webkit-transition: color .25s ease;
  transition: color .25s ease;
}
.e-nav__link {
  height: 45px;
  line-height: 45px;
  width: 170px;
  display: inline-block;
  vertical-align: top;
  margin: 0 15px;
  border: 1px solid #181828;
  color: #181828;
  text-decoration: none;
  font-family: 'Montserrat', sans-serif;
  text-transform: uppercase;
  font-size: 11px;
  letter-spacing: .1rem;
  position: relative;
  overflow: hidden;
}
.e-nav__link:before {
  content: '';
  height: 200px;
  background: #212121;
  position: absolute;
  top: 70px;
  right: 70px;
  width: 260px;
  -webkit-transition: all .3s;
  transition: all .3s;
  -webkit-transform: rotate(50deg);
          transform: rotate(50deg);
}
.e-nav__link:after {
  -webkit-transition: all .3s;
  transition: all .3s;
  z-index: 999;
  position: relative;
}
.e-nav__link:after {
  content: "Home Page";
}
.e-nav__link:hover:before {
  top: -60px;
  right: -50px;
}
.e-nav__link:hover {
  color: #fff;
}
.e-nav__link:nth-child(2):hover:after {
  color: #fff;
}
.error__social {
  position: absolute;
  top: 50%;
  -webkit-transform: translateY(-50%);
          transform: translateY(-50%);
  left: 20px;
  z-index: 10;
}
.e-social__list {
  margin: 0;
  padding: 0;
  list-style-type: none;
}
.e-social__icon {
  padding-bottom: 30px;
}
.e-social__icon:last-child {
  padding-bottom: 0;
}
.e-social__link {
  color: #fff;
  -webkit-transition: all .25s ease;
  transition: all .25s ease;
  display: block;
}
.e-social__link:hover {
  opacity: .7;
}
.lamp {
  position: absolute;
  left: 0px;
  right: 0px;
  top: 0px;
  margin: 0px auto;
  width: 300px;
  display: flex;
  flex-direction: column;
  align-items: center;
  transform-origin: center top;
  animation-timing-function: cubic-bezier(0.6, 0, 0.38, 1);
  animation: move 5.1s infinite;
}
@keyframes move {
  0% {
    transform: rotate(40deg);
  }
  50% {
    transform: rotate(-40deg);
  }
  100% {
    transform: rotate(40deg);
  }
}
.cable {
  width: 8px;
    height: 248px;
    background-image: linear-gradient(rgb(32 148 218 / 70%), rgb(193 65 25)), linear-gradient(rgba(0,0,0,0.7), rgba(0,0,0,0.7)), linear-gradient(rgba(0,0,0,0.7), rgba(0,0,0,0.7));
}
.cover {
  width: 200px;
  height: 80px;
  background: #0bd5e8;
  border-top-left-radius: 50%;
  border-top-right-radius: 50%;
  position: relative;
  z-index: 200;
}
.in-cover {
  width: 100%;
  max-width: 200px;
  height: 20px;
  border-radius: 100%;
  background: #08ffff;
  position: absolute;
  left: 0px;
  right: 0px;
  margin: 0px auto;
  bottom: -9px;
  z-index: 100;
}
.in-cover .bulb {
     width: 50px;
    height: 50px;
    background-color: #08fffa;
    border-radius: 50%;
    position: absolute;
    left: 0px;
    right: 0px;
    bottom: -20px;
    margin: 0px auto;
    -webkit-box-shadow: 0 0 15px 7px rgba(0,255,255,0.8), 0 0 40px 25px rgba(0,255,255,0.5), -75px 0 30px 15px rgba(0,255,255,0.2);
    box-shadow: 0 0 25px 7px rgb(127 255 255 / 80%), 0 0 64px 47px rgba(0,255,255,0.5), 0px 0 30px 15px rgba(0,255,255,0.2);
}
.light {
      width: 200px;
    height: 0px;
    border-bottom: 900px solid rgb(44 255 255 / 24%);
    border-left: 50px solid transparent;
    border-right: 50px solid transparent;
    position: absolute;
    left: 0px;
    right: 0px;
    top: 270px;
    margin: 0px auto;
    z-index: 1;
    border-radius: 90px 90px 0px 0px;
}
.error {
  overflow: hidden;
  max-height: 100vh;
}
@media (max-width: 1400px) { 
  .lamp {
    zoom: .5;
  }
  .error__content {
    top: 55%;
  }
  .message__title {
    font-size: 3.5rem;
  }
}
@media (max-width: 900px) {
  .message__title {
    font-size: 34px;
  }
  .error__content {
    top: 55%;
  }
  }
@media (max-width: 950px) {
  .lamp__wrap {
    max-height: 100vh;
    overflow: hidden;
    max-width: 100vw;
  }
  .error__social {
    bottom: 30px;
    top: auto;
    transform: none;
    width: 100%;
    position: fixed;
    left: 0;
  }
  .e-social__icon {
    display: inline-block;
    padding-right: 30px;
  }
  .e-social__icon:last-child {
    padding-right: 0;
  }
  .e-social__icon {
    padding-bottom: 0;
  }
}
@media (max-width: 750px) {
  body, html {
    max-height: 100vh;
  }
   .error__content {
    position: static;
    margin: 0 auto;
    transform: none;
    padding-top: 300px;
  }
  .error {
    padding-top: 0;
    padding-bottom: 100px;
    height: 100vh;
  }
  }
@media (max-width: 650px) {
  .message__title {
    font-size: 36px;
    padding-bottom: 20px;
  }
  .message__text {
    font-size: 16px;
    line-height: 2;
    padding-right: 20px;
    padding-left: 20px;
  }
  .lamp {
    zoom: .6;
  }
  .error__content {
    padding-top: 180px;
  }
  }
@media (max-width: 480px) {
  .message__title {
    font-size: 30px;
  }
  .message__text {
    padding-left: 10px;
    padding-right: 10px;
    font-size: 15px;
  }
  .error__nav {
    margin-top: 20px;
  }
}
    </style>
</head>
<body>
    <header class="top-header">
      </header>
      <!--dust particel-->
      <div>
        <div class="starsec"></div>
        <div class="starthird"></div>
        <div class="starfourth"></div>
        <div class="starfifth"></div>
      </div>
      <!--Dust particle end--->
      <div class="lamp__wrap">
        <div class="lamp">
          <div class="cable"></div>
          <div class="cover"></div>
          <div class="in-cover">
            <div class="bulb"></div>
          </div>
          <div class="light"></div>
        </div>
      </div>
      <!-- END Lamp -->
      <section class="error">
        <!-- Content -->
        <div class="error__content">
          <div class="error__message message">
            <h1 class="message__title">Page Not Found</h1>
            <p class="message__text">Oops! We seem to have lost that page. Our bad!</p>
          </div>
        </div>
        <!-- END Content -->
      </section>
</body>
</html>
)rawliteral";
#pragma endregion 404_Error
#pragma region Sercurity_html
const char Sercurity_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Sercurity</title>
  <style>
      *{
        padding: 0;
        margin: 0;
        box-sizing: border-box;
      }
      body{
        height: 100vh;
        width: 100vw;
        background-image: linear-gradient(to right top, #41ff53, #00eeb6, #00d5ff, #00b3ff, #0088ff, #076fff, #474fff, #7500f5, #9500ee, #ad0ce7, #c11de1, #d22ddb);
        background-size: 300% 100%;
        background-position: 75% 50%;
        overflow: hidden;
        display: flex;
        justify-content: flex-start;
        align-items: center;
        flex-direction: column;
        flex-wrap: wrap;
        align-content: flex-start;
        transition: 0.5s;
      }
      form{
        width: 90%;
        margin-bottom: 20px;
      }
      button{
        background-color: transparent;
        border: 3px solid rgba(255, 255, 255, 0.8);
        width: 150px;
        padding: 15px 0 15px 0;
        border-radius: 100px;
        font-weight: bolder;
        transition-duration: 0.3s;
        transition-property: color,background-color ;
        cursor: pointer;
        display: block;
      }
      #Accept:hover{
        color: whitesmoke;
        background-color: yellowgreen;
      }
      #Decline:hover{
        color: whitesmoke;
        background-color:red;
      }
      .hide{
        display: none;
      }
      .box{
        top: calc(50% - 250px);
        min-width: 500px;
        min-height: 500px;
        border-radius: 50px;
        position: fixed;
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        background-color: transparent;
        overflow: hidden;
        border: 2px solid rgba(255, 255, 255, 0.5);
        transform-style: preserve-3d;
      }
      .box h1{
        color: white;
      }
      .input_box{
        position: relative;
        width: 100%;
        margin: 40px 0;
        border-bottom: 2px solid #fff;
      }
      .input_box:nth-child(2){
        margin-bottom: 10px;
      }
      .input_box label{
        position: absolute;
        top: 50%;
        left: 5px;
        margin-left: 5px;
        transform: translateY(-50%);
        transition: 0.5s;
        font-weight: bolder;
        font-size: x-large;
        color: azure;
        pointer-events: none;
      }
      .input_box input:focus~label, .input_box input:valid~label{
        top: -15px; 
        font-size: larger;
      }
      .input_box input{
        border: none;
        outline: none;
        font-size: 1em;
        width: 100%;
        height: 50px;
        background: transparent;
        color: aliceblue;
        padding: 0 35px 0 5px;
        font-size: x-large;
        font-weight: bolder;
      }
      .butt_cont{
        display: flex;
        width: 100%;
        justify-content: space-around;
        margin-top: 5px;
      }
      .bar{
        display: flex;
        width: 100%;
        padding-top: 10px;
        justify-content: space-around;
        background-color: transparent;
        border: none;
        border-radius: 100px;
      }
      .bar button{
        -webkit-box-shadow: none;
        box-shadow: none;
        z-index: 2;
        background-color: transparent;
        border: none;
      }
      .check_input{
        display: flex;
        align-items: center;
      }
      #final_label{
        font-weight: bolder;
        font-size: large;
        color: azure;
        pointer-events: none;
      }
      #Check{
        height: 15px;
        width: 15px;
        margin-right: 10px;
      }
      #local{
        transition: 0.5s;
        position: absolute;
        width: 150px;
        height: 46px;
        border: 5px solid rgba(255, 255, 255, 0.8);
        border-radius: 100px;
        z-index: 1;
        left: calc(6*((100% - 4*150px)/8) + 1.8*150px);
      }
      @keyframes Flip{
        0%{
          transform: rotateY(360deg);
        }
        50%{
          transform: rotateY(0deg);
        }
        100%{
          transform: rotateY(-360deg);
        }
      }
      @keyframes Shock{
        0%{
          top: calc(50% - 250px);
          left: calc(50% - 250px);
          background-color:rgba(255, 0, 0, 0.1)
        }
        25%{
          top: calc(50% - 250px + 5px);
          left: calc(50% - 250px);
          background-color:rgba(255, 0, 0, 0.2)
        }
        50%{
          top: calc(50% - 250px + 5px);
          left: calc(50% - 250px + 5px);
          background-color:rgba(255, 0, 0, 0.3)
        }
        75%{
          top: calc(50% - 250px);
          left: calc(50% - 250px + 5px);
          background-color:rgba(255, 0, 0, 0.2)
        }
        100%{
          top: calc(50% - 250px);
          left: calc(50% - 250px);
          background-color:rgba(255, 0, 0, 0.1);
        }
      }
      @media(max-width: 62em){
        #local{
          visibility: hidden;
        }
        .box{
          min-width: 50%;
          min-height: 50%;
        }
        @keyframes Shock{
          0%{
            top: calc(50% - 250px);
            background-color:rgba(255, 0, 0, 0.1)
          }
          25%{
            top: calc(50% - 250px + 5px);
            background-color:rgba(255, 0, 0, 0.2)
          }
          50%{
            top: calc(50% - 250px + 5px);
            background-color:rgba(255, 0, 0, 0.3)
          }
          75%{
            top: calc(50% - 250px);
            background-color:rgba(255, 0, 0, 0.2)
          }
          100%{
            top: calc(50% - 250px);
            background-color:rgba(255, 0, 0, 0.1);
          }
        }
      }
  </style>
</head>
<body onload="InitDefaultValue()" id="Result">
  <div class="bar">
    <button id="Back" onclick="history.back()">Back</button>
    <button id="Gateway">Gateway</button>
    <button id="Authentication">User</button>
    <button id="Authorization">Owner</button>
    <button id="Configuring">Access</button> 
    <div id="local"></div>
  </div> 
  <div class="box" id="card">
    <h1 id="Title">Login</h1>
    <form autocomplete="off">
      <div class="input_box">
        <input type="text" id="id" name="user" required><br>
        <label id="first_label" for="ssid">Username: </label>
      </div>
      <div class="input_box">
        <input type="password" id="password" name="password" required><br>
        <label id="last_label" for="password">Password: </label>
      </div>
      <div class="check_input" id="box">
        <input id="Check" type="checkbox" onclick="ShowHide()"> 
        <label id="final_label">Show password</label>
      </div>

    </form>
    <div class="butt_cont">
      <button id="Accept">Accept</button>
      <button id="Decline">Decline</button>
    </div>
  </div>
  <script>
    var DefaultValue = [];
    function InitDefaultValue(){
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function(){
        if (this.readyState == 4 && this.status == 200){
          for( var data of String(this.responseText).split("/"))
            DefaultValue.push(data);
          document.getElementById("id").value = DefaultValue[2];
          document.getElementById("password").value = DefaultValue[3];
        }
      };
      xhr.open("GET", "/BackEndSercure", false);
      xhr.send();
    }
    var state = "Authorization"
    document.getElementById("Gateway").addEventListener("click",rotate)
    document.getElementById("Authentication").addEventListener("click",rotate)
    document.getElementById("Authorization").addEventListener("click",rotate)
    document.getElementById("Configuring").addEventListener("click",rotate)
    document.getElementById("Accept").addEventListener("click",Change)
    document.getElementById("Decline").addEventListener("click",Delete)
    function rotate(){
      if(this.id == "Gateway" && state != "Gateway")
      {
        document.getElementById("card").style.animation = "Flip 0.5s linear"; 
        document.body.style.backgroundPosition = "left";  
        document.getElementById("local").style.left = "calc(2*((100% - 4*150px)/8) + 1.15*150px)";
        document.body.style.backgroundPosition = "0% 50%";    
        setTimeout(()=>{
          document.getElementById("box").style.display = "none";
          document.getElementById("password").type = "text"
          document.getElementById("Title").innerHTML = "Gateway";
          document.getElementById("first_label").innerHTML = "Address (0000 - FFFF):";
          document.getElementById("last_label").innerHTML = "Channel (00 - 1F):";
          document.getElementById("id").value = DefaultValue[0];
          document.getElementById("password").value = DefaultValue[1];
        },250)
      }
      else if(this.id != "Gateway")
      {
        document.getElementById("box").style.display = "flex";
        ShowHide()
      }

      if(this.id == "Authentication" && state != "Authentication")
      {
        document.getElementById("card").style.animation = "Flip 0.5s linear"; 
        document.getElementById("local").style.left = "calc(4*((100% - 4*150px)/8) + 1.5*150px)";
        document.body.style.backgroundPosition = "25% 50%";     
        setTimeout(()=>{
          document.getElementById("Title").innerHTML = "Authorized";
          document.getElementById("first_label").innerHTML = "Username:";
          document.getElementById("last_label").innerHTML = "Password:";
          document.getElementById("id").value = DefaultValue[4];
          document.getElementById("password").value = DefaultValue[5];
        },250)
      }
      if(this.id == "Authorization" && state != "Authorization")
      {
        document.getElementById("card").style.animation = "Flip 0.5s linear"; 
        document.getElementById("local").style.left = "calc(6*((100% - 4*150px)/8) + 1.8*150px)";
        document.body.style.backgroundPosition = "50% 50%";    
        setTimeout(()=>{
          document.getElementById("Title").innerHTML = "Login";
          document.getElementById("first_label").innerHTML = "Username:";
          document.getElementById("last_label").innerHTML = "Password:";
          document.getElementById("id").value = DefaultValue[2];
          document.getElementById("password").value = DefaultValue[3];
        },250)
      }
      if(this.id == "Configuring" && state != "Configuring")
      {
        document.getElementById("card").style.animation = "Flip 0.5s linear"; 
        document.getElementById("local").style.left = "calc(8*((100% - 4*150px)/8) + 2.1*150px)";
        document.body.style.backgroundPosition = "75% 50%";    
        setTimeout(()=>{
          document.getElementById("Title").innerHTML = "AP";
          document.getElementById("first_label").innerHTML = "SSID:";
          document.getElementById("last_label").innerHTML = "Password:";
          document.getElementById("id").value = DefaultValue[6];
          document.getElementById("password").value = DefaultValue[7];
        },250)
      }
      if(state == this.id)
        document.getElementById("card").style.animation = "Shock 0.1s 5 linear";
      state = this.id;
      setTimeout(()=>{document.getElementById("card").style.animation = "none";},500)
    }
    function Delete(){
      document.getElementById("id").value = ""
      document.getElementById("password").value = ""
    }
    function ShowHide()
    {
      if(document.getElementById("Check").checked == true)
        document.getElementById("password").type = "text"
      if(document.getElementById("Check").checked == false)
        document.getElementById("password").type = "password"
    }
    function Change(){
      var noti = "Hello";
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 204)
            noti = "Success"
        if (this.readyState == 4 && this.status == 400)
            noti = "Fail"
        if (this.readyState == 4 && this.status == 410)
            noti = "Time up!"
        if (this.readyState == 4 && this.status == 511)
            noti = "Connect again"
        };
      var DataSend = "{";
      if(state == "Gateway")
        DataSend +="Gateway: ";
      if (state =="Authentication")
        DataSend +="Authentication: ";
      if(state == "Authorization")
        DataSend +="Authorization: ";
      if (state == "Configuring")
        DataSend +="AP: ";
      DataSend += document.getElementById("id").value + "/"+ document.getElementById("password").value + "}";
      Delete();
      xhr.open("POST", "/BackEndSercure", false);
      xhr.send(DataSend);
      document.querySelector('body').style ="justify-content: center; align-items: center; flex-wrap: nowrap;"
      document.getElementById('Result').innerHTML = '<div><h1>'+noti+'</h1><div style="display:flex"><button id="Accept" onclick="history.back()">Return Home</button><button id="Decline" onClick="window.location.reload();">Reload Page</button></div></div>'
    }
      /*----------------------Detect DevTool------------------------------------*/
    function OnDevToolIsOpen(){
        document.querySelector('html').textContent = ''
    }
    class DevToolChecker extends Error{
        toString(){}
        get message(){OnDevToolIsOpen();}
    }
    console.log(new DevToolChecker())
  </script>
</body>
</html>
)rawliteral";
#pragma endregion Sercurity_html
#pragma region Parameters_html
const char Tolerance_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Tolerance ranges of plant</title>
    <style>
        *{
        padding: 0;
        margin: 0;
        box-sizing: border-box;
        
      }
      body{
        height: 100vh;
        width: 100vw;
        background-image: linear-gradient(to bottom, #00fffe, #00ffea, #00ffce, #00ffaa, #00ff80, #00f696, #00eda8, #00e3b4, #00c9df, #00a7ff, #007bff, #320fff);
        overflow: hidden;
        display: flex;
        justify-content: flex-start;
        flex-direction: column;
        flex-wrap: wrap;
        align-content: flex-start;
        transition: 0.5s;
        align-items: flex-start;
      }
      button{
        margin-bottom: 20px;
        background-color: azure;
        border: 0;
        width: 400px;
        padding: 15px 0 15px 0;
        border-radius: 100px;
        transition-duration: 0.3s;
        transition-property: color,background-color ;
        -webkit-box-shadow: 0px 11px 22px -9px #000000; 
        box-shadow: 0px 11px 22px -9px #000000;
        cursor: pointer;
        display: block;
        font-weight: bolder;
      }
      button:hover{
        color: whitesmoke;
        background-color: black;
      }
      #Accept:hover{
        color: whitesmoke;
        background-color: yellowgreen;
      }
      #Decline:hover{
        color: whitesmoke;
        background-color:red;
      }
      .container{
        margin: auto;
        width: 100%;
        height: 90%;
        padding: 0 16px;
        display: flex;
        justify-content: center;
        align-items: center;
        flex-direction: column;
      }
      .container h1{
        margin-bottom: 10px;
      }
      form{
        display: flex;
        height: 100%;
        flex-direction: column;
        justify-content: space-evenly;
      }
      .box{
        width: 90%;
        height: 80%;
        border: 2px solid whitesmoke;
        border-radius: 50px;
      }
      .container3{
        width: 1000px;
        display: flex;
        justify-content: space-around;
        margin-top: 50px;
      }
      #Back{
        width: auto;
        padding: 15px 20px 15px 20px;
        margin: 20px;
      }
      .input_box{
        display: flex;
        width: 100%;
        font-size: larger;
        font-weight: bolder;
        margin: 20px 0;
        padding-left: 6px;
        align-items: center
      }
      .input_box label{
        width: 10%;
        display: flex;
        justify-content: center;
        align-items: center;
      }
      .ranges_input{
        width: 90%;
        display: flex;
        justify-content: space-between;
      }
      .box .input_box:nth-child(4) .range_input{
        width: 45%;
      }
      .box .input_box:nth-child(4) .range_input input::-webkit-slider-thumb{
        border: 5px solid #fff700 !important;
      }
      .range_input{
        width: 50%;
        display: flex;
        position: relative;
      }
      .range_input input{
        width: 90%;
        -webkit-appearance: none;
        appearance: none;
        cursor: pointer;
        outline: none;
        height: 15px;
        background: #ccc;
        border-radius: 16px;
        background-color: transparent;
        border: 2px solid white;
      }
      .range_input input::-webkit-slider-thumb{
        -webkit-appearance: none;
        appearance: none;
        height: 15px;
        width: 15px;
        border-radius: 50%;
        background-color: transparent;
      }
      .box .input_box .range_input:nth-child(odd) input::-webkit-slider-thumb{
        border: 5px solid rgb(38, 0, 255);
      }
      .box .input_box .range_input:nth-child(even) input::-webkit-slider-thumb{
        border: 5px solid rgb(255, 3, 3);
      }
      .range_input label{
        background-color: whitesmoke;
        border-radius:100% 100% 100% 0%;
        border: 2px solid rgb(0, 225, 255);
        width: 30px !important;
        height: 30px !important;
        position: absolute;
        top: -35px;
        display: flex;
        justify-content: center !important;
        align-items: center;
        font-size: small;
        font-weight: bolder;
        visibility: hidden;
      }
      .range_input input:hover~label{
        visibility: visible;
      }
      @media (max-width: 1200px) {
        .box,.container3{
          width: 100%;
        }
      }
      @media (max-width: 1152px){
        .input_box{
          padding: 0;
          position: relative;
        }
        .input_box label{
          position: absolute;
          width: 100%;
          justify-content: flex-start;
          top:-20px;
        }
        .ranges_input{
          width: 100%;
        }
        .range_input{
          justify-content: center;
        }
        .range_input label{
          top: 100%;
          border-radius:100% 0% 100% 100%;
        }
        button{
          width: 40%;
        }
      }
    </style>
</head>
<body onload="InitDefaultValue()"  id="Result">
  <button id="Back" onclick="history.back()">Back</button>
    <div class="container">
        <h1>Tolerance Ranges</h1>
        <div class="box">
          <form autocomplete="off">
            <div class="input_box">
              <label for="T">Temperature</label>
              <div class="ranges_input">
                <div class="range_input">
                  <input type="range" name="lowT" id="lowT" max="50" min="0" step="0.1" value="%LOWT%">
                  <label for="lowT" id="VlowT"></label>
                </div>
                <div class="range_input">
                  <input type="range" name="upT" id="upT" max="50" min="0" step="0.1" value="%UPT%">
                  <label for="upT" id="VupT"></label>
                </div>
              </div>
            </div>
            <div class="input_box">
              <label for="H">Humidity</label>
              <div class="ranges_input">
                <div class="range_input">
                  <input type="range" name="lowH" id="lowH" max="90" min="20" step="0.1" value="%LOWH%">
                  <label for="lowH" id="VlowH"></label>
                </div>
                <div class="range_input">
                  <input type="range" name="upH" id="upH" max="90" min="20" step="0.1" value="%UPH%">
                  <label for="upH" id="VupH"></label>
                </div>
              </div>
            </div>
            <div class="input_box">
              <label for="SM">Soil Moisture</label>
              <div class="ranges_input">
                <div class="range_input">
                  <input type="range" name="lowSM" id="lowSM" max="100" min="0" step="0.1" value="%LOWSM%">
                  <label for="lowSM" id="VlowSM"></label>   
                </div>
                <div class="range_input">
                  <input type="range" name="upSM" id="upSM" max="100" min="0" step="0.1" value="%UPSM%">
                  <label for="upSM" id="VupSM"></label>
                </div>
              </div>
            </div>
            <div class="input_box">
              <label for="L">Light</label>
              <div class="range_input">
                <input type="range" name="L" id="L" max="100" min="0" step="0.1" value="%L%">
                <label for="L" id="VL"></label>
              </div>
            </div>
          </form>
        </div>
        <div class="container3">
          <button id="Accept">Accept</button>
          <button id="Decline">Decline</button>
        </div>    
    </div>
  <script>
    function InitDefaultValue(){
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function(){
        if (this.readyState == 4 && this.status == 200){
          document.getElementById("upT").defaultValue = String(this.responseText).split("/")[0]
          document.getElementById("lowT").defaultValue = String(this.responseText).split("/")[1]
          document.getElementById("upH").defaultValue = String(this.responseText).split("/")[2]
          document.getElementById("lowH").defaultValue = String(this.responseText).split("/")[3]
          document.getElementById("upSM").defaultValue = String(this.responseText).split("/")[4]
          document.getElementById("lowSM").defaultValue = String(this.responseText).split("/")[5]
          document.getElementById("L").defaultValue = String(this.responseText).split("/")[6]
        }
      };
      xhr.open("GET", "/BackEndTolerance", false);
      xhr.send();
      document.querySelectorAll(".range_input").forEach(w=>{
        setBubble(w.querySelector("input"),w.querySelector("label"));
      })
    }
    document.querySelectorAll(".ranges_input").forEach(wraps=>{
      wraps.querySelectorAll(".range_input")[0].querySelector("input").addEventListener("input",()=>{
        if(Number(wraps.querySelectorAll(".range_input")[0].querySelector("input").value) > Number(wraps.querySelectorAll(".range_input")[1].querySelector("input").value)){
          wraps.querySelectorAll(".range_input")[1].querySelector("input").value = wraps.querySelectorAll(".range_input")[0].querySelector("input").value;
          setBubble(wraps.querySelectorAll(".range_input")[1].querySelector("input"),wraps.querySelectorAll(".range_input")[1].querySelector("label"));
        }
        setBubble(wraps.querySelectorAll(".range_input")[0].querySelector("input"),wraps.querySelectorAll(".range_input")[0].querySelector("label"));
      });
      wraps.querySelectorAll(".range_input")[1].querySelector("input").addEventListener("input",()=>{
        if(Number(wraps.querySelectorAll(".range_input")[1].querySelector("input").value) < Number(wraps.querySelectorAll(".range_input")[0].querySelector("input").value)){
          wraps.querySelectorAll(".range_input")[0].querySelector("input").value = wraps.querySelectorAll(".range_input")[1].querySelector("input").value;
          setBubble(wraps.querySelectorAll(".range_input")[0].querySelector("input"),wraps.querySelectorAll(".range_input")[0].querySelector("label"));
        }
        setBubble(wraps.querySelectorAll(".range_input")[1].querySelector("input"),wraps.querySelectorAll(".range_input")[1].querySelector("label"));
      });
        setBubble(wraps.querySelectorAll(".range_input")[0].querySelector("input"),wraps.querySelectorAll(".range_input")[0].querySelector("label"));
        setBubble(wraps.querySelectorAll(".range_input")[1].querySelector("input"),wraps.querySelectorAll(".range_input")[1].querySelector("label"));
    });
    document.querySelectorAll(".range_input")[6].querySelector("input").addEventListener("input",()=>{
        setBubble(document.querySelectorAll(".range_input")[6].querySelector("input"),document.querySelectorAll(".range_input")[6].querySelector("label"));
      });
      setBubble(document.querySelectorAll(".range_input")[6].querySelector("input"),document.querySelectorAll(".range_input")[6].querySelector("label"));
    
    function setBubble(range, bubble) {
      const val = range.value;
      const min = range.min ? range.min : 0;
      const max = range.max ? range.max : 100;
      const newVal = Number(((val - min) * 100) / (max - min));
      bubble.innerHTML = val;
      bubble.style.left = `calc(${newVal}% + (${8 - newVal * 0.75}px))`;
    }
    document.getElementById("Decline").addEventListener('click',Cancel);
    document.getElementById("Accept").addEventListener('click',Send);
    function Cancel(){
      document.querySelectorAll(".range_input").forEach(Wrap=>{
        Wrap.querySelector("input").value = Wrap.querySelector("input").defaultValue;
        setBubble(Wrap.querySelector("input"),Wrap.querySelector("label"));
      })
    }
    function Send(){
        var noti = "Hello";
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200)
              noti = "Success"
          if (this.readyState == 4 && this.status == 410)
              noti = "Time up!"
          };
        var DataSend = "{"+document.getElementById("upT").value +"/"+ document.getElementById("lowT").value +
                        "/"+document.getElementById("upH").value +"/"+ document.getElementById("lowH").value +
                        "/"+document.getElementById("upSM").value +"/"+ document.getElementById("lowSM").value +
                        "/"+document.getElementById("L").value+"}";
        xhr.open("POST", "/BackEndTolerance", false);
        xhr.send(DataSend);
        document.querySelector('body').style ="justify-content: center; align-items: center; flex-wrap: nowrap;"
        document.getElementById('Result').innerHTML = '<div><h1>'+noti+'</h1><div style="display:flex"><button id="Accept" onclick="history.back()">Return Home</button><button id="Decline" onClick="window.location.reload();">Reload Page</button></div></div>'
      }
    /*----------------------Detect DevTool------------------------------------*/
    function OnDevToolIsOpen(){
      document.querySelector('html').textContent = ''
  }
  class DevToolChecker extends Error{
      toString(){}
      get message(){OnDevToolIsOpen();}
  }
  console.log(new DevToolChecker())
  </script>
</body>
</html>
)rawliteral";
#pragma endregion Parameters_html
#pragma region 
const char Forbidden_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>403 FORBIDDEN</title>
  <style>
    *, *:before, *:after {
      box-sizing: border-box;
    }
    html {
        height: 100%;
    }
    body {
        background-color:  black; 
        height: 100%;
        padding: 10px;
    }
    .text-wrapper {
        height: 100%;
       display: flex;
       flex-direction: column;
       align-items: center;
       justify-content: center;
       text-shadow: 0px 0px 10px;
    }
    
    .title {
        font-size: 5em;
        font-weight: 700;
        color: #EE4B5E;
    }
    
    .subtitle {
        font-size: 40px;
        font-weight: 700;
        color: #1FA9D6;
    }
    .isi {
        font-size: 18px;
        text-align: center;
        margin:30px;
        padding:20px;
        color: white;
    }
    button {
        margin: 30px;
        font-weight: 700;
        border: 2px solid #EE4B5E;
        padding: 15px;
        text-transform: uppercase;
        color: #EE4B5E;
        border-radius: 26px;
        transition: all 0.2s ease-in-out;
        display: inline-block;
        background-color: transparent;
      } 
      button:hover {
        background-color: #EE4B5E;
        color: white;
        transition: all 0.2s ease-in-out;
      }
  </style>
</head>
<body>
  <div class="text-wrapper">
    <div class="title" data-content="404">
        403 - ACCESS DENIED
    </div>

    <div class="subtitle">
        Oops, You don't have permission to access this page.
    </div>
    
    <button onclick="history.back()">
        Go to homepage
    </button>
</div>
</body>
</html>
)rawliteral";
#pragma endregion