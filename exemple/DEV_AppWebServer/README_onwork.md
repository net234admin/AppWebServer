## Main version is here :arrow_right: [Main Branch](https://github.com/net234/AppWebServer)

# AppWebServer beta version B01

Simple Arduino lib to build an interative aplication upon local html pages
A file oriented Web Server build for dynamic user inteface with ESP8266 application.
Handle simple Server connected to local WiFi or Standalone WiFi server with no connection to internet</P>
This library uses classic HTML CSS javascipt files with dynamic (ajax like) update.

#### This works is build and tested with the ESP8266 Arduino platform with release(2.7.0) https://github.com/esp8266/Arduino

## Contents
 - [How it works](#how-it-works)
 - [Quick start](#quick-start)
   - [Installing](#Installing)
   - [Using](#using)
 - [Documentation](#documentation)
   - [Configuration](#Configuration)
   - [Simple Web page](#Simple-Web-page)
   - [Debug Output](#debug)
 - [Releases](#releases)
 - [Contributors](#Contributors)


## How It Works
- Standard web pages recorded on the flash memory can be displayed on a browser.
- Specific keywords on html pages can be replace with dynamic data on the fly.

## Quick Start
- Open "FirstDemo" in your arduino IDE (File Sample AppWebServer).
- Flash the web part with "ESP8266"" "Tools Esp8266 LittleFS Data Upload"
- Select erase flash "Sketch and WiFi Settings" (only for this demo) 
- Compile and download Firstdemo.

- Connect to WiFi "APPWEBDEMO" with your computer or tablet.
- After few seconds a window will popup with a configuration page (if no popup windows call http://10.10.10.10)
- Configure your local WiFi name and password and save.
- Reconnect your computer to your local WiFi and call  http://appwebdemo.local
- You will got access to your ESP8266 and you can turn off or on the board LED.
- See more dynamics demo in Exemple

### Installing

__This works is build and tested with the ESP8266 Arduino platform with release(2.7.0) https://github.com/esp8266/Arduino__
- Checkout library to your Arduino libraries folder

### Using
- Include in your sketch
```cpp
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
```

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <AppWebServer.h>          //https://github.com/net234/AppWebServer


- Initialize library, in your setup function add
```cpp
AppWebServer myWebServer;

myWebServer.begin();
```

After you write your sketch and start the ESP, it will try to connect to WiFi. If it fails it starts in Access Point mode.
While in AP mode, connect to it then open a browser to the gateway IP, default 192.168.4.1, configure wifi, save and it should reboot and connect.

Also see [examples](https://github.com/net234/AppWebServer/tree/main/exemple).

## Documentation

## first initialisation
At very first boot the device name will be set to 'APPWEB_XXXX'  xxxx will be different number for each device.
The device will act as a WiFi Access Point (AP Mode) named same as as the device name.
When a user connect on this WiFI (APPWEB_XXXX) the device will open the "wifisetup" web to allow wifi setup.
the user will chose the local WiFi to use with the password and eventualy a specific device for the usage.
If the device connect succefully to the wifi all the setting will be recorded 
the device will connect on the wifi and can be acceded on any web browser with the name "devicename.local"

## standard mode : local wifi mode
Wifi is connected to a local wifi and web pages can be reach with http://devicename.local to deal with user application
Configuration is done  on standalone mode with a captive portal named devicename
the captive portal is started only on the following 
at boot time if the local WiFi have never been configured
2 minutes after lost of connection of the local WiFi 
if activeCaptiveAP() is called

## standalone mode : AccesPoint mode (work in progress)
this mode is set with setStandAlone(true) or with  SetStandAlone=true in .ini file
Acces point is activated with SSID devicename
web pages can be reach with http://devicename.local do deal with user application
Configuration can be done within current page on the web site



#### Password protect the configuration Access Point
todo
```

#### Callbacks
For each http request there is differents callback to handle the data displayed
## onrequest
this callback is used to protect some page or to prepare specific data
onRequest(filename,*submitptr) is called for each http request before any data is send back to end user
filename is the full pathname of the requested file   submitprt is the value of the 'submit' argument
if the callback job need to redirect the request to another webpage just fill the redirectUri with the requested Uri of the page.
  the redirect location will be send to the client

## translateKey  (only for .html file)
each text enclosed between [# akeyword #] will trigger a translateKey(aKey) aKey will contain the enclosed text (akeyword)
the whole sequence "[# akeyword #]" will be replaced with the value set to aKey by the call back routine 


## onRepeatLine  (only for .html file)
this callback is used to display table or any repetitive strucure
html exemple :
<tr><td>user</td><td>age</td><td>adresse</td></tr>
[#REPEATLINE mytable#]<tr><td>[#username#]</td><td>[#userage#]</td><td>[#useradresse#]</td></tr>

onRepeatLine(aStrKey, repeatNumber) is called if a line of the requested file *start* with '[#REPEATLINE mytable#]'
aStrkey is the full text after REAPEATLINE (mytable)
repeatNumber is 0 on first call
if the call back answer true, the rest of the line after [#REPEATLINE mytable#] will be displayed and another OnRepeatLine will be called until the job answer false.




## onEndOfRequest
onEndOfRequest(fileName, *submitPtr);  is called after the web page is completly send to the end user
this allow to start specific job only after the page is diplayed to the end user





##### Enter Config mode
todo

##### Save settings
todo
 
#### Configuration Portal Timeout
If you need to set a timeout so the ESP doesn't hang waiting to be configured, for instance after a power failure, you can add
```cpp
wifiManager.setConfigPortalTimeout(180);
```
which will wait 3 minutes (180 seconds). When the time passes, the autoConnect function will return, no matter the outcome.
Check for connection and if it's still not established do whatever is needed (on some modules I restart them to retry, on others I enter deep sleep)

#### On Demand Configuration Portal
todo

#### Debug
todo```

## Releases
##### betaversion


works with the staging release ver. 1.6.5-1044-g170995a, built on Aug 10, 2015 of the ESP8266 Arduino library.


### Contributions and thanks
The support and help I got from the community has been nothing short of phenomenal. I can't thank you guys enough. This is my first real attept in developing open source stuff and I must say, now I understand why people are so dedicated to it, it is because of all the wonderful people involved.

__THANK YOU__

[tzapu](https://github.com/tzapu/WiFiManager)

lot more to add

Sorry if i have missed anyone.

#### Inspiration
- http://www.esp8266.com/viewtopic.php?f=29&t=2520
