/*************************************************
 *************************************************
    AppWebServer.cpp  is part of of lib AppWebServer - Arduino lib for easy wifi interface with http and css
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
*/

#include "AppWebServer.h"
//#include <ESP8266WiFi.h>
// pointeur vers l'instance utilisateur
AppWeb*    AppWebPtr = NULL;

#include "AppWebConfig.h"  //include LittleFS
FileConfig  TWConfig;


#include <ESP8266WebServer.h>   // web server to answer WEB request and EEDOMUS BOX request
#include <ESP8266mDNS.h>      // not used here we stay with IP on this side (Wifimanageg need id on his side)
#include <DNSServer.h>        // only used by captive to redirect web request to main page
// ====== DO NOT REMOVE ===========================


// Server Instance
//#include "user_interface.h"
ESP8266WebServer   Server(SERVER_PORT);    // Serveur HTTP
DNSServer          dnsServer;

namespace TWS {
// Out of instance variables
String  redirectUri;        // uri to redirect after an onSubmit (used by AppWebHttp)
String  localIp;

// Out of instance function
#include "AppWebHTTP.h"    //out of instance functions
#include "AppWebCaptive.h"    //out of instance functions

}

using namespace TWS;


// Objet AppWeb
//// constructeur
AppWeb::AppWeb() {
  if (AppWebPtr != NULL) {
    if (debugLevel > 0) Serial.print(F("tws - Error: Only one instance for MiniServeurWeb"));
    while (true) delay(100);
  }
  AppWebPtr = this;
}
// Destructor
AppWeb::~AppWeb() {
  if (AppWebPtr == NULL) return;
  this->end();
  AppWebPtr = NULL;
}


void AppWeb::end() {
  //  WiFi.mode(WIFI_OFF);
  //  //  softAP = false;
  //  delay(10);
  //  WiFi.forceSleepBegin();
  //  delay(10);
  Server.close();
  ////  delay(10);
}




void AppWeb::begin() {
  // FS
  if (!TWFS.begin()) {
    D1_println(F("TW: FS en erreur  !!!!!"));
  } else {
    D_println(F("TW: FS Ok"));
  }
  // Recuperation du fichier de config
  TWConfig.read();

  Serial.setDebugOutput(true);



  // grab WiFi actual status
  D_println(F("tws: Read wifi current status "));
  Serial.print(F("tws: WIFI Mode "));
  Serial.println(WiFi.getMode());
  Serial.print(F("tws: SoftAP SSID "));
  Serial.println(WiFi.softAPSSID());
  Serial.print(F("tws: SoftAP IP "));
  Serial.println(WiFi.softAPIP());
  WiFi.persistent(false);
  // controle de la configuration du WiFi et de la configuration demandée
  //reconfig eventuelle de l'ip AP
  //if ( (WiFi.getMode() & WIFI_AP) && (WiFi.softAPIP() != IPAddress(10, 10, 10, 10)) ) {
  if ( WiFi.softAPIP() != IPAddress(10, 10, 10, 10) ) {
    WiFiMode_t mode = WiFi.getMode();
    D_print(F("WS: reconfig APIP 10.10.10.10"));
    IPAddress local_IP(10, 10, 10, 10);
    IPAddress mask(255, 255, 255, 0);
    bool result = WiFi.softAPConfig(local_IP, local_IP, mask);
    D_print(F(" ==> ")); D_println(result);
    D_print(F("SW: SoftAP IP = "));
    D_println(WiFi.softAPIP());
    WiFi.mode(mode);
  }
  //  if ( TWConfig.bootForceAP > 0 && !(WiFi.getMode() & WIFI_AP) ) {
  //    D_println(F("TWS: Force mode AP !!!"));
  //    WiFi.enableAP(true);
  //    //timerLimitAP=TWConfig.bootForceAP;
  //  }
  WiFi.persistent(true);

  Serial.print(F("tws: Station SSID "));
  Serial.println(WiFi.SSID());
  Serial.print(F("tws: Station IP "));
  Serial.println(WiFi.localIP());
  Serial.print(F("tws: WIFI Mode "));
  Serial.println(WiFi.getMode());
  //delay(1000); // Without delay I've seen the IP address blank
  _deviceName = WiFi.softAPSSID();              //device name from WiFi
  //if no SSID name I suppose it is a first boot so
  //   set hostname and softap ssid to default
  //   TODO : better detect empty config
  if ( TWConfig.deviceName != _deviceName) {
    D_print(F("SW: need to init WiFi same as config   !!!!! "));
    D_print(TWConfig.deviceName);
    D_print(F("!="));
    D_println(_deviceName);
    setDeviceName(TWConfig.deviceName);  //check devicename validity
    WiFi.softAP(_deviceName);
    //WiFi.persistent(true);
    if (TWConfig.deviceName != WiFi.softAPSSID()) {
      D_print(F("SW: need to need to rewrite config   !!!!! "));
      D_print(TWConfig.deviceName);
      D_print(F("!="));
      D_println(WiFi.softAPSSID());
      TWConfig.deviceName = WiFi.softAPSSID();  //put back devicename in config if needed
      TWConfig.changed = true;
      TWConfig.save();
    }
  }
  D_print(F("TW: softap="));
  D_println(WiFi.softAPSSID());
  // mise en place des call back web
  Server.onNotFound(HTTP_HandleRequests);
  D_println(F("TW: Serveur.begin"));
  Server.begin();
  D_print(F("TW: AP WIFI Mode "));
  D_println(WiFi.getMode());
  IPAddress myIP = WiFi.softAPIP();
  D_print(F("TW: AP IP address: "));
  D_println(myIP);

  if (WiFi.getMode() != WIFI_OFF ) {
    bool result = MDNS.begin(_deviceName);
    Serial.print(F("TWS: MS DNS ON : "));
    Serial.print(_deviceName);
    Serial.print(F(" r="));
    Serial.println(result);
  }

  return ;
}

