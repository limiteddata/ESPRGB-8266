#include <vector>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <LITTLEFS.h>
#include <SchedulerESP8266.h>
#include "../html/compressed/config.html.h"
#include "../html/compressed/index.html.h"
#define ESPRGB_VERSION "1.1.0.3"
#define reset_pin 16

Scheduler scheduler;
WebSocketsServer webSocket = WebSocketsServer(81); 
ESP8266WebServer server(80); 


bool startSaving = false;
unsigned long currentMillis = millis();;

void sendData(uint8_t num);
void saveConfig();
void saveUserData();
bool loadConfig();
void SetColorValue(int red,int green, int blue,float _brightness);

DynamicJsonDocument configJson(28672);

void userDefaultJson(){
  JsonObject Animations = configJson.createNestedObject("Animations");
  JsonObject parameters = Animations.createNestedObject("parameters");
  JsonObject SolidColor = parameters.createNestedObject("SolidColor");
  JsonArray SolidColor_Color = SolidColor.createNestedArray("Color");
  JsonObject ColorCycle = parameters.createNestedObject("ColorCycle");
  JsonObject Breathing = parameters.createNestedObject("Breathing");
  JsonArray _staticColorBreathing = Breathing.createNestedArray("staticColorBreathing");
  Breathing.createNestedArray("colorListBreathing");
  parameters.createNestedObject("Disco");
  JsonObject SolidDisco = parameters.createNestedObject("SolidDisco"); 
  JsonArray _colorSolidDisco = SolidDisco.createNestedArray("colorSolidDisco");
  JsonObject morseCode = parameters.createNestedObject("MorseCode");
  JsonArray _colorMorseCode = morseCode.createNestedArray("colorMorseCode");
  morseCode.createNestedObject("encodedMorseCode");
  morseCode.createNestedObject("unitTimeMorseCode");
  JsonObject Schedules = Animations.createNestedObject("Schedules");
  Schedules.createNestedArray("timeSchedule");  
  SolidColor_Color[0] = 1020;
  SolidColor_Color[1] = 0;
  SolidColor_Color[2] = 0;
  SolidColor["Brightness"] = 1.0; 
  ColorCycle["ColorCycleSpeed"] = 50;
  Breathing["breathingSpeed"] = 50;  
  Breathing["useColorList"] = false;                
  _staticColorBreathing[0] = 1020;
  _staticColorBreathing[1] = 0;
  _staticColorBreathing[2] = 0;
  _colorSolidDisco[0] = 1020;
  _colorSolidDisco[1] = 0;
  _colorSolidDisco[2] = 0;      
  morseCode["encodedMorseCode"] = "";
  morseCode["unitTimeMorseCode"] = 70;      
  morseCode["useBuzzer"] = false; 
  _colorMorseCode[0] = 1020;
  _colorMorseCode[1] = 0;
  _colorMorseCode[2] = 0;   
  Schedules["enableScheduler"] = false;
  Animations["powerConnected"] = false;  
  Animations["playingAnimation"] = "Solid Color";
  Animations["PowerState"] = true;
}
void defaultJson(){
  JsonObject ESP_Config = configJson.createNestedObject("ESP_Config");
  JsonObject Network_Config = configJson.createNestedObject("Network_Config");
  ESP_Config["HOSTNAME"] = "";
  ESP_Config["SSID"] = "";
  ESP_Config["PASSWORD"] = "";
  ESP_Config["REDPIN"] = 0;
  ESP_Config["GREENPIN"] = 0;
  ESP_Config["BLUEPIN"] = 0;
  ESP_Config["BUZZERPIN"] = 0;
  ESP_Config["Initialized"] = false;
  Network_Config["startStatic"] = false;
  Network_Config["local_IP"] = "";
  Network_Config["gateway"] = "";
  Network_Config["subnet"] = "";
  Network_Config["dns"] = "";
  userDefaultJson();
}
void restartESP(){
    Serial.println("Restarting ESP");
    server.send(200, "text/html", "Success"); 
    SetColorValue(0,0,0,0);
    delay(1000);
    ESP.restart();   
}
void removeUserData() {
    userDefaultJson();
    saveUserData();
    server.send(200, "text/html", "Success");
    SetColorValue(0,0,0,0);
    delay(1000);
    ESP.restart();   
}
void removeUserSettings(){
    LittleFS.format();
    server.send(200, "text/html", "Success");
    SetColorValue(0,0,0,0);
    delay(1000);
    ESP.restart();   
}

void handleHardReset(){
  int seconds=0;
  pinMode(reset_pin, INPUT);
  while(digitalRead(reset_pin) == HIGH){
    seconds+=1;   
    if(seconds >= 100){     
      digitalWrite(LED_BUILTIN, LOW); delay(1000); digitalWrite(LED_BUILTIN, HIGH);
      for (int i = 0; i < 70; i++)
      {
        digitalWrite(LED_BUILTIN, LOW); delay(40); 
        digitalWrite(LED_BUILTIN, HIGH); delay(40); 
        Serial.print(".");
      }    
      delay(1000);   
      Serial.println("Hard reset initialized");      
      removeUserSettings();
    } 
    delay(100);
  }
}

void SetColorValue(int red,int green, int blue,float _brightness){
  analogWrite(configJson["ESP_Config"]["REDPIN"].as<int>(), constrain(red*_brightness, 0, 1020)); 
  analogWrite(configJson["ESP_Config"]["GREENPIN"].as<int>(), constrain(green*_brightness, 0, 1020));
  analogWrite(configJson["ESP_Config"]["BLUEPIN"].as<int>(), constrain(blue*_brightness, 0, 1020));
}

bool startDisco;
void handleDisco(int r,int g, int b){
  if(startDisco) {
    SetColorValue(r,g,b,1);
  }
}

bool startSolidDisco;
void handleSolidDisco(float val){
  if(startSolidDisco) {
    SetColorValue(configJson["Animations"]["parameters"]["SolidDisco"]["colorSolidDisco"][0].as<int>(),configJson["Animations"]["parameters"]["SolidDisco"]["colorSolidDisco"][1].as<int>(),configJson["Animations"]["parameters"]["SolidDisco"]["colorSolidDisco"][2].as<int>(),val);
  }
}

