/*************************************************
 *************************************************
    Sketch FirstDemo.ino   exemple of lib AppWebServer - Arduino lib for easy wifi interface with http and css
    Copyright 2020 Pierre HENRY net23@frdev.com All - right reserved.


  This file is part of AppWebServer.

    AppWebServer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AppWebServer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AppWebServer.  If not, see <https://www.gnu.org/licenses/gpl.txt>.




**********************************************************************************/



#include <Arduino.h>


#define LED_LIFE      LED_BUILTIN
#define APP_VERSION   "FirstDemo AppWebServer"
#define LED_ON        LOW

//Objet serveur WEB
#include  "AppWebServer.h"
AppWebServer    myWebServer;



bool ledState = LED_ON;


void setup() {

  // Setup the led 
  pinMode( LED_LIFE, OUTPUT );
  digitalWrite( LED_LIFE, ledState );

  // init Serial
  Serial.begin(115200);
  Serial.println(F(APP_VERSION));

  // setup the WebServer 
  myWebServer.setCallBack_OnTranslateKey(&on_TranslateKey);
  myWebServer.setCallBack_OnStartRequest(&on_HttpRequest);
  myWebServer.begin("FIRSTDEMO");   // setup the device name only on the very first boot (or after a new FS download)
}






void loop() {

  myWebServer.handleEvent();     // handle http services 

}// loop


////// === call back to handle request (grab the "Swith the led button") ============================


void on_HttpRequest(const String &filename, const  String &submitValue) {
  Serial.println(submitValue);
  if  ( submitValue ==  "switchLed" ) {
    ledState = ! ledState;
    Serial.print("Led state = ");
       if ( ledState == LED_ON ) {
      Serial.println("LED ON");
    } else {
      Serial.println("LED OFF");
    }

    digitalWrite(LED_LIFE, ledState); 
  }
}



////// ==== callback to display data on page /////////////

//on_TranslateKey is call on each tag "[#key#]" the tag will be replaced by the value you put in 'key' parameter
//webdemo request 2 key
void on_TranslateKey(String & key) {

  if ( key.equals("APP_VERSION") ) {

  //  APP_VERSION appname wich is displayed on almost every page
    key = APP_VERSION;

  } else if ( key.equals("ledState") ) {

  // ledState to say if led is on or off
    if ( ledState == LED_ON ) {
      key = "LED ON";
    } else {
      key = "LED OFF";
    }
  } else {
    // to track typo on key
    Serial.print("Missing key '");
    Serial.print(key);
    Serial.println("'");
    key = "?" + key + "?"; // to make a visual on the page
  }
}
