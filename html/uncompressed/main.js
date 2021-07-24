document.getElementById("hostname").innerText = window.location.hostname;
var myDevice = new ESPRGB(window.location.hostname);
myDevice.SolidColor.on('Color:change', (e)=>{
  solidColor.color.rgb = { r: e.Color[0], g: e.Color[1], b: e.Color[2] };
  setBrightnessColor(document.getElementById("brightness").value/100);
});
myDevice.SolidColor.on('powerButton:change', (e)=>{
  if(e.powerButton) document.getElementById("btn-bg").classList.add("active");
  else document.getElementById("btn-bg").classList.remove("active");
});
myDevice.SolidColor.on('Brightness:change', (e)=>{
  setBrightnessColor(e.Brightness);
  document.getElementById('brightness').value = e.Brightness*100
});
myDevice.ColorCycle.on('Speed:change', (e)=> {
  document.getElementById('ColorCycleSpeed_input').value = e.Speed;
  document.getElementById('ColorCycleSpeed').innerHTML = e.Speed;
});
myDevice.Breathing.on('Speed:change', (e)=> {
  document.getElementById('breathingSpeed_input').value = e.Speed;
  document.getElementById('breathingSpeed').innerHTML = e.Speed;
});
myDevice.Breathing.on('useColorList:change', (e)=> document.getElementById('useColorList').checked = e.useColorList);
myDevice.Breathing.on('StaticColor:change', (e)=> {
  staticColorBreathing.color.rgb = { r: e.StaticColor[0], g: e.StaticColor[1], b: e.StaticColor[2] };
  document.getElementById("BreathingHex").value = e.Hex;
});
myDevice.Breathing.on('colorList:change', (e)=> redrawColorList('colorListResult',e.colorList));
myDevice.MorseCode.on('Color:change', (e)=> {
  colorMorseCode.color.rgb = { r: e.Color[0], g: e.Color[1], b: e.Color[2] };
  document.getElementById("MorseCodeHex").value = e.Hex;
});
myDevice.MorseCode.on('Speed:change', (e)=> document.getElementById('unitTime').value = e.Speed);
myDevice.MorseCode.on('useBuzzer:change', (e)=> document.getElementById('useBuzzer').checked = e.useBuzzer);
myDevice.MorseCode.on('encodedText:change', (e)=> {
  document.getElementById('morsePlainText').value = e.PlainText;
  document.getElementById('encodedMsgResult').value = e.EncodedText;
});
myDevice.TimeSchedule.on('Enabled:change', (e)=> document.getElementById('enableScheduler').checked = e.Enabled);
myDevice.TimeSchedule.on('Schedules:change', (e)=> createAllTimeItems(e.Schedules));
myDevice.on('playingAnimation:change', (e)=> {
  document.getElementById("startBreathing").checked = false;
  document.getElementById("startColorCycle").checked = false;
  document.getElementById("startMorseCode").checked = false;
  if(e.playingAnimation == "Breathing") document.getElementById("startBreathing").checked = true;
  if(e.playingAnimation == "Color Cycle") document.getElementById("startColorCycle").checked = true;
  if(e.playingAnimation == "Morse Code") document.getElementById("startMorseCode").checked = true;
  document.getElementById("playingAnimation").innerText = e.playingAnimation;
});
myDevice.on('powerConnected:change', (e)=> {
  if(e.powerConnected) document.getElementById("powerConnected").classList.add("active");
  else document.getElementById("powerConnected").classList.remove("active");
});
myDevice.on('Version:change', (e)=> document.getElementById("ESPRGB-Version").innerText = e.Version);
myDevice.on('RSSI:change', (e)=> {
  if (e.RSSI >= -50) document.getElementById("signalStrenght").innerText = "Excellent";
  else if (e.RSSI < -50 && e.RSSI >= -60) document.getElementById("signalStrenght").innerText = "Good";
  else if (e.RSSI < -60 && e.RSSI >= -70) document.getElementById("signalStrenght").innerText = "Fair";
  else if (e.RSSI < -70) document.getElementById("signalStrenght").innerText = "Weak";
});
myDevice.on('connected:change', (e)=> {
  if(e.connected) document.getElementById("disconnectButton").innerText = "Disconnect";
  else document.getElementById("disconnectButton").innerText = "Connect";
});

