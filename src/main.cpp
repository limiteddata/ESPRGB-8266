#include <vector>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include "FS.h"
#define reset_pin 16

WebSocketsServer webSocket = WebSocketsServer(81); 
ESP8266WebServer server(80); 

const char configPage[] PROGMEM= R"=====(
<html>
<head>
  <title>ESP | Config</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
        *{box-sizing: inherit;font-family: Montserrat, sans-serif; font-weight: 400}
        .btn{ color: #fff; background: #06d79c; border: 1px solid #06d79c; cursor: pointer; width:230px; height: 25px; border-radius:4px; padding:4px; margin:15px auto; display:block;}
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
            <form method="post" id="postForm" onsubmit="saveConfig();return false;">
                <h4 style="text-align:center; color: #fff;">Wifi credentials</h4>
                <input id="SSID" oninput="passwordBehaviour(this)" class="inputStyle" type="text" placeholder="SSID" name="product" list="ssidList" required="">
                <datalist id="ssidList"></datalist>
                <input class="inputStyle" id="PASSWORD" placeholder="PASSWORD" name="PASSWORD" type="password" required="">
                <h4 style="text-align:center; color: #fff;">Device details</h4>
                <div>
                  <p style="display:inline-block; color:white; margin-left:15px;">esprgb-</p>
                  <input class="inputStyle" id="HOSTNAME" placeholder="HOSTNAME" name="HOSTNAME" type="text" style="display:inline-block; width:170px" required="">
                </div>
                <h4 style="text-align:center; color: #fff;">Device pinout</h4>
                <div style="color: #fff; font-size:12pt;">
                  <label style="display: inline-block; margin-left:15px;width:80px;" for="pins">Red pin:</label>
                  <select class="inputStyle" style="display:inline-block;width:75px; height:20px; margin:5 auto;" id="REDPIN" name="REDPIN" onchange="disableOption()" required="">
                    <option value="0">0</option><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="4">4</option><option value="5">5</option>
                    <option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option>
                  </select>
                  <button style="display: inline-block; width:60px; height:20px; font-size:9pt; margin:5 auto; " class="btn" onclick="testPin(document.getElementById('REDPIN').value)" type="button">TEST</button>
                </div>
                <div style="color: #fff; font-size:12pt;">
                  <label style="display: inline-block; margin-left:15px; width:80px;" for="pins">Green pin:</label>
                  <select class="inputStyle" style="display:inline-block;width:75px; height:20px; margin:5 auto;" id="GREENPIN" name="GREENPIN" onchange="disableOption()" required="">
                    <option value="0">0</option><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="4">4</option><option value="5">5</option>
                    <option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option>
                  </select>
                  <button style="display: inline-block; width:60px; height:20px; font-size:9pt; margin:5 auto;" class="btn" onclick="testPin(document.getElementById('GREENPIN').value)" type="button">TEST</button>
                </div>
                <div style="color: #fff; font-size:12pt;">
                  <label style="display: inline-block; margin-left:15px; width:80px;" for="pins">Blue pin:</label>
                  <select class="inputStyle" style="display:inline-block;width:75px; height:20px; margin:5 auto;" id="BLUEPIN" name="BLUEPIN" onchange="disableOption()" required="">
                    <option value="0">0</option><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="4">4</option><option value="5">5</option>
                    <option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option>
                  </select>
                  <button style="display: inline-block; width:60px; height:20px; font-size:9pt; margin:5 auto;" class="btn" onclick="testPin(document.getElementById('BLUEPIN').value)" type="button">TEST</button>
                </div>
                <div style="color: #fff; font-size:12pt;">
                  <label style="display: inline-block; margin-left:15px; width:80px; font-size:11pt;" for="pins">Buzzer pin:</label>
                  <select class="inputStyle" style="display:inline-block;width:75px; height:20px; margin:5 auto;" id="BUZZERPIN" name="BUZZERPIN" onchange="disableOption()" required="">
                    <option value="0">0</option><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="4">4</option><option value="5">5</option>
                    <option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option>
                  </select>
                  <button style="display: inline-block; width:60px; height:20px; font-size:9pt; margin:5 auto;" class="btn" onclick="testPin(document.getElementById('BUZZERPIN').value)" type="button">TEST</button>
                </div>
                <h4 style="text-align:center; color: #fff; margin:2px;" id="pinResponse"></h4>
                <h4 style="text-align:center; color: #fff; display:inline-block; margin-left:15px;">Static ipaddress (optional)</h4> <input onchange="staticToggle(this.checked)" style="border-radius:4px" id="startStatic" name="startStatic" type="checkbox">
                <input class="inputStyle" id="local_IP" placeholder="IPAddress ex. 192.168.1.50" name="local_IP" type="text" disabled="" required="">
                <input class="inputStyle" id="gateway" placeholder="Gateway ex. 192.168.1.1" name="gateway" type="text" disabled="" required="">
                <input class="inputStyle" id="subnet" placeholder="subnet ex. 255.255.255.0" name="subnet" type="text" disabled="" required="">
                <input class="inputStyle" id="dns" placeholder="dns ex 8.8.8.8" name="dns" type="text" disabled="" required="">
                <button style="width:230px" class="btn" type="submit">Send</button>
                </form>
          </div>
  <script>
    function saveConfig() {
      if(confirm("Are you sure you want to save this config?"))
      {
        var data ={"ESP_Config":{
                  "HOSTNAME": "esprgb-"+document.getElementById("HOSTNAME").value,
                  "SSID": document.getElementById("SSID").value,
                  "PASSWORD":document.getElementById("PASSWORD").value,
                  "REDPIN":parseInt(document.getElementById("REDPIN").value),
                  "GREENPIN":parseInt(document.getElementById("GREENPIN").value),
                  "BLUEPIN":parseInt(document.getElementById("BLUEPIN").value),
                  "BUZZERPIN":parseInt(document.getElementById("BUZZERPIN").value)
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
    }
    function staticToggle(value){
      document.getElementById("startStatic").checked = value;
      document.getElementById("local_IP").disabled = !value;
      document.getElementById("gateway").disabled = !value;
      document.getElementById("subnet").disabled = !value;
      document.getElementById("dns").disabled = !value;
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
    function disableOption() {
      var a = [document.getElementById("REDPIN"),document.getElementById("GREENPIN"),document.getElementById("BLUEPIN"),document.getElementById("BUZZERPIN")];
      var usedValues = [a[0].selectedIndex,a[1].selectedIndex,a[2].selectedIndex,a[3].selectedIndex];
      for (var i = 0; i < a.length; i++) {
          for (var j = 0; j < a[i].options.length; j++) a[i].options[j].disabled = false;
          for (var j = 0; j < usedValues.length; j++) a[i].options[usedValues[j]].disabled = true;
      }
    }
     function testPin(e){
         var xhr = new XMLHttpRequest();
         xhr.open("POST","/pinTester",true);
         xhr.send(e);
         xhr.onload=function(){document.getElementById("pinResponse").innerText = xhr.responseText;}
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
              document.getElementById("BUZZERPIN").value = data.BUZZERPIN;
              staticToggle(data.startStatic);
              document.getElementById("local_IP").value = data.local_IP;
              document.getElementById("gateway").value = data.gateway;
              document.getElementById("subnet").value = data.subnet;
              document.getElementById("dns").value = data.dns;
              if(data.PASSWORD == "") document.getElementById("PASSWORD").disabled = true;
            }
            disableOption();
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
  <title>ESPRGB | Home</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://kit.fontawesome.com/a076d05399.js"></script>
  <script src="https://cdn.jsdelivr.net/npm/@jaames/iro@5"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
    <style>
        *{margin: 0; padding: 0}
        body {font-family: "Lato", sans-serif; background-color: #151b29}
        #main_wrapper{width: 750px;height: 510px;display: block;margin: 100 auto;background-color: #152036;}
        #wrapper{width: 100%; height: 100%;background-color: #1b2a47;color: white;}
        .tab {float: left;background-color: #1b2a47; width: 15%;height: 100%; text-align: center;color:white; font-size:11px;}
        .tab button {background-color: inherit;color: white;padding: 15px 0px;width: 100%;border: none;text-align: center;font-size: 13px;}
        .tab button.active,button:hover {background-color: #06d79c;}
        .tabcontent {padding: 15px;width: 80%;height: 90%;background-color: #1b2a47;float: left;font-size: 13px;}
        .btn{width: 80px; height: 90px; background-color: #2e2e36; color:white; border: none;font-size:11px; margin:0 8;}
        input[type="range"] {-webkit-appearance: none;overflow: hidden;height: 18px;width: 350px;cursor: pointer;}
        input[type="checkbox"] {cursor: pointer;width: 18px;height: 18px;}
        ::-webkit-slider-runnable-track {background: #ddd;}
        ::-webkit-slider-thumb {-webkit-appearance: none;width: 15px;height: 20px;background: #fff;border: 1px solid gray;box-shadow: -100vw 0 0 100vw #06d79c}
        #btn-bg{width: 52px;height: 52px;border-radius: 50%;position: relative;cursor: pointer;left:400px;top:-80px;background: #cdd3c9;}
        #btn-ring{position: absolute;width: 26px;height: 26px;border: 7px solid #2a2a2a;top: 6px;left: 6px;border-radius: 31px}
        #ring-line{width: 8px;height: 25px;margin: -10px auto;background: #2a2a2a;border-radius: 6px;border-right: 3px solid #eee;border-left: 3px solid #cdd3c9}
        .active #ring-line{border-right: 3px solid #06d79c;border-left: 3px solid #06d79c;}
        #btn-bg.active{background: #06d79c;}
        #powerConected{width: 38px;height: 38px;left:21px;top:0px; background-color:white;border-radius: 50%;position: relative;cursor: pointer;}
        #powerConected.active{background: #06d79c;}
    </style>
    <style data="inputstyle"></style>
  </head>
  <body>
    <div id="main_wrapper">
      <p style="padding:15px 15px; color:#06d79c; font-size:20px;" id="title">ESPRGB-Testtest</p>
      <div id="wrapper">
        <div class="tab">
          <button class="tablinks" onclick="selectTab(event, 'Solid_Color')" id="defaultOpen" ondblclick="selectAnimation('Solid Color')">Solid Color</button>
          <button class="tablinks" onclick="selectTab(event, 'Color_Cycle')" ondblclick="selectAnimation('Color Cycle')">Color Cycle</button>
          <button class="tablinks" onclick="selectTab(event, 'Breathing')" ondblclick="selectAnimation('Breathing')">Breathing</button>
          <button class="tablinks" onclick="selectTab(event, 'Disco')">Disco</button>
          <button class="tablinks" onclick="selectTab(event, 'SolidDisco')" >Solid Disco</button>
          <button class="tablinks" onclick="selectTab(event, 'MorseCode')" ondblclick="selectAnimation('Morse Code')">Morse Code</button>
          <button class="tablinks" onclick="selectTab(event, 'Ambilight')" >Ambilight</button>
          <button class="tablinks" onclick="selectTab(event, 'Schedule')" >Schedule</button>
          <button class="tablinks" onclick="selectTab(event, 'Other')">Other</button>
          <p style="margin-top:15px">Signal:</p><p id="signalStrenght"></p>
          <p style="margin-top:15px">Playing:</p><p id="playingAnimation" style="width:90px; margin:0 auto;"></p>
        </div>
        <div id="Solid_Color" class="tabcontent">
          <p>Solid Color</p>
          <div style="margin-top: 65px;text-align:center; font-size:12px">
            <div id="solidColor" style="-webkit-box-align:center;-webkit-box-pack:center;display:-webkit-box;"></div>
            <input  type="range" min="0" max="100" value="100" style="width:300px; margin-top:20px" id="brightness" oninput='sendBrightness(this)'>
          </div>
            <div id="btn-bg" onclick="checkboxToggle()"><div id="btn-ring"><div id="ring-line"></div></div></div>
        </div>
        <div id="Color_Cycle" class="tabcontent">
          <p>Color Cycle</p>
          <div style="margin-top: 20px;text-align:center;font-size:12px">
            <input  type="range" min="1" max="200" value="1" id="ColorCycleSpeed_input" oninput="ws.send(JSON.stringify({'Animations':{'ColorCycle':{'ColorCycleSpeed':this.value}}})); document.getElementById('ColorCycleSpeed').innerText = this.value;">
            <input type="checkbox" id="startColorCycle" oninput="if(this.checked){selectAnimation('Color Cycle')} else {selectAnimation('Solid Color')}">
            <p>Speed:<i id="ColorCycleSpeed">1</i></p>
          </div>
        </div>
        <div id="Breathing" class="tabcontent">
          <p>Breathing</p>
          <div style="margin-top: 20px;text-align:center; font-size:12px">
            <input type="range" min="1" max="200" value="1" id="breathingSpeed_input" oninput="ws.send(JSON.stringify({'Animations':{'Breathing':{'breathingSpeed':this.value}}})); document.getElementById('breathingSpeed').innerText = this.value;">
            <input type="checkbox" id="startBreathing" oninput="if(this.checked) {selectAnimation('Breathing')} else{ selectAnimation('Solid Color')}">
            <div style="height: 25px;width: 378px; margin:4px auto;">
              <input type="checkbox" style="float:right; margin: 0px 4px;" id="useColorList" oninput="ws.send(JSON.stringify({'Animations':{'Breathing':{'useColorList':this.checked}}}));">
              <p style="float:right; margin: 0px 4px;">Use Color List</p>
              <p style="float:right; margin: 0px 4px;">Speed:<i id="breathingSpeed">1</i></p>
            </div>
              <div id="colorListResult" style="height: 25px;width: 370px; margin:4px auto; display: table; table-layout: fixed;"><div style="background-color:#152036; display: table-cell;"></div></div>
              <div style="height: 25px;width: 370px; margin:0 auto; display:inline-block;">
                <button class="btn" style="width:25px; height:25px; float:right; margin:0px 5px; background-color:#3498db;" onclick="br_clearColorlist()">C</button>
                <button class="btn" style="width:25px; height:25px; float:right; margin:0px 5px; background-color:#c0392b;" onclick="br_removeLastColor()">-</button>
                <button class="btn" style="width:25px; height:25px; float:right; margin:0px 5px; background-color:#06d79c;" onclick="br_addCurrentColor()">+</button>
              </div>
          </div>
          <div id="staticColorBreathing" style="-webkit-box-align:center;-webkit-box-pack:center;display:-webkit-box; margin:10;"></div>
        </div>
        <div id="Disco" class="tabcontent">
          <p>Disco</p>
          <p>Disco is not avalible on web version get the windows client</p>
        </div>
        <div id="SolidDisco" class="tabcontent">
          <p>Solid Disco</p>
          <p>Solid Disco is not avalible on web version get the windows client</p>
        </div>
        <div id="MorseCode" class="tabcontent">
          <p>Morse Code</p>
          <div style="margin:40px auto; width:560px">
            <div style="display:inline-block;">
            <p style="margin:10px 0px 5px 0px;">Plain message:</p>
            <textarea id="morsePlainText" style="resize:none; background-color: #152036; color:white; border: 1px solid #5e5e5e;text-transform: lowercase; font-size:10pt;" rows="5" cols="50" maxlength="200" oninput="encode(this)"></textarea>
            <p style="margin:10px 0px 5px 0px;">Encoded message:</p>
            <textarea id="encodedMsgResult" style="resize:none; background-color: #152036; color:white; border: 1px solid #5e5e5e; font-size:10pt;" rows="5" cols="50" readonly></textarea>
            <div style="margin-top:10px;">
                <p style="display:inline-block;">Speed</p>
                <input type="range" min="35" max="550" value="35" id="unitTime" style="width:110px; height:15px;" oninput="ws.send(JSON.stringify({'Animations':{'MorseCode':{'unitTimeMorseCode':this.value}}}));">
                <p style="display:inline-block;">Use buzzer</p>
                <input type="checkbox" style="display:inline-block; height:15px;" id="useBuzzer" oninput="ws.send(JSON.stringify({'Animations':{'MorseCode':{'useBuzzer':this.checked}}}));">
                <p style="display:inline-block;">Start Morse Code</p>
                <input type="checkbox" style="display:inline-block; height:15px;" id="startMorseCode" oninput="if(this.checked){ selectAnimation('Morse Code')} else{ selectAnimation('Solid Color')}">
            </div>
          </div>
          <div id="colorMorseCode" style="display:inline-block; margin-left:10px;"></div>
          </div>
        </div>
        <div id="Ambilight" class="tabcontent">
          <p>Ambilight</p>
          <p>Ambilight is not avalible on web version get the windows client</p>
        </div>
        <div id="Schedule" class="tabcontent">
          <p>Schedule</p>
          <p>Schedule work in progress</p>
        </div>
        <div id="Other" class="tabcontent">
          <button class="btn" onclick="disconnect()"><i class="fas fa-wifi fa-3x"></i><p id="disconnectButton">Connect<p></button>
          <button class="btn" onclick="fpowerConected()" style="position:relative; top:10px;"><div id="powerConected"><div id="btn-ring" style="width: 18px;height: 18px;top: 5px;left: 5px; border: 5px solid #2a2a2a"><div id="ring-line" style="width: 5px;height: 20px; margin: -8px auto;"></div></div></div><p>Power if connected<p></button>  
          <button class="btn" onclick="if(confirm('Are you sure you want to restart this device?')){disconnect(); var wifiData=new XMLHttpRequest();wifiData.open('GET','/restartESP',true);wifiData.send();}"><i class="fas fa-redo-alt fa-3x"></i></i><br>Restart Device</button>
          <button class="btn" onclick="window.location = '/config'"><i class="fas fa-cogs fa-3x"></i><br>Config</button>
          <button class="btn" onclick="if(confirm('Are you sure you want to format this device?')){disconnect(); var wifiData=new XMLHttpRequest();wifiData.open('GET','/formatDevice',true);wifiData.send();}"><i class="fas fa-eraser fa-3x"></i><br>Factory reset</button>
        </div>
      </div>
    </div>
  <script>
    let ws;
    var solidColor = new iro.ColorPicker("#solidColor", {width: 300,layout: [{component: iro.ui.Wheel,options: {wheelAngle:0,wheelDirection:'clockwise'}}]});
    solidColor.on("input:change", solidColorChange);
    var staticColorBreathing = new iro.ColorPicker("#staticColorBreathing", {width: 270,layout: [{component: iro.ui.Wheel,options: {wheelAngle:0,wheelDirection:'clockwise'}}]});
    staticColorBreathing.on("input:change", staticColorBreathingChange);
    var colorList = [];
    var colorMorseCode = new iro.ColorPicker("#colorMorseCode", {width: 170,layout: [{component: iro.ui.Wheel,options: {wheelAngle:0,wheelDirection:'clockwise'}}]});
    colorMorseCode.on("input:change", colorMorseCodeChange);
    var bar = document.querySelector("[data=\"inputstyle\"]");
    var getRSSIinterval;
    function selectAnimation(animation,parameters=null){
      var anim = {"playAnimation" : animation};
      if(parameters != null) anim = Object.assign(parameters, anim);
      ws.send(JSON.stringify(anim));
    }
    function Connect() {
      ws = new WebSocket("ws://" + window.location.hostname + ":81/");
      ws.onopen = function(event) {
         getRSSIinterval = setInterval(getRSSI, 15000);
         document.getElementById("disconnectButton").innerText = "Disconnect";
      };
      ws.onmessage = function(event) {
            var data = JSON.parse(event.data);
            if("Animations" in data) data = data["Animations"];
            if("SolidColor" in data) {
              if("Color" in data["SolidColor"]) solidColor.color.rgb = { r: data.SolidColor.Color[0]/4, g: data.SolidColor.Color[1]/4, b: data.SolidColor.Color[2]/4 };
              if("Brightness" in data["SolidColor"]){
                var color = [solidColor.color.rgb.r*data.SolidColor.Brightness,solidColor.color.rgb.g*data.SolidColor.Brightness,solidColor.color.rgb.b*data.SolidColor.Brightness];
                bar.innerHTML = "#brightness::-webkit-slider-thumb {box-shadow: -100vw 0 0 100vw rgb("+color+")}";
                document.getElementById("brightness").value = data.SolidColor.Brightness * 100;
              }
            }
            if("PowerState" in data){
               if(data.PowerState) document.getElementById("btn-bg").classList.add("active");
               else document.getElementById("btn-bg").classList.remove("active");
            }
            if("ColorCycle" in data){
              if("ColorCycleSpeed" in data["ColorCycle"]) {
                document.getElementById("ColorCycleSpeed").innerText = data.ColorCycle.ColorCycleSpeed;
                document.getElementById("ColorCycleSpeed_input").value = data.ColorCycle.ColorCycleSpeed;
             }
            }
            if("Breathing" in data){
              if("breathingSpeed" in data["Breathing"]){
                document.getElementById("breathingSpeed").innerText = data.Breathing.breathingSpeed;
                document.getElementById("breathingSpeed_input").value = data.Breathing.breathingSpeed;
              }
              if ("colorListBreathing" in data["Breathing"]){
                  colorList = [];
                  data["Breathing"]["colorListBreathing"].forEach((item, i) => {
                    colorList.push([item[0]/4,item[1]/4,item[2]/4]);
                  });
                  redraw();}
              if ("addColortoList" in data["Breathing"]){
                  colorList.push([data["Breathing"]["addColortoList"][0]/4,data["Breathing"]["addColortoList"][1]/4,data["Breathing"]["addColortoList"][2]/4]);
                  redraw();}
              if ("removeLastfromList" in data["Breathing"]){
                if(colorList.length != 0) {
                  colorList.splice(-1,1)
                  redraw();
              }}
              if ("clearColorList" in data["Breathing"]){
                  colorList = [];
                  redraw();}
              if("staticColorBreathing" in data["Breathing"]) staticColorBreathing.color.rgb = { r: data.Breathing.staticColorBreathing[0]/4, g: data.Breathing.staticColorBreathing[1]/4, b: data.Breathing.staticColorBreathing[2]/4 };
              if("useColorList" in data["Breathing"]) document.getElementById("useColorList").checked = data.Breathing.useColorList;
            }
            if("MorseCode" in data){
              if("encodedMorseCode" in data["MorseCode"]) {
                document.getElementById("morsePlainText").value = decodeMessage(data.MorseCode.encodedMorseCode);
                document.getElementById("encodedMsgResult").value = data.MorseCode.encodedMorseCode;
              }
              if("unitTimeMorseCode" in data["MorseCode"]) document.getElementById("unitTime").value = data.MorseCode.unitTimeMorseCode;
              if("colorMorseCode" in data["MorseCode"]) colorMorseCode.color.rgb = { r: data.MorseCode.colorMorseCode[0]/4, g: data.MorseCode.colorMorseCode[1]/4, b: data.MorseCode.colorMorseCode[2]/4 };
              if("useBuzzer" in data["MorseCode"]) document.getElementById("useBuzzer").checked = data.MorseCode.useBuzzer;
            }

            if("playingAnimation" in data){
              document.getElementById("startBreathing").checked = false;
              document.getElementById("startColorCycle").checked = false;
              if(data["playingAnimation"] == "Breathing") document.getElementById("startBreathing").checked = true;
              if(data["playingAnimation"] == "Color Cycle") document.getElementById("startColorCycle").checked = true;
              if(data["playingAnimation"] == "Morse Code") document.getElementById("startMorseCode").checked = true;
              document.getElementById("playingAnimation").innerText = data.playingAnimation;
            }
            if("powerConected" in data){
               if(data.powerConected) document.getElementById("powerConected").classList.add("active");
               else document.getElementById("powerConected").classList.remove("active");
            }
          };
          ws.onclose = function(event) {
             clearInterval(getRSSIinterval);
             document.getElementById("disconnectButton").innerText = "Connect";
             document.getElementById("playingAnimation").innerText = "Disconnected";
          };
    }
    Connect();
    function disconnect(){
       if(ws.readyState === WebSocket.OPEN){
         ws.close();
       }else{
         Connect();
       }
    }
    function solidColorChange(color) {
        bar.innerHTML = "#brightness::-webkit-slider-thumb {box-shadow: -100vw 0 0 100vw "+color.hexString+"}";
        selectAnimation("Solid Color",{"Animations":{"SolidColor":{'Color':[color.rgb.r*4,color.rgb.g*4,color.rgb.b*4]}}});
    }
    function colorMorseCodeChange(color) {
        ws.send(JSON.stringify({"Animations":{"MorseCode":{"colorMorseCode":[color.rgb.r*4,color.rgb.g*4,color.rgb.b*4]}}}));
    }
    function sendBrightness(e) {
      e = e.value/100;
      var color = [solidColor.color.rgb.r*e,solidColor.color.rgb.g*e,solidColor.color.rgb.b*e];
      bar.innerHTML = "#brightness::-webkit-slider-thumb {box-shadow: -100vw 0 0 100vw rgb("+color+")}";
      selectAnimation("Solid Color",{'Animations':{'SolidColor':{'Brightness':e}}});
    }

    function fpowerConected(){
      $('#powerConected').toggleClass('active');
      var a = $('#powerConected').hasClass("active");
      ws.send(JSON.stringify({"Animations":{'powerConected':a}}));
    }
    function staticColorBreathingChange(color) {
      var colors = [color.rgb.r*4,color.rgb.g*4,color.rgb.b*4];
      ws.send(JSON.stringify({"Animations":{"Breathing":{"staticColorBreathing":colors}}}));
    }
    function checkboxToggle(){
      $('#btn-bg').toggleClass('active');
      var a = $('#btn-bg').hasClass("active");
      ws.send(JSON.stringify({"Animations":{'PowerState':a}}));
    }
    function selectTab(evt, cityName) {
      var i, tabcontent, tablinks;
      tabcontent = document.getElementsByClassName("tabcontent");
      for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = "none";
      }
      tablinks = document.getElementsByClassName("tablinks");
      for (i = 0; i < tablinks.length; i++) {
        tablinks[i].className = tablinks[i].className.replace(" active", "");
      }
      document.getElementById(cityName).style.display = "block";
      evt.currentTarget.className += " active";
    }
    document.getElementById("defaultOpen").click();
    function getRSSI() {
      var wifiData=new XMLHttpRequest();
      wifiData.open("GET",'/getSignalStrenght',true);
      wifiData.send();
      wifiData.onload=function(){
            var data=JSON.parse(wifiData.responseText);
            if ("RSSI" in data)
            {
                if (data["RSSI"] >= -50) document.getElementById("signalStrenght").innerText = "Excellent";
                else if (data["RSSI"] < -50 && data["RSSI"] >= -60) document.getElementById("signalStrenght").innerText = "Good";
                else if (data["RSSI"] < -60 && data["RSSI"] >= -70) document.getElementById("signalStrenght").innerText = "Fair";
                else if (data["RSSI"] < -70) document.getElementById("signalStrenght").innerText = "Weak";
            }
      }
    }
    function br_addCurrentColor(){
        if(colorList.length != 25){
          colorList.push([staticColorBreathing.color.rgb.r,staticColorBreathing.color.rgb.g,staticColorBreathing.color.rgb.b]);
          redraw();
          ws.send(JSON.stringify({"Animations":{"Breathing":{"addColortoList":[staticColorBreathing.color.rgb.r*4,staticColorBreathing.color.rgb.g*4,staticColorBreathing.color.rgb.b*4]}}}));
        }else {
           alert("25 colors limit");
        }
    }
    function br_removeLastColor(){
      if(colorList.length != 0){
        colorList.splice(-1,1)
        ws.send(JSON.stringify({"Animations":{"Breathing":{"removeLastfromList":true}}}));
        redraw();
      }
    }
    function br_clearColorlist(){
      colorList = [];
      ws.send(JSON.stringify({"Animations":{"Breathing":{"clearColorList":true}}}));
      redraw();
    }
    function redraw(){
        document.getElementById("colorListResult").innerHTML = "";
        if(colorList.length>0){
        colorList.forEach((item, i) => {
          var obj = document.createElement('div');
          obj.style.cssText = 'background-color:rgb('+item[0]+','+item[1]+','+item[2]+'); display: table-cell; border:1px solid #2e2e36';
          document.getElementById("colorListResult").appendChild(obj);
        });
      }else{
        var obj = document.createElement('div');
        obj.style.cssText = 'background-color:#152036; display: table-cell;';
        document.getElementById("colorListResult").appendChild(obj);
      }
    }
    var alphabet = ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "!", "@", "&", "(", ")", "-", "_", "=", "+", ".", ",", "/", "?", ";", ":", "\"", "\'" ];
    var morse = [".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--..",".----","..---","...--","....-","-....","--...","---..","----.","-----","-.-.--",".--.-.", ".....", ".-...", "-.--.", "-.--.-", "-....-", "..--.-", "-...-", ".-.-.", ".-.-.-", "--..--", "-..-.", "..--..", "-.-.-.", "---...", ".----.", ".-..-." ];
    let timeout = null;
    function encode(e){
      e.addEventListener('keyup', function (k){
        clearTimeout(timeout);
        timeout = setTimeout(function(){encodeMessage(e.value)}, 700);
      });
    }
    function encodeMessage(e){
      var encoded = "";
      var words = e.split(/\s+/).filter(function(i){return i});
      for (var i = 0; i < words.length; i++) {
        var ch = words[i].split('').filter(function(i){return i});
        for (var j = 0; j < ch.length; j++) {
            if (alphabet.indexOf(ch[j]) > -1){
              encoded += morse[alphabet.indexOf(ch[j])];
              if (j + 1 != ch.length) encoded += "*";
            }
        }
        if (i + 1 != words.length) encoded += "|";
      }
      document.getElementById("encodedMsgResult").value = encoded;
      ws.send(JSON.stringify({'Animations':{'MorseCode':{'encodedMorseCode':encoded}}}));
      return encoded;
    }
    function decodeMessage(e){
      var decoded = "";
      var encodedWords = e.split('|').filter(function(i){return i});
      for (var i = 0; i < encodedWords.length; i++)
      {
          var ch = encodedWords[i].split('*').filter(function(i){return i});
          for (var j = 0; j < ch.length; j++)
          {
            if(morse.indexOf(ch[j]) > -1)decoded += alphabet[morse.indexOf(ch[j])];
          }
          if (i + 1 != encodedWords.length)  decoded += " ";
      }
      return decoded;
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
int buzzer_pin = 0;
IPAddress local_IP = IPAddress(192,168,1,50);
IPAddress gateway= IPAddress(192,168,1,1);
IPAddress subnet= IPAddress(255,255,255,0);
IPAddress dns = IPAddress(8,8,8,8);
bool startSaving = false;
int SolidColor[3] = {1020,0,0};
float brightness = 1.0f;
bool PowerState = true;
bool powerConected = false;

unsigned long currentMillis = millis();;

void sendData(uint8_t num);
void saveConfig();
void saveUserData();
bool loadConfig();
void SetColorValue(int red,int green, int blue,float _brightness);
DynamicJsonDocument AnimationsJson();

void restartESP(){
      Serial.println("Restarting ESP");
      server.send(200, "text/html", "Success"); 
      SetColorValue(0,0,0,0);
      delay(1000);
      ESP.restart();   
}

void removeUserSettings(){
      SPIFFS.remove("/user_settings.json");
      server.send(200, "text/html", "Success");
      SetColorValue(0,0,0,0);
      delay(1000);
      ESP.restart();   
}

void handleHardReset(){
  int secconds=0;
  pinMode(reset_pin, INPUT);
  while(digitalRead(reset_pin) == HIGH){
    secconds+=1;   
    if(secconds >= 100){     
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
    analogWrite(red_pin, constrain(red*_brightness, 0, 1020)); 
    analogWrite(green_pin, constrain(green*_brightness, 0, 1020));
    analogWrite(blue_pin, constrain(blue*_brightness, 0, 1020));
}

bool startDisco;
void handleDisco(int r,int g, int b){
  if(startDisco) {
    SetColorValue(r,g,b,1);
  }
}

bool startSolidDisco;
int colorSolidDisco[3] = {1020,0,0};
void handleSolidDisco(float val){
  if(startSolidDisco) {
    SetColorValue(colorSolidDisco[0],colorSolidDisco[1],colorSolidDisco[2],val);
  }
}

bool startAmbilight;
void handleAmbilight(int r,int g, int b){
  if(startAmbilight) {
    SetColorValue(r,g,b,1);
  }
}

bool startMorseCode;
char *encodedMessage = "";
bool state=false;
unsigned long previousMorse = 0;
int unitTimeMorseCode = 70;
unsigned int currUnit=0;
unsigned int morseIndex=0;
int colorMorseCode[3] = {1020,0,0};
bool useBuzzer;

bool morse_turn(bool turn){
   if(turn){
      SetColorValue(colorMorseCode[0],colorMorseCode[1],colorMorseCode[2],1);
      if(useBuzzer) tone(buzzer_pin, 850); 
   }else{
      SetColorValue(colorMorseCode[0],colorMorseCode[1],colorMorseCode[2],0);
      noTone(buzzer_pin);
   }
   return turn;
}
void resetMorseCode(int unit){
    morseIndex = 0;
    state = false;
    noTone(buzzer_pin);
    currUnit = unit;
}
void handleMorseCode(){
  if(currentMillis - previousMorse >= currUnit) {
    previousMorse = currentMillis; 
    if(encodedMessage != NULL&&strlen(encodedMessage)>0) {
        if(morseIndex<strlen(encodedMessage)){
          if(encodedMessage[morseIndex] == '.'){
              if(!state){
                state = morse_turn(true);
                currUnit = unitTimeMorseCode;          
              }else{
                state = morse_turn(false);
                currUnit = unitTimeMorseCode;            
                morseIndex++;
              }
          }
          else if(encodedMessage[morseIndex] == '-'){
              if(!state){
                currUnit = unitTimeMorseCode*3;
                state = morse_turn(true);
              }else{   
                state = morse_turn(false);
                currUnit = unitTimeMorseCode;
                morseIndex++;
              }       
          }
          else  if(encodedMessage[morseIndex] == '*'){
            morse_turn(false);
            currUnit = unitTimeMorseCode*3;
            morseIndex++;
          }
          else  if(encodedMessage[morseIndex] == '|'){
            morse_turn(false);
            currUnit = unitTimeMorseCode*7;
            morseIndex++;
          }
        }
        else{
          resetMorseCode(unitTimeMorseCode*25);
        }
    }
  }
  
}

bool startBreathing = false, inverseBool = true;
unsigned int breathingSpeed = 10;
float breathingPulse=1.0f;
unsigned long breathingMillis;
int staticColorBreathing[3] = {1020,0,0};
DynamicJsonDocument colorList(2048); 
int colorListindex = 0;
bool colorChanged = false;
bool useColorList = false;

void handleBreathing(){
  if (currentMillis - breathingMillis >= breathingSpeed) {
    breathingMillis = currentMillis;
      if(inverseBool) {
        breathingPulse=breathingPulse-0.005f;
        if(breathingPulse<=0.0f) {
          breathingPulse=0.0f;
          inverseBool = !inverseBool;
        }
        colorChanged = false;
      }
      else
      {
        breathingPulse=breathingPulse+0.005f;
        if(breathingPulse>=1.0f) {
          breathingPulse = 1.0f;
          inverseBool = !inverseBool;
        }
        if(useColorList & !colorChanged) {
          colorListindex++;
          if(colorListindex+1 > colorList.size()) colorListindex = 0;  
          colorChanged = true;
        }
      } 
      if(useColorList) SetColorValue(colorList[colorListindex][0],colorList[colorListindex][1],colorList[colorListindex][2],breathingPulse);
      else SetColorValue(staticColorBreathing[0],staticColorBreathing[1],staticColorBreathing[2],breathingPulse);
  }
}

bool startColorCycle = false;
bool startBool[]={true,true,true};
int colorIndexColorCycle[]={1020,0,0};
unsigned long prevMillisColorCycle;
unsigned int ColorCycleSpeed = 50;

void handleColorCycle(){
   if(currentMillis - prevMillisColorCycle > ColorCycleSpeed){
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

String playingAnimation = "Solid Color";
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

void setAnimation(String animation,bool power){
  if(power != PowerState){
    PowerState = power;
    webSocket.broadcastTXT("{\"PowerState\":"+String(PowerState ? "true":"false")+"}") ;   
  }
  if(power){
    if(playingAnimation != animation){
      turnOffAnimations();
      playingAnimation = animation;
      webSocket.broadcastTXT("{\"Animations\":{\"playingAnimation\":\""+playingAnimation+"\"}}");
    }  
    if(animation == "Solid Color")  SetColorValue(SolidColor[0],SolidColor[1],SolidColor[2],brightness);
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
      playingAnimation = "Solid Color";
      webSocket.broadcastTXT("{\"Animations\":{\"playingAnimation\":\""+playingAnimation+"\"}}");
      SetColorValue(SolidColor[0],SolidColor[1],SolidColor[2],brightness);
    }  
    Serial.println("Playing:"+playingAnimation);
  }else{
    turnOffAnimations();
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if(type == WStype_CONNECTED){
      Serial.println("Client "+String(num)+" connected");  
      if(powerConected) setAnimation(playingAnimation,true);
      String config = AnimationsJson().as<String>();
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
        JsonObject Animations = json["Animations"];
        if (Animations.containsKey("SolidColor"))
        {        
          JsonObject _SolidColor = Animations["SolidColor"];
          if (_SolidColor.containsKey("Brightness")) brightness = _SolidColor ["Brightness"];   
          if(_SolidColor.containsKey("Color")){
            SolidColor[0] = _SolidColor["Color"][0];
            SolidColor[1] = _SolidColor["Color"][1];
            SolidColor[2] = _SolidColor["Color"][2];
          }
        }       
        if (Animations.containsKey("ColorCycle"))
        {
          JsonObject ColorCycle = Animations["ColorCycle"];
          if(ColorCycle.containsKey("ColorCycleSpeed")) ColorCycleSpeed = ColorCycle["ColorCycleSpeed"];
        }
        if (Animations.containsKey("Breathing"))
        {
          JsonObject Breathing = Animations["Breathing"];
          if(Breathing.containsKey("staticColorBreathing")) {
            staticColorBreathing[0] = Breathing["staticColorBreathing"][0];
            staticColorBreathing[1] = Breathing["staticColorBreathing"][1];
            staticColorBreathing[2] = Breathing["staticColorBreathing"][2];
          }   
          if(Breathing.containsKey("colorListBreathing")){
              colorList.clear();
              colorList = Breathing["colorListBreathing"].as<JsonArray>();
          }
          if(Breathing.containsKey("addColortoList")) {
            if(colorList.size()!=25)
            colorList.add(Breathing["addColortoList"].as<JsonArray>());  
          }
          if(Breathing.containsKey("removeLastfromList")){
            if(colorList.size()!=0) colorList.remove(colorList.size()-1);  
          }
          if(Breathing.containsKey("clearColorList")) colorList.clear();
          if(Breathing.containsKey("breathingSpeed")) breathingSpeed = Breathing["breathingSpeed"];
          if(Breathing.containsKey("useColorList")) useColorList = Breathing["useColorList"];
          inverseBool = true; colorListindex = 0; colorChanged = true; breathingPulse=1.0f; 
        }
        if (Animations.containsKey("PowerState")) setAnimation(playingAnimation,Animations["PowerState"]);
        if (Animations.containsKey("Disco"))
        {
          JsonObject Disco = Animations["Disco"];
          if(Disco.containsKey("colorDisco")) handleDisco( Disco["colorDisco"][0], Disco["colorDisco"][1], Disco["colorDisco"][2]);   
            
        }  
        if (Animations.containsKey("SolidDisco"))
        {
          JsonObject SolidDisco = Animations["SolidDisco"];
          if(SolidDisco.containsKey("pulseSolidDisco")) handleSolidDisco(SolidDisco["pulseSolidDisco"]);
          if(SolidDisco.containsKey("colorSolidDisco")) { 
            colorSolidDisco[0] = SolidDisco["colorSolidDisco"][0];
            colorSolidDisco[1] = SolidDisco["colorSolidDisco"][1];
            colorSolidDisco[2] = SolidDisco["colorSolidDisco"][2];
          }             
        }  
        if (Animations.containsKey("MorseCode"))
        {
          JsonObject MorseCode = Animations["MorseCode"];  
          if(MorseCode.containsKey("useBuzzer")) useBuzzer = MorseCode["useBuzzer"];
          if(MorseCode.containsKey("unitTimeMorseCode")) {
            unitTimeMorseCode = MorseCode["unitTimeMorseCode"];
            resetMorseCode(0);
          }
          if(MorseCode.containsKey("encodedMorseCode")) {    
            free(encodedMessage);
            encodedMessage = strdup(MorseCode["encodedMorseCode"]);
            resetMorseCode(0);
          }
          if(MorseCode.containsKey("colorMorseCode")) { 
            colorMorseCode[0] = MorseCode["colorMorseCode"][0];
            colorMorseCode[1] = MorseCode["colorMorseCode"][1];
            colorMorseCode[2] = MorseCode["colorMorseCode"][2];
          }     
        } 
        if (Animations.containsKey("Ambilight"))
        {
          JsonObject Ambilight = Animations["Ambilight"];
          if(Ambilight.containsKey("AmbilightColor")) handleAmbilight(Ambilight["AmbilightColor"][0], Ambilight["AmbilightColor"][1], Ambilight["AmbilightColor"][2]);      
        } 
        if (Animations.containsKey("powerConected")) powerConected = Animations["powerConected"];
        if (json.containsKey("playAnimation")) setAnimation(json["playAnimation"] , true);                        
        if (json.containsKey("command")) {
            if(json["command"] == "formatDevice") removeUserSettings();
            if(json["command"] == "restartAnimation") {
              setAnimation(playingAnimation,PowerState);
            }
        } 
        json.clear();
  }
  if(type == WStype_DISCONNECTED){
    Serial.println("Client "+String(num)+" disconnected");  
    if(webSocket.connectedClients() == 0) {
      if(powerConected) setAnimation(playingAnimation,false);
      startSaving = false;
    }
    saveUserData();
  }
}

DynamicJsonDocument AnimationsJson(){
  DynamicJsonDocument doc(4496);
    JsonObject Animations = doc.createNestedObject("Animations");
      JsonObject _SolidColor = Animations.createNestedObject("SolidColor");
        _SolidColor["Brightness"] = brightness;
        JsonArray _SolidColor_Color = _SolidColor.createNestedArray("Color");
        for (int i = 0; i < 3; i++) _SolidColor_Color.add(SolidColor[i]);
      JsonObject ColorCycle = Animations.createNestedObject("ColorCycle");
        ColorCycle["ColorCycleSpeed"] = ColorCycleSpeed;
      JsonObject Breathing = Animations.createNestedObject("Breathing");
        Breathing["breathingSpeed"] = breathingSpeed;  
        Breathing["useColorList"] = useColorList;                
        JsonArray _staticColorBreathing = Breathing.createNestedArray("staticColorBreathing");
          for (int i = 0; i < 3; i++) _staticColorBreathing.add(staticColorBreathing[i]);           
        JsonArray _colorListBreathing = Breathing.createNestedArray("colorListBreathing");
          _colorListBreathing.set(colorList.as<JsonArray>());
      JsonObject Disco = Animations.createNestedObject("Disco");
      JsonObject SolidDisco = Animations.createNestedObject("SolidDisco");    
        JsonArray _colorSolidDisco = SolidDisco.createNestedArray("colorSolidDisco");
          for (int i = 0; i < 3; i++) _colorSolidDisco.add(colorSolidDisco[i]); 
      JsonObject morseCode = Animations.createNestedObject("MorseCode");  
        morseCode["encodedMorseCode"] = encodedMessage;
        morseCode["unitTimeMorseCode"] = unitTimeMorseCode;      
        morseCode["useBuzzer"] = useBuzzer;
        JsonArray _colorMorseCode = morseCode.createNestedArray("colorMorseCode");
          for (int i = 0; i < 3; i++) _colorMorseCode.add(colorMorseCode[i]); 
      Animations["powerConected"] = powerConected;  
      Animations["playingAnimation"] = playingAnimation;
      Animations["PowerState"] = PowerState;      
  return doc;
}
void combine(JsonDocument& dst, const JsonDocument& src) {
    for (auto p : src.as<JsonObject>()) dst[p.key()] = p.value();
}
void saveConfig(){  
  DynamicJsonDocument json(6144);
  deserializeJson(json, server.arg("plain"));

  // try recived config
  if(ssid == json["ESP_Config"]["SSID"])
      json["ESP_Config"]["PASSWORD"] = password;

  Serial.println("Testing recived led pins");
  analogWrite(json["ESP_Config"]["REDPIN"], 1020); digitalWrite(LED_BUILTIN, LOW); delay(1000); analogWrite(json["ESP_Config"]["REDPIN"], 0); digitalWrite(LED_BUILTIN, HIGH);delay(1000);
  analogWrite(json["ESP_Config"]["GREENPIN"], 1020); digitalWrite(LED_BUILTIN, LOW); delay(1000); analogWrite(json["ESP_Config"]["GREENPIN"], 0); digitalWrite(LED_BUILTIN, HIGH);delay(1000);
  analogWrite(json["ESP_Config"]["BLUEPIN"], 1020); digitalWrite(LED_BUILTIN, LOW); delay(1000); analogWrite(json["ESP_Config"]["BLUEPIN"], 0); digitalWrite(LED_BUILTIN, HIGH);delay(1000);
  tone(json["ESP_Config"]["BUZZERPIN"], 850); digitalWrite(LED_BUILTIN, LOW); delay(1000); noTone(json["ESP_Config"]["BUZZERPIN"]); digitalWrite(LED_BUILTIN, HIGH); delay(1000);

  Serial.println("Testing recived credentials");
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  WiFi.begin(json["ESP_Config"]["SSID"].as<String>(), json["ESP_Config"]["PASSWORD"].as<String>());  
  int timeout=0;
  while(WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, HIGH); delay(250); digitalWrite(LED_BUILTIN, LOW);
    Serial.print("."); 
    timeout++;
    if(timeout >= 300) break;
  }
  
  if(WiFi.isConnected()){
    json["ESP_Config"]["Initialized"] = true;
    File configFile = SPIFFS.open("/user_settings.json", "w+");
    combine(json, AnimationsJson());
    serializeJson(json, configFile);
    configFile.close();
  }
  json.clear();
  ESP.restart();
}

bool loadConfig() {
  File configFile = SPIFFS.open("/user_settings.json", "r");
  if (!configFile) return false;
  DynamicJsonDocument json(8192);
  deserializeJson(json, configFile);
  configFile.close();
  if(json.containsKey("ESP_Config")) {
    JsonObject _ESP_Config = json["ESP_Config"];
    if(_ESP_Config.containsKey("Initialized")) initialized = _ESP_Config["Initialized"];
    if(_ESP_Config.containsKey("HOSTNAME")) hostname = strdup(_ESP_Config["HOSTNAME"]);
    if(_ESP_Config.containsKey("SSID")) ssid = strdup(_ESP_Config["SSID"]);
    if(_ESP_Config.containsKey("PASSWORD")) password = strdup(_ESP_Config["PASSWORD"]);
    if(_ESP_Config.containsKey("REDPIN")) red_pin = _ESP_Config["REDPIN"];
    if(_ESP_Config.containsKey("GREENPIN")) green_pin = _ESP_Config["GREENPIN"];
    if(_ESP_Config.containsKey("BLUEPIN")) blue_pin = _ESP_Config["BLUEPIN"];
    if(_ESP_Config.containsKey("BUZZERPIN")) buzzer_pin = _ESP_Config["BUZZERPIN"];
  }
  if(json.containsKey("Network_Config")) {
    JsonObject _Network_Config = json["Network_Config"];
    if(_Network_Config.containsKey("startStatic")) startStatic = _Network_Config["startStatic"];
    if(_Network_Config.containsKey("local_IP")) local_IP.fromString(_Network_Config["local_IP"].as<String>());
    if(_Network_Config.containsKey("gateway")) gateway.fromString(_Network_Config["gateway"].as<String>());
    if(_Network_Config.containsKey("subnet")) subnet.fromString(_Network_Config["subnet"].as<String>());
    if(_Network_Config.containsKey("dns")) dns.fromString(_Network_Config["dns"].as<String>());
  }
  if(json.containsKey("Animations")) {
    JsonObject _Animations = json["Animations"];
      if(_Animations.containsKey("SolidColor")){
        JsonObject _SolidColor = _Animations["SolidColor"];
        if(_SolidColor.containsKey("Brightness")) brightness = _SolidColor["Brightness"];
        if(_SolidColor.containsKey("Color")) {
          for (int i = 0; i < 3; i++) SolidColor[i] = _SolidColor["Color"][i];
        }      
      }
      if(_Animations.containsKey("ColorCycle")){
        JsonObject _ColorCycle = _Animations["ColorCycle"];
        if(_ColorCycle.containsKey("ColorCycleSpeed")) ColorCycleSpeed = _ColorCycle["ColorCycleSpeed"]; 
      }
      if(_Animations.containsKey("Breathing")){
        JsonObject _Breathing = _Animations["Breathing"];
        if(_Breathing.containsKey("staticColorBreathing")){
          for (int i = 0; i < 3; i++) staticColorBreathing[i] = _Breathing["staticColorBreathing"][i];
        }
        if(_Breathing.containsKey("colorListBreathing")) colorList = _Breathing["colorListBreathing"].as<JsonArray>();       
        if(_Breathing.containsKey("useColorList")) useColorList = _Breathing["useColorList"];    
        if(_Breathing.containsKey("breathingSpeed")) breathingSpeed = _Breathing["breathingSpeed"]; 
      }
      if(_Animations.containsKey("SolidDisco")){
        JsonObject _SolidDisco = _Animations["SolidDisco"];
        if(_SolidDisco.containsKey("colorSolidDisco")){
          for (int i = 0; i < 3; i++) colorSolidDisco[i] = _SolidDisco["colorSolidDisco"][i];
        }    
      }
      if(_Animations.containsKey("MorseCode")){
        JsonObject _MorseCode = _Animations["MorseCode"];
        if(_MorseCode.containsKey("colorMorseCode")){
          for (int i = 0; i < 3; i++) colorMorseCode[i] = _MorseCode["colorMorseCode"][i];
        }
        if(_MorseCode.containsKey("encodedMorseCode")) encodedMessage = strdup(_MorseCode["encodedMorseCode"]); 
        if(_MorseCode.containsKey("unitTimeMorseCode")) unitTimeMorseCode = _MorseCode["unitTimeMorseCode"]; 
        if(_MorseCode.containsKey("useBuzzer")) useBuzzer = _MorseCode["useBuzzer"]; 
      }
      if(_Animations.containsKey("powerConected")) powerConected = _Animations["powerConected"];  
      
      if(_Animations.containsKey("playingAnimation") && _Animations.containsKey("PowerState")) setAnimation(_Animations["playingAnimation"], _Animations["PowerState"]);
  }
  json.clear();
  return true;
}

void saveUserData(){
    DynamicJsonDocument doc(8192);
    File configFile = SPIFFS.open("/user_settings.json", "r");
    deserializeJson(doc, configFile);
    configFile.close();
    combine(doc, AnimationsJson());
    configFile = SPIFFS.open("/user_settings.json", "w+");
    serializeJson(doc, configFile);
    doc.clear();
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
void syncData(){
  String config = AnimationsJson().as<String>();
  server.send(200, "application/json", config); 
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
  doc["Initialized"] = initialized;
  if(initialized){
      doc["SSID"] = ssid;
      if(String(password) != "") doc["PASSWORD"] = "**********";
      else doc["PASSWORD"] = "";
      doc["HOSTNAME"] = hostname;    
      doc["REDPIN"] = red_pin;
      doc["GREENPIN"] = green_pin;
      doc["BLUEPIN"] = blue_pin;
      doc["BUZZERPIN"] = buzzer_pin; 
      doc["startStatic"] = startStatic;
      doc["local_IP"] = local_IP.toString();
      doc["gateway"] = gateway.toString();
      doc["subnet"] = subnet.toString();
      doc["dns"] = dns.toString();
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
    pinMode(red_pin,OUTPUT);
    pinMode(green_pin,OUTPUT);
    pinMode(blue_pin,OUTPUT);
    WiFi.mode(WIFI_STA);  
    WiFi.hostname(hostname);
    if(startStatic) WiFi.config(local_IP, gateway, subnet, dns);  
    WiFi.begin(ssid, password);  
    while(WiFi.status() != WL_CONNECTED){
      digitalWrite(LED_BUILTIN, HIGH); delay(250); digitalWrite(LED_BUILTIN, LOW);
      Serial.print("."); 
    }
    Serial.println("");
    if(MDNS.begin(hostname)){
      Serial.println("http://"+String(wifi_station_get_hostname())+".local : "+WiFi.localIP().toString());
      MDNS.addService("esprgb", "tcp", 80);
    } 
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    ArduinoOTA.onStart([]() {
      SetColorValue(0,0,0,0);
    });
    ArduinoOTA.begin();
    server.on("/", []() {server.send(200, "text/html", homePage);});
    server.on("/config", []() {server.send(200, "text/html", configPage);});  
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
  server.on("/syncData", syncData);
  server.on("/restartESP", restartESP);
  server.on("/getSignalStrenght", getSignalStrenght);
  server.onNotFound([]() { server.send(404, "text/plain", "Error 404!");});
  server.begin();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); digitalWrite(LED_BUILTIN, HIGH); 
  Serial.begin(115200);     
  SPIFFS.begin();
  handleHardReset();
  if (loadConfig() ?  Serial.println("Config loaded") : Serial.println("Config doesn't exist"));
  handleInit();
}

void loop() { 
  currentMillis = millis();
  server.handleClient(); 
  if(initialized) {
    ArduinoOTA.handle();
    webSocket.loop();
    if(startBreathing) handleBreathing();
    if(startColorCycle) handleColorCycle();
    if(startMorseCode) handleMorseCode();
    if(startSaving) handleSave();   
  }  
}