bool startAmbilight;
void handleAmbilight(int r,int g, int b){
  if(startAmbilight) {
    SetColorValue(r,g,b,1);
  }
}

bool startMorseCode;
bool state=false;
unsigned long previousMorse = 0;
unsigned int morseIndex=0;
unsigned int currUnit;

bool morse_turn(bool turn){
   if(turn){
      SetColorValue(configJson["Animations"]["parameters"]["MorseCode"]["colorMorseCode"][0],configJson["Animations"]["parameters"]["MorseCode"]["colorMorseCode"][1],configJson["Animations"]["parameters"]["MorseCode"]["colorMorseCode"][2],1);
      if(configJson["Animations"]["parameters"]["MorseCode"]["useBuzzer"].as<bool>()) tone(configJson["ESP_Config"]["BUZZERPIN"].as<int>(), 850); 
   }else{
      SetColorValue(configJson["Animations"]["parameters"]["MorseCode"]["colorMorseCode"][0],configJson["Animations"]["parameters"]["MorseCode"]["colorMorseCode"][1],configJson["Animations"]["parameters"]["MorseCode"]["colorMorseCode"][2],0);
      noTone(configJson["ESP_Config"]["BUZZERPIN"].as<int>());
   }
   return turn;
}
void resetMorseCode(int unit){
    morseIndex = 0;
    state = false;
    noTone(configJson["ESP_Config"]["BUZZERPIN"].as<int>());
    currUnit = unit;
}
void handleMorseCode(){
  if(currentMillis - previousMorse >= currUnit) {
    previousMorse = currentMillis;    
    String encodedMessage = configJson["Animations"]["parameters"]["MorseCode"]["encodedMorseCode"].as<String>();
    if(encodedMessage != NULL,encodedMessage.length()>0) {      
      if(morseIndex<encodedMessage.length()){
        if(encodedMessage[morseIndex] == '.'){
            if(!state){
              state = morse_turn(true);
              currUnit = configJson["Animations"]["parameters"]["MorseCode"]["unitTimeMorseCode"].as<int>();          
            }else{
              state = morse_turn(false);
              currUnit = configJson["Animations"]["parameters"]["MorseCode"]["unitTimeMorseCode"].as<int>();            
              morseIndex++;
            }
        }
        else if(encodedMessage[morseIndex] == '-'){
            if(!state){
              currUnit = configJson["Animations"]["parameters"]["MorseCode"]["unitTimeMorseCode"].as<int>()*3;
              state = morse_turn(true);
            }else{   
              state = morse_turn(false);
              currUnit = configJson["Animations"]["parameters"]["MorseCode"]["unitTimeMorseCode"].as<int>();
              morseIndex++;
            }       
        }
        else  if(encodedMessage[morseIndex] == '*'){
          morse_turn(false);
          currUnit = configJson["Animations"]["parameters"]["MorseCode"]["unitTimeMorseCode"].as<int>()*3;
          morseIndex++;
        }
        else  if(encodedMessage[morseIndex] == '|'){
          morse_turn(false);
          currUnit = configJson["Animations"]["parameters"]["MorseCode"]["unitTimeMorseCode"].as<int>()*7;
          morseIndex++;
        }
      }
      else  resetMorseCode(configJson["Animations"]["parameters"]["MorseCode"]["unitTimeMorseCode"].as<int>()*25);   
    }
  }
  
}

bool startBreathing = false, inverseBool = true;
float breathingPulse=1.0f;
unsigned long breathingMillis;
size_t colorListindex = 0;
bool colorChanged = false;


void handleBreathing(){
  if (currentMillis - breathingMillis >= configJson["Animations"]["parameters"]["Breathing"]["breathingSpeed"].as<unsigned long>()) {
    breathingMillis = currentMillis;
      if(inverseBool) {
        breathingPulse=breathingPulse-0.005f;
        if(breathingPulse<=0.0f) {
          breathingPulse=0.0f;
          inverseBool = !inverseBool;
        }
        colorChanged = false;
      }else{
        breathingPulse=breathingPulse+0.005f;
        if(breathingPulse>=1.0f) {
          breathingPulse = 1.0f;
          inverseBool = !inverseBool;
        }
        if(configJson["Animations"]["parameters"]["Breathing"]["useColorList"].as<bool>() && !colorChanged) {
          colorListindex++;
          if(colorListindex+1 > configJson["Animations"]["parameters"]["Breathing"]["colorListBreathing"].as<JsonArray>().size()) colorListindex = 0;  
          colorChanged = true;
        }
      } 
      if(configJson["Animations"]["parameters"]["Breathing"]["useColorList"].as<bool>()){
        if(configJson["Animations"]["parameters"]["Breathing"]["colorListBreathing"].as<JsonArray>().size() > 0 )
          SetColorValue(configJson["Animations"]["parameters"]["Breathing"]["colorListBreathing"][colorListindex][0].as<int>(),configJson["Animations"]["parameters"]["Breathing"]["colorListBreathing"][colorListindex][1].as<int>(),configJson["Animations"]["parameters"]["Breathing"]["colorListBreathing"][colorListindex][2].as<int>(),breathingPulse);
        else SetColorValue(0,0,0,0);
      }   
      else SetColorValue(configJson["Animations"]["parameters"]["Breathing"]["staticColorBreathing"][0].as<int>(),configJson["Animations"]["parameters"]["Breathing"]["staticColorBreathing"][1].as<int>(),configJson["Animations"]["parameters"]["Breathing"]["staticColorBreathing"][2].as<int>(),breathingPulse);
  }
}

bool startColorCycle = false;
bool startBool[]={true,true,true};
int colorIndexColorCycle[]={1020,0,0};
unsigned long prevMillisColorCycle;

