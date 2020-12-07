## Main version is here :arrow_right: [Main Branch](https://github.com/net234/AppWebServer)

# AppWebServer
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
- A standard web is download via LittleFS on the flash

## Quick Start

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
```

- Initialize library, in your setup function add
```cpp
AppWebServer myWebServer;


- Also in the setup function add
```cpp
myWebServer.begin();
```

```

After you write your sketch and start the ESP, it will try to connect to WiFi. If it fails it starts in Access Point mode.
While in AP mode, connect to it then open a browser to the gateway IP, default 192.168.4.1, configure wifi, save and it should reboot and connect.

Also see [examples](https://github.com/net234/AppWebServer/tree/main/exemple).

## Documentation

#### Password protect the configuration Access Point
todo
```

#### Callbacks
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