// set device name
// Check for valid name otherwhise EFAULT_DEVICENAME "*" is used
// if device name terminate with *  we add some mac adresse number
//   usefull if you setup different device at the same place
// device name is used as APname and as DNSname

void AppWeb::setDeviceName(const String devicename) {
  _deviceName = devicename;
  // Check a valid hostname
  // configation du nom du reseau AP : LITTLEWEB_XXYY  avec les 2 dernier chifre hexa de la mac adresse
  if (_deviceName.length() > 30) _deviceName.remove(30);   // chop at 30char
  _deviceName.trim();
  if (_deviceName.length() < 4 ) _deviceName = F(DEFAULT_DEVICENAME "*");
  if (_deviceName.endsWith(F("*"))) {
    _deviceName.remove(_deviceName.length() - 1);
    _deviceName += WiFi.macAddress().substring(12, 14);
    _deviceName += WiFi.macAddress().substring(15, 17);
  }
  _deviceName.replace(' ', '_');
}



void AppWeb::handleEvent() {
  // Check if mode changed
  WiFiMode_t wifimode = WiFi.getMode();
  if ( _WiFiMode != wifimode) {
    _WiFiMode = wifimode;
    // grab WiFi actual mode
    D_print(F("SW: -- Wifi mode change to "));
    D_println(_WiFiMode);
    D_println(F("SW: Read wifi current mode and config "));
    D_print(F("SW: SoftAP SSID "));
    D_println(WiFi.softAPSSID());
    D_print(F("SW: SoftAP IP "));
    D_println(WiFi.softAPIP());

    D_print(F("SW: Station SSID "));
    D_println(WiFi.SSID());
    D_print(F("SW: Station IP "));
    D_println(WiFi.localIP());
    MDNS.end();  // will be restarted if needed
    if ( (_WiFiMode & WIFI_AP) && (WiFi.softAPIP() != IPAddress(10, 10, 10, 10)) ) {
      //    if (_WiFiMode == WIFI_AP) {
      //      WiFi.softAP(_deviceName);
      //    }
      //        delay(100);
      D_println(F("WS: reconfig APIP 10.10.10.10   !!!!!!!!!!"));
      IPAddress local_IP(10, 10, 10, 10);
      IPAddress mask(255, 255, 255, 0);
      bool result = WiFi.softAPConfig(local_IP, local_IP, mask);
      D_print(F("SW: softapconfig = ")); D_println(result);
      D_print(F("SW: SoftAP IP = "));
      D_println(WiFi.softAPIP());

    }

    if (_WiFiMode != WIFI_OFF) {
      //delay(100);
      bool result = MDNS.begin(_deviceName);
      //MDNS.addService("http", "tcp", 80);
      Serial.print(F("TWS: MS DNS ON : "));
      Serial.print(_deviceName);
      Serial.print(F(" r="));
      Serial.println(result);
    }
    D_println(F("SW: -- end Wifi mode change"));
  }




  static int oldStatus = -1;
  int status = WiFi.status();
  //    0 : WL_IDLE_STATUS when Wi-Fi is in process of changing between statuses
  //    1 : WL_NO_SSID_AVAILin case configured SSID cannot be reached
  //    3 : WL_CONNECTED after successful connection is established
  //    4 : WL_CONNECT_FAILED if password is incorrect
  //    6 : WL_DISCONNECTED if module is not configured in station mode
  if (status != oldStatus) {
    Serial.print("Wifi Status : ");
    Serial.println(status);
    oldStatus = status;
    if (status == WL_CONNECTED) {
      TWS::localIp = WiFi.localIP().toString();
    }


  }


  MDNS.update();
  Server.handleClient();


}


void AppWeb::setCallBack_OnTranslateKey(void (*ontranslatekey)(String & key))  {
  onTranslateKeyPtr =  ontranslatekey;
}


void AppWeb::setCallBack_OnRefreshItem(bool (*onrefreshitem)(const String & keyname, String & key)) {
  onRefreshItemPtr = onrefreshitem;
}
//
//
//void AppWeb::setCallBack_OnRepeatLine(bool (*onrepeatline)(const int num)) {     // call back pour gerer les Repeat
//  onRepeatLinePtr = onrepeatline;
//}

bool AppWeb::razConfig() {                             // efface la config enregistree
  return (TWConfig.erase());
}