void handleColorCycle(){
   if(currentMillis - prevMillisColorCycle > configJson["Animations"]["parameters"]["ColorCycle"]["ColorCycleSpeed"].as<unsigned long>()){
    prevMillisColorCycle = currentMillis;
    if(startBool[0]==true && colorIndexColorCycle[0]>0){
      colorIndexColorCycle[0] = constrain(colorIndexColorCycle[0]-1, 0, 1020);
      if(colorIndexColorCycle[0]<=0) startBool[0]=false;
    }
    if(startBool[1]==true&&colorIndexColorCycle[0]<=750){
      colorIndexColorCycle[1] = constrain(colorIndexColorCycle[1]+1, 0, 1020);
      if(colorIndexColorCycle[1] >= 1020) startBool[1]=false;
    }
    if(startBool[1]==false && colorIndexColorCycle[1]>0) colorIndexColorCycle[1] = constrain(colorIndexColorCycle[1]-1, 0, 1020); 
    if(startBool[2]==true&&startBool[1]==false&&colorIndexColorCycle[1] < 750){
      colorIndexColorCycle[2] = constrain(colorIndexColorCycle[2]+1, 0, 1020);
      if(colorIndexColorCycle[2] >= 1020)  startBool[2] = false;
    }
    if(startBool[2]==false && colorIndexColorCycle[2]>0) colorIndexColorCycle[2] = constrain(colorIndexColorCycle[2]-1, 0, 1020);    
    if(startBool[2]==false&&startBool[0]==false&&colorIndexColorCycle[2] < 750){
        colorIndexColorCycle[0] = constrain(colorIndexColorCycle[0]+1, 0, 1020);
        if(colorIndexColorCycle[0] >= 1020) {
        startBool[0] = true; startBool[1] = true; startBool[2] = true;
        }
    }
    SetColorValue(colorIndexColorCycle[0],colorIndexColorCycle[1],colorIndexColorCycle[2],1);
   }
}

void turnOffAnimations(){
  startBreathing = false;
  startColorCycle = false;
  startDisco = false;
  startSolidDisco = false;
  startMorseCode = false;
  startAmbilight = false;
  resetMorseCode(0); 
  SetColorValue(0,0,0,0);
}

void setAnimation(String animation){
    if(configJson["Animations"]["playingAnimation"].as<String>() != animation){
      if(animation == "Power Off"){
        configJson["Animations"]["PowerState"] = false;
        turnOffAnimations();
        String payload = "{\"Animations\":{\"playingAnimation\":\"Power Off\",\"PowerState\":"+String(configJson["Animations"]["PowerState"].as<bool>() ? "true":"false")+"}}";
        webSocket.broadcastTXT(payload);   
        Serial.println("Playing: Power Off");
        return;
      }   
      else if(animation == "Power On"){
        animation = configJson["Animations"]["playingAnimation"].as<String>();
        configJson["Animations"]["PowerState"] = true;
      }else{
        configJson["Animations"]["playingAnimation"] = animation;
        configJson["Animations"]["PowerState"] = true;
        turnOffAnimations();
      }
      String payload = "{\"Animations\":{\"playingAnimation\":\""+configJson["Animations"]["playingAnimation"].as<String>()+"\",\"PowerState\":"+String(configJson["Animations"]["PowerState"].as<bool>() ? "true":"false")+"}}";
      webSocket.broadcastTXT(payload);  
      Serial.println("Playing: "+configJson["Animations"]["playingAnimation"].as<String>());
    }
    else
    {
      if (configJson["Animations"]["PowerState"] == false){
        configJson["Animations"]["PowerState"] = true;
        turnOffAnimations();
        String payload = "{\"Animations\":{\"playingAnimation\":\""+configJson["Animations"]["playingAnimation"].as<String>()+"\",\"PowerState\":"+String(configJson["Animations"]["PowerState"].as<bool>() ? "true":"false")+"}}";
        webSocket.broadcastTXT(payload);
        Serial.println("Playing: "+configJson["Animations"]["playingAnimation"].as<String>());
      }
    }
    
    if (animation == "Solid Color") SetColorValue(configJson["Animations"]["parameters"]["SolidColor"]["Color"][0].as<int>(),configJson["Animations"]["parameters"]["SolidColor"]["Color"][1].as<int>(),configJson["Animations"]["parameters"]["SolidColor"]["Color"][2].as<int>(),configJson["Animations"]["parameters"]["SolidColor"]["Brightness"].as<float>());
    else if (animation == "Color Cycle")
    {
        startBool[0] = true; startBool[1] = true; startBool[2] = true;
        colorIndexColorCycle[0] = 1020; colorIndexColorCycle[1] = 0; colorIndexColorCycle[2] = 0;
        startColorCycle = true;
    }
    else if(animation == "Breathing")
    {
        inverseBool = true; colorListindex = 0; colorChanged = true; breathingPulse=1.0f; 
        startBreathing = true;     
    }
    else if(animation == "Disco")  startDisco = true;
    else if(animation == "Solid Disco")  startSolidDisco = true; 
    else if(animation == "Morse Code")
    {
        resetMorseCode(0);             
        startMorseCode = true;
    }
    else if(animation == "Ambilight")  startAmbilight = true;
    else
    {
      // default 
      configJson["Animations"]["playingAnimation"] = "Solid Color";
      String payload = "{\"Animations\":{\"playingAnimation\":\""+configJson["Animations"]["playingAnimation"].as<String>()+"\"}}";
      webSocket.broadcastTXT(payload);
      SetColorValue(configJson["Animations"]["parameters"]["SolidColor"]["Color"][0].as<int>(),configJson["Animations"]["parameters"]["SolidColor"]["Color"][1].as<int>(),configJson["Animations"]["parameters"]["SolidColor"]["Color"][2].as<int>(),configJson["Animations"]["parameters"]["SolidColor"]["Brightness"].as<int>());
    }  
}

