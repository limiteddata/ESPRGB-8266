(function (global, factory) {
    typeof exports === 'object' && typeof module !== 'undefined' ? module.exports = factory() :
    typeof define === 'function' && define.amd ? define(factory) :
    (global = global || self, global.ESPRGB = factory());
  }(this, function () { 'use strict';
    class SolidColor{
        #this;
        #powerButton;
        #Color;
        #Hex;
        #Brightness;
        constructor(_this){
            this.#this = _this;
            this.#powerButton = false;
            this.#Color = [255,255,255];
            this.#Hex = '#ffffff';
            this.#Brightness = 1.0;
        }

        set _Color(value){
            this.#Color = value;
            this.#Hex = this.#this.rgbToHex(this.#Color[0],this.#Color[1],this.#Color[2]);
            this.#this.emit(this,'Color:change', {Color:this.#Color, Hex:this.#Hex});
        }
        get Color(){
            return this.#Color;
        }
        get Hex(){
            return this.#Hex;
        }
        set _Brightness(value){
            this.#Brightness = value;
            this.#this.emit(this,'Brightness:change', {Brightness:this.#Brightness});
        }
        get Brightness(){
            return this.#Brightness;
        }
        set _powerButton(value){
            this.#powerButton = value;
            this.#this.emit(this,'powerButton:change', {powerButton:this.#powerButton});
        }
        get powerButton(){
            return this.#powerButton;
        }
        /**
         * Sets the RGB color of the animation
         * @param {number} red 
         * @param {number} green 
         * @param {number} blue 
         */
        setRGBColor(red,green,blue){
            this.#Color = [red,green,blue];
            this.#Hex = this.#this.rgbToHex(red,green,blue);
            const params = this.#this._animParam({"SolidColor":{"Color":[this.#Color[0]*4,this.#Color[1]*4,this.#Color[2]*4]}});
            this.#this.setAnimation("Solid Color",params);
        }
        /**
         * Sets the color of the animation from a hex color
         * @param {*} hex color
         */
        setHexColor(hex){
            if (!this.#this.isHexColor(hex)) return;
            const color = this.#this.hexToColor(hex);
            this.setRGBColor(color.r,color.g,color.b);
            this.#this.emit(this,'Color:change', {Color:this.#Color,Hex:this.#Hex});
        }
        /**
         * Sets the brightness of the solid color 
         * @param {float} brightness value from 0.0 to 1.0 
         * @returns 
         */
        setBrightness(brightness){
            if (brightness>1.0 || brightness<0.0) {
                console.log('Input range is between 0.0 to 1.0');
                return;
            }
            this.#Brightness = brightness;
            const params = this.#this._animParam({"SolidColor":{'Brightness':this.#Brightness}});
            this.#this.setAnimation("Solid Color",params);
        }
        /**
         * Toggles the ON/OFF state of the animation
         */
        powerButtonToggle(){
            if(this.#this.CheckIfConnected()) return;
            this.#powerButton = !this.#powerButton;
            if (this.powerButton) this.#this.setAnimation("Power On");
            else this.#this.setAnimation("Power Off");
        }
        on(eventList, callback){
            this.#this.setEvent(this,eventList, callback);
        }
    }

    class ColorCycle{
        #this;
        #Speed;
        constructor(_this){
            this.#this = _this;
            this.#Speed = 0;
        }
        set _Speed(value){
            this.#Speed = value;
            this.#this.emit(this,'Speed:change', {Speed:this.#Speed});
        }
        get Speed(){
            return this.#Speed;
        }
        /**
         * Sets the speed of the animation
         * @param {number} value 
         */
        setSpeed(value){
            if(this.#this.CheckIfConnected()) return;
            this.#Speed = value;
            this.#this.WebSocket.send( JSON.stringify( this.#this._animParam({'ColorCycle':{'ColorCycleSpeed':this.#Speed}} )));
        }
        on(eventList, callback){
            this.#this.setEvent(this,eventList, callback);
        }
    }

    class Breathing{
        #this;
        #ColorList;
        #Hex;
        #Speed;
        #useColorList;
        #StaticColor;
        constructor(_this){
            this.#this = _this;
            this.#ColorList = [];
            this.#useColorList = false;
            this.#StaticColor = [255,255,255];
            this.#Hex = '#ffffff';
            this.#Speed = 0;
        }
        set _Speed(value){
            this.#Speed = value;
            this.#this.emit(this,'Speed:change', {Speed:this.#Speed});
        }
        get Speed(){
            return this.#Speed;
        }
        set _useColorList(value){
            this.#useColorList = value;
            this.#this.emit(this,'useColorList:change', {useColorList:this.#useColorList});
        }
        get useColorList(){
            return this.#useColorList;
        }
        get Hex(){
            return this.#Hex;
        }
        get StaticColor(){
            return this.#StaticColor;
        }
        set _StaticColor(value){    
            this.#StaticColor = value;
            this.#Hex = this.#this.rgbToHex(this.#StaticColor[0],this.#StaticColor[1],this.#StaticColor[2]);
            this.#this.emit(this,'StaticColor:change', {StaticColor:this.#StaticColor,Hex:this.#Hex});
        }
        get colorList(){
            return this.#ColorList;
        }
        set _colorList(value){    
            this.#ColorList = value;
            this.#this.emit(this,'colorList:change', {colorList:this.#ColorList});
        }
        get colorList(){
            return this.#ColorList;
        }
        /**
         * Sets the speed of the animation
         * @param {number} value 
         */
        setSpeed(value){
            if(this.#this.CheckIfConnected()) return;
            this.#Speed = value;
            this.#this.WebSocket.send( JSON.stringify( this.#this._animParam({'Breathing':{'breathingSpeed':this.#Speed}} )));
        }
        /**
         * Sets the UseColorList variable to tell the controler what 
         * @param {boolean} checked 
         * @returns 
         */
        setUseColorList(checked){
            if(this.#this.CheckIfConnected()) return;
            this.#useColorList = checked;
            this.#this.WebSocket.send( JSON.stringify( this.#this._animParam({'Breathing':{'useColorList':this.#useColorList}} )));
        }
        /**
         * Sets the color of the animation from a hex color
         * @param {*} hex color
         */
        setHexColor(hex){
            if (!this.#this.isHexColor(hex)) return;
            const color = this.#this.hexToColor(hex);
            this.setRGBColor(color.r,color.g,color.b);
            this.#this.emit(this,'StaticColor:change', {StaticColor:this.#StaticColor,Hex:this.#Hex});
        }
        /**
         * Sets the RGB color of the animation
         * @param {number} red 
         * @param {number} green 
         * @param {number} blue 
         */
        setRGBColor(red,green,blue){
            if(this.#this.CheckIfConnected()) return;
            this.#StaticColor = [red,green,blue];
            this.#Hex = this.#this.rgbToHex(red,green,blue);
            this.#this.WebSocket.send( JSON.stringify( this.#this._animParam({"Breathing":{"staticColorBreathing":[this.#StaticColor[0]*4,this.#StaticColor[1]*4,this.#StaticColor[2]*4]}} )));
        }
        /**
         * Clears all the colors from the color list
         * @returns 
         */
        clearList(){
            if(this.#this.CheckIfConnected()) return;
            this.#this.WebSocket.send( JSON.stringify( this.#this._animParam({'Breathing':{'clearColorList':true}} )));
        }
        /**
         * Removes the last color from the color list
         * @returns 
         */
        removeLastfromList(){
            if(this.#this.CheckIfConnected()) return;
            this.#this.WebSocket.send( JSON.stringify( this.#this._animParam({'Breathing':{'removeLastfromList':true}} )));
        }
        /**
         * Adds to the current color list the given color 
         * @param {number} red 
         * @param {number} green 
         * @param {number} blue 
         */
        addColortoList(red, green, blue){
            if(this.#this.CheckIfConnected()) return;
            if(this.#ColorList.length != 25) this.#this.WebSocket.send( JSON.stringify( this.#this._animParam({'Breathing':{'addColortoList':[red*4,green*4,blue*4]}} )));
            else alert('25 colors limit');
        }
        on(eventList, callback){
            this.#this.setEvent(this,eventList, callback);
        }
    }

    class MorseCode{
        #this;
        #alphabet;
        #morse;
        #useBuzzer;
        #Speed;
        #Color;
        #Hex;
        #PlainText;
        #EncodedText;
        constructor(_this){
            this.#this = _this;
            this.#useBuzzer = false;
            this.#Speed = 0;
            this.#Color = [255,255,255];
            this.#Hex = '#ffffff';
            this.#PlainText = "";
            this.#EncodedText = "";     
            this.#alphabet = ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "!", "@", "&", "(", ")", "-", "_", "=", "+", ".", ",", "/", "?", ";", ":", "\"", "\'" ];
            this.#morse = [".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--..",".----","..---","...--","....-","-....","--...","---..","----.","-----","-.-.--",".--.-.", ".....", ".-...", "-.--.", "-.--.-", "-....-", "..--.-", "-...-", ".-.-.", ".-.-.-", "--..--", "-..-.", "..--..", "-.-.-.", "---...", ".----.", ".-..-." ];    
        }
        get Speed(){ return this.#Speed }
        set _Speed(value){
            this.#Speed = value;
            this.#this.emit(this,'Speed:change', {Speed:this.#Speed});
        }
        get useBuzzer(){ return this.#useBuzzer }
        set _useBuzzer(value){
            this.#useBuzzer = value;
            this.#this.emit(this,'useBuzzer:change', {useBuzzer:this.#useBuzzer});
        }
        get EncodedText(){  return this.#EncodedText  }
        get PlainText(){  return this.#PlainText  }
        set _EncodedText(value){
            this.#EncodedText = value;
            this.#PlainText = this.decodeMessage(this.#EncodedText)
            this.#this.emit(this,'encodedText:change', {PlainText:this.#PlainText,EncodedText:this.#EncodedText});
        }
        set _Color(value){
            this.#Color = value;
            this.#Hex = this.#this.rgbToHex(this.#Color[0],this.#Color[1],this.#Color[2]);
            this.#this.emit(this,'Color:change', {Color:this.#Color, Hex:this.#Hex});
        }
        get Color(){  return this.#Color  }
        get Hex(){  return this.#Hex  }
        /**
         * Sets the speed of the animation
         * @param {number} value 
         */
        setSpeed(value){
            if(this.#this.CheckIfConnected()) return;
            this.#Speed = value;
            this.#this.WebSocket.send( JSON.stringify( this.#this._animParam({'MorseCode':{'unitTimeMorseCode': this.#Speed}} )));
        }
        /**
         * Sets whether or not the controler uses the buzzer 
         * @param {boolean} checked 
         */
        setUseBuzzer(checked){
            if(this.#this.CheckIfConnected()) return;
            this.#useBuzzer = checked;
            this.#this.WebSocket.send( JSON.stringify( this.#this._animParam({'MorseCode':{'useBuzzer': this.#useBuzzer}} )));
        }
        /**
         * Encodes the plain message and sends it to the controler
         * @param {string} text 
         */
        setText(text){
            if(this.#this.CheckIfConnected()) return;
            this.#PlainText = text;
            this.#EncodedText = this.encodeMessage(this.#PlainText);
            this.#this.WebSocket.send( JSON.stringify( this.#this._animParam({'MorseCode':{'encodedMorseCode': this.#EncodedText}} )));
            return this.#EncodedText;
        }
        /**
         * Sets the color of the animation from a hex color
         * @param {*} hex color
         */
        setHexColor(hex){
            if (!this.#this.isHexColor(hex)) return;
            const color = this.#this.hexToColor(hex);
            this.setRGBColor(color.r,color.g,color.b);
            this.#this.emit(this,'Color:change', {Color:this.#Color,Hex:this.#Hex});
        }
        /**
         * Sets the RGB color of the animation
         * @param {number} red 
         * @param {number} green 
         * @param {number} blue 
         */
        setRGBColor(red,green,blue){
            if(this.#this.CheckIfConnected()) return;
            this.#Color = [red,green,blue];
            this.#Hex = this.#this.rgbToHex(red,green,blue);
            this.#this.WebSocket.send( JSON.stringify( this.#this._animParam({"MorseCode":{"colorMorseCode":[this.#Color[0]*4, this.#Color[1]*4, this.#Color[2]*4]}} )));
        }
        /**
         * Encodes the plain message to morse code this is used internally by setText
         * @param {string} e plain message
         * @returns the encoded message
         */
        encodeMessage(e){
            var encoded = "";
            var words = e.split(/\s+/).filter(function(i){return i});
            for (var i = 0; i < words.length; i++) {
                var ch = words[i].split('').filter(function(i){return i});
                for (var j = 0; j < ch.length; j++) {
                    if (this.#alphabet.indexOf(ch[j]) > -1){
                    encoded += this.#morse[this.#alphabet.indexOf(ch[j])];
                    if (j + 1 != ch.length) encoded += "*";
                    }
                }
                if (i + 1 != words.length) encoded += "|";
            }
            return encoded;
        }
        /**
         * Decodes a morse code message to plain text
         * @param {string} e morse code message
         * @returns the decoded message
         */
        decodeMessage(e){
            var decoded = "";
            var encodedWords = e.split('|').filter(function(i){return i});
            for (var i = 0; i < encodedWords.length; i++)
            {
                var ch = encodedWords[i].split('*').filter(function(i){return i});
                for (var j = 0; j < ch.length; j++) if(this.#morse.indexOf(ch[j]) > -1)decoded += this.#alphabet[this.#morse.indexOf(ch[j])];
                if (i + 1 != encodedWords.length)  decoded += " ";
            }
            return decoded;
        }
        on(eventList, callback){  this.#this.setEvent(this,eventList, callback)  }
    }

    class TimeSchedule{
        #this;
        #Enabled;
        #Schedules;
        constructor(_this){
            this.#this = _this;
            this.#Enabled = false;
            this.#Schedules = [];
        }
        get Enabled(){
            return this.#Enabled;
        }
        set _Enabled(value){
            this.#Enabled = value;
            this.#this.emit(this,'Enabled:change', {Enabled:this.#Enabled});
        }
        get Schedules(){
            return this.#Schedules;
        }
        set _Schedules(value){
            this.#Schedules = value;
            this.#this.emit(this,'Schedules:change', {Schedules:this.#Schedules});
        }
        /**
         * Enables and disables the whole scheduler 
         * @param {boolean} value true or false
        */
        enable(value){
            if(this.#this.CheckIfConnected()) return;
            this.#Enabled = value;
            this.#this.WebSocket.send( JSON.stringify( {'Animations':{'Schedules':{'enableScheduler': this.#Enabled}}}  ));
        }
        /**
         * Removes a schedule by its timestamp
         * @param {string} timestamp the timestamp of that specific schedule
        */
        Remove(timestamp){
            if(this.#this.CheckIfConnected()) return;
            this.#this.WebSocket.send( JSON.stringify( {'Animations':{'Schedules':{'removeTimeSchedule':{'Timestamp': timestamp}}}}  ));
        }
        /**
         * Enable and disable a specific schedule
         * @param {string} timestamp the timestamp of that specific schedule
         * @param {boolean} value true or false 
        */
        EnableSchedule(timestamp,value){
            if(this.#this.CheckIfConnected()) return;
            this.#this.WebSocket.send( JSON.stringify( {'Animations':{'Schedules':{'editTimeSchedule':{'oldTimestamp':timestamp,"newData":{"enable":value}}}}}  ));
        }
        /**
         * Returns a valid schedule json
         * @param {string} label is the name of the schedule
         * @param {array} days represents the days when the schedule is activated in the format Su to Sa [true,true,true,true,true,true,true]
         * @param {number} timestamp represents the time when the schedule is activated
         * @param {string} animation is the name of the animation to be played
         * @param {object} parameters this are the parameters that this schedule will have when the animation plays
        */
        addSchedule(label,days,timestamp,animation,parameters){
            if(this.#this.CheckIfConnected()) return;
            this.#this.WebSocket.send( JSON.stringify( {"Animations":{"Schedules":{"newTimeSchedule":this.scheduleJson(label,days,timestamp,animation,parameters)}}} ));
        }
        /**
         * Edits a schedule by his timestamp
         * @param {string} oldtimestamp the current schedule timestamp to be edited
         * @param {string} label is the name of the schedule
         * @param {array} days represents the days when the schedule is activated in the format Su to Sa [true,true,true,true,true,true,true]
         * @param {number} timestamp represents the time when the schedule is activated
         * @param {string} animation is the name of the animation to be played
         * @param {object} parameters this are the parameters that this schedule will have when the animation plays
        */
        editSchedule(oldtimestamp,_label,_days,_timestamp,_animation,_parameters){
            if(this.#this.CheckIfConnected()) return;
            this.#this.WebSocket.send( JSON.stringify( {"Animations":{"Schedules":{"editTimeSchedule":{"oldTimestamp":oldtimestamp,"newData":this.scheduleJson(_label,_days,_timestamp,_animation,_parameters)}}}} ));
        }
        /**
         * Returns a valid schedule json
         * @param {string} label is the name of the schedule
         * @param {array} days represents the days when the schedule is activated in the format Su to Sa [true,true,true,true,true,true,true]
         * @param {number} timestamp represents the time when the schedule is activated
         * @param {string} animation is the name of the animation to be played
         * @param {object} parameters this are the parameters that this schedule will have when the animation plays
        */
        scheduleJson(label,days,timestamp,animation,parameters){
            return {
                "Label": label,
                "Days": days,
                "Timestamp": timestamp,
                "playingAnimation": animation,
                "parameters": parameters,
                "enable":true
            };
        }
        on(eventList, callback){
            this.#this.setEvent(this,eventList, callback);
        }
    }
    
    class ESPRGB{
        /**
         * Creates the ESPRGB client object
         * @param {string} ipaddress 
         * @param {number} port 
         */
        #RSSIInterval;
        #events;
        #deferredEvents;
        #activeEvents;
        #powerConnected;
        #playingAnimation;
        #RSSI;
        #Version;
        constructor(ipaddress,port=81){
            this.ipaddress = ipaddress;
            this.port = port;
            this.#powerConnected = false;
            this.#playingAnimation = "";
            this.#RSSI = 0;
            this.#Version = '';
            this.SolidColor = new SolidColor(this);
            this.ColorCycle = new ColorCycle(this);
            this.Breathing = new Breathing(this);
            this.MorseCode = new MorseCode(this);
            this.TimeSchedule = new TimeSchedule(this);
            this.#RSSIInterval;
            this.#events = {};
            this.#deferredEvents = {};
            this.#activeEvents = {};
        }
        /**
         * Get the powerConnected variable
        */
        get powerConnected(){
            return this.#powerConnected;
        }
        /**
         * Sets the local powerConnected variable and emits an event
         * @emits 'powerConnected:change'
        */
        set powerConnected(value){
            this.#powerConnected = value;
            this.emit(this,'powerConnected:change', {powerConnected:this.#powerConnected});
        }
        /**
         * Get current animation name
         */
        get playingAnimation(){
            return this.#playingAnimation;
        }
        /**
         * Sets the local current animation name and emits an event
         * @emits 'playingAnimation:change'
        */
        set playingAnimation(value){
            this.#playingAnimation = value;
            this.emit(this,'playingAnimation:change', {playingAnimation:this.#playingAnimation});
        }
        /**
         * Get current signal strength
         */
        get RSSI(){
            return this.#RSSI;
        }
        /**
         * Sets the local current signal strength and emits an event
         * @emits 'RSSI:change'
         */
        set RSSI(value){
            this.#RSSI = value;
            this.emit(this,'RSSI:change', {RSSI:this.#RSSI});
        }
        /**
         * Get current version of esprgb
         */
        get Version(){
            return this.#Version;
        }
        /**
         * Set current version of esprgb
         */
        set Version(value){
            this.#Version = value;
            this.emit(this,'Version:change', {Version:this.#Version});
        }
        /**
         * Check if there's a active websocket connection
         */
        CheckIfConnected(dontprintWarning = false){
            if(this.WebSocket == null || this.WebSocket === 'undefined' || this.WebSocket.readyState === WebSocket.CLOSED ) {
                if(!dontprintWarning) console.log('WebSocket connection is not available.');
                return true;
            }
            return false;
        }
        /**
         * Toggle between Connect and Disconnect state
         */
        ConnectToggle(){
            if(!this.CheckIfConnected(true)){
                this.Disconnect();
                return false;
            }
            this.Connect();
            return true;
        }
        /**
         * Creates the websocket connection with the specified ipaddress and port 
         */
        Connect(){
            this.WebSocket = new WebSocket(`ws://${this.ipaddress}:${this.port}/`);
            this.WebSocket.onopen = (e) =>{ 
                console.log("Connected");
                this.playingAnimation = "Connected"; 
                this.emit(this,'connected:change', {connected:true});
                this.Version = this.getVersion();
                this.#RSSIInterval = setInterval(()=> this.RSSI = this.getSignalStrength(), 10000);
            }
            this.WebSocket.onmessage = (event) => {
                var data = JSON.parse(event.data);
                if("Animations" in data) {
                var anim = data["Animations"];
                if("parameters" in anim){
                    var params = anim["parameters"];
                    if("SolidColor" in params) {
                    if("Color" in params["SolidColor"]) this.SolidColor._Color = [params.SolidColor.Color[0]/4,params.SolidColor.Color[1]/4,params.SolidColor.Color[2]/4];
                    if("Brightness" in params["SolidColor"]) this.SolidColor._Brightness = params.SolidColor.Brightness;
                    }
                    if("ColorCycle" in params){
                    if("ColorCycleSpeed" in params["ColorCycle"]) this.ColorCycle._Speed = params.ColorCycle.ColorCycleSpeed;  
                    }
                    if("Breathing" in params){
                    if("breathingSpeed" in params["Breathing"]) this.Breathing._Speed = params.Breathing.breathingSpeed;
                    if ("colorListBreathing" in params["Breathing"]) {
                        const colors = [];
                        params["Breathing"]["colorListBreathing"].forEach(item=>colors.push([item[0]/4, item[1]/4, item[2]/4]))
                        this.Breathing._colorList = colors;
                    }
                    if("staticColorBreathing" in params["Breathing"])
                        this.Breathing._StaticColor = [params.Breathing.staticColorBreathing[0]/4,params.Breathing.staticColorBreathing[1]/4,params.Breathing.staticColorBreathing[2]/4];
                    if("useColorList" in params["Breathing"]) this.Breathing._useColorList = params.Breathing.useColorList;
                    }
                    if("MorseCode" in params){
                    if("encodedMorseCode" in params["MorseCode"]) this.MorseCode._EncodedText = params.MorseCode.encodedMorseCode;
                    if("unitTimeMorseCode" in params["MorseCode"]) this.MorseCode._Speed = params.MorseCode.unitTimeMorseCode;
                    if("useBuzzer" in params["MorseCode"]) this.MorseCode._useBuzzer = params.MorseCode.useBuzzer;
                    if("colorMorseCode" in params["MorseCode"])
                        this.MorseCode._Color = [params.MorseCode.colorMorseCode[0]/4,params.MorseCode.colorMorseCode[1]/4,params.MorseCode.colorMorseCode[2]/4]
                    }
                }  
                if ("Schedules" in anim)
                {
                    if ("enableScheduler" in anim["Schedules"]) this.TimeSchedule._Enabled = anim["Schedules"].enableScheduler;
                    if ("timeSchedule" in anim["Schedules"]) this.TimeSchedule._Schedules = anim["Schedules"].timeSchedule;
                }
                if("PowerState" in anim) this.SolidColor._powerButton =anim.PowerState;
                if("playingAnimation" in anim) this.playingAnimation = anim.playingAnimation;
                if("powerConnected" in anim) this.powerConnected = anim.powerConnected;
                }
            }
            this.WebSocket.onclose = (e)=>{
                console.log("Disconnected");
                this.playingAnimation = "Disconnected";
                this.emit(this,'connected:change', {connected:false});
            }
            this.WebSocket.onerror = (e)=>{
                console.log("Disconnected");
                this.playingAnimation = "Disconnected";
                this.emit(this,'connected:change', {connected:false});
            }
        }
        /**
         * Disconnects from the websocket connection and clears all active intervals
         */
        Disconnect(){
            if(this.CheckIfConnected()) return;
            this.WebSocket.close();
            clearInterval(this.#RSSIInterval);
        }
        /**
         * Creates a animation JSON with the given parameters
         */
        _animParam(param){
            return {"Animations":{"parameters":param}};
        }
        /**
         * Merges two json objects together
         */
        #merge(obj1,obj2){
            for (var p in obj2) {
                try {
                if ( obj2[p].constructor==Object ) obj1[p] = this.#merge(obj1[p], obj2[p]);
                else obj1[p] = obj2[p];
                } catch(e) {
                obj1[p] = obj2[p];
                }
            }
            return obj1;
        }
        /**
         * Converts a valid hex color to rgb values
         */
        hexToColor(hex){
            if(hex[0] == '#') hex = hex.substring(1);
            var bigint = parseInt(hex, 16);
            return { r: (bigint >> 16) & 255, g: (bigint >> 8) & 255, b: bigint & 255 };
        }
        /**
         * Function to play the given animation
         * @param {string} animation  @example 'Solid Color' , 'Breathing', 'Morse Code' , 'Color Cycle'
         * @param {boolean} status default=true if set false the playing animation will be set to the default 'Solid Color'
         */
        playAnimation(animation,status=true){
            if (status==false){
                this.setAnimation('Solid Color');
                return;
            }
            this.setAnimation(animation);
        }
        /**
         * Used to set the animation with a set parameter, if you only need to activate a animation use playAnimation instead 
         * @param {string} animation @example 'Solid Color' , 'Breathing', 'Morse Code' , 'Color Cycle'
         * @param {object} parameters sets the animation with the given parameters for the animation
         */
        setAnimation(animation,parameters=null){
            if(this.CheckIfConnected()) return;
            var anim ={"Animations":{"playingAnimation" : animation }};
            if(parameters!=null) anim = this.#merge(anim,parameters);
            this.WebSocket.send(JSON.stringify(anim));
        }
        /**
         * Toggles the power connected value
         */
        powerConnectedToggle(){
            if(this.CheckIfConnected()) return;
            this.powerConnected = !this.powerConnected;
            this.WebSocket.send(JSON.stringify({"Animations":{'powerConnected': this.powerConnected}}));
        }
        /**
         * Gets the wifi signal strength
         */
        getSignalStrength(){
            const xhr =new XMLHttpRequest();
            xhr.open("GET",`http://${this.ipaddress}/getSignalStrenght`,false);
            xhr.send();
            if (xhr.status === 200) {
                const data=JSON.parse(xhr.responseText);
                if ("RSSI" in data) return data["RSSI"];
            }
            return "-70";
        }
        /**
         * Restarts the controler
         */
        restart(){
            if(confirm('Are you sure you want to restart this device?')){
                this.Disconnect(); 
                const xhr=new XMLHttpRequest();
                xhr.open('GET',`http://${this.ipaddress}/restartESP`,true);
                xhr.send();
            }
        }
        /**
         * Formats the controler
         */
        format(){
            if(confirm('Are you sure you want to format this device?')){
                this.Disconnect(); 
                const xhr=new XMLHttpRequest();
                xhr.open('GET',`http://${this.ipaddress}/formatDevice`,true);
                xhr.send();
            }
        }
        /**
         * Removes only the user data not the wifi config from controler
         */
        removeUserData(){
            if(confirm('Are you sure you want to remove all the user data from this device?')){
                this.Disconnect(); 
                const xhr=new XMLHttpRequest();
                xhr.open('GET',`http://${this.ipaddress}/removeUserData`,true);
                xhr.send();
            }
        }
        /**
         * Gets the version of the firmware from the controler
         */
        getVersion() {
            const xhr =new XMLHttpRequest();
            xhr.open("GET",`http://${this.ipaddress}/getVersion`,false);
            xhr.send();
            if (xhr.status === 200) {
                const data=JSON.parse(xhr.responseText);
                if ("ESPRGB_VERSION" in data) return data["ESPRGB_VERSION"];
            }
            return "0.0.0.0";
        }
        emit(sender,eventType, ...args) {
            eventType = `${sender.__proto__.constructor.name}_${eventType}`;
            const activeEvents = this.#activeEvents;
            const isEventActive = activeEvents.hasOwnProperty(eventType) ? activeEvents[eventType] : false;
            if (!isEventActive) {
            activeEvents[eventType] = true;
            const callbackList = this.#events[eventType] || [];
            callbackList.forEach(fn => fn.apply(this, args));
            activeEvents[eventType] = false;
            }
        }
        setEvent(sender,eventList, callback) {
            eventList = `${sender.__proto__.constructor.name}_${eventList}`;
            const events = this.#events;
            (!Array.isArray(eventList) ? [eventList] : eventList).forEach(eventType => {
            (events[eventType] || (events[eventType] = [])) .push(callback);
            if (this.#deferredEvents[eventType]) {
                this.#deferredEvents[eventType].forEach(args => {
                    callback.apply(null, args); 
                });
                this.#deferredEvents[eventType] = [];
            }
            });
        }
        /**
         * Checks if the hex is a valid color 
         */
        isHexColor(hex){
            return typeof hex === 'string' && 
            hex.charAt(0) === '#' &&
            hex.length === 7 &&
            !isNaN(Number('0x' + hex.substring(1)));
        }
        /**
         * converts a rgb color to hex color
         * @param {number} r red value from 0 to 255
         * @param {number} g green value from 0 to 255
         * @param {number} b green value from 0 to 255
         */
        rgbToHex(r, g, b) {
            return "#" + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1);
        }
        on(eventList, callback){
            this.setEvent(this,eventList, callback);
        }
    }
    return ESPRGB;
}));

