## Telechergement du source :arrow_right: [Main Branch](https://github.com/net234/AppWebServer)
## Text in english :arrow_right: [In English](README.MD)

# AppWebServer beta version B01

Une simple librairie Arduino pour construire des applications IoT Basée sur des pages Web incluse dans un serveur local.
C'est un serveur de fichier construit pour developper un interface utilisateur dynamic pour une application sur un ESP8266.
AppWebServer gère un simple serveur Web connecté au WiFi local ou fonctionnant en autonome sans connexion à l'internet.
Cette librairie utilise des fichiers Web standards (HTML CSSS javascript) avec un rafraichissement dynamic des données affichées (Genre ajax simplifié).

#### Cette version est construite et testée avec la plateforme Arduino ESP8266 Version(2.7.0) https://github.com/esp8266/Arduino (ou ultérieur)

## Contenu
 - [Comment Cela Fonctionne](#)
 - [Quick start](#quick-start)
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


## Releases https://github.com/net234/AppWebServer
##### betaversion B01  27/12/2020 - pre release of the lib Copyright 2020 Pierre HENRY net23@frdev.com 


works with the staging release ver. 1.6.5-1044-g170995a, built on Aug 10, 2015 of the ESP8266 Arduino library.


### Contributions and thanks
The support and help I got from the community has been nothing short of phenomenal. I can't thank you guys enough. This is my first real attept in developing open source stuff and I must say, now I understand why people are so dedicated to it, it is because of all the wonderful people involved.

__THANK YOU__

[tzapu](https://github.com/tzapu/WiFiManager) for the great work on WiFimanager.  AppWebServer dont use WiFimanager but the same functionality are implemented.

lot more to add

Sorry if i have missed anyone.

#### Inspiration
- http://www.esp8266.com/viewtopic.php?f=29&t=2520