void setAnimationParameters(JsonObject &parameters){
    JsonObject configParam = configJson["Animations"]["parameters"];
    if (parameters.containsKey("SolidColor"))
    {        
      JsonObject _SolidColor = parameters["SolidColor"];
      if (_SolidColor.containsKey("Brightness")) configParam["SolidColor"]["Brightness"] = _SolidColor["Brightness"].as<float>();   
      if(_SolidColor.containsKey("Color")) {
        configParam["SolidColor"]["Color"][0] = _SolidColor["Color"][0].as<int>();
        configParam["SolidColor"]["Color"][1] = _SolidColor["Color"][1].as<int>();
        configParam["SolidColor"]["Color"][2] = _SolidColor["Color"][2].as<int>();
      }
    }       
    if (parameters.containsKey("ColorCycle"))
    {
      JsonObject _ColorCycle = parameters["ColorCycle"];
      if(_ColorCycle.containsKey("ColorCycleSpeed")) configParam["ColorCycle"]["ColorCycleSpeed"] = _ColorCycle["ColorCycleSpeed"].as<int>();  
    }
    
    if (parameters.containsKey("Breathing"))
    {
      JsonObject _Breathing = parameters["Breathing"];
      if(_Breathing.containsKey("staticColorBreathing")) {
        configParam["Breathing"]["staticColorBreathing"][0] = _Breathing["staticColorBreathing"][0].as<int>();
        configParam["Breathing"]["staticColorBreathing"][1] = _Breathing["staticColorBreathing"][1].as<int>();
        configParam["Breathing"]["staticColorBreathing"][2] = _Breathing["staticColorBreathing"][2].as<int>();
      }   
      if(_Breathing.containsKey("colorListBreathing")){
        configParam["Breathing"]["colorListBreathing"] = _Breathing["colorListBreathing"].as<JsonArray>();
      }     
      if(_Breathing.containsKey("addColortoList")) {
        if(configParam["Breathing"]["colorListBreathing"].size()!=25){
          configParam["Breathing"]["colorListBreathing"].add(_Breathing["addColortoList"].as<JsonArray>()); 
          String payload = "{\"Animations\":{\"parameters\":{\"Breathing\":{\"colorListBreathing\":"+configParam["Breathing"]["colorListBreathing"].as<String>()+"}}}}";
          webSocket.broadcastTXT(payload);
        }
      }
      if(_Breathing.containsKey("removeLastfromList")){
        if(configParam["Breathing"]["colorListBreathing"].size()!=0) {
          configParam["Breathing"]["colorListBreathing"].remove(configParam["Breathing"]["colorListBreathing"].size()-1);  
          String payload = "{\"Animations\":{\"parameters\":{\"Breathing\":{\"colorListBreathing\":"+configParam["Breathing"]["colorListBreathing"].as<String>()+"}}}}";
          webSocket.broadcastTXT(payload);
        }
      }
      if(_Breathing.containsKey("clearColorList")) {
        configParam["Breathing"]["colorListBreathing"] = configParam["Breathing"].createNestedArray("colorListBreathing");
        String payload = "{\"Animations\":{\"parameters\":{\"Breathing\":{\"colorListBreathing\":"+configParam["Breathing"]["colorListBreathing"].as<String>()+"}}}}";
        webSocket.broadcastTXT(payload);
      }
      if(_Breathing.containsKey("breathingSpeed")) configParam["Breathing"]["breathingSpeed"] = _Breathing["breathingSpeed"].as<int>();
      if(_Breathing.containsKey("useColorList")) configParam["Breathing"]["useColorList"] = _Breathing["useColorList"].as<bool>();
      inverseBool = true; colorListindex = 0; colorChanged = true; breathingPulse=1.0f; 
      }     
      if (parameters.containsKey("Disco"))
      {
        JsonObject _Disco = parameters["Disco"];
        if(_Disco.containsKey("colorDisco")) handleDisco(_Disco["colorDisco"][0].as<int>(), _Disco["colorDisco"][1].as<int>(), _Disco["colorDisco"][2].as<int>());            
      }        
      if (parameters.containsKey("SolidDisco"))
      {
        JsonObject _SolidDisco = parameters["SolidDisco"];
        if(_SolidDisco.containsKey("pulseSolidDisco")) handleSolidDisco(_SolidDisco["pulseSolidDisco"].as<float>());
        if(_SolidDisco.containsKey("colorSolidDisco")) { 
          configParam["SolidDisco"]["colorSolidDisco"][0] = _SolidDisco["colorSolidDisco"][0].as<int>();
          configParam["SolidDisco"]["colorSolidDisco"][1] = _SolidDisco["colorSolidDisco"][1].as<int>();
          configParam["SolidDisco"]["colorSolidDisco"][2] = _SolidDisco["colorSolidDisco"][2].as<int>();
        }             
      }  
      if (parameters.containsKey("MorseCode"))
      {
        JsonObject _MorseCode = parameters["MorseCode"];  
        if(_MorseCode.containsKey("useBuzzer")) configParam["MorseCode"]["useBuzzer"] = _MorseCode["useBuzzer"].as<bool>();
        if(_MorseCode.containsKey("unitTimeMorseCode")) {
          configParam["MorseCode"]["unitTimeMorseCode"] = _MorseCode["unitTimeMorseCode"].as<int>();
          resetMorseCode(0);
        }
        if(_MorseCode.containsKey("encodedMorseCode")) {    
          configParam["MorseCode"]["encodedMorseCode"] = _MorseCode["encodedMorseCode"].as<String>();
          resetMorseCode(0);
        }
        if(_MorseCode.containsKey("colorMorseCode")) { 
          configParam["MorseCode"]["colorMorseCode"][0] = _MorseCode["colorMorseCode"][0].as<int>();
          configParam["MorseCode"]["colorMorseCode"][1] = _MorseCode["colorMorseCode"][1].as<int>();
          configParam["MorseCode"]["colorMorseCode"][2] = _MorseCode["colorMorseCode"][2].as<int>();
        }     
      } 
      if (parameters.containsKey("Ambilight"))
      {
        JsonObject _Ambilight = parameters["Ambilight"];
        if(_Ambilight.containsKey("AmbilightColor")) handleAmbilight(_Ambilight["AmbilightColor"][0].as<int>(), _Ambilight["AmbilightColor"][1].as<int>(), _Ambilight["AmbilightColor"][2].as<int>());      
      } 
} 

