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
    along with AppWebServer.  If not, see <https://www.gnu.org/licenses/glp.txt>.

    Gestion du fichier de configuration
*/

#include "AppWebConfig.h"


bool FileConfig::read() {
  deviceName="";              //invalid value are replacet by APPWEB_XXYY
  webFolder = F("/web");      //default value
  D_println(F("TW Config.read"));
  File aFile = TWFS.open(F("/AppWebServer.conf"), "r");

  if (aFile) {
    D_println(F("TW: appweb.conf present"));
  } else {
    aFile = TWFS.open(F("/AppWebServer.ini"), "r");
    if (aFile) {
      D_println(F("TW: AppWebServer.ini present"));
    }  else {
      D_println(F("TW: no config"));
      return (false);
    }
  }
  aFile.setTimeout(0);
  String  aString;
  while ( aFile.available() )  {
    aString = aFile.readStringUntil('\n');
    //    D_print(F("TW: line ='")); D_print(aString); D_println("'");
    // byte N = aString[0];
    // Serial.println(N);
    if ( aString.startsWith(F("devicename")) ) {
      deviceName = getParam(aString);
      D_print(F("TW: devicename ='")); D_print(deviceName); D_println("'");
    }
    if ( aString.startsWith(F("webfolder")) ) {
      webFolder = getParam(aString);
      D_print(F("TW: webfolder ='")); D_print(webFolder); D_println("'");
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
    aFile.print("devicename=");
    aFile.println(deviceName);
  }
  if (webFolder.length() > 0) {
    aFile.print("webfolder=");
    aFile.println(webFolder);
  }

  aFile.close();


  D_println(F("TW Config.init re read read"));
  aFile = TWFS.open(F("/AppWebServer.conf"), "r");
  if (!aFile) {
    D_println(F("TW: no config .conf"));
    return (false);
  }
  D_println(F("TW: AppWebServer.conf present"));


  String  aString;
  while ( aFile.available() )  {
    aString = aFile.readStringUntil('\n');
    D_print(F("TW: line ='")); D_print(aString); D_println("'");
  }
  aFile.close();
  return (true);


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
