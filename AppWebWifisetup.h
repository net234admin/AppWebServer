/*************************************************
 *************************************************
    AppWebServer.h   part of of lib AppWebServer - Arduino lib for easy wifi interface with http and css
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

  History
  cleaner version of WebServer (C) V1.2 6/6/2020  NET234 P.HENRY net23@frdev.com

   V1.0    Extracted from Betaporte 
   V1.0.1  Add interactive js
   V1.0.2  Stand alone captive portal C) 07/2020 P.HENRY NET234
   V1.0.3  1/12/2020  rewriting


problems
   dhcp dont work on AP with other ip than 192.164.4.1   (not a real problem)
   unable to save in wifi flash config another AP ip than 192.164.4.1  (not a real problem)
   mdns doesnt respond on AP   (fixed 06/12/2020)


**********/



// Scan les reseaux wifi le fill up des page comportant un arg appweb=show_wifi
//#define MAXNETWORK 30
const uint8_t MAXNETWORK=30;

int network[MAXNETWORK + 1];
int networkSize = 0;
int currentLine = 0;

// call back pour sort network
int compareNetwork (const void * a, const void * b) {
  return ( WiFi.RSSI(*(int*)b) - WiFi.RSSI(*(int*)a) );  // this is why I dont like C
}

// Fill up network array with current network
void scanNetwork() {
  networkSize = WiFi.scanNetworks();
  if (networkSize > MAXNETWORK) networkSize = MAXNETWORK;
  //Serial.print(F("scanNetworks done"));
  if (networkSize == 0) return;

  //sort networks
  for (int i = 0; i < networkSize; i++)  network[i] = i;
  qsort(network, networkSize, sizeof(network[0]), compareNetwork);
  // remove duplicates SSID
  for (int i = 0; i < networkSize - 1; i++) {
    for (int j = i + 1; j < networkSize; j++) {
      if ( WiFi.SSID(network[i]) == WiFi.SSID(network[j]) )  {
        networkSize--;
        for (int k = j; k < networkSize; k++) network[k] = network[k + 1];
        j--;
      }
    }
  }
}

#define RSSIdbToPercent(x) ( 2 * (WiFi.RSSI(x) + 100) )

//gestion du repeat line specifique "APPWEB_SCANSETWORK"
bool repeatLineScanNetwork(const int num) {
  if ( !Server.arg(F("appweb")).equals(F("show_wifi")) ) return (false);
  currentLine = num;
  if (currentLine == 0) scanNetwork();
  return (num < networkSize && RSSIdbToPercent(network[num]) > 25 );
}

// gestion du submit appweb_wifisetup
void checkSubmitappwebWifisetup() {

  // TODO : add a check for pagename
  String aString = Server.arg(F("SSID"));
  aString.trim();
  if (aString.length() >= 2 && aString.length() <= 32) {

    String wifiSSD = aString;
    String wifiPASS = Server.arg(F("PASS"));
    aString = Server.arg(F("HOSTNAME"));
    aString.trim();
    Serial.print(F("Got Hostname="));
    Serial.print(aString);
    if (!aString.equals(TWConfig.deviceName) ) {
      AppWebPtr->setDeviceName(aString);
    }
    delay(100);
    // TODO : check wifi before validate
    //    AppWebPtr->setWiFiMode(twm_WIFI_STA, wifiSSD.c_str(), wifiPASS.c_str());
    Serial.print(F("request connectWiFi="));
    Serial.println(wifiSSD);
    TWS::redirectUri = F("wait.html");
  }
}