void merge(const JsonObject dest, const JsonObject src) {
   for (auto kvp : src) {
    String key = kvp.key().c_str();
    if(kvp.value().is<JsonArray>()) dest[key] = kvp.value().as<JsonArray>();
    else if(kvp.value().is<float>()) dest[key] = kvp.value().as<float>();
    else if(kvp.value().is<int>()) dest[key] = kvp.value().as<int>();
    else if(kvp.value().is<bool>()) dest[key] = kvp.value().as<bool>();
    else if(kvp.value().is<String>()) dest[key] = kvp.value().as<String>();
    else if(kvp.value().is<JsonObject>()){
       JsonObject obj = dest.createNestedObject(key);
       merge(obj, src[key]);
    }
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if(type == WStype_CONNECTED){
      Serial.println("Client "+String(num)+" connected");  
      if(configJson["Animations"]["powerConnected"]) setAnimation("Power On");
      String config = "{\"Animations\":"+ configJson["Animations"].as<String>() + "}";  
      webSocket.sendTXT(num,config);
      startSaving = true;
  }
  if (type == WStype_TEXT)
  {
        if(webSocket.connectedClients() > 1) {
          for (int i = 0; i < webSocket.connectedClients(); i++)  if(i != num) webSocket.sendTXT(i,payload,length);
        }
        //Serial.printf("%s\n", payload);
        DynamicJsonDocument json(length+2548);   
        deserializeJson(json, payload);             
        JsonObject _Animations = json["Animations"];
        if (_Animations.containsKey("parameters")){
          JsonObject parameters = _Animations["parameters"];
          setAnimationParameters(parameters);
        }      
        if (_Animations.containsKey("Schedules")){  
          JsonObject _schedules = _Animations["Schedules"];   
          JsonArray timeSchedule = configJson["Animations"]["Schedules"]["timeSchedule"];
          if (_schedules.containsKey("timeSchedule")){            
              timeSchedule = configJson["Animations"]["Schedules"].createNestedArray("timeSchedule");
              scheduler.schedules.clear();
              for (size_t j = 0; j < _schedules["timeSchedule"].size(); j++)
              {
                JsonObject newSchedule = timeSchedule.createNestedObject();       
                newSchedule["Label"] = _schedules["timeSchedule"][j]["Label"].as<String>();
                newSchedule["Days"] = _schedules["timeSchedule"][j]["Days"].as<JsonArray>();
                newSchedule["Timestamp"] = _schedules["timeSchedule"][j]["Timestamp"].as<unsigned long>();
                newSchedule["playingAnimation"] = _schedules["timeSchedule"][j]["playingAnimation"].as<String>();
                JsonObject params = newSchedule.createNestedObject("parameters");
                merge(params, _schedules["timeSchedule"][j]["parameters"].as<JsonObject>());
                newSchedule["enable"] = _schedules["timeSchedule"][j]["enable"].as<bool>();
                bool days[7];
                for (size_t i = 0; i < 7; i++) days[i] = _schedules["timeSchedule"][j]["Days"][i];      
                ScheduleTask task = ScheduleTask(newSchedule["Label"], days, newSchedule["Timestamp"],  
                  [=](){    
                    for (JsonArray::iterator it=timeSchedule.begin(); it!=timeSchedule.end(); ++it) {
                      if ((*it)["Timestamp"] == newSchedule["Timestamp"]) {
                        JsonObject params = (*it)["parameters"];
                        if(params.size()>0) setAnimationParameters(params);
                        setAnimation(newSchedule["playingAnimation"]); 
                        String payload = "{\"Animations\":{\"parameters\":" + (*it)["parameters"].as<String>()+"}}";
                        webSocket.broadcastTXT(payload);
                        break;                     
                      }
                    }
                  });
                task.enable = newSchedule["enable"];
                scheduler.add(task);
              }         
              String payload = "{\"Animations\":{\"Schedules\":{\"timeSchedule\":"+configJson["Animations"]["Schedules"]["timeSchedule"].as<String>()+"}}}";
              webSocket.broadcastTXT(payload); 
          }

          if (_schedules.containsKey("newTimeSchedule")){
            if (timeSchedule.size()<10){              
              JsonObject newSchedule = timeSchedule.createNestedObject();       
              newSchedule["Label"] = _schedules["newTimeSchedule"]["Label"].as<String>();
              newSchedule["Days"] = _schedules["newTimeSchedule"]["Days"].as<JsonArray>();
              newSchedule["Timestamp"] = _schedules["newTimeSchedule"]["Timestamp"].as<unsigned long>();
              newSchedule["playingAnimation"] = _schedules["newTimeSchedule"]["playingAnimation"].as<String>();
              JsonObject params = newSchedule.createNestedObject("parameters");
              merge(params, _schedules["newTimeSchedule"]["parameters"].as<JsonObject>());
              newSchedule["enable"] = _schedules["newTimeSchedule"]["enable"].as<bool>();

              bool days[7];
              for (size_t i = 0; i < 7; i++) days[i] = newSchedule["Days"][i];      
              ScheduleTask task = ScheduleTask(newSchedule["Label"], days, newSchedule["Timestamp"],  
                [=](){    
                  for (JsonArray::iterator it=timeSchedule.begin(); it!=timeSchedule.end(); ++it) {
                    if ((*it)["Timestamp"] == newSchedule["Timestamp"]) {
                      JsonObject params = (*it)["parameters"];
                      if(params.size()>0) setAnimationParameters(params);
                      setAnimation(newSchedule["playingAnimation"]);
                      String payload = "{\"Animations\":{\"parameters\":" + (*it)["parameters"].as<String>()+"}}";
                      webSocket.broadcastTXT(payload);  
                      break;                     
                    }
                  }
                });
              task.enable = newSchedule["enable"];
              scheduler.add(task);
              String payload = "{\"Animations\":{\"Schedules\":{\"timeSchedule\":"+configJson["Animations"]["Schedules"]["timeSchedule"].as<String>()+"}}}";
              webSocket.broadcastTXT(payload);       
            }
            else{
              Serial.println("Can't add more than 10 time schedules!");
            }  
          }
          
          if (_schedules.containsKey("removeTimeSchedule")){
            JsonObject removeSchedule = _schedules["removeTimeSchedule"]; 
            for (size_t i = 0; i < configJson["Animations"]["Schedules"]["timeSchedule"].size(); i++)
            {
              if (configJson["Animations"]["Schedules"]["timeSchedule"][i]["Timestamp"] == removeSchedule["Timestamp"]){
                configJson["Animations"]["Schedules"]["timeSchedule"].remove(i);
                scheduler.Remove(i);
                String payload = "{\"Animations\":{\"Schedules\":{\"timeSchedule\":"+configJson["Animations"]["Schedules"]["timeSchedule"].as<String>()+"}}}";
                webSocket.broadcastTXT(payload);   
                break;
              }
            }
          }

          if (_schedules.containsKey("editTimeSchedule")){          
            if (_schedules["editTimeSchedule"].containsKey("newData") && _schedules["editTimeSchedule"].containsKey("oldTimestamp")) {
              JsonObject newdata = _schedules["editTimeSchedule"]["newData"]; 

              for (size_t i = 0; i < timeSchedule.size(); i++)
              {
                if (timeSchedule[i]["Timestamp"] == _schedules["editTimeSchedule"]["oldTimestamp"]){
                  if (newdata.containsKey("Label")) {
                    timeSchedule[i]["Label"] = newdata["Label"].as<String>();
                    scheduler.schedules[i].label = newdata["Label"].as<String>();
                  }
                  if (newdata.containsKey("Days")) {                  
                    timeSchedule[i]["Days"] = newdata["Days"].as<JsonArray>();           
                    for (size_t x = 0; x < 7; x++) scheduler.schedules[i].days[x] = newdata["Days"][x].as<bool>();
                  }
                  if (newdata.containsKey("Timestamp")) {
                    timeSchedule[i]["Timestamp"] = newdata["Timestamp"].as<unsigned long>();
                    scheduler.schedules[i].timestamp = newdata["Timestamp"].as<unsigned long>();
                    scheduler.schedules[i].taskCompleted = false;
                  }  
                  if (newdata.containsKey("playingAnimation") && newdata.containsKey("parameters")) {             
                    timeSchedule[i]["playingAnimation"] = newdata["playingAnimation"].as<String>();
                    JsonObject _params = timeSchedule[i].createNestedObject("parameters");
                    merge(_params, newdata["parameters"].as<JsonObject>());

                    unsigned long timestamp = timeSchedule[i]["Timestamp"];
                    scheduler.schedules[i].toDoTask = [=](){     
                      for (JsonArray::iterator it=timeSchedule.begin(); it!=timeSchedule.end(); ++it) {
                        if ((*it)["Timestamp"] == timestamp) {
                          JsonObject params = (*it)["parameters"];
                          if(params.size()>0) setAnimationParameters(params);
                          setAnimation(timeSchedule[i]["playingAnimation"].as<String>()); 
                          String payload = "{\"Animations\":{\"parameters\":" + (*it)["parameters"].as<String>()+"}}";
                          webSocket.broadcastTXT(payload);   
                          break;                     
                        }
                      }
                    };   
                  }
                  if (newdata.containsKey("enable")) {
                    timeSchedule[i]["enable"] = newdata["enable"].as<bool>();
                    scheduler.schedules[i].enable = newdata["enable"].as<bool>();
                  }
                  String payload = "{\"Animations\":{\"Schedules\":{\"timeSchedule\":"+configJson["Animations"]["Schedules"]["timeSchedule"].as<String>()+"}}}";
                  webSocket.broadcastTXT(payload);   
                  break;
                }
              }
            }
          }
          
          
          if (_schedules.containsKey("enableScheduler")) configJson["Animations"]["Schedules"]["enableScheduler"] = _schedules["enableScheduler"].as<bool>();
        }

        if (_Animations.containsKey("powerConnected")) configJson["Animations"]["powerConnected"] = _Animations["powerConnected"];
        if (_Animations.containsKey("playingAnimation")) setAnimation(_Animations["playingAnimation"]);   
        if (json.containsKey("command")) {
            if(json["command"] == "formatDevice") removeUserSettings();
            if(json["command"] == "restartAnimation") {
              setAnimation(configJson["Animations"]["playingAnimation"]);
            }
        }
        json.clear();
  }
  if(type == WStype_DISCONNECTED){
    Serial.println("Client "+String(num)+" disconnected");  
    if(webSocket.connectedClients() == 0) {
      if(configJson["Animations"]["powerConnected"]) setAnimation("Power Off");
      startSaving = false;
    }
    saveUserData();
  }
}

void combine(JsonDocument& dst, const JsonDocument& src) {
    for (auto p : src.as<JsonObject>()) dst[p.key()] = p.value();
}

void saveConfig(){  
  DynamicJsonDocument recv_json(1024);
  deserializeJson(recv_json, server.arg("plain"));
  
  // try recived config
  if(configJson["ESP_Config"]["Initialized"].as<bool>() && recv_json["ESP_Config"]["PASSWORD"].as<String>() == "**********") 
   recv_json["ESP_Config"]["PASSWORD"] = configJson["ESP_Config"]["PASSWORD"].as<String>();
      
  Serial.println("Testing recived led pins");
  analogWrite(recv_json["ESP_Config"]["REDPIN"], 1020); digitalWrite(LED_BUILTIN, LOW); delay(1000); analogWrite(recv_json["ESP_Config"]["REDPIN"], 0); digitalWrite(LED_BUILTIN, HIGH);delay(1000);
  analogWrite(recv_json["ESP_Config"]["GREENPIN"], 1020); digitalWrite(LED_BUILTIN, LOW); delay(1000); analogWrite(recv_json["ESP_Config"]["GREENPIN"], 0); digitalWrite(LED_BUILTIN, HIGH);delay(1000);
  analogWrite(recv_json["ESP_Config"]["BLUEPIN"], 1020); digitalWrite(LED_BUILTIN, LOW); delay(1000); analogWrite(recv_json["ESP_Config"]["BLUEPIN"], 0); digitalWrite(LED_BUILTIN, HIGH);delay(1000);
  tone(recv_json["ESP_Config"]["BUZZERPIN"], 850); digitalWrite(LED_BUILTIN, LOW); delay(1000); noTone(recv_json["ESP_Config"]["BUZZERPIN"]); digitalWrite(LED_BUILTIN, HIGH); delay(1000);

  Serial.println("Testing recived credentials");
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  WiFi.begin(recv_json["ESP_Config"]["SSID"].as<String>(), recv_json["ESP_Config"]["PASSWORD"].as<String>());  
  int timeout=0;
  while(WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, HIGH); delay(250); digitalWrite(LED_BUILTIN, LOW);
    Serial.print('.'); 
    timeout++;
    if(timeout >= 300) break;
  }
  
  if(WiFi.isConnected()){
    recv_json["ESP_Config"]["Initialized"] = true;
    File configFile = LittleFS.open("/user_settings.json", "w+");
    combine(configJson, recv_json);
    serializeJson(configJson, configFile);
    configFile.close();
  }
  recv_json.clear();
  ESP.restart();
}

