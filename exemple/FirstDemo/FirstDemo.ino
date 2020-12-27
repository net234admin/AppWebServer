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






void setup() {

  // Setup the led 
  pinMode( LED_LIFE, OUTPUT );
  digitalWrite( LED_LIFE, LED_ON );

  // init Serial
  Serial.begin(115200);
  Serial.println(F(APP_VERSION));

  // setup the WebServer 
  myWebServer.setCallBack_OnTranslateKey(&on_TranslateKey);
  myWebServer.begin();
}






void loop() {

  myWebServer.handleEvent();     // handle http services 

}// loop


////// ==== callback to display data on page /////////////

//on_TranslateKey is call on each tag "[#key#]" the tag will be replaced by the value you put in 'key' parameter
//Pages request 2 key
void on_TranslateKey(String & key) {

  if ( key.equals("APP_VERSION") ) {

  //  APP_VERSION appname wich is displayed on almost every page
    key = APP_VERSION;

  } else if ( key.equals("ledState") ) {

  // ledState to say if led is on or off
    if ( digitalRead(LED_LIFE) == LED_ON ) {
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
