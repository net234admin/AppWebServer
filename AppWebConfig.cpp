/*************************************************
 *************************************************
    AppWebConfig.cpp  is part of of lib AppWebServer - Arduino lib for easy wifi interface with http and css
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
   V1.0.2  Stand alone captive portal
    Gestion du fichier de configuration
*/

#include "AppWebConfig.h"

//String in FLASH (use FPSTR to get them)
 static const char F_captiveWebFolder[] PROGMEM = "captiveWebFolder";

bool FileConfig::read() {
  changed=false;
  initInfo="";
  deviceName="";              //invalid value are replacet by APPWEB_XXYY
  defaultWebFolder = ""; //F("/web");      //default value
  captiveWebFolder = ""; //F("/web/wifisetup");      //default value
  bootForceAP = 0;
  D_println(F("TW get config"));
  File aFile = TWFS.open(F("/AppWebServer.conf"), "r");  
  if (!aFile) {
    D_println(F("TW: AppWebServer.conf not found"));
    aFile = TWFS.open(F("/AppWebServer.ini"), "r");
    if (!aFile) {
       D1_println(F("TW: AppWebServer.ini not found !!!"));
      return (false);
    }
  }
  aFile.setTimeout(0);
  String  aString;
  while ( aFile.available() )  {
    aString = aFile.readStringUntil('\n');
    D_print(F("TW: line ='")); D_print(aString); D_println("'");
   
    if ( aString.startsWith(F("#")) ) {
      D_print(F("TW: '#")); D_print(aString); D_println("'");
      
   } else if ( aString.startsWith(FPSTR(F_captiveWebFolder)) ) {
       captiveWebFolder = getParam(aString);
      D_print(F("TW: captiveWebFolder ='")); D_print(captiveWebFolder); D_println("'");
      
  } else if ( aString.startsWith(F("initInfo")) ) {
      initInfo = getParam(aString);
      D_print(F("TW: initInfo ='")); D_print(initInfo); D_println("'");
    } else if ( aString.startsWith(F("deviceName")) ) {
      deviceName = getParam(aString);
      D_print(F("TW: deviceName ='")); D_print(deviceName); D_println("'");
    } else if ( aString.startsWith(F("defaultWebFolder")) ) {
      defaultWebFolder = getParam(aString);
      D_print(F("TW: defaultWebFolder ='")); D_print(defaultWebFolder); D_println("'");
    } else if ( aString.startsWith(F("bootForceAP")) ) {
      bootForceAP = getParam(aString).toInt(); 
      D_print(F("TW: bootForceAP ='")); D_print(bootForceAP); D_println("'");
    }
  }
  aFile.close();
  return (true);


}

bool FileConfig::save() {
  if (!changed) return (true);
  changed = false;
  D_println(F("TW Config.init write"));
  File aFile = TWFS.open(F("/AppWebServer.conf"), "w");
  if (!aFile) {
    D_println(F("TW: error write"));
    return (false);
  }
  if (deviceName.length() > 0) {
    aFile.print("deviceName=");
    aFile.println(deviceName);
    D_print("deviceName=");
    D_println(deviceName);
 }
 
  if (captiveWebFolder.length() > 0) {
    aFile.print(FPSTR(F_captiveWebFolder));
    aFile.print('=');
    aFile.println(captiveWebFolder);
  }

  if (defaultWebFolder.length() > 0) {
    aFile.print("defaultWebFolder=");
    aFile.println(defaultWebFolder);
  }
 if (initInfo.length() > 0) {
    aFile.print("initInfo=");
    aFile.println(initInfo);
  }
  if (bootForceAP>0) {
    aFile.print(F("bootForceAP="));
    aFile.println(bootForceAP);
  }
  aFile.close();


  D_println(F("TW Config.init re read "));
  aFile = TWFS.open(F("/AppWebServer.conf"), "r");
  if (!aFile) {
    D_println(F("TW: no config .conf"));
    return (false);
  }
  D_println(F("TW: AppWebServer.conf present :"));


  String  aString;
  while ( aFile.available() )  {
    aString = aFile.readStringUntil('\n');
    D_print(F("TW: line ='")); D_print(aString); D_println("'");
  }
   D_println(F("TW: eof AppWebServer.conf"));
  aFile.close();
  return (true);
}

// efface le fichier .conf
bool FileConfig::erase() {
  const String aFileName = F("/AppWebServer.conf");
  D_println(F("TW: Erase config !!"));
  if ( !TWFS.exists(aFileName) )  return(true);
  return TWFS.remove(aFileName);
}


// recupere la chaine apres le '='  et retire l'enventuel commentaire #
String FileConfig::getParam(const String &astring) {
  int aStart = astring.indexOf("=");
  if (aStart < 0) return ("");
  int aStop = astring.indexOf("#", aStart);
  String result;
  if (aStop < 0) {
    result = astring.substring(aStart + 1);
  } else {
    result = astring.substring(aStart + 1, aStop);
  }
  result.trim();
  return (result);
}