bool loadConfig() {

  File configFile = LittleFS.open("/user_settings.json", "r");
  if (!configFile) return false;
  deserializeJson(configJson, configFile);
  if(configJson["Animations"].containsKey("Schedules")){
    JsonObject sch = configJson["Animations"]["Schedules"];
    if(sch.containsKey("timeSchedule")){
      JsonArray timeSchedule = sch["timeSchedule"];
      for (size_t j = 0; j < timeSchedule.size(); j++)
      {
        bool days[7];
        for (size_t i = 0; i < 7; i++) days[i] = timeSchedule[j]["Days"][i];      
        ScheduleTask task = ScheduleTask(timeSchedule[j]["Label"], days, timeSchedule[j]["Timestamp"],  
          [=](){    
            for (JsonArray::iterator it=timeSchedule.begin(); it!=timeSchedule.end(); ++it) {
              if ((*it)["Timestamp"] == timeSchedule[j]["Timestamp"]) {
                JsonObject params = (*it)["parameters"];
                if(params.size()>0) setAnimationParameters(params);
                setAnimation(timeSchedule[j]["playingAnimation"]); 
                String payload = "{\"Animations\":{\"parameters\":" + (*it)["parameters"].as<String>()+"}}";
                webSocket.broadcastTXT(payload);   
                break;                     
              }
            }
          });
        task.enable = timeSchedule[j]["enable"];
        scheduler.add(task);
      }   
    }
  }        
  configFile.close();
  return true;
}

