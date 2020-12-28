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


#define APP_VERSION   "Demo2 dynamic data"

// 2 or 16 on ESP8266
#define LED_LIFE      LED_BUILTIN
#define LED_ON        LOW

// 0 on ESP8266
#define BP_FLASH      0
#define BP_DOWN       LOW

//Objet serveur WEB
#include  "AppWebServer.h"
AppWebServer    myWebServer;


bool ledStatus = LED_ON;    // led is on
bool bpStatus = !BP_DOWN;   // bp is supose to be up (not DOWN)
int  lastClickTime10s = 0;    // last click on board FLASH button in 10' second


void setup() {

  // Setup the led
  pinMode( LED_LIFE, OUTPUT );
  digitalWrite( LED_LIFE, ledStatus );

  // Setup BP_FLASH
  pinMode( BP_FLASH, INPUT);

  // init Serial
  Serial.begin(115200);
  Serial.println(F(APP_VERSION));

  // setup the WebServer
  myWebServer.setCallBack_OnTranslateKey(&on_TranslateKey);
  myWebServer.setCallBack_OnStartRequest(&on_HttpRequest);
  myWebServer.begin(F(APP_VERSION));   // setup the device name only on the very first boot (or after a new FS download)

  //Check if WEB in flash is the good on  (dont forget to UPLOAD the web on the flash with LittleFS)
  if (myWebServer.getWebName() != APP_VERSION) {
    Serial.print("Invalid WEB pages '");
    Serial.print(myWebServer.getWebName());
    Serial.println("'");
    delay(3000);
    ESP.reset();
  }

}





uint32_t lastTime = millis();

void loop() {

  myWebServer.handleEvent();     // handle http services

  // detect 10' of seconds to avoid rebonce of BP
  if (millis() - lastTime >= 100) {
    lastTime += 100;
    lastClickTime10s++;
    if ( digitalRead(BP_FLASH) != bpStatus) {
      // FLASH button changed
      bpStatus = !bpStatus;
      Serial.print("BP is ");
      if (bpStatus == BP_DOWN) {
        Serial.println("DOWN");
        // change the led status
        switchLed();
        // raz the lastClickTime
        lastClickTime10s = 0;
      } else {
        Serial.println("UP");
      }

    }
  }

}// loop


////// === function to deal with led ================================================================


void switchLed() {
  ledStatus = ! ledStatus;
  digitalWrite(LED_LIFE, ledStatus);

  Serial.print("Led is ");
  if ( ledStatus == LED_ON ) {
    Serial.println("On");
  } else {
    Serial.println("Off");
  }

}


////// === call back to handle request (grab the "Swith the led button") ============================


void on_HttpRequest(const String &filename, const  String &submitValue) {
  Serial.println(submitValue);
  if  ( submitValue ==  "switchLed" ) {
    switchLed();
  }
}



////// ==== callback to display data on page /////////////

//on_TranslateKey is call on each tag "[#key#]" the tag will be replaced by the value you put in 'key' parameter
//webdemo request 2 key
void on_TranslateKey(String & key) {

  if ( key.equals("APP_VERSION") ) {

    //  APP_VERSION appname wich is displayed on almost every page
    key = APP_VERSION;

  } else if ( key.equals("ledStatus") ) {

    // ledState to say if led is on or off
    if ( ledStatus == LED_ON ) {
      key = "LED ON";
    } else {
      key = "LED OFF";
    }

  } else if ( key.equals("lastClickTime") ) {

    // lastClickTime in second
    key = lastClickTime10s / 10;

  } else {

    // to track typo on key
    Serial.print("Missing key '");
    Serial.print(key);
    Serial.println("'");
    key = "?" + key + "?"; // to make a visual on the page
  }
}
