/*************************************************
 *************************************************
    AppWebHTTP.cpp  is part of of lib AppWebServer - Arduino lib for easy wifi interface with http and css
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

    Gestion du http et des callback http
*/

#include "AppWebWifisetup.h"
//pointer du traducteur de Key
void (*onTranslateKeyPtr)(String &key) = NULL;

//todo start all internal key with "_" to skip them faster
void translateKey(String &key) {
  if ( key.equals(F("_CHIP_ID")) ) {
    key = ESP.getChipId();
  } else if ( key.equals(F("_FLASH_CHIP_ID")) ) {
    key = ESP.getFlashChipId();
  } else if ( key.equals(F("_IDE_FLASH_SIZE")) ) {
    key = ESP.getFlashChipSize();
  } else if ( key.equals(F("_REAL_FLASH_SIZE")) ) {
    key = ESP.getFlashChipRealSize();
  } else if ( key.equals(F("_SOFTAP_IP")) ) {
    key = WiFi.softAPIP().toString();
  } else if ( key.equals(F("_SOFTAP_MAC")) ) {
    key = WiFi.softAPmacAddress();
  } else if ( key.equals(F("_STATION_IP")) ) {
    key = TWS::localIp;
  } else if ( key.equals(F("_HOSTNAME")) ) {
    key =  TWConfig.deviceName;
  } else if ( key.equals(F("_STATION_MAC")) ) {
    key = WiFi.macAddress();

    //specific wifisetuo
  } else if ( key.equals(F("_SSID_NAME")) ) {
    key = WiFi.SSID(network[currentLine]);
  } else if ( key.equals(F("_SSID_LEVEL")) ) {
    int level = RSSIdbToPercent(network[currentLine]);
    if (level > 100) level = 100;
    key = level;
  } else if ( key.equals(F("_SSID_LOCK")) ) {
    key = "&nbsp;";
    if (WiFi.encryptionType(network[currentLine]) != ENC_TYPE_NONE) key = "&#128274;";  //htmlcode icone cadena
  } else if (onTranslateKeyPtr) {
    (*onTranslateKeyPtr)(key);
  }
}



// onRequest callback
//pointer du gestionaire de request
void (*onRequestPtr)(const String &filename, const String &submitvalue) = NULL;

void onRequest(const String &filename, const String &submitvalue) {

  // track "form appweb_wifisetup" qui retoune SSID PASS et HOSTNAME eq deviceName
  if (&submitvalue && submitvalue.equals(F("appweb_wifisetup")) ) {
    checkSubmitappwebWifisetup();
    return;
  }
  if (onRequestPtr) (*onRequestPtr)(filename, submitvalue);
}


//pointeur du gestionanire de refresh
bool  (*onRefreshItemPtr)(const String &keyname, String &key) = NULL;

bool onRefreshItem(const String &keyname, String &key) {
  if (onRefreshItemPtr) return (*onRefreshItemPtr)(keyname, key);
  return (false);
}

//pointeur du gestionaire de repeatLine
bool (*onRepeatLinePtr)(const String &repeatname, const int num) = NULL;
// appelé avant chaque ligne precédée d'un [#REPEAT_LINE xxxxxx#]  repeatname = xxxxxx
bool onRepeatLine(const String &repeatname, const int num) {
  D_print(F("WEB: got a repeat ")); D_print(repeatname);
  D_print(F(" num=")); D_println(num);
  if ( repeatname.equals(F("_WIFI")) ) return repeatLineScanNetwork(num);
  if (onRepeatLinePtr) return (*onRepeatLinePtr)(repeatname, num);
  D_println(F("WEB: repeat not catched "));
  return (false);
}