void saveUserData(){
    File configFile = LittleFS.open("/user_settings.json", "w+");
    serializeJson(configJson, configFile);
    configFile.close(); 
}

unsigned long previousMillis2 = 0;   

void handleSave(){
  if (currentMillis - previousMillis2 >= 15000) {
      previousMillis2 = currentMillis;
      for (int i = 0; i < webSocket.connectedClients(); i++) webSocket.sendPing(i);
      saveUserData();
  }
}

void pinTester(){
  int pin = server.arg("plain").toInt();
  tone(pin, 900, 1000); 
  server.send(200, "text/html", "Pin "+String(pin)+" has been set HIGH for 1 sec"); 
}
void getVersion(){
  server.send(200, "application/json", "{\"ESPRGB_VERSION\":\""+String(ESPRGB_VERSION)+"\"}");
}
void syncData(){
  server.send(200, "application/json", "{\"Animations\":"+configJson["Animations"].as<String>()+"}"); 
}

void getSignalStrenght(){
  String data;
  DynamicJsonDocument doc(100);
  doc["RSSI"] = WiFi.RSSI();
  serializeJson(doc, data);
  doc.clear();
  server.send(200, "application/json", data); 
}

void returnConfigData(){
  String data;
  DynamicJsonDocument doc(1024);
  doc["Initialized"] = configJson["ESP_Config"]["Initialized"];
  if(configJson["ESP_Config"]["Initialized"]){
      doc["SSID"] = configJson["ESP_Config"]["SSID"];
      if(configJson["ESP_Config"]["PASSWORD"] != "") doc["PASSWORD"] = "**********";
      else doc["PASSWORD"] = "";
      doc["HOSTNAME"] = configJson["ESP_Config"]["HOSTNAME"];  
      doc["REDPIN"] = configJson["ESP_Config"]["REDPIN"]; 
      doc["GREENPIN"] = configJson["ESP_Config"]["GREENPIN"]; 
      doc["BLUEPIN"] = configJson["ESP_Config"]["BLUEPIN"]; 
      doc["BUZZERPIN"] = configJson["ESP_Config"]["BUZZERPIN"]; 
      doc["startStatic"] = configJson["Network_Config"]["startStatic"];
      doc["local_IP"] = configJson["Network_Config"]["local_IP"];
      doc["gateway"] = configJson["Network_Config"]["gateway"];
      doc["subnet"] = configJson["Network_Config"]["subnet"];
      doc["dns"] = configJson["Network_Config"]["dns"];
  }
  serializeJson(doc, data);
  doc.clear();
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
  doc.clear();
  server.send(200, "application/json", data); 
}

