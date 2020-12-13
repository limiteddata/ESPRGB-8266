#include <vector>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <SchedulerESP8266.h>
#define ESPRGB_VERSION "1.1.0.2"
#define reset_pin 16

Scheduler scheduler;
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
        .title{margin-bottom: 20px;}
        #timeScheduleContainer{width: 470px;height: 400px;background-color: #152036;display: inline-block;margin: 0px 15px 0px 20px;overflow-y: scroll;font-size: 0px;}
        .greenBtn{width:90px;height:45px;background-color: #06d79c;border:none;color: white;vertical-align: bottom;}
        .switch {position: relative;width: 40px;height: 20px;display: inline-block;}
        .switch input {opacity: 0;width: 0;height: 0;}
        .slider {position: absolute;cursor: pointer;top: 0;left: 0;right: 0;bottom: 0;background-color: #ccc;-webkit-transition: .4s;transition: .4s;}
        .slider:before {position: absolute;content: ""; height: 20px;width: 20px;left: 0px;bottom: 0px;background-color: white;-webkit-transition: .4s;transition: .4s;}
        input:checked + .slider { background-color: #06d79c; }
        input:checked + .slider:before {-webkit-transform: translateX(26px);-ms-transform: translateX(26px);transform: translateX(26px);}
        .slider.round { border-radius: 34px; }
        .slider.round:before { border-radius: 50%; }
    </style>
    <style data="inputstyle"></style>
  </head>
  <body>
    <div id="main_wrapper">
      <p style="padding:15px 15px; color:#06d79c; font-size:20px;" id="hostname">ESPRGB-Name</p>
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
          <p class="title">Solid Color</p>
          <div style="margin-top: 65px;text-align:center; font-size:12px">
            <div id="solidColor" style="-webkit-box-align:center;-webkit-box-pack:center;display:-webkit-box;"></div>
            <input  type="range" min="0" max="100" value="100" style="width:300px; margin-top:20px" id="brightness" oninput='sendBrightness(this)'>
          </div>
            <div id="btn-bg" onclick="checkboxToggle()"><div id="btn-ring"><div id="ring-line"></div></div></div>
        </div>
        <div id="Color_Cycle" class="tabcontent">
          <p >Color Cycle</p>
          <div style="margin-top: 20px;text-align:center;font-size:12px">
            <input  type="range" min="1" max="200" value="1" id="ColorCycleSpeed_input" oninput="ws.send(JSON.stringify({'Animations':{'parameters':{'ColorCycle':{'ColorCycleSpeed':this.value}}}})); document.getElementById('ColorCycleSpeed').innerText = this.value;">
            <input type="checkbox" id="startColorCycle" oninput="if(this.checked){selectAnimation('Color Cycle')} else {selectAnimation('Solid Color')}">
            <p>Speed:<i id="ColorCycleSpeed">1</i></p>
          </div>
        </div>
        <div id="Breathing" class="tabcontent">
          <p class="title">Breathing</p>
          <div style="margin-top: 20px;text-align:center; font-size:12px">
            <input type="range" min="1" max="200" value="1" id="breathingSpeed_input" oninput="ws.send(JSON.stringify({'Animations':{'parameters':{'Breathing':{'breathingSpeed':this.value}}}})); document.getElementById('breathingSpeed').innerText = this.value;">
            <input type="checkbox" id="startBreathing" oninput="if(this.checked) {selectAnimation('Breathing')} else{ selectAnimation('Solid Color')}">
            <div style="height: 25px;width: 378px; margin:4px auto;">
              <input type="checkbox" style="float:right; margin: 0px 4px;" id="useColorList" oninput="ws.send(JSON.stringify({'Animations':{'parameters':{'Breathing':{'useColorList':this.checked}}}}));">
              <p style="float:right; margin: 0px 4px;">Use Color List</p>
              <p style="float:right; margin: 0px 4px;">Speed:<i id="breathingSpeed">1</i></p>
            </div>
              <div id="colorListResult" style="height: 25px;width: 370px; margin:4px auto; display: table; table-layout: fixed;"><div style="background-color:#152036; display: table-cell;"></div></div>
              <div style="height: 25px;width: 370px; margin:0 auto; display:inline-block;">
                <button class="btn" style="width:25px; height:25px; float:right; margin:0px 5px; background-color:#3498db;" onclick="ws.send(JSON.stringify({'Animations':{'parameters':{'Breathing':{'clearColorList':true}}}}));">C</button>
                <button class="btn" style="width:25px; height:25px; float:right; margin:0px 5px; background-color:#c0392b;" onclick="ws.send(JSON.stringify({'Animations':{'parameters':{'Breathing':{'removeLastfromList':true}}}}));">-</button>
                <button class="btn" style="width:25px; height:25px; float:right; margin:0px 5px; background-color:#06d79c;" onclick="if(colorList.length != 25) ws.send(JSON.stringify({'Animations':{'parameters':{'Breathing':{'addColortoList':[staticColorBreathing.color.rgb.r*4,staticColorBreathing.color.rgb.g*4,staticColorBreathing.color.rgb.b*4]}}}})); else alert('25 colors limit');">+</button>
              </div>
          </div>
          <div id="staticColorBreathing" style="-webkit-box-align:center;-webkit-box-pack:center;display:-webkit-box; margin:10;"></div>
        </div>
        <div id="Disco" class="tabcontent">
          <p class="title">Disco</p>
          <p>Disco is not avalible on web version get the windows client</p>
        </div>
        <div id="SolidDisco" class="tabcontent">
          <p class="title">Solid Disco</p>
          <p>Solid Disco is not avalible on web version get the windows client</p>
        </div>
        <div id="MorseCode" class="tabcontent">
          <p class="title">Morse Code</p>
          <div style="display:inline-block;">
            <p style="margin:10px 0px 5px 0px;">Plain message:</p>
            <textarea id="morsePlainText" style="resize:none; background-color: #152036; color:white; border: 1px solid #5e5e5e;text-transform: lowercase; font-size:10pt;" rows="5" cols="50" maxlength="75" oninput="encode(this)"></textarea>
            <p style="margin:10px 0px 5px 0px;">Encoded message:</p>
            <textarea id="encodedMsgResult" style="resize:none; background-color: #152036; color:white; border: 1px solid #5e5e5e; font-size:10pt;" rows="5" cols="50" readonly></textarea>
            <div style="margin-top:10px;">
                <p style="display:inline-block;">Speed</p>
                <input type="range" min="35" max="550" value="35" id="unitTime" style="width:110px; height:15px;" oninput="ws.send(JSON.stringify({'Animations':{'parameters':{'MorseCode':{'unitTimeMorseCode':this.value}}}}));">
                <p style="display:inline-block;">Use buzzer</p>
                <input type="checkbox" style="display:inline-block; height:15px;" id="useBuzzer" oninput="ws.send(JSON.stringify({'Animations':{'parameters':{'MorseCode':{'useBuzzer':this.checked}}}}));">
                <p style="display:inline-block;">Start Morse Code</p>
                <input type="checkbox" style="display:inline-block; height:15px;" id="startMorseCode" oninput="if(this.checked){ selectAnimation('Morse Code')} else{ selectAnimation('Solid Color')}">
            </div>
          </div>
          <div id="colorMorseCode" style="display:inline-block; margin-left:10px;"></div>
        </div>
        <div id="Ambilight" class="tabcontent">
          <p class="title">Ambilight</p>
          <p>Ambilight is not avalible on web version get the windows client</p>
        </div>
        <div id="Schedule" class="tabcontent">
          <p class="title">Schedule</p>
          <p style="margin:0 0 20 20">Time schedule:</p>
          <div id="timeScheduleContainer"></div>
          <div style="display:inline-block">
            <div style="margin-bottom:20px;">
              <p style="display:inline-block; vertical-align: bottom;">Enable</p>
              <label class="switch">
                <input type="checkbox" id="enableScheduler" oninput="ws.send(JSON.stringify({'Animations':{'Schedules':{'enableScheduler':this.checked}}}));">
                <span class="slider round"></span>
              </label>
            </div>
            <button class="greenBtn" onclick="if(timeScheduleItems.length<10) createNewScheduleWindow(-1,null); else alert('10 schedules limit');">ADD NEW</button>
          </div>
        </div>
        <div id="Other" class="tabcontent" style="position: relative;">
          <button class="btn" onclick="disconnect()"><i class="fas fa-wifi fa-3x"></i><p id="disconnectButton">Connect<p></button>
          <button class="btn" onclick="fpowerConected()" style="position:relative; top:10px;"><div id="powerConected"><div id="btn-ring" style="width: 18px;height: 18px;top: 5px;left: 5px; border: 5px solid #2a2a2a"><div id="ring-line" style="width: 5px;height: 20px; margin: -8px auto;"></div></div></div><p>Power if connected<p></button>
          <button class="btn" onclick="if(confirm('Are you sure you want to restart this device?')){disconnect(); var wifiData=new XMLHttpRequest();wifiData.open('GET','/restartESP',true);wifiData.send();}"><i class="fas fa-redo-alt fa-3x"></i></i><br>Restart Device</button>
          <button class="btn" onclick="window.location = '/config'"><i class="fas fa-cogs fa-3x"></i><br>Config</button>
          <button class="btn" onclick="if(confirm('Are you sure you want to format this device?')){disconnect(); var wifiData=new XMLHttpRequest();wifiData.open('GET','/formatDevice',true);wifiData.send();}"><i class="fas fa-eraser fa-3x"></i><br>Factory reset</button>
          <button class="btn" onclick="if(confirm('Are you sure you want to format this device?')){disconnect(); var removeUserConfig=new XMLHttpRequest();removeUserConfig.open('GET','/removeUserData',true);removeUserConfig.send();}"><i class="far fa-trash-alt fa-3x"></i><br>Remove User Data</button>
          <p style="position: absolute;bottom: 8px;right: 16px;">ESPRGB Version: <i id="ESPRGB-Version">0.0.0.0</i></p>
        </div>
      </div>
    </div>
  <script>
    document.getElementById("hostname").innerText = window.location.hostname;
    function getVersion() {
      var versionHttp=new XMLHttpRequest();
      versionHttp.open("GET",'/getVersion',true);
      versionHttp.send();
      versionHttp.onload=function(){
            var data=JSON.parse(versionHttp.responseText);
            if ("ESPRGB_VERSION" in data) document.getElementById("ESPRGB-Version").innerText = data["ESPRGB_VERSION"];
      }
    }
    getVersion();
    let ws;
    var solidColor = new iro.ColorPicker("#solidColor", {width: 300,layout: [{component: iro.ui.Wheel,options: {wheelAngle:0,wheelDirection:'clockwise'}}]});
    solidColor.on("input:change", (color)=>{bar.innerHTML = "#brightness::-webkit-slider-thumb {box-shadow: -100vw 0 0 100vw "+color.hexString+"}";
    selectAnimation("Solid Color",{"Animations":{"parameters":{"SolidColor":{"Color":[color.rgb.r*4,color.rgb.g*4,color.rgb.b*4]}}}});});
    var staticColorBreathing = new iro.ColorPicker("#staticColorBreathing", {width: 270,layout: [{component: iro.ui.Wheel,options: {wheelAngle:0,wheelDirection:'clockwise'}}]});
    staticColorBreathing.on("input:change", (color)=>ws.send(JSON.stringify({"Animations":{"parameters":{"Breathing":{"staticColorBreathing":[color.rgb.r*4,color.rgb.g*4,color.rgb.b*4]}}}})));
    var colorList = [];
    var sch_colorList = [];
    var colorMorseCode = new iro.ColorPicker("#colorMorseCode", {width: 170,layout: [{component: iro.ui.Wheel,options: {wheelAngle:0,wheelDirection:'clockwise'}}]});
    colorMorseCode.on("input:change", (color)=>ws.send(JSON.stringify({"Animations":{"parameters":{"MorseCode":{"colorMorseCode":[color.rgb.r*4,color.rgb.g*4,color.rgb.b*4]}}}})));
    var bar = document.querySelector("[data=\"inputstyle\"]");
    var getRSSIinterval;
    var timeScheduleItems;
    function MergeRecursive(obj1, obj2) {
      for (var p in obj2) {
        try {
          if ( obj2[p].constructor==Object ) obj1[p] = MergeRecursive(obj1[p], obj2[p]);
          else obj1[p] = obj2[p];
        } catch(e) {
          obj1[p] = obj2[p];
        }
      }
      return obj1;
    }
    function selectAnimation(animation,parameters=null){
      var anim ={"Animations":{"playingAnimation" : animation}};
      if(parameters != null) anim = MergeRecursive(anim,parameters);
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
            if("Animations" in data) {
              var anim = data["Animations"];
              if("parameters" in anim){
                var params = anim["parameters"];
                if("SolidColor" in params) {
                  if("Color" in params["SolidColor"]) solidColor.color.rgb = { r: params.SolidColor.Color[0]/4, g: params.SolidColor.Color[1]/4, b: params.SolidColor.Color[2]/4 };
                  if("Brightness" in params["SolidColor"]){
                    var color = [solidColor.color.rgb.r*params.SolidColor.Brightness,solidColor.color.rgb.g*params.SolidColor.Brightness,solidColor.color.rgb.b*params.SolidColor.Brightness];
                    bar.innerHTML = "#brightness::-webkit-slider-thumb {box-shadow: -100vw 0 0 100vw rgb("+color+")}";
                    document.getElementById("brightness").value = params.SolidColor.Brightness * 100;
                  }
                }
                if("ColorCycle" in params){
                  if("ColorCycleSpeed" in params["ColorCycle"]) {
                    document.getElementById("ColorCycleSpeed").innerText = params.ColorCycle.ColorCycleSpeed;
                    document.getElementById("ColorCycleSpeed_input").value = params.ColorCycle.ColorCycleSpeed;
                 }
                }
                if("Breathing" in params){
                  if("breathingSpeed" in params["Breathing"]){
                    document.getElementById("breathingSpeed").innerText = params.Breathing.breathingSpeed;
                    document.getElementById("breathingSpeed_input").value = params.Breathing.breathingSpeed;
                  }
                  if ("colorListBreathing" in params["Breathing"]) {
                    colorList = params["Breathing"]["colorListBreathing"];
                    redraw("colorListResult",colorList);
                  }
                  if("staticColorBreathing" in params["Breathing"]) staticColorBreathing.color.rgb = { r: params.Breathing.staticColorBreathing[0]/4, g: params.Breathing.staticColorBreathing[1]/4, b: params.Breathing.staticColorBreathing[2]/4 };
                  if("useColorList" in params["Breathing"]) document.getElementById("useColorList").checked = params.Breathing.useColorList;
                }
                if("MorseCode" in params){
                  if("encodedMorseCode" in params["MorseCode"]) {
                    document.getElementById("morsePlainText").value = decodeMessage(params.MorseCode.encodedMorseCode);
                    document.getElementById("encodedMsgResult").value = params.MorseCode.encodedMorseCode;
                  }
                  if("unitTimeMorseCode" in params["MorseCode"]) document.getElementById("unitTime").value = params.MorseCode.unitTimeMorseCode;
                  if("colorMorseCode" in params["MorseCode"]) colorMorseCode.color.rgb = { r: params.MorseCode.colorMorseCode[0]/4, g: params.MorseCode.colorMorseCode[1]/4, b: params.MorseCode.colorMorseCode[2]/4 };
                  if("useBuzzer" in params["MorseCode"]) document.getElementById("useBuzzer").checked = params.MorseCode.useBuzzer;
                }
              }
              if ("Schedules" in anim)
              {
                var Schedules = anim["Schedules"];
                if ("enableScheduler" in Schedules) document.getElementById("enableScheduler").checked = Schedules.enableScheduler;
                if ("timeSchedule" in Schedules){
                  timeScheduleItems = Schedules.timeSchedule;
                  createAllTimeItems()
                }
              }
              if("PowerState" in anim){
                 if(anim.PowerState) document.getElementById("btn-bg").classList.add("active");
                 else document.getElementById("btn-bg").classList.remove("active");
              }
              if("playingAnimation" in anim){
                document.getElementById("startBreathing").checked = false;
                document.getElementById("startColorCycle").checked = false;
                document.getElementById("startMorseCode").checked = false;
                if(anim["playingAnimation"] == "Breathing") document.getElementById("startBreathing").checked = true;
                if(anim["playingAnimation"] == "Color Cycle") document.getElementById("startColorCycle").checked = true;
                if(anim["playingAnimation"] == "Morse Code") document.getElementById("startMorseCode").checked = true;
                document.getElementById("playingAnimation").innerText = anim.playingAnimation;
              }
              if("powerConected" in anim){
                 if(anim.powerConected) document.getElementById("powerConected").classList.add("active");
                 else document.getElementById("powerConected").classList.remove("active");
              }
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
    function sendBrightness(e) {
      e = e.value/100;
      var color = [solidColor.color.rgb.r*e,solidColor.color.rgb.g*e,solidColor.color.rgb.b*e];
      bar.innerHTML = "#brightness::-webkit-slider-thumb {box-shadow: -100vw 0 0 100vw rgb("+color+")}";
      selectAnimation("Solid Color",{"Animations":{"parameters":{"SolidColor":{'Brightness':e}}}});
    }
    var schedule_solidColor;
    var schedule_colorMorseCode;
    var schedule_staticColorBreathing;
    function selectSchedulePage(e){
      var index = document.getElementById("selectedPage").value;
      index = parseInt(index) + parseInt(e);
      document.getElementById("selectedPage").value = index;
      document.getElementById("scheduleBackButton").disabled = false;
      document.getElementById("timePanel").style.display = "none";
      document.getElementById("timeAnim").style.display = "none";
      document.getElementById("scheduleNextButton").style.display = "inline-block";
      document.getElementById("scheduleFinishButton").style.display = "none";
      document.getElementById("noParameters").style.display = "none";
      document.getElementById("scheduleSolidColor").style.display = "none";
      document.getElementById("scheduleColorCycle").style.display = "none";
      document.getElementById("scheduleBreathing").style.display = "none";
      document.getElementById("scheduleMorseCode").style.display = "none";
      switch (index) {
        case 0:
          document.getElementById("scheduleBackButton").disabled = true;
          document.getElementById("timePanel").style.display = "block";
          break;
        case 1:
          document.getElementById("timeAnim").style.display = "block";
          break;
        case 2:
          document.getElementById("scheduleNextButton").style.display = "none";
          document.getElementById("scheduleFinishButton").style.display = "inline-block";
          var anim = document.getElementById("timeAnimList").value;
          if(anim == "Solid Color"){
            document.getElementById("scheduleSolidColor").style.display = "block";
            break;
          }
          else if(anim == "Color Cycle"){
            document.getElementById("scheduleColorCycle").style.display = "block";
            break;
          }
          else if(anim == "Breathing"){
            document.getElementById("scheduleBreathing").style.display = "block";
            break;
          }
          else if(anim == "Morse Code"){
            document.getElementById("scheduleMorseCode").style.display = "block";
            break;
          }
          document.getElementById("noParameters").style.display = "block";
          break;
      }
    }

    function finishSchedule(e){
      var time = new Date();
      var timestamp = new Date(time.getFullYear(), time.getMonth(), time.getDate(), document.getElementById("hourTextBox").value, document.getElementById("minuteTextBox").value, 0);
      var param = {};
      if (document.getElementById("timeAnimList").value == "Solid Color"){
        param["SolidColor"] = {
          "Color":[schedule_solidColor.color.rgb.r*4,schedule_solidColor.color.rgb.g*4,schedule_solidColor.color.rgb.b*4],
          "Brightness":document.getElementById("schedule_brightness").value /100
        };
      }
      else if(document.getElementById("timeAnimList").value == "Color Cycle"){
        param["ColorCycle"] = {
          "ColorCycleSpeed":document.getElementById("schedule_ColorCycleSpeed").value
        };
      }
      else if(document.getElementById("timeAnimList").value == "Breathing"){
        param["Breathing"] = {
          "breathingSpeed":document.getElementById("schedule_breathingSpeed").value,
          "staticColorBreathing":[schedule_staticColorBreathing.color.rgb.r*4,schedule_staticColorBreathing.color.rgb.g*4,schedule_staticColorBreathing.color.rgb.b*4],
          "colorListBreathing":sch_colorList,
          "useColorList":document.getElementById("schedule_useColorList").checked
        };
      }
      else if(document.getElementById("timeAnimList").value == "Morse Code"){
        param["MorseCode"] = {
          "colorMorseCode":[schedule_colorMorseCode.color.rgb.r*4,schedule_colorMorseCode.color.rgb.g*4,schedule_colorMorseCode.color.rgb.b*4],
          "encodedMorseCode":document.getElementById("schedule_encodedMsgResult").value,
          "unitTimeMorseCode":document.getElementById("schedule_unitTime").value,
          "useBuzzer":document.getElementById("schedule_useBuzzer").checked
        };
      }
      var json = {
        "Label": document.getElementById("LabelText").value,
        "Days":[document.getElementById("SuCheckbox").checked,document.getElementById("MoCheckbox").checked,document.getElementById("TuCheckbox").checked,document.getElementById("WeCheckbox").checked,document.getElementById("ThCheckbox").checked,document.getElementById("FrCheckbox").checked,document.getElementById("SaCheckbox").checked],
        "Timestamp":timestamp.getTime()/1000,
        "playingAnimation": document.getElementById("timeAnimList").value,
        "parameters":param,
        "enable":true
      };
      if(e == -1) ws.send(JSON.stringify({"Animations":{"Schedules":{"newTimeSchedule":json}}}));
      else ws.send(JSON.stringify({"Animations":{"Schedules":{"editTimeSchedule":{"oldTimestamp":timeScheduleItems[e].Timestamp,"newData":json}}}}));
    }
    var scheduleWindowOpened=false;
    function createNewScheduleWindow(e,json){
      if(!scheduleWindowOpened){

        var time = new Date();
        var addItemForm = `<div style="width:700px; height:650px;  background-color:#1b2a47; border: 10px solid #152036; position:absolute; top: 50%;left: 50%;transform: translate(-50%, -50%); color:white;" id="addNewSchedule">
                <input type="number" style="visibility:hidden" id="selectedPage" value="0">
                <div style="width:450px; height:300px; background-color:#152036; margin:100 auto; padding:10px;" id="timePanel">
                  <p style="font-size:11pt;">Time schedule</p>
                  <table style="margin:80 auto;color:white;"><tr><td>Label</td><td>Hour</td><td>Minute</td></tr><tr>
                      <td><input id="LabelText"style="width:160px;height:25;" maxlength="16" value="Schedule"/></td>
                      <td><input id="hourTextBox" style="width:50px;height:25;" type="number" min="0" max="23" value="${("0" + time.getHours()).slice(-2)}"/></td>
                      <td><input id="minuteTextBox" style="width:50px;height:25;" type="number" min="0" max="59" value="${("0" + time.getMinutes()).slice(-2)}"/></td></tr><tr><td>
                        <p style="display:inline-block; margin-right:5px; font-size:8pt;">Su</p>
                        <p style="display:inline-block; margin-right:5px; font-size:8pt;">Mo</p>
                        <p style="display:inline-block; margin-right:5px; font-size:8pt;">Tu</p>
                        <p style="display:inline-block; margin-right:5px; font-size:8pt;">We</p>
                        <p style="display:inline-block; margin-right:5px; font-size:8pt;">Th</p>
                        <p style="display:inline-block; margin-right:5px; font-size:8pt;">Fr</p>
                        <p style="display:inline-block; margin-right:5px; font-size:8pt;">Sa</p></td></tr><tr><td>
                        <input id="SuCheckbox" type="checkbox" style="display:inline-block; margin-right:3px; width:15px; height15px;" checked/>
                        <input id="MoCheckbox" type="checkbox" style="display:inline-block; margin-right:3px; width:15px; height15px;" checked/>
                        <input id="TuCheckbox" type="checkbox" style="display:inline-block; margin-right:3px; width:15px; height15px;" checked/>
                        <input id="WeCheckbox" type="checkbox" style="display:inline-block; margin-right:3px; width:15px; height15px;" checked/>
                        <input id="ThCheckbox" type="checkbox" style="display:inline-block; margin-right:3px; width:15px; height15px;" checked/>
                        <input id="FrCheckbox" type="checkbox" style="display:inline-block; margin-right:3px; width:15px; height15px;" checked/>
                        <input id="SaCheckbox" type="checkbox" style="display:inline-block; margin-right:3px; width:15px; height15px;" checked/></td></tr></table></div>
                <div style="width:350px; height:350px; margin:50 auto; display:none;" id="timeAnim">
                <select size="5" style="width:350px; height:350px; font-size:13pt;background-color:#152036;border:none;overflow:hidden;color:white;" id="timeAnimList"><option value="Power Off" selected="selected">Power Off</option><option value="Power On">Power On</option><option value="Solid Color">Solid Color</option><option value="Color Cycle">Color Cycle</option><option value="Breathing">Breathing</option><option value="Morse Code">Morse Code</option></select></div>
                <div id="scheduleSolidColor" style="width:500px; height:450px; background-color:#152036; margin:50 auto; padding:10px; display:none">
                  <p class="title">Solid Color</p>
                  <div style="margin-top: 65px;text-align:center; font-size:12px">
                    <div id="schedule_solidColor" style="-webkit-box-align:center;-webkit-box-pack:center;display:-webkit-box;"></div>
                    <input  type="range" min="0" max="100" value="100" style="width:300px; margin-top:20px" id="schedule_brightness"></div></div>
                <div id="scheduleColorCycle" style="width:500px; height:350px; background-color:#152036; margin:50 auto; padding:10px; display:none">
                  <p>Color Cycle</p>
                  <div style="margin-top: 20px;text-align:center;font-size:12px">
                    <input  type="range" min="1" max="200" value="1" id="schedule_ColorCycleSpeed" oninput="document.getElementById('schedule_ColorCycleSpeed_text').innerText = this.value;">
                    <p>Speed:<i id="schedule_ColorCycleSpeed_text">1</i></p></div></div>
                <div id="scheduleBreathing" style="width:500px; height:450px; background-color:#152036; margin:50 auto; padding:10px; display:none">
                  <p class="title">Breathing</p>
                  <div style="margin-top: 20px;text-align:center; font-size:12px">
                    <input type="range" min="1" max="200" value="1" id="schedule_breathingSpeed" oninput="document.getElementById('schedule_breathingSpeed_text').innerText = this.value;">
                    <div style="height: 25px;width: 378px; margin:4px auto;">
                      <input type="checkbox" style="float:right; margin: 0px 4px;" id="schedule_useColorList">
                      <p style="float:right; margin: 0px 4px;">Use Color List</p>
                      <p style="float:right; margin: 0px 4px;">Speed:<i id="schedule_breathingSpeed_text">1</i></p></div>
                      <div id="schedule_colorListResult" style="height: 25px;width: 370px; margin:4px auto; display: table; table-layout: fixed;"><div style="background-color:#152036; display: table-cell;"></div></div>
                      <div style="height: 25px;width: 370px; margin:0 auto; display:inline-block;">
                        <button class="btn" style="width:25px; height:25px; float:right; margin:0px 5px; background-color:#3498db;" onclick="sch_br_clearColorlist()">C</button>
                        <button class="btn" style="width:25px; height:25px; float:right; margin:0px 5px; background-color:#c0392b;" onclick="sch_br_removeLastColor()">-</button>
                        <button class="btn" style="width:25px; height:25px; float:right; margin:0px 5px; background-color:#06d79c;" onclick="sch_br_addCurrentColor()">+</button>
                      </div></div><div id="schedule_staticColorBreathing" style="-webkit-box-align:center;-webkit-box-pack:center;display:-webkit-box; margin:10;"></div></div>
                <div id="scheduleMorseCode" style="width:580px; height:450px; background-color:#152036; margin:50 auto; padding:10px; display:none">
                      <p class="title">Morse Code</p>
                      <div style="display:inline-block;">
                        <p style="margin:10px 0px 5px 0px;">Plain message:</p>
                        <textarea id="schedule_morsePlainText" style="resize:none; background-color: #152036; color:white; border: 1px solid #5e5e5e;text-transform: lowercase; font-size:10pt;" rows="5" cols="50" maxlength="75" oninput="document.getElementById('schedule_encodedMsgResult').value = encodeMessage(this.value);"></textarea>
                        <p style="margin:10px 0px 5px 0px;">Encoded message:</p>
                        <textarea id="schedule_encodedMsgResult" style="resize:none; background-color: #152036; color:white; border: 1px solid #5e5e5e; font-size:10pt;" rows="5" cols="50" readonly></textarea>
                        <div style="margin-top:10px;">
                            <p style="display:inline-block;">Speed</p>
                            <input type="range" min="35" max="550" value="35" id="schedule_unitTime" style="width:110px; height:15px;">
                            <p style="display:inline-block;">Use buzzer</p>
                            <input type="checkbox" style="display:inline-block; height:15px;" id="schedule_useBuzzer">
                        </div>
                      </div>
                      <div id="schedule_colorMorseCode" style="display:inline-block; margin-left:10px;"></div></div>
                <div id="noParameters" style="width:500px; height:350px; background-color:#152036; margin:50 auto; padding:10px; display:none"><p style="text-align:center;">No parameters for this animation</p></div>
                <div style="width: 280px; height: 45px; position:absolute; bottom:10; left:210; ">
                  <button class="greenBtn" style="height:35px;" onClick="document.getElementById('addNewSchedule').remove(); scheduleWindowOpened=false;">Cancel</button>
                  <button class="greenBtn" style="height:35px;" onClick="selectSchedulePage(-1);" disabled id="scheduleBackButton">Back</button>
                  <button class="greenBtn" style="height:35px;" onClick="selectSchedulePage(1);" id="scheduleNextButton">Next</button>
                  <button class="greenBtn" style="height:35px; display:none" onClick="finishSchedule(${e}); document.getElementById('addNewSchedule').remove(); scheduleWindowOpened=false;" id="scheduleFinishButton">Finish</button><div></div>`;

        let frag = document.createRange().createContextualFragment(addItemForm);
        document.body.appendChild(frag);
        dragElement(document.getElementById('addNewSchedule'));
        schedule_solidColor = new iro.ColorPicker("#schedule_solidColor", {width: 300,layout: [{component: iro.ui.Wheel,options: {wheelAngle:0,wheelDirection:'clockwise'}}]});
        schedule_solidColor.on("input:change", (color)=>{bar.innerHTML = "#schedule_brightness::-webkit-slider-thumb {box-shadow: -100vw 0 0 100vw "+color.hexString+"}";}  );
        schedule_staticColorBreathing = new iro.ColorPicker("#schedule_staticColorBreathing", {width: 270,layout: [{component: iro.ui.Wheel,options: {wheelAngle:0,wheelDirection:'clockwise'}}]});
        schedule_colorMorseCode = new iro.ColorPicker("#schedule_colorMorseCode", {width: 170,layout: [{component: iro.ui.Wheel,options: {wheelAngle:0,wheelDirection:'clockwise'}}]});
        sch_colorList = [];
        scheduleWindowOpened=true;
      }

      if(json!=null){
        if("Label" in json) document.getElementById("LabelText").value = json["Label"];
        if("playingAnimation" in json) document.getElementById("timeAnimList").value = json["playingAnimation"];
        if("Days" in json) {
          document.getElementById("SuCheckbox").checked = json["Days"][0];
          document.getElementById("MoCheckbox").checked = json["Days"][1];
          document.getElementById("TuCheckbox").checked = json["Days"][2];
          document.getElementById("WeCheckbox").checked = json["Days"][3];
          document.getElementById("ThCheckbox").checked = json["Days"][4];
          document.getElementById("FrCheckbox").checked = json["Days"][5];
          document.getElementById("SaCheckbox").checked = json["Days"][6];
        }
        if("Timestamp" in json) {
          var time = new Date(json["Timestamp"] * 1000);
          document.getElementById("hourTextBox").value = ("0" + time.getHours()).slice(-2);
          document.getElementById("minuteTextBox").value = ("0" + time.getMinutes()).slice(-2);
        }
        if("parameters" in json) {
            var params = json["parameters"];
            if("SolidColor" in params) {
              if("Color" in params["SolidColor"]) schedule_solidColor.color.rgb = { r: params.SolidColor.Color[0]/4, g: params.SolidColor.Color[1]/4, b: params.SolidColor.Color[2]/4 };
              if("Brightness" in params["SolidColor"]){
                var color = [schedule_solidColor.color.rgb.r* params.SolidColor.Brightness,schedule_solidColor.color.rgb.g*params.SolidColor.Brightness,schedule_solidColor.color.rgb.b*params.SolidColor.Brightness];
                bar.innerHTML = "#schedule_brightness::-webkit-slider-thumb {box-shadow: -100vw 0 0 100vw rgb("+color+")}";
                document.getElementById("schedule_brightness").value = params.SolidColor.Brightness * 100;
              }
            }
            if("ColorCycle" in params){
              if("ColorCycleSpeed" in params["ColorCycle"]) {
                document.getElementById("schedule_ColorCycleSpeed").value = params.ColorCycle.ColorCycleSpeed;
                document.getElementById("schedule_ColorCycleSpeed_text").innerText = params.ColorCycle.ColorCycleSpeed;
             }
            }
            if("Breathing" in params){
              if("breathingSpeed" in params["Breathing"]){
                document.getElementById("schedule_breathingSpeed").value = params.Breathing.breathingSpeed;
                document.getElementById("schedule_breathingSpeed_text").innerText = params.Breathing.breathingSpeed;
              }
              if ("colorListBreathing" in params["Breathing"]) {
                sch_colorList = params["Breathing"]["colorListBreathing"];
                redraw("schedule_colorListResult",sch_colorList);
              }
              if("staticColorBreathing" in params["Breathing"]) schedule_staticColorBreathing.color.rgb = { r: params.Breathing.staticColorBreathing[0]/4, g: params.Breathing.staticColorBreathing[1]/4, b: params.Breathing.staticColorBreathing[2]/4 };
              if("useColorList" in params["Breathing"]) document.getElementById("schedule_useColorList").checked = params.Breathing.useColorList;
            }
            if("MorseCode" in params){
              if("encodedMorseCode" in params["MorseCode"]) {
                document.getElementById("schedule_morsePlainText").value = decodeMessage(params.MorseCode.encodedMorseCode);
                document.getElementById("schedule_encodedMsgResult").value = params.MorseCode.encodedMorseCode;
              }
              if("unitTimeMorseCode" in params["MorseCode"]) document.getElementById("schedule_unitTime").value = params.MorseCode.unitTimeMorseCode;
              if("colorMorseCode" in params["MorseCode"]) schedule_colorMorseCode.color.rgb = { r: params.MorseCode.colorMorseCode[0]/4, g: params.MorseCode.colorMorseCode[1]/4, b: params.MorseCode.colorMorseCode[2]/4 };
              if("useBuzzer" in params["MorseCode"]) document.getElementById("schedule_useBuzzer").checked = params.MorseCode.useBuzzer;
            }
        }
      }
    }
    function createAllTimeItems(){
      document.getElementById("timeScheduleContainer").innerHTML = "";
      timeScheduleItems.forEach((item, i) => createTimeSchedule(item["Label"], item["playingAnimation"], item["Days"],item["Timestamp"],item["enable"],i));
    }
    function createTimeSchedule(_Label, _playingAnimation, _days,_timestamp,_enable,index){
      var time = new Date(_timestamp * 1000);
      var container = document.getElementById("timeScheduleContainer");
      var item = `
        <div id="itemContainer" style="width:90%; height:50px; background-color:#1b2a47; margin:20 auto; " value="3">
          <button style="width:17px; height:100%; background-color:#f44336; border:none; display:inline-block; color:white; vertical-align: top;" onClick="ws.send(JSON.stringify( {'Animations':{'Schedules':{'removeTimeSchedule':{'Timestamp': ${_timestamp}}}}}));" value="${index}">X</button>
          <button style="width:17px; height:100%; background-color:#626262; border:none; display:inline-block; color:white; vertical-align: top;" onClick="createNewScheduleWindow(${index},timeScheduleItems[${index}])"><i class="fas fa-cog"></i></button>
          <p style="width:70px; height:100%; line-height: 50px; text-align: center; display:inline-block; color:white; font-size: 14pt; vertical-align: top;"> ${("0" + time.getHours()).slice(-2)}:${("0" + time.getMinutes()).slice(-2)} </p>
          <div style="display:inline-block; width:120px; vertical-align: top; ">
              <p style="font-size: 10pt; margin:5 0 0 5; display:block;"> ${_Label}</p>
              <p style="font-size: 10pt; margin:5 0 0 5; display:block;">${_playingAnimation}</p>
          </div>
          <div style="display:inline-block; vertical-align: top; margin-right:10px; width:120px; height:100%;">`;
      var week = ["Su","Mo","Tu","We","Th","Fr","Sa"];
      for (var i=0; i<7; i++){
        if (_days[i]) item+= `<label style="font-size:8pt; line-height: 48px; margin-right:4px; color:#06d79c">${week[i]}</label>`;
        else item+= `<label style="font-size:8pt; line-height: 48px; margin-right:4px; color:white">${week[i]}</label>`;
      }
      item+=`</div><label class="switch" style="margin-top:14px;">`;
      if (_enable) item +=  `<input type="checkbox" id="enableTimeSchedule" oninput="ws.send(JSON.stringify( {'Animations':{'Schedules':{'editTimeSchedule':{'oldTimestamp':${_timestamp},"newData":{"enable":this.checked}}}}}));" checked />`;
      else item +=  `<input type="checkbox" id="enableTimeSchedule" oninput="ws.send(JSON.stringify( {'Animations':{'Schedules':{'editTimeSchedule':{'oldTimestamp':${_timestamp},"newData":{"enable":this.checked}}}}}));" />`;
      item += `<span class="slider round"></span></label></div>`;
      container.innerHTML += item;
    }
    function fpowerConected(){
      $('#powerConected').toggleClass('active');
      var a = $('#powerConected').hasClass("active");
      ws.send(JSON.stringify({"Animations":{"parameters":{'powerConected':a}}}));
    }
    function checkboxToggle(){
      $('#btn-bg').toggleClass('active');
      var powerCheckBox = $('#btn-bg').hasClass("active");
      if (powerCheckBox) selectAnimation("Power On");
      else selectAnimation("Power Off");
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
    function sch_br_addCurrentColor(){
        if(sch_colorList.length != 25){
          sch_colorList.push([schedule_staticColorBreathing.color.rgb.r*4,schedule_staticColorBreathing.color.rgb.g*4,schedule_staticColorBreathing.color.rgb.b*4]);
          redraw("schedule_colorListResult",sch_colorList);
        }else alert("25 colors limit");
    }
    function sch_br_removeLastColor(){
      if(sch_colorList.length != 0){
        sch_colorList.splice(-1,1)
        redraw("schedule_colorListResult",sch_colorList);
      }
    }
    function sch_br_clearColorlist(){
      sch_colorList = [];
      redraw("schedule_colorListResult",sch_colorList);
    }
    function redraw(e,list){
        document.getElementById(e).innerHTML = "";
        if(list.length>0){
        list.forEach((item, i) => {
          var obj = document.createElement('div');
          obj.style.cssText = 'background-color:rgb('+item[0]/4+','+item[1]/4+','+item[2]/4+'); display: table-cell; border:1px solid #2e2e36';
          document.getElementById(e).appendChild(obj);
        });
      }else{
        var obj = document.createElement('div');
        obj.style.cssText = 'background-color:#152036; display: table-cell;';
        document.getElementById(e).appendChild(obj);
      }
    }
    var alphabet = ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "!", "@", "&", "(", ")", "-", "_", "=", "+", ".", ",", "/", "?", ";", ":", "\"", "\'" ];
    var morse = [".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--..",".----","..---","...--","....-","-....","--...","---..","----.","-----","-.-.--",".--.-.", ".....", ".-...", "-.--.", "-.--.-", "-....-", "..--.-", "-...-", ".-.-.", ".-.-.-", "--..--", "-..-.", "..--..", "-.-.-.", "---...", ".----.", ".-..-." ];
    let timeout = null;
    function encode(e){
      e.addEventListener('keyup', function (k){
        clearTimeout(timeout);
        timeout = setTimeout(function(){
          var encoded = encodeMessage(e.value);
          document.getElementById("encodedMsgResult").value = encoded;
          ws.send(JSON.stringify({'Animations':{"parameters":{'MorseCode':{'encodedMorseCode':encoded}}}})); }, 700);});
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
    function dragElement(elmnt) {
      var pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0;
      elmnt.onmousedown = dragMouseDown;
      function dragMouseDown(e) {
        e = e || window.event;
        if(e.srcElement.id == elmnt.id){
          pos3 = e.clientX;
          pos4 = e.clientY;
          document.onmouseup = closeDragElement;
          document.onmousemove = elementDrag;
        }
      }
      function elementDrag(e) {
        e = e || window.event;
        e.preventDefault();
        pos1 = pos3 - e.clientX;
        pos2 = pos4 - e.clientY;
        pos3 = e.clientX;
        pos4 = e.clientY;
        elmnt.style.top = (elmnt.offsetTop - pos2) + "px";
        elmnt.style.left = (elmnt.offsetLeft - pos1) + "px";
      }
      function closeDragElement() {document.onmouseup = null;document.onmousemove = null;}
    }
  </script>
</body>
</html>
)=====";


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
  Animations["powerConected"] = false;  
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
      else{
        resetMorseCode(configJson["Animations"]["parameters"]["MorseCode"]["unitTimeMorseCode"].as<int>()*25);
      }        
    }
  }
  
}

bool startBreathing = false, inverseBool = true;

float breathingPulse=1.0f;
unsigned long breathingMillis;
int colorListindex = 0;
bool colorChanged = false;


void handleBreathing(){
  if (currentMillis - breathingMillis >= configJson["Animations"]["parameters"]["Breathing"]["breathingSpeed"].as<int>()) {
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
        if(configJson["Animations"]["parameters"]["Breathing"]["useColorList"].as<bool>() && !colorChanged) {
          colorListindex++;
          if(colorListindex+1 > configJson["Animations"]["parameters"]["Breathing"]["colorListBreathing"].as<JsonArray>().size()) colorListindex = 0;  
          colorChanged = true;
        }
      } 
      if(configJson["Animations"]["parameters"]["Breathing"]["useColorList"].as<bool>()) SetColorValue(configJson["Animations"]["parameters"]["Breathing"]["colorListBreathing"][colorListindex][0].as<int>(),configJson["Animations"]["parameters"]["Breathing"]["colorListBreathing"][colorListindex][1].as<int>(),configJson["Animations"]["parameters"]["Breathing"]["colorListBreathing"][colorListindex][2].as<int>(),breathingPulse);
      else SetColorValue(configJson["Animations"]["parameters"]["Breathing"]["staticColorBreathing"][0].as<int>(),configJson["Animations"]["parameters"]["Breathing"]["staticColorBreathing"][1].as<int>(),configJson["Animations"]["parameters"]["Breathing"]["staticColorBreathing"][2].as<int>(),breathingPulse);
  }
}

bool startColorCycle = false;
bool startBool[]={true,true,true};
int colorIndexColorCycle[]={1020,0,0};
unsigned long prevMillisColorCycle;

void handleColorCycle(){
   if(currentMillis - prevMillisColorCycle > configJson["Animations"]["parameters"]["ColorCycle"]["ColorCycleSpeed"].as<int>()){
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
        webSocket.broadcastTXT("{\"Animations\":{\"playingAnimation\":\"Power Off\",\"PowerState\":"+String(configJson["Animations"]["PowerState"].as<bool>() ? "true":"false")+"}}");
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
      webSocket.broadcastTXT("{\"Animations\":{\"playingAnimation\":\""+configJson["Animations"]["playingAnimation"].as<String>()+"\",\"PowerState\":"+String(configJson["Animations"]["PowerState"].as<bool>() ? "true":"false")+"}}");
      Serial.println("Playing: "+configJson["Animations"]["playingAnimation"].as<String>());
    }
    else
    {
      if (configJson["Animations"]["PowerState"] == false){
        configJson["Animations"]["PowerState"] = true;
        turnOffAnimations();
        webSocket.broadcastTXT("{\"Animations\":{\"playingAnimation\":\""+configJson["Animations"]["playingAnimation"].as<String>()+"\",\"PowerState\":"+String(configJson["Animations"]["PowerState"].as<bool>() ? "true":"false")+"}}");
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
      webSocket.broadcastTXT("{\"Animations\":{\"playingAnimation\":\""+configJson["Animations"]["playingAnimation"].as<String>()+"\"}}");
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
          webSocket.broadcastTXT("{\"Animations\":{\"parameters\":{\"Breathing\":{\"colorListBreathing\":"+configParam["Breathing"]["colorListBreathing"].as<String>()+"}}}}");
        }
      }
      if(_Breathing.containsKey("removeLastfromList")){
        if(configParam["Breathing"]["colorListBreathing"].size()!=0) {
          configParam["Breathing"]["colorListBreathing"].remove(configParam["Breathing"]["colorListBreathing"].size()-1);  
          webSocket.broadcastTXT("{\"Animations\":{\"parameters\":{\"Breathing\":{\"colorListBreathing\":"+configParam["Breathing"]["colorListBreathing"].as<String>()+"}}}}");
        }
      }
      if(_Breathing.containsKey("clearColorList")) {
        configParam["Breathing"]["colorListBreathing"] = configParam["Breathing"].createNestedArray("colorListBreathing");
        webSocket.broadcastTXT("{\"Animations\":{\"parameters\":{\"Breathing\":{\"colorListBreathing\":"+configParam["Breathing"]["colorListBreathing"].as<String>()+"}}}}");
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
      if(configJson["Animations"]["powerConected"]) setAnimation("Power On");
      String config = configJson.as<String>();
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
                        webSocket.broadcastTXT("{\"Animations\":{\"parameters\":" + (*it)["parameters"].as<String>()+"}}");
                        break;                     
                      }
                    }
                  });
                task.enable = newSchedule["enable"];
                scheduler.add(task);
              }           
              webSocket.broadcastTXT("{\"Animations\":{\"Schedules\":{\"timeSchedule\":"+configJson["Animations"]["Schedules"]["timeSchedule"].as<String>()+"}}}");
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
                      webSocket.broadcastTXT("{\"Animations\":{\"parameters\":" + (*it)["parameters"].as<String>()+"}}");
                      break;                     
                    }
                  }
                });
              task.enable = newSchedule["enable"];
              scheduler.add(task);
              webSocket.broadcastTXT("{\"Animations\":{\"Schedules\":{\"timeSchedule\":"+configJson["Animations"]["Schedules"]["timeSchedule"].as<String>()+"}}}");            
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
                webSocket.broadcastTXT("{\"Animations\":{\"Schedules\":{\"timeSchedule\":"+configJson["Animations"]["Schedules"]["timeSchedule"].as<String>()+"}}}");
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
                          webSocket.broadcastTXT("{\"Animations\":{\"parameters\":" + (*it)["parameters"].as<String>()+"}}");
                          break;                     
                        }
                      }
                    };   
                  }
                  if (newdata.containsKey("enable")) {
                    timeSchedule[i]["enable"] = newdata["enable"].as<bool>();
                    scheduler.schedules[i].enable = newdata["enable"].as<bool>();
                  }
                  webSocket.broadcastTXT("{\"Animations\":{\"Schedules\":{\"timeSchedule\":"+configJson["Animations"]["Schedules"]["timeSchedule"].as<String>()+"}}}");
                  break;
                }
              }
            }
          }
          
          
          if (_schedules.containsKey("enableScheduler")) configJson["Animations"]["Schedules"]["enableScheduler"] = _schedules["enableScheduler"].as<bool>();
        }

        if (_Animations.containsKey("powerConected")) configJson["Animations"]["powerConected"] = _Animations["powerConected"];
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
      if(configJson["Animations"]["powerConected"]) setAnimation("Power Off");
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
  if(configJson["ESP_Config"]["Initialized"].as<bool>() && recv_json["ESP_Config"]["PASSWORD"].as<String>() == "**********"){
    recv_json["ESP_Config"]["PASSWORD"] = configJson["ESP_Config"]["PASSWORD"].as<String>();
  }
      
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
    File configFile = SPIFFS.open("/user_settings.json", "w+");
    combine(configJson, recv_json);
    serializeJson(configJson, configFile);
    configFile.close();
  }
  recv_json.clear();
  ESP.restart();
}

bool loadConfig() {
  File configFile = SPIFFS.open("/user_settings.json", "r");
  if (!configFile) return false;
  deserializeJson(configJson, configFile);
  Serial.println(configJson.as<String>());
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
                webSocket.broadcastTXT("{\"Animations\":{\"parameters\":" + (*it)["parameters"].as<String>()+"}}");
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
    File configFile = SPIFFS.open("/user_settings.json", "w+");
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
    ArduinoOTA.onStart([]() {
      SetColorValue(0,0,0,0);
    });
    ArduinoOTA.begin();
    scheduler.begin();

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
  server.on("/pinTester", pinTester);
  server.on("/sendConfig", saveConfig);
  server.on("/getWifi", getWifi);
  server.on("/returnConfigData", returnConfigData);
  server.on("/formatDevice", removeUserSettings);
  server.on("/removeUserData", removeUserData);
  server.on("/syncData", syncData);
  server.on("/restartESP", restartESP);
  server.on("/getSignalStrenght", getSignalStrenght);
  server.on("/getVersion", getVersion);
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