//// Dealing with WEB HTTP request
//#define LOCHex2Char(X) (X + (X <= 9 ? '0' : ('A' - 10)))
char LOCHex2Char( byte aByte) {
  return aByte + (aByte <= 9 ? '0' : 'A' -  10);
}
//// looking for requested file into local web pages
void HTTP_HandleRequests() {

  Serial.print(F("WEB receved a "));
  Serial.print( String((Server.method() == HTTP_GET) ? "GET '" : "POST '" ));
  Serial.print(Server.uri());
  Serial.print(F("' from "));
  Serial.print(Server.client().remoteIP());
  Serial.print(':');
  Serial.print(Server.client().remotePort());
  Serial.print(F(" <= "));
  Serial.print(Server.client().localIP());
  Serial.print(':');
  Serial.print(Server.client().localPort());
  Serial.println();

  // find if client call STATION or AP
  String fileName = TWConfig.webFolder; //default /web;
  if ( Server.client().localIP() == WiFi.localIP() ) {
    // specific for station nothing special to do
    D_println(F("WEB: answer as STATION"));
    
  } else if ( Server.client().localIP() != WiFi.softAPIP() ) {
    // specific for unknow client -> abort request
    D1_println(F("WEB: unknown client IP !!!!"));
    return;
  } else {
    // specific for AP client -> check specific config for filename and handle Captive mode
    D_println(F("WEB: answer as AP"));
    if (TWConfig.APwebFolder.length() > 0) fileName = TWConfig.APwebFolder; //default /web/wifisetup
    //  // interception en mode captive
    D_print(F("WEB: hostHeader : "));
    D_println(Server.hostHeader());
    //
    //  // in captive mode all requests to html or txt are re routed to "http://localip()" with a 302 reply
    //  if (softAP && !(Serveur.hostHeader().startsWith("169.254.169.254")) && (Serveur.uri().endsWith(".html") || Serveur.uri().endsWith("redirect") || Serveur.uri().endsWith(".txt")) ) {
    //    Serial.println(F("Request redirected to captive portal"));
    //    String aStr = F("http://");
    //    aStr += Serveur.client().localIP().toString();
    //    //   aStr += F("/APSetup/WifiManagement.html");
    //    Serveur.sendHeader("Location", aStr, true);
    //    //    Serveur.sendHeader("Location", String("http://") + Serveur.client().localIP().toString() + "/APSetup/WifiManagement.html", true);
    //    Serveur.send ( 302, "text/plain", "");
    //    Serveur.client().stop();
    //    return;
    //  }
    //
    //  // specific for firefox to hide captive mode
    //  if (softAP && Serveur.uri().endsWith("generate_204") ) {
    //    Serial.println(F("Generate204"));
    //    Serveur.setContentLength(0);
    //    Serveur.send ( 204 );
    //    Serveur.client().stop();
    //    return;
    //  }
    //
    //  // rearm timeout for captive portal
    //  // to hide captive mode stop DNS captive if a request is good (hostheader=localip)
    //  if (softAP) {
    //    timerCaptivePortal = millis();
    //    if (captiveDNS) captiveDNSStop();
    //  }
    //
    //
  }

  fileName += Server.uri();
  // todo   protection against ../

  if (fileName.endsWith(F("/")) ) fileName += "index.html";   //default page ;

  String fileMIME;

  // find MIMETYPE for standard files
  enum fileType_t { NONE, CSS, ICO, PNG, JPG, GIF, JS, HTML } fileType = NONE;
  if ( fileName.endsWith(F(".css")) ) {
    fileType = CSS;
    fileMIME = F("text/css");
  } else if ( fileName.endsWith(F(".ico")) ) {
    fileType = ICO;
    fileMIME = F("image/x-icon");
  } else if ( fileName.endsWith(F(".png")) ) {
    fileType = PNG;
    fileMIME = F("image/png");
  } else if ( fileName.endsWith(F(".jpg")) ) {
    fileType = JPG;
    fileMIME = F("image/jpg");
  } else if ( fileName.endsWith(F(".gif")) ) {
    fileType = GIF;
    fileMIME = F("image/gif");
  } else if ( fileName.endsWith(F(".js")) ) {
    fileType = JS;
    fileMIME = F("application/javascript");
  } else if ( fileName.endsWith(F(".html")) ) {
    fileType = HTML;
    fileMIME = F("text/html");
  }

  // On each http request a callback to onRequest is call to inform sketch of any http request with the submit name if submit arg exist
  // if redirectTo(aUri) is set then an error 302 will be sent to redirect request

  TWS::redirectUri = "";
  const String* submitPtr = nullptr;
  if ( Server.hasArg(F("submit")) ) {
    submitPtr = &(Server.arg(F("submit")));
    Serial.print(F("WEB: Submit action '"));
    Serial.print(*submitPtr);
    Serial.println("'");
    //    for (uint8_t i = 0; i < Server.args(); i++) {
    //      String argname = Server.argName(i);
    //      if ( !argname.equals(F("submit")) && !argname.equals(F("plain")) ) {
    //        Serial.print(F("WEB: Submit arg "));
    //        Serial.print(argname);
    //        Serial.print(F("->"));
    //        Serial.println(Server.arg(i));
    //        String value = Server.arg(i);
    //        //  onSubmit(value);   //appel du callback
    //      }
    //    }

  }

  onRequest(fileName, *submitPtr);

  if (TWS::redirectUri.length() > 0) {
    Serial.print(F("WEB redirect "));
    Serial.println(TWS::redirectUri);
    Server.sendHeader("Location", TWS::redirectUri, true);
    Server.send ( 302, "text/plain", "");
    Server.client().stop();
    return;
  }




  //  // ================ SPECIFIC FOR "QUERY REFRESH"  START =======================
  //
  // query 'refresh' is a POST sended by 'appwebserver.js' to update class 'refresh' web items in real time
  // if first arg is named 'refresh'  all itmes are translated with  onRefreshItem  call back
  // then they are sended back to client in a urlencoded string
  if (Server.method() == HTTP_POST && Server.args() > 0 && Server.argName(0) == "refresh") {
    // debug track
    Serial.print(F("WEB: Query refresh ("));
    Serial.print(Server.arg(Server.args() - 1).length());
    Serial.print(F(") "));
    Serial.println(Server.arg(Server.args() - 1)); // try to get 'plain'
    // traitement des chaines par le Sketch
    String answer;
    answer.reserve(1000);   // should be max answer (all answers)
    String aKey;
    aKey.reserve(500);      // should be max for 1 value
    String aKeyName;
    aKeyName.reserve(50);
    for (uint8_t N = 0; N < Server.args(); N++) {
      //     Serial.print(F("WEB: refresh "));
      //     Serial.print(Serveur.argName(N));
      //     Serial.print(F("="));
      //      Serial.println(Serveur.arg(N));
      aKeyName = Server.argName(N);
      aKey = Server.arg(N);
      if ( !aKeyName.equals(F("plain")) && onRefreshItem(aKeyName, aKey) ) {
        if (answer.length() > 0) answer += '&';
        answer +=  aKeyName;
        answer +=  '=';
        if ( aKey.length() > 500) {
          aKey.remove(490);
          aKey += "...";
        }
        // uri encode maison :)
        for (int N = 0; N < aKey.length(); N++) {
          char aChar = aKey[N];
          if (aChar == ' ') {
            answer += '+';
          } else if ( isAlphaNumeric(aChar) ) {
            answer +=  aChar;
          } else if (String(".-~_").indexOf(aChar) >= 0) {
            answer +=  aChar;
          } else {
            answer +=  '%';
            answer += LOCHex2Char( aChar >> 4 );
            answer += LOCHex2Char( aChar & 0xF);
          } // if alpha
        } // for
      } // valide keyname
    } // for each arg

    Serial.print(F("WEB: refresh answer ("));
    Serial.print(answer.length());
    Serial.print(F(") "));
    Serial.println(answer);
    //
    Server.sendHeader("Cache-Control", "no-cache");
    Server.setContentLength(answer.length());
    Server.send(200, fileMIME.c_str(), answer);
    //    Serveur.client().stop();
    //    Serial.print(answer.length());
    //    Serial.println(F(" car."));


    return;
  }
  //  // ================ QUERY REFRESH  END =======================
  //
  //
  //  ================= Deal with local web pages ============
  File aFile;
  if (fileType != NONE) aFile = LittleFS.open(fileName, "r");
  bool doChunk = false;
  if (aFile) {
    if (fileType == HTML) {
      Server.sendHeader("Cache-Control", "no-cache");
      Server.setContentLength(CONTENT_LENGTH_UNKNOWN);
      doChunk = true;
    } else {
      Server.sendHeader("Cache-Control", "max-age=10800, public");
      Server.setContentLength(aFile.size());
    }
    Server.send(200, fileMIME.c_str());
    aFile.setTimeout(0);   // to avoid delay at EOF
    static    char aBuffer[1025];               // static dont overload heap
    static    char repeatBuffer[1025];          // static dont overload heap
    //todo avoid repeatBuffer if repeat not used
    int  repeatNumber;
    bool repeatActive = false;
    String aStrKey;  // la clef du repeat
    // repeat until end of file
    while (aFile.available()) {
      int size;
      if (!doChunk) {

        // standard file (not HTML) are with 1024 byte buffer
        size = aFile.readBytes( aBuffer, 1024 );
      } else {

        // chunked file are read line by line with spefic keyword detection
        // first keyword is [# REPEAT_LINE xxxxxx#]
        if (!repeatActive) {  // if repeat we will send the repeat line
          // if not in repeat line mode   just read one line
          size = aFile.readBytesUntil( '\n', aBuffer, 1000 );
          if (size < 1000) aBuffer[size++] = '\n'; //!!! on exactly 1000 bytes lines the '\n' will be lost :)
          aBuffer[size] = 0x00; // make aBuffer a Cstring
          // Gestion du [# REPEAT_LINE xxxxxx#]
          // if a line start with  [#REPEAT_LINE *#] it will be sended until OnRepat Call bask returne false
          // this help to display records of database ot any table (see wifisetup.html)

          if (strncmp( "[#REPEAT_LINE", aBuffer, 13) == 0) {
            char* startPtr = aBuffer + 14;
            char* stopPtr = strstr( startPtr + 1, "#]" ); // at least 1 letter keyword [#  #]
            int len = stopPtr - startPtr;
            if (  stopPtr && len >= 3 && len <= 40 ) { // grab keyword if stop ok and lenth of keyword under 40
              // grab keyword
              char aKey[41];
              strncpy(aKey, startPtr, len);
              aKey[len] = 0x00;   // aKey is Cstring
              aStrKey = aKey;
              aStrKey.trim();
              // Save the line in  repeat buffer
              strcpy(repeatBuffer, stopPtr + 2);
              repeatActive = true;
              repeatNumber = 0;
            } // end if  repeat KEY ok
          }  // end if REPEAT found
        }  // endif  !repeat active  (detection REPEAT)
        if (repeatActive) {
          aBuffer[0] = 0x00;
          // ask the sketch if we should repeat
          repeatActive = onRepeatLine(aStrKey, repeatNumber++);
          if ( repeatActive ) strcpy(aBuffer, repeatBuffer);
          size = strlen(aBuffer);
        }

        char* currentPtr = aBuffer;
        // cut line in part to deal with kerwords "[# xxxxx #]"
        while ( currentPtr = strstr(currentPtr, "[#") ) {  // start detected
          char* startPtr = currentPtr + 2;
          char* stopPtr = strstr( startPtr + 1, "#]" ); // at least 1 letter keyword [#  #]
          int len = stopPtr - startPtr;
          if (  !stopPtr || len <= 0 || len >= 40 ) { // abort if no stop or lenth of keyword over 40
            break;
          }
          // grab keyword
          char aKey[41];
          strncpy(aKey, startPtr, len);
          aKey[len] = 0x00;   // aKey is Cstring
          String aStr;
          aStr.reserve(100);
          aStr = aKey;
          aStr.trim();
          // callback to deal with keywords
          translateKey(aStr);

          // Copie de la suite de la chaine ailleur
          static  char bBuffer[500];   //  todo   deal correctly with over 500 char lines  // static dont overload heap
          strncpy(bBuffer, stopPtr + 2, 500);

          // Ajout de la chaine de remplacement
          strncpy(currentPtr, aStr.c_str(), 100);
          currentPtr += aStr.length();
          // Ajoute la suite
          strncpy(currentPtr, bBuffer, 500);
          size = strlen(aBuffer);

          //
        }// while
      } // else do chunk
      //
      //      Serial.print('.');
      if (size) Server.sendContent_P(aBuffer, size);
    }  // if avail
    if (doChunk) Server.chunkedResponseFinalize();
    //    Serial.println("<");
    Server.client().stop();
    aFile.close();
    return;
  }
  //  deal with file not found
  Serial.println("error 404");
  String message = F("File Not Found\n");
  message += "URI: ";
  message += Server.uri();
  message += F("\nMethod: ");
  message += (Server.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += Server.args(); // last is plain = all arg
  message += F("\n<br>");
  Serial.println(message);
  for (uint8_t i = 0; i < Server.args(); i++) {
    message += " " + Server.argName(i) + ": " + Server.arg(i) + "\n<br>";
  }
  message += "<H2><a href=\"/\">go home</a></H2><br>";
  Server.send(404, "text/html", message);
  Server.client().stop();
}