bool startSTA(){
    pinMode(configJson["ESP_Config"]["REDPIN"].as<int>(),OUTPUT);
    pinMode(configJson["ESP_Config"]["GREENPIN"].as<int>(),OUTPUT);
    pinMode(configJson["ESP_Config"]["BLUEPIN"].as<int>(),OUTPUT);
    WiFi.mode(WIFI_STA);  
    WiFi.hostname(configJson["ESP_Config"]["HOSTNAME"].as<String>());
    if(configJson["Network_Config"]["startStatic"].as<bool>()) {
      IPAddress local_IP = IPAddress();
      local_IP.fromString(configJson["Network_Config"]["local_IP"].as<String>());
      IPAddress gateway = IPAddress();
      gateway.fromString(configJson["Network_Config"]["gateway"].as<String>());
      IPAddress subnet = IPAddress();
      subnet.fromString(configJson["Network_Config"]["subnet"].as<String>());
      IPAddress dns = IPAddress();
      dns.fromString(configJson["Network_Config"]["dns"].as<String>());
      WiFi.config(local_IP, gateway, subnet, dns);  
    }
    WiFi.begin(configJson["ESP_Config"]["SSID"].as<String>(), configJson["ESP_Config"]["PASSWORD"].as<String>());  
    while(WiFi.status() != WL_CONNECTED){
      digitalWrite(LED_BUILTIN, HIGH); delay(250); digitalWrite(LED_BUILTIN, LOW);
      Serial.print("."); 
    }
    Serial.println("");

    if(MDNS.begin(configJson["ESP_Config"]["HOSTNAME"].as<String>())){
      Serial.println("http://"+String(wifi_station_get_hostname())+".local : "+WiFi.localIP().toString());
      MDNS.addService("esprgb", "tcp", 80);
    } 
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    
    ArduinoOTA.setHostname(configJson["ESP_Config"]["HOSTNAME"].as<char*>());
    ArduinoOTA.onStart([]() {
      SetColorValue(0,0,0,0);
    });
    ArduinoOTA.begin();
    
    scheduler.begin();

    server.on("/", []() {
      server.sendHeader("Content-Encoding", "gzip");
      server.send(200, "text/html", (const char*)index_html_gz,index_html_gz_len);
    }); 
    
    server.on("/config", []() {
      server.sendHeader("Content-Encoding", "gzip");
      server.send(200, "text/html", (const char*)config_html_gz,config_html_gz_len);
    }); 
    digitalWrite(LED_BUILTIN, HIGH); 
    return true;
}

bool startAP(){  
    unsigned char mac[6];
    char dataString[50] = {0};    
    WiFi.macAddress(mac);
    sprintf(dataString, "ESPRGB_Config(%02X:%02X:%02X)",mac[3],mac[4],mac[5]);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(192,168,1,1), IPAddress(192,168,1,1), IPAddress(255, 255, 255, 0));
    WiFi.softAP(dataString);
    Serial.println(WiFi.softAPIP());

    server.on("/", []() {
      server.sendHeader("Content-Encoding", "gzip");
      server.send(200, "text/html", (const char*)config_html_gz,config_html_gz_len);
    });       

    digitalWrite(LED_BUILTIN, LOW);
    defaultJson();
    return true;
}


void handleInit(){
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  if(configJson["ESP_Config"]["Initialized"].as<bool>()){
    Serial.println("ESP Started in station mode.");
    if (!startSTA()) startAP();
  }else{
    Serial.println("ESP Started in ap mode.");
    startAP();
  }  
  server.enableCORS(true); 
  server.on("/pinTester",  HTTP_OPTIONS, []() { server.sendHeader("access-control-allow-credentials", "false"); server.send(204); });
  server.on("/pinTester", pinTester);
  server.on("/sendConfig",  HTTP_OPTIONS, []() { server.sendHeader("access-control-allow-credentials", "false"); server.send(204); });
  server.on("/sendConfig", saveConfig);
  server.on("/getWifi",  HTTP_OPTIONS, []() { server.sendHeader("access-control-allow-credentials", "false"); server.send(204); });
  server.on("/getWifi", getWifi);
  server.on("/returnConfigData",  HTTP_OPTIONS, []() { server.sendHeader("access-control-allow-credentials", "false"); server.send(204); });
  server.on("/returnConfigData", returnConfigData);
  server.on("/formatDevice",  HTTP_OPTIONS, []() { server.sendHeader("access-control-allow-credentials", "false"); server.send(204); });
  server.on("/formatDevice", removeUserSettings);
  server.on("/removeUserData",  HTTP_OPTIONS, []() { server.sendHeader("access-control-allow-credentials", "false"); server.send(204); });
  server.on("/removeUserData", removeUserData);
  server.on("/syncData",  HTTP_OPTIONS, []() { server.sendHeader("access-control-allow-credentials", "false"); server.send(204); });
  server.on("/syncData", syncData);
  server.on("/restartESP",  HTTP_OPTIONS, []() { server.sendHeader("access-control-allow-credentials", "false"); server.send(204); });
  server.on("/restartESP", restartESP);
  server.on("/getSignalStrenght",  HTTP_OPTIONS, []() { server.sendHeader("access-control-allow-credentials", "false"); server.send(204); });
  server.on("/getSignalStrenght", getSignalStrenght);
  server.on("/getVersion",  HTTP_OPTIONS, []() { server.sendHeader("access-control-allow-credentials", "false"); server.send(204); });
  server.on("/getVersion", getVersion);
  server.onNotFound([]() { server.send(404, "text/plain", "Error 404!");});
  server.begin();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); digitalWrite(LED_BUILTIN, HIGH); 
  Serial.begin(115200);     
  LittleFS.begin();
  handleHardReset();
  if (loadConfig() ?  Serial.println("Config loaded") : Serial.println("Config doesn't exist"));
  handleInit();
}
unsigned long flashmillis;
void loop() { 
  currentMillis = millis();
  server.handleClient(); 
  if(configJson["ESP_Config"]["Initialized"].as<bool>()) {
    ArduinoOTA.handle();
    webSocket.loop();
    if(configJson["Animations"]["Schedules"]["enableScheduler"].as<bool>()) scheduler.update();       
    if(startBreathing) handleBreathing();
    if(startColorCycle) handleColorCycle();
    if(startMorseCode) handleMorseCode();
    if(startSaving) handleSave();   
  }  
}