var solidColor = new iro.ColorPicker("#solidColor", {width: 300,layout: [{component: iro.ui.Wheel,options: {wheelAngle:0,wheelDirection:'clockwise'}}]});
solidColor.on("input:change", (color)=>{  
  myDevice.SolidColor.setRGBColor(color.rgb.r, color.rgb.g, color.rgb.b);
  setBrightnessColor(document.getElementById("brightness").value/100);
  document.getElementById("SolidColorHex").value = myDevice.SolidColor.Hex;
});
var staticColorBreathing = new iro.ColorPicker("#staticColorBreathing", {width: 270,layout: [{component: iro.ui.Wheel,options: {wheelAngle:0,wheelDirection:'clockwise'}}]});
staticColorBreathing.on("input:change", (color)=>{
  myDevice.Breathing.setRGBColor(color.rgb.r, color.rgb.g, color.rgb.b);
  document.getElementById("BreathingHex").value = myDevice.Breathing.Hex;
});
var colorMorseCode = new iro.ColorPicker("#colorMorseCode", {width: 170,layout: [{component: iro.ui.Wheel,options: {wheelAngle:0,wheelDirection:'clockwise'}}]});
colorMorseCode.on("input:change", (color)=>{
  myDevice.MorseCode.setRGBColor(color.rgb.r, color.rgb.g, color.rgb.b);
  document.getElementById("MorseCodeHex").value = myDevice.MorseCode.Hex;
});
var bar = document.querySelector("[data=\"inputstyle\"]");
function setBrightnessColor(value){
  const color = [];
  myDevice.SolidColor.Color.forEach(x => color.push(x*value));
  bar.innerHTML = `#brightness::-webkit-slider-thumb {box-shadow: -100vw 0 0 100vw rgb(${color})}`;
}
function redrawColorList(id,list){
  const colorListDiv = document.getElementById(id);
  colorListDiv.innerHTML = "";
  if(list.length>0){
    list.forEach((item, i) => {
      var obj = document.createElement('div');
      obj.style.cssText = `background-color:rgb(${item[0]},${item[1]},${item[2]}); display: table-cell; border:1px solid #2e2e36`;
      colorListDiv.appendChild(obj);
    });
    return;
  }
  var obj = document.createElement('div');
  obj.style.cssText = 'background-color:#152036; display: table-cell;';
  document.getElementById(id).appendChild(obj);
}
function createAllTimeItems(e){
  document.getElementById("timeScheduleContainer").innerHTML = "";
  e.forEach((item, i) => createTimeSchedule(item["Label"], item["playingAnimation"], item["Days"],item["Timestamp"],item["enable"],i));
}
function createTimeSchedule(_Label, _playingAnimation, _days,_timestamp,_enable,index){
  var time = new Date(_timestamp * 1000);
  var container = document.getElementById("timeScheduleContainer");
  var item = `
    <div id="itemContainer" style="width:90%; height:50px; background-color:#1b2a47; margin:20 auto; " value="3">
      <button style="width:17px; height:100%; background-color:#f44336; border:none; display:inline-block; color:white; vertical-align: top;" onClick="myDevice.TimeSchedule.Remove(${_timestamp})" value="${index}">X</button>
      <button style="width:17px; height:100%; background-color:#626262; border:none; display:inline-block; color:white; vertical-align: top;" onClick="createNewScheduleWindow(${index},myDevice.TimeSchedule.Schedules[${index}])"><i class="fas fa-cog"></i></button>
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
  if (_enable) item +=  `<input type="checkbox" id="enableTimeSchedule" oninput="myDevice.TimeSchedule.EnableSchedule(${_timestamp},this.checked)" checked />`;
  else item +=  `<input type="checkbox" id="enableTimeSchedule" oninput="myDevice.TimeSchedule.EnableSchedule(${_timestamp},this.checked)" />`;
  item += `<span class="slider round"></span></label></div>`;
  container.innerHTML += item;
}   
var scheduleWindowOpened=false;
var schedule_solidColor;
var schedule_staticColorBreathing;
var schedule_colorMorseCode;
var sch_colorList = [];
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
                    <p style="display:inline-block; margin-right:5px; font-size:8pt;">Mo</p>
                    <p style="display:inline-block; margin-right:5px; font-size:8pt;">Tu</p>
                    <p style="display:inline-block; margin-right:5px; font-size:8pt;">We</p>
                    <p style="display:inline-block; margin-right:5px; font-size:8pt;">Th</p>
                    <p style="display:inline-block; margin-right:5px; font-size:8pt;">Fr</p>
                    <p style="display:inline-block; margin-right:5px; font-size:8pt;">Sa</p>
                    </td></tr><tr><td>
                    <input id="SuCheckbox" type="checkbox" style="display:inline-block; margin-right:5px; width:15px; height15px;" checked/>
                    <input id="MoCheckbox" type="checkbox" style="display:inline-block; margin-right:5px; width:15px; height15px;" checked/>
                    <input id="TuCheckbox" type="checkbox" style="display:inline-block; margin-right:3px; width:15px; height15px;" checked/>
                    <input id="WeCheckbox" type="checkbox" style="display:inline-block; margin-right:3px; width:15px; height15px;" checked/>
                    <input id="ThCheckbox" type="checkbox" style="display:inline-block; margin-right:3px; width:15px; height15px;" checked/>
                    <input id="FrCheckbox" type="checkbox" style="display:inline-block; margin-right:3px; width:15px; height15px;" checked/>
                    <input id="SaCheckbox" type="checkbox" style="display:inline-block; margin-right:3px; width:15px; height15px;" checked/>
                    </td></tr></table></div>
            <div style="width:350px; height:350px; margin:50 auto; display:none;" id="timeAnim">
            <select size="5" style="width:350px; height:350px; font-size:13pt;background-color:#152036;border:none;overflow:hidden;color:white;" id="timeAnimList"><option value="Power Off" selected="selected">Power Off</option><option value="Power On">Power On</option><option value="Solid Color">Solid Color</option><option value="Color Cycle">Color Cycle</option><option value="Breathing">Breathing</option><option value="Morse Code">Morse Code</option></select></div>
            <div id="scheduleSolidColor" style="width:500px; height:450px; background-color:#152036; margin:50 auto; padding:10px; display:none">
              <p class="title">Solid Color</p>
              <div style=" width: 300px; margin:20 auto; height: 400px;">
                <div id="schedule_solidColor" style="-webkit-box-align:center;-webkit-box-pack:center;display:-webkit-box;"></div>
                <div class="relative" style='height:52px;'></div>
                <input id="schedule_SolidColorHex" type="input" value="#ffffff" maxlength="7"  class="relative hexInput"/>
                <div style="height: 80px; position: relative; top:-45px;"> <input  type="range" min="0" max="100" value="100" style="width:300px" id="schedule_brightness"/></div>    </div></div>
            <div id="scheduleColorCycle" style="width:500px; height:350px; background-color:#152036; margin:50 auto; padding:10px; display:none">
              <p>Color Cycle</p>
              <div style="margin-top: 20px;text-align:center;font-size:12px">
                <input  type="range" min="1" max="200" value="1" id="schedule_ColorCycleSpeed" oninput="document.getElementById('schedule_ColorCycleSpeed_text').innerText = this.value;">
                <p>Speed:<i id="schedule_ColorCycleSpeed_text">1</i></p></div></div>
            <div id="scheduleBreathing" style="width:500px; height:450px; background-color:#152036; margin:50 auto; padding:10px; display:none">
              <p class="title">Breathing</p>
              <div style="margin: 20px auto;font-size:12px; width: 400;">
                <div style="margin: 0 auto; width: 380; text-align: center;"> <input type="range" min="1" max="200" value="1" id="schedule_breathingSpeed" oninput="document.getElementById('schedule_breathingSpeed_text').innerText = this.value;"></div>
                <div style="height: 25px;width: 378px; margin:4px auto;">
                  <input type="checkbox" style="float:right; margin: 0px 4px;" id="schedule_useColorList">
                  <p style="float:right; margin: 0px 4px;">Use Color List</p>
                  <p style="float:right; margin: 0px 4px;">Speed:<i id="schedule_breathingSpeed_text">1</i></p></div>
                  <div id="schedule_colorListResult" style="height: 25px;width: 370px; margin:4px auto; display: table; table-layout: fixed;"><div style="background-color:#152036; display: table-cell;"></div></div>
                  <div style="height: 25px;width: 370px; margin:0 auto; display:inline-block;">
                    <button class="btn" style="width:25px; height:25px; float:right; margin:0px 5px; background-color:#3498db;" onclick="sch_br_clearColorlist()">C</button>
                    <button class="btn" style="width:25px; height:25px; float:right; margin:0px 5px; background-color:#c0392b;" onclick="sch_br_removeLastColor()">-</button>
                    <button class="btn" style="width:25px; height:25px; float:right; margin:0px 5px; background-color:#06d79c;" onclick="sch_br_addCurrentColor()">+</button></div>
                  <div id="schedule_staticColorBreathing" style="-webkit-box-align:center;-webkit-box-pack:center;display:-webkit-box; margin:10;"></div>
                  <input type="input" value="#ffffff" maxlength="7" id="schedule_BreathingHex" class="relative hexInput" style="left: 45; top:-30;">  </div></div>
            <div id="scheduleMorseCode" style="width:580px; height:450px; background-color:#152036; margin:50 auto; padding:10px; display:none">
                  <p class="title">Morse Code</p>
                  <div style="display:inline-block;">
                    <p style="margin:10px 0px 5px 0px;">Plain message:</p>
                    <textarea id="schedule_morsePlainText" style="resize:none; background-color: #152036; color:white; border: 1px solid #5e5e5e;text-transform: lowercase; font-size:10pt;" rows="5" cols="50" maxlength="75" oninput="document.getElementById('schedule_encodedMsgResult').value = myDevice.MorseCode. encodeMessage(this.value);"></textarea>
                    <p style="margin:10px 0px 5px 0px;">Encoded message:</p>
                    <textarea id="schedule_encodedMsgResult" style="resize:none; background-color: #152036; color:white; border: 1px solid #5e5e5e; font-size:10pt;" rows="5" cols="50" readonly></textarea>
                    <div style="margin-top:10px;">
                      <p style="display:inline-block;">Speed</p>
                      <input type="range" min="35" max="550" value="35" id="schedule_unitTime" style="width:110px; height:15px;">
                      <p style="display:inline-block;">Use buzzer</p>
                      <input type="checkbox" style="display:inline-block; height:15px;" id="schedule_useBuzzer"></div></div>
                  <div style="display: inline-block; width: 170; margin-left:10px;"> <div id="schedule_colorMorseCode"></div>
                  <input type="input" value="#ffffff" maxlength="7" id="schedule_MorseCodeHex" class="relative hexInput" style="left: 0; top:0;"></div></div>
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
    schedule_staticColorBreathing = new iro.ColorPicker("#schedule_staticColorBreathing", {width: 270,layout: [{component: iro.ui.Wheel,options: {wheelAngle:0,wheelDirection:'clockwise'}}]});
    schedule_colorMorseCode = new iro.ColorPicker("#schedule_colorMorseCode", {width: 170,layout: [{component: iro.ui.Wheel,options: {wheelAngle:0,wheelDirection:'clockwise'}}]});
    schedule_colorMorseCode.on("input:change", (color)=> document.getElementById('schedule_MorseCodeHex').value = color.hexString );
    schedule_staticColorBreathing.on("input:change", (color)=> document.getElementById('schedule_BreathingHex').value = color.hexString );
    schedule_solidColor.on("input:change", (color)=> {
      bar.innerHTML = "#schedule_brightness::-webkit-slider-thumb {box-shadow: -100vw 0 0 100vw "+color.hexString+"}";
      document.getElementById('schedule_SolidColorHex').value = color.hexString
    });    
    document.getElementById('schedule_MorseCodeHex').oninput = (e)=>{if(myDevice.isHexColor(e.target.value)) schedule_colorMorseCode.color.hexString = e.target.value};
    document.getElementById('schedule_BreathingHex').oninput = (e)=>{if(myDevice.isHexColor(e.target.value)) schedule_staticColorBreathing.color.hexString = e.target.value};
    document.getElementById('schedule_SolidColorHex').oninput = (e)=>{if(myDevice.isHexColor(e.target.value)) schedule_solidColor.color.hexString = e.target.value};
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
          if("Color" in params["SolidColor"]) {
            schedule_solidColor.color.rgb = { r: params.SolidColor.Color[0]/4, g: params.SolidColor.Color[1]/4, b: params.SolidColor.Color[2]/4 };
            document.getElementById("schedule_SolidColorHex").value = myDevice.rgbToHex(params.SolidColor.Color[0]/4, params.SolidColor.Color[1]/4, params.SolidColor.Color[2]/4);
          }
          if("Brightness" in params["SolidColor"]){
            var color = [schedule_solidColor.color.rgb.r* params.SolidColor.Brightness,schedule_solidColor.color.rgb.g*params.SolidColor.Brightness,schedule_solidColor.color.rgb.b*params.SolidColor.Brightness];
            bar.innerHTML = `#schedule_brightness::-webkit-slider-thumb {box-shadow: -100vw 0 0 100vw rgb(${color})}`;
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
            redrawColorList("schedule_colorListResult",sch_colorList);
          }
          if("staticColorBreathing" in params["Breathing"]) {
            schedule_staticColorBreathing.color.rgb = { r: params.Breathing.staticColorBreathing[0]/4, g: params.Breathing.staticColorBreathing[1]/4, b: params.Breathing.staticColorBreathing[2]/4 };
            document.getElementById("schedule_BreathingHex").value = myDevice.rgbToHex(params.Breathing.staticColorBreathing[0]/4, params.Breathing.staticColorBreathing[1]/4, params.Breathing.staticColorBreathing[2]/4);            
          }
          if("useColorList" in params["Breathing"]) document.getElementById("schedule_useColorList").checked = params.Breathing.useColorList;
        }
        if("MorseCode" in params){
          if("encodedMorseCode" in params["MorseCode"]) {
            document.getElementById("schedule_morsePlainText").value =  myDevice.MorseCode.decodeMessage(params.MorseCode.encodedMorseCode);
            document.getElementById("schedule_encodedMsgResult").value = params.MorseCode.encodedMorseCode;
          }
          if("unitTimeMorseCode" in params["MorseCode"]) document.getElementById("schedule_unitTime").value = params.MorseCode.unitTimeMorseCode;
          if("colorMorseCode" in params["MorseCode"]) {
            schedule_colorMorseCode.color.rgb = { r: params.MorseCode.colorMorseCode[0]/4, g: params.MorseCode.colorMorseCode[1]/4, b: params.MorseCode.colorMorseCode[2]/4 };
            document.getElementById("schedule_MorseCodeHex").value = myDevice.rgbToHex(params.MorseCode.colorMorseCode[0]/4, params.MorseCode.colorMorseCode[1]/4, params.MorseCode.colorMorseCode[2]/4);  
          }
          if("useBuzzer" in params["MorseCode"]) document.getElementById("schedule_useBuzzer").checked = params.MorseCode.useBuzzer;
        }
    }
  }
}
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
  if (document.getElementById("timeAnimList").value == "Solid Color")
    param["SolidColor"] = {
      "Color":[schedule_solidColor.color.rgb.r*4,schedule_solidColor.color.rgb.g*4,schedule_solidColor.color.rgb.b*4],
      "Brightness":document.getElementById("schedule_brightness").value /100
    };
  else if(document.getElementById("timeAnimList").value == "Color Cycle")
    param["ColorCycle"] = {
      "ColorCycleSpeed": parseInt(document.getElementById("schedule_ColorCycleSpeed").value)
    };
  else if(document.getElementById("timeAnimList").value == "Breathing")
    param["Breathing"] = {
      "breathingSpeed":parseInt(document.getElementById("schedule_breathingSpeed").value),
      "staticColorBreathing":[schedule_staticColorBreathing.color.rgb.r*4,schedule_staticColorBreathing.color.rgb.g*4,schedule_staticColorBreathing.color.rgb.b*4],
      "colorListBreathing":sch_colorList,
      "useColorList":document.getElementById("schedule_useColorList").checked
    };
  else if(document.getElementById("timeAnimList").value == "Morse Code")
    param["MorseCode"] = {
      "colorMorseCode":[schedule_colorMorseCode.color.rgb.r*4,schedule_colorMorseCode.color.rgb.g*4,schedule_colorMorseCode.color.rgb.b*4],
      "encodedMorseCode":document.getElementById("schedule_encodedMsgResult").value,
      "unitTimeMorseCode":parseInt(document.getElementById("schedule_unitTime").value),
      "useBuzzer":document.getElementById("schedule_useBuzzer").checked
    };
  const _label = document.getElementById("LabelText").value;
  const _timestamp = timestamp.getTime()/1000;
  const _days = [document.getElementById("SuCheckbox").checked,document.getElementById("MoCheckbox").checked,document.getElementById("TuCheckbox").checked,document.getElementById("WeCheckbox").checked,document.getElementById("ThCheckbox").checked,document.getElementById("FrCheckbox").checked,document.getElementById("SaCheckbox").checked];
  const _animation = document.getElementById("timeAnimList").value;
  if(e == -1) myDevice.TimeSchedule.addSchedule(_label,_days,_timestamp,_animation,param);
  else myDevice.TimeSchedule.editSchedule(myDevice.TimeSchedule.Schedules[e].Timestamp,_label,_days,_timestamp,_animation,param);
}
function sch_br_addCurrentColor(){
    if(sch_colorList.length != 25){
      sch_colorList.push([schedule_staticColorBreathing.color.rgb.r*4,schedule_staticColorBreathing.color.rgb.g*4,schedule_staticColorBreathing.color.rgb.b*4]);
      redrawColorList("schedule_colorListResult",sch_colorList);
    }else alert("25 colors limit");
}
function sch_br_removeLastColor(){
  if(sch_colorList.length != 0){
    sch_colorList.splice(-1,1)
    redrawColorList("schedule_colorListResult",sch_colorList);
  }
}
function sch_br_clearColorlist(){
  sch_colorList = [];
  redrawColorList("schedule_colorListResult",sch_colorList);
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
myDevice.Connect();