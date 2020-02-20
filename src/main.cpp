#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include "FS.h"

WebSocketsServer webSocket = WebSocketsServer(81); 
ESP8266WebServer server(80); 

const char configPage[] PROGMEM= R"=====(
<html>
<head>
  <title>ESP | Config</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
        *{box-sizing: inherit;font-family: Montserrat, sans-serif; font-weight: 400}
        .btn{ color: #fff; background: #06d79c; border: 1px solid #06d79c; padding: 7px 12px;  cursor: pointer}
        .select-editable {position:relative; background-color: white; display:block; border-radius:4px; width:230px; height:25px; padding:4px; margin:15px auto;}
        .select-editable select { position:absolute; top:0px; left:0px; border-radius:4px; width:230px; height:25px; padding:4px;margin:0; }
        .select-editable input { position:absolute; top:4px; left:4px; width:217px; height:25px;  border:none; }
        .select-editable select:focus, .select-editable input:focus { outline:none;  height:25px; }
        .inputStyle { width:230px; height: 25px; display:block; border-radius:4px; padding:4px; margin:15px auto;}
    </style>
</head>
<body style="background: #152036;">
      <h1 style="text-align:center; margin-top: 30px; color: #fff;">ESP Config</h1>
          <div style="background:#1b2a47; width: 270px;padding:25px; margin:0 auto; border-radius:15px;">
                <h4 style="text-align:center; color: #fff;">Wifi credentials</h4>
                <input id="SSID" oninput="passwordBehaviour(this)" class="inputStyle" type="text" placeholder="SSID" name="product" list="ssidList"/>
                <datalist id="ssidList" ></datalist>
                <input class="inputStyle" id="PASSWORD" placeholder="PASSWORD" name="PASSWORD" type="password" />
                <h4 style="text-align:center; color: #fff;">Device details</h4>
                <input class="inputStyle" id="HOSTNAME" placeholder="HOSTNAME" name="HOSTNAME" type="text" />
                <h4 style="text-align:center; color: #fff;">Device pinout</h4>
                <input class="inputStyle" id="REDPIN" placeholder="REDPIN" name="REDPIN" type="number" />
                <input class="inputStyle" id="GREENPIN" placeholder="GREENPIN" name="GREENPIN" type="number" />
                <input class="inputStyle" id="BLUEPIN" placeholder="BLUEPIN" name="BLUEPIN" type="number" />
                <h4 style="text-align:center; color: #fff; display:inline-block; margin-left:15px;" >Static ipaddress (optional)</h4> <input required value style="border-radius:4px" id="startStatic" name="startStatic" type="checkbox"/>
                <input class="inputStyle" id="local_IP" placeholder="IPAddress ex. 192.168.1.50" name="local_IP" type="text"/>
                <input class="inputStyle" id="gateway" placeholder="Gateway ex. 192.168.1.1" name="gateway" type="text"/>
                <input class="inputStyle" id="subnet" placeholder="subnet ex. 255.255.255.0" name="subnet" type="text"/>
                <input class="inputStyle" id="dns" placeholder="dns ex 8.8.8.8" name="dns" type="text"/>
                <button style="width:230px; height: 25px; display:block; border-radius:4px; padding:4px; margin:15px auto;" class="btn" onclick="sendFunction()">Send</button>
                <h4 style="text-align:center; color: #fff; margin:2px;">Test pin</h4>
                <input style="margin-left:15px; width:100px; height:20px;  border-radius:4px; padding:4px;" id="selectedPin" placeholder="Select Pin" name="selectedPin" type="number" />
                <input id="turnMode" name="turnMode" type="checkbox"><p  style="color:#fff; display:inline-block;">HIGH/LOW </p></input>
                <button style="width:230px; height: 25px; display:block; border-radius:4px; padding:4px; margin:15px auto;" class="btn" id="turnMode" onclick="testPin()">Test</button>
                <button style="width:230px; height: 25px; display:block; border-radius:4px; padding:4px; margin:15px auto;" class="btn" onclick="if(confirm('Are you sure?')) window.location = '/formatDevice';">Factory reset</button>
          </div>
  <script>
     function sendFunction(){
       var Initialized=false;
       if(document.getElementById("SSID").value !="") Initialized=true;
       var data ={"ESP_Config":{ "Initialized":Initialized,
                   "HOSTNAME": "esprgb-"+document.getElementById("HOSTNAME").value,
                   "SSID": document.getElementById("SSID").value,
                   "PASSWORD":document.getElementById("PASSWORD").value,
                   "REDPIN":parseInt(document.getElementById("REDPIN").value),
                   "GREENPIN":parseInt(document.getElementById("GREENPIN").value),
                   "BLUEPIN":parseInt(document.getElementById("BLUEPIN").value),
                 },
                 "Network_Config":{
                   "startStatic":document.getElementById("startStatic").checked,
                   "local_IP":document.getElementById("local_IP").value,
                   "gateway":document.getElementById("gateway").value,
                   "subnet":document.getElementById("subnet").value,
                   "dns":document.getElementById("dns").value
                 }
               };
        var xhr = new XMLHttpRequest();
        xhr.open("POST","/sendConfig",true);
        xhr.send(JSON.stringify(data));
     }
    function passwordBehaviour(e){
      var psw = document.getElementById("PASSWORD");
      psw.value = null;
      var option = document.querySelector("#ssidList option[value='"+e.value+"']");
      if (option != null) {
        if((option.innerHTML.split(',')[2].split(':')[1] == 'true')) psw.disabled = false;
        else psw.disabled = true;
      }
    }
     function testPin(){
         var data ={"turnMode":document.getElementById("turnMode").checked,"selectedPin":parseInt(document.getElementById("selectedPin").value)};
         var xhr = new XMLHttpRequest();
         xhr.open("POST","/pinTester",true);
         xhr.send(JSON.stringify(data));
       }
     window.addEventListener('load', function () {
       var configData=new XMLHttpRequest();
       configData.open("GET",'/returnConfigData',true);
       configData.send();
       configData.onload=function(){
            data=JSON.parse(configData.responseText);
            if(data.Initialized){
              document.getElementById("SSID").value = data.SSID;
              document.getElementById("PASSWORD").value = data.PASSWORD;
              document.getElementById("HOSTNAME").value = data.HOSTNAME.split('-')[1];
              document.getElementById("REDPIN").value = data.REDPIN;
              document.getElementById("GREENPIN").value = data.GREENPIN;
              document.getElementById("BLUEPIN").value = data.BLUEPIN;
              document.getElementById("startStatic").checked = data.startStatic;
              document.getElementById("local_IP").value = data.local_IP;
              document.getElementById("gateway").value = data.gateway;
              document.getElementById("subnet").value = data.subnet;
              document.getElementById("dns").value = data.dns;
              if(data.PASSWORD == "") document.getElementById("PASSWORD").disabled = true;
            }
          }
          var wifiData=new XMLHttpRequest();
          wifiData.open("GET",'/getWifi',true);
          wifiData.send();
          wifiData.onload=function(){
                data=JSON.parse(wifiData.responseText);
                for(network in data.networks) {
                    var node = document.createElement("OPTION");
                    node.value = data.networks[network][0];
                    var textnode = document.createTextNode(data.networks[network][0] +", Strength:"+ data.networks[network][1]+"dBm, Secured:"+ data.networks[network][2]);
                    node.appendChild(textnode);
                    document.getElementById("ssidList").appendChild(node);
                }
          }
        })
  </script>
</body>
</html>
)=====";
const char homePage[] PROGMEM= R"=====(
<html>
<head>
  <title>ESP | Home</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
        *{box-sizing: inherit;font-family: Montserrat, sans-serif;color:white;margin: 8 5;font-weight: 500}
        .btn{ color: #fff;background: #06d79c;border: 1px solid #06d79c;padding: 6px;cursor: pointer}
        .slider {-webkit-appearance: none;appearance: none;width: 245px;height: 20px;background: #d3d3d3;outline: none;border-radius:4px;}
        .slider::-webkit-slider-thumb {-webkit-appearance: none;appearance: none;width: 15px;height: 20px;background: #06d79c;cursor: pointer;}
        input{color: black;}
        p,a{font-size:14px; text-align:center;}
        .input_Text{width:150px;height: 18px; border-radius:4px;padding:4px;}
  </style>
</head>
<body style="background: #152036;">
    <div style="background:#1b2a47; width: 280px;padding:35px; margin:50px auto; border-radius:15px; ">
      <h1 style="text-align:center; margin-top: 30px; color: #fff;">ESPRGB</h1>
      <p style="display:inline-block;">R:</p><input type="range" min="0" max="255" value="255" class="slider" id="red" oninput="sendColor()" />
      <p style="display:inline-block;">G:</p><input type="range" min="0" max="255" value="255" class="slider" id="green" oninput="sendColor()"/>
      <p style="display:inline-block;">B:</p><input type="range" min="0" max="255" value="255" class="slider" id="blue" oninput="sendColor()"/>
      <p style="display:inline-block;">A:</p><input type="range" min="1" max="100" value="100" class="slider" id="brightness" oninput="ws.send(JSON.stringify({'Brightness':this.value/100}));"/>
      <div style="height:25px; width:270; background-color:white; margin:8 auto; border-radius:4px;" id="resultColor"></div>
      <input type="checkbox" id="powerState" oninput="ws.send(JSON.stringify({'PowerState':this.checked}));"/>Turn ON/OFF<br/>
      <input class="input_Text" placeholder="Breething Speed" id="breethingSpeed" oninput="startBreething()"/>
      <input type="checkbox" id="startBreething" oninput="startBreething()"/>Breething<br>
      <input class="input_Text" placeholder="Pulse Speed" id="pulseSpeed" oninput="startPulse()"/>
      <input type="checkbox" id="startPulse" oninput="startPulse()" />Pulse<br>
      
      <p>You can change your config <a id="config" href="/config">here</a></p>
    </div>
    <script>
        var rgbToHex = function (rgb) {
          var hex = Number(rgb).toString(16);
          if (hex.length < 2)  hex = "0" + hex;
          return hex;
        };
        var fullColorHex = function(r,g,b) {
          var red = rgbToHex(r);
          var green = rgbToHex(g);
          var blue = rgbToHex(b);
          return red+green+blue;
        };
        let ws;
            ws = new WebSocket("ws://" + window.location.hostname + ":81/")
            ws.onmessage = function(event) {
              var data = JSON.parse(event.data);
              if("SolidColor" in data) {
                  document.getElementById("red").value = data.SolidColor[0] / 4;
                  document.getElementById("green").value = data.SolidColor[1] / 4;
                  document.getElementById("blue").value = data.SolidColor[2] / 4;
                  document.getElementById("resultColor").style.backgroundColor = fullColorHex(data.SolidColor[0] / 4,data.SolidColor[1] / 4,data.SolidColor[2] / 4);
              }
              if("Brightness" in data) document.getElementById("brightness").value = data.Brightness * 100;
              if("PowerState" in data)  document.getElementById("powerState").checked = data.PowerState;
              if("BreethingAnimation" in data){
                document.getElementById("breethingSpeed").value = data.BreethingAnimation.breethingSpeed;
                document.getElementById("startBreething").checked = data.BreethingAnimation.startBreething;
              }
              if("PulseAnimation" in data){
                document.getElementById("pulseSpeed").value = data.PulseAnimation.pulseSpeed;
                document.getElementById("startPulse").checked = data.PulseAnimation.startPulse;
              }
            };
        function sendColor(){
          var a = JSON.stringify({'SolidColor':[document.getElementById("red").value * 4,document.getElementById("green").value * 4,document.getElementById("blue").value * 4]});
          document.getElementById("resultColor").style.backgroundColor = fullColorHex(document.getElementById("red").value,document.getElementById("green").value,document.getElementById("blue").value);
          ws.send(a);
        }
        function startBreething(){
          ws.send(JSON.stringify({'BreethingAnimation':{'startBreething':document.getElementById("startBreething").checked,'breethingSpeed':document.getElementById("breethingSpeed").value}}));
        }
        function startPulse(){
          ws.send(JSON.stringify({'PulseAnimation':{'startPulse':document.getElementById("startPulse").checked,'pulseSpeed':document.getElementById("pulseSpeed").value}}));
        }
    </script>
</body>
</html>
)=====";

bool initialized;
char *hostname;
char *ssid;
char *password; 
bool startStatic=false;
int red_pin = 0;
int green_pin = 0;
int blue_pin = 0;
IPAddress local_IP = IPAddress(192,168,1,50);
IPAddress gateway= IPAddress(192,168,1,1);
IPAddress subnet= IPAddress(255,255,255,0);
IPAddress dns = IPAddress(8,8,8,8);

int SolidColor[3] = {1020,0,0};
float brightness = 1.0f;
int animationSelector=0;
String playingAnimation;

unsigned long currentMillis;

void sendData(uint8_t num);
void saveConfig();
void saveUserData();
bool loadConfig();
DynamicJsonDocument AnimationsJson();

void removeUserSettings(){
      SPIFFS.remove("/user_settings.json");
      delay(500);
      ESP.restart();   
}

void SetColorValue(int red,int green, int blue,float _brightness){
    analogWrite(red_pin, constrain(red*_brightness, 0, 1020)); 
    analogWrite(green_pin, constrain(green*_brightness, 0, 1020));
    analogWrite(blue_pin, constrain(blue*_brightness, 0, 1020));
}

bool startAudioReactive;
int colorAudioReactive[3] = {1020,0,0};
void handleAudioReactive(float _pulse){
  SetColorValue(colorAudioReactive[0],colorAudioReactive[1],colorAudioReactive[2],_pulse);
}

bool startPulse = false, inverseBool = true;
unsigned int pulseSpeed = 10, pulseMillis;
float pulse=1.0f;
int colorPulse[3] = {1020,0,0};
void handlePulse(){
  if (currentMillis - pulseMillis >= pulseSpeed) {
      pulseMillis = currentMillis;
      if(inverseBool) {
         pulse=constrain((pulse-0.001f), 0.01f, 1.0f);
         if(pulse<=0.01f) inverseBool = !inverseBool;
      }else{
          pulse=constrain((pulse+0.001f), 0.01f, 1.0f);
         if(pulse>=1.0f) inverseBool = !inverseBool;
      }
      SetColorValue(colorPulse[0],colorPulse[1],colorPulse[2],pulse);
  }
}

bool startBreething = false;
bool startBool[]={true,true,true};
int prevMillis, colorIndex[]={1020,0,0};
unsigned int breethingSpeed = 50;

void Breathing(){
   if(currentMillis - prevMillis > breethingSpeed){
    prevMillis = currentMillis;
    if(startBool[0]==true && colorIndex[0]>0){
      colorIndex[0] = constrain(colorIndex[0]-1, 0, 1020);
      if(colorIndex[0]<=0) startBool[0]=false;
    }
    if(startBool[1]==true&&colorIndex[0]<=750){
      colorIndex[1] = constrain(colorIndex[1]+1, 0, 1020);
      if(colorIndex[1] >= 1020) startBool[1]=false;
    }
    if(startBool[1]==false && colorIndex[1]>0) colorIndex[1] = constrain(colorIndex[1]-1, 0, 1020); 
    if(startBool[2]==true&&startBool[1]==false&&colorIndex[1] < 750){
      colorIndex[2] = constrain(colorIndex[2]+1, 0, 1020);
      if(colorIndex[2] >= 1020)  startBool[2] = false;
    }
    if(startBool[2]==false && colorIndex[2]>0) colorIndex[2] = constrain(colorIndex[2]-1, 0, 1020);    
    if(startBool[2]==false&&startBool[0]==false&&colorIndex[2] < 750){
        colorIndex[0] = constrain(colorIndex[0]+1, 0, 1020);
        if(colorIndex[0] >= 1020) {
        startBool[0] = true; startBool[1] = true; startBool[2] = true;
        }
    }
    SetColorValue(colorIndex[0],colorIndex[1],colorIndex[2],brightness);
   }
}

void turnOffAnimations(){
  startPulse = false;
  startBreething = false;
  startAudioReactive = false;
  SetColorValue(0,0,0,0);
}
void handlePower(bool PowerState,int anim,bool animstate){
    if(PowerState) {
      bool send = false;
        if(animationSelector != anim){
          turnOffAnimations();
          send = true;
        }  
        if(animstate == true)
          animationSelector = anim;
        else{
          turnOffAnimations();
          send = true;
          animationSelector = 1;
        }
        switch (animationSelector)
        {
          case 1:      
            SetColorValue(SolidColor[0],SolidColor[1],SolidColor[2],brightness);
            playingAnimation = "Solid Color"; 
            break;
          case 2:         
            startBool[0] = true; startBool[1] = true; startBool[2] = true;
            colorIndex[0] = 1020; colorIndex[1] = 0; colorIndex[2] = 0;
            startBreething = animstate;
            playingAnimation = "Breething Animation";
            break;
          case 3:     
            pulse=1.0f;
            startPulse = animstate;     
            playingAnimation = "Pulse Animation";
            break;
          case 4:                 
            startAudioReactive = animstate;
            playingAnimation = "Audio Reactive";
            break;
          default:      
            SetColorValue(SolidColor[0],SolidColor[1],SolidColor[2],brightness);
            playingAnimation = "Solid Color"; 
            break;
        }              
       if(send) webSocket.broadcastTXT("{\"Animations\":{\"playingAnimation\":\""+playingAnimation+"\",\"animationSelector\":"+String(animationSelector)+"}}");
    }else{
      turnOffAnimations();
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if(type == WStype_CONNECTED){
      Serial.println("Client "+String(num)+" connected");  
      handlePower(true,animationSelector,true);
      String config = AnimationsJson().as<String>();
      webSocket.sendTXT(num,config);
  }
  if (type == WStype_TEXT)
  {
        if(webSocket.connectedClients() > 1)
          for (int i = 0; i < webSocket.connectedClients(); i++)  if(i != num) webSocket.sendTXT(i,payload,length);
        DynamicJsonDocument json(length+50);   
        deserializeJson(json, payload);
        Serial.println(json.as<String>());
        JsonObject Animations = json["Animations"];
        if (Animations.containsKey("SolidColor"))
        {        
          SolidColor[0] = Animations["SolidColor"][0];
          SolidColor[1] = Animations["SolidColor"][1];
          SolidColor[2] = Animations["SolidColor"][2];
          handlePower(true,1,true); 
        }
        if (Animations.containsKey("Brightness")){
          brightness = Animations ["Brightness"];
          handlePower(true,animationSelector,true);
        }           
        if (Animations.containsKey("BreethingAnimation"))
        {
          JsonObject breethingAnimation = Animations["BreethingAnimation"];
          if(breethingAnimation.containsKey("breethingSpeed")) breethingSpeed = breethingAnimation["breethingSpeed"];
          if(breethingAnimation.containsKey("startBreething")) handlePower(true,2,breethingAnimation["startBreething"]);   
        }
        if (Animations.containsKey("PulseAnimation"))
        {
          JsonObject pulseAnimation = Animations["PulseAnimation"];
          if(pulseAnimation.containsKey("colorPulse")) {
            colorPulse[0] = pulseAnimation["colorPulse"][0];
            colorPulse[1] = pulseAnimation["colorPulse"][1];
            colorPulse[2] = pulseAnimation["colorPulse"][2];
          }    
          if(pulseAnimation.containsKey("pulseSpeed")) pulseSpeed = pulseAnimation["pulseSpeed"];    
          if(pulseAnimation.containsKey("startPulse")) handlePower(true,3,pulseAnimation["startPulse"]);   
        }
        if (Animations.containsKey("PowerState")) handlePower(Animations["PowerState"],animationSelector,true);
        if (Animations.containsKey("AudioReactive"))
        {
          JsonObject AudioReactive = Animations["AudioReactive"];
          if(AudioReactive.containsKey("colorAudioReactive")) {
            colorAudioReactive[0] = AudioReactive["colorAudioReactive"][0];
            colorAudioReactive[1] = AudioReactive["colorAudioReactive"][1];
            colorAudioReactive[2] = AudioReactive["colorAudioReactive"][2];
          }    
          if(AudioReactive.containsKey("startAudioReactive")) handlePower(true,4,AudioReactive["startAudioReactive"]); 
          if(AudioReactive.containsKey("volumeAudioReactive")) handleAudioReactive(AudioReactive["volumeAudioReactive"]);             
        }  
        if (json.containsKey("command")) {
            if(json["command"] == "formatDevice") removeUserSettings();
            if(json["command"] == "getConfig") {
                String config = AnimationsJson().as<String>();
                webSocket.sendTXT(num,"{\"response\":"+config+"}");
            }
        } 
  }
  if(type == WStype_DISCONNECTED){
    Serial.println("Client "+String(num)+" disconnected");  
    if(webSocket.connectedClients() == 0){
        handlePower(false,animationSelector,true);
        saveUserData();  
    }
  }
}

DynamicJsonDocument AnimationsJson(){
  DynamicJsonDocument doc(1024);
    JsonObject Animations = doc.createNestedObject("Animations");
      JsonArray _SolidColor = Animations.createNestedArray("SolidColor");
      for (int i = 0; i < 3; i++) _SolidColor.add(SolidColor[i]);
      Animations["Brightness"] = brightness;
      JsonObject BreethingAnimation = Animations.createNestedObject("BreethingAnimation");
        BreethingAnimation["startBreething"] = startBreething;
        BreethingAnimation["breethingSpeed"] = breethingSpeed;
      JsonObject PulseAnimation = Animations.createNestedObject("PulseAnimation");
        PulseAnimation["startPulse"] = startPulse;
        PulseAnimation["pulseSpeed"] = pulseSpeed;
        JsonArray _colorPulse = PulseAnimation.createNestedArray("colorPulse");
          for (int i = 0; i < 3; i++) _colorPulse.add(colorPulse[i]);  
      JsonObject AudioReactive = Animations.createNestedObject("AudioReactive");
        AudioReactive["startAudioReactive"] = startAudioReactive;
        JsonArray _colorAudioReactive = AudioReactive.createNestedArray("colorAudioReactive");
          for (int i = 0; i < 3; i++) _colorAudioReactive.add(colorAudioReactive[i]);  
      Animations["animationSelector"] = animationSelector;
      Animations["playingAnimation"] = playingAnimation;
  return doc;
}
void combine(JsonDocument& dst, const JsonDocument& src) {
    for (auto p : src.as<JsonObject>()) dst[p.key()] = p.value();
}
void saveConfig(){
  File configFile = SPIFFS.open("/user_settings.json", "w+");
  DynamicJsonDocument json(1024);
  deserializeJson(json, server.arg("plain"));
  if(ssid == json["ESP_Config"]["SSID"])
      json["ESP_Config"]["PASSWORD"] = password;
  combine(json, AnimationsJson());
  serializeJson(json, configFile);
  configFile.close();
  ESP.restart();
}

bool loadConfig() {
  File configFile = SPIFFS.open("/user_settings.json", "r");
  if (!configFile) return false;
  DynamicJsonDocument json(2048);
  deserializeJson(json, configFile);
  if(json.containsKey("ESP_Config")) {
    initialized = json["ESP_Config"]["Initialized"];
    hostname = strdup(json["ESP_Config"]["HOSTNAME"]);
    ssid = strdup(json["ESP_Config"]["SSID"]);
    password = strdup(json["ESP_Config"]["PASSWORD"]);
    red_pin = json["ESP_Config"]["REDPIN"];
    green_pin = json["ESP_Config"]["GREENPIN"];
    blue_pin = json["ESP_Config"]["BLUEPIN"];
  }
  if(json.containsKey("Network_Config")) {
    startStatic = json["Network_Config"]["startStatic"];
    local_IP.fromString(json["Network_Config"]["local_IP"].as<String>());
    gateway.fromString(json["Network_Config"]["gateway"].as<String>());
    subnet.fromString(json["Network_Config"]["subnet"].as<String>());
    dns.fromString(json["Network_Config"]["dns"].as<String>());
  }
  if(json.containsKey("Animations")) {
    for (int i = 0; i < 3; i++) SolidColor[i] = json["Animations"]["SolidColor"][i];
    brightness = json["Animations"]["Brightness"];
    breethingSpeed = json["Animations"]["BreethingAnimation"]["breethingSpeed"]; 
    startBreething = json["Animations"]["BreethingAnimation"]["startBreething"]; 
    for (int i = 0; i < 3; i++) colorPulse[i] = json["Animations"]["PulseAnimation"]["colorPulse"][i];
    pulseSpeed = json["Animations"]["PulseAnimation"]["pulseSpeed"]; 
    startPulse = json["Animations"]["PulseAnimation"]["startPulse"]; 
    for (int i = 0; i < 3; i++) colorAudioReactive[i] = json["Animations"]["AudioReactive"]["colorAudioReactive"][i];
    animationSelector = json["Animations"]["animationSelector"];
  }
  configFile.close();
  return true;
}

void saveUserData(){
    DynamicJsonDocument doc(2048);
    File configFile = SPIFFS.open("/user_settings.json", "r");
    deserializeJson(doc, configFile);
    configFile.close();
    combine(doc, AnimationsJson());
    configFile = SPIFFS.open("/user_settings.json", "w+");
    serializeJson(doc, configFile);
    configFile.close(); 
}

unsigned long previousMillis2 = 0;   
void handleSave(){
  if (currentMillis - previousMillis2 >= 30000) {
      previousMillis2 = currentMillis;
      saveUserData();
  }
}
void pinTester(){
  DynamicJsonDocument doc(200);
  deserializeJson(doc, server.arg("plain"));
  if(doc["turnMode"]) analogWrite(doc["selectedPin"],1020);
  else analogWrite(doc["selectedPin"],0);
  server.send(200, "text/html", "ok"); 
}
void returnConfigData(){
  String data;
  DynamicJsonDocument doc(1024);
  doc["Initialized"] = initialized;
  if(initialized){
      doc["SSID"] = ssid;
      if(String(password) != "") doc["PASSWORD"] = "**********";
      else doc["PASSWORD"] = "";
      doc["HOSTNAME"] = hostname;    
      doc["REDPIN"] = red_pin;
      doc["GREENPIN"] = green_pin;
      doc["BLUEPIN"] = blue_pin;
      doc["startStatic"] = startStatic;
      doc["local_IP"] = local_IP.toString();
      doc["gateway"] = gateway.toString();
      doc["subnet"] = subnet.toString();
      doc["dns"] = dns.toString();
  }
  serializeJson(doc, data);
  server.send(200, "application/json", data); 
}
void getWifi(){
  int n = WiFi.scanNetworks();  
  String data;
  DynamicJsonDocument doc(2048);   
  JsonArray networks = doc.createNestedArray("networks");

  for (int i = 0; i < n; i++){
    JsonArray network = networks.createNestedArray();
    network.add(WiFi.SSID(i));
    network.add(WiFi.RSSI(i));
    network.add(WiFi.encryptionType(i) == ENC_TYPE_NONE ? false : true);
  }
  serializeJson(doc, data);
  server.send(200, "application/json", data); 
}
bool startSTA(){
    pinMode(red_pin,OUTPUT);
    pinMode(green_pin,OUTPUT);
    pinMode(blue_pin,OUTPUT);
    WiFi.mode(WIFI_STA);  
    WiFi.hostname(hostname);
    
    if(startStatic) WiFi.config(local_IP, gateway, subnet, dns);  
    WiFi.begin(ssid, password);  
    int timeout=0;
    while(WiFi.status() != WL_CONNECTED){
      digitalWrite(LED_BUILTIN, HIGH); delay(250); digitalWrite(LED_BUILTIN, LOW);
      Serial.print(".");
      if(timeout++ > 250) return false;      
    }
    Serial.println("");
    if(MDNS.begin(hostname)){
      Serial.println("http://"+String(wifi_station_get_hostname())+".local : "+WiFi.localIP().toString());
      MDNS.addService("esprgb", "tcp", 80);
    } 

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    ArduinoOTA.begin();
    server.on("/", []() {server.send(200, "text/html", homePage);});
    server.on("/config", []() {server.send(200, "text/html", configPage);});  
    digitalWrite(LED_BUILTIN, HIGH); 
    return true;
}

bool startAP(){  
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(192,168,1,1), IPAddress(192,168,1,1), IPAddress(255, 255, 255, 0));
    WiFi.softAP("ESPRGB_Config");
    Serial.println(WiFi.softAPIP());
    server.on("/", []() {server.send(200, "text/html", configPage);});       
    digitalWrite(LED_BUILTIN, LOW);
    return true;
}

void handleInit(){
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  if(initialized){
    Serial.println("ESP Started in station mode.");
    if (!startSTA()) startAP();
  }else{
    Serial.println("ESP Started in ap mode.");
    startAP();
  }
  server.on("/pinTester", pinTester);
  server.on("/sendConfig", saveConfig);
  server.on("/getWifi", getWifi);
  server.on("/returnConfigData", returnConfigData);
  server.on("/formatDevice", removeUserSettings);
  server.onNotFound([]() { server.send(404, "text/plain", "Error 404!");});
  server.begin();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); digitalWrite(LED_BUILTIN, HIGH); 
  Serial.begin(115200);
  SPIFFS.begin();
  if (loadConfig() ?  Serial.println("Config loaded") : Serial.println("Config doesn't exist"));
  handleInit();
}

void loop() {
  currentMillis = millis();
  server.handleClient(); 
  if(initialized) {
    ArduinoOTA.handle();
    webSocket.loop();
    if(startPulse) handlePulse();
    if(startBreething) Breathing();
    handleSave();   
  }
}