/*
***************************************************************************  
**  Program  : Network.h, part of DSMRloggerAPI
**  Version  : v3.0.0
**
**  Copyright (c) 2021 Willem Aandewiel / Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
#include "DSMRloggerAPI.h"

#include <ESP8266WiFi.h>        //ESP8266 Core WiFi Library         
#include <ESP8266WebServer.h>   // Version 1.0.0 - part of ESP8266 Core https://github.com/esp8266/Arduino
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>        // part of ESP8266 Core https://github.com/esp8266/Arduino
#include <WiFiUdp.h>            // part of ESP8266 Core https://github.com/esp8266/Arduino
#include "ModUpdateServer.h"  // https://github.com/mrWheel/ModUpdateServer
#include "UpdateServerHtml.h"
#include <WiFiManager.h>        // version 0.16.0 - https://github.com/tzapu/WiFiManager

ESP8266WebServer        httpServer (80);
ESP8266HTTPUpdateServer httpUpdater(true);

#define   MaxWifiReconnect      10
DECLARE_TIMER_SEC(WifiReconnect, 5); //try after x sec

//bool        isConnected         = false;
bool        WifiBoot            = true;
byte        WiFiReconnectCount  = 0;
bool        WifiConnected       = false;

void P1StatusWrite();
void P1Reboot();
void LogFile(const char*, bool);
void startWiFi(const char* , int );


// naar setup van https://github.com/gmag11/ESPNtpClient/blob/main/examples/advancedExample/advancedExample.ino
void onWifiEvent (WiFiEvent_t event) {
    switch (event) {
    case WIFI_EVENT_STAMODE_CONNECTED:
        DebugTf ("Connected to %s. Asking for IP address.\r\n", WiFi.BSSIDstr().c_str());
        break;
    case WIFI_EVENT_STAMODE_GOT_IP:
        LogFile("Wifi Connected",true);
        digitalWrite(LED, LOW); //ON
        Debug (F("\nConnected to " )); Debugln (WiFi.SSID());
        Debug (F("IP address: " ));  Debug (WiFi.localIP());
        Debug (F(" ( gateway: " ));  Debug (WiFi.gatewayIP());Debug(" )\n\n");
        WiFiReconnectCount = 0;
        WifiBoot = false;
        WifiConnected = true;
        break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
        if (DUE(WifiReconnect)) {
          if ( WifiConnected ) LogFile("Wifi connection lost",true); //log only once 
          WifiConnected = false;                 
          WiFi.reconnect();
          if ( (WiFiReconnectCount++ > MaxWifiReconnect)  && !WifiBoot ) P1Reboot();
        }
        break;
    default:
        DebugTf ("[WiFi-event] event: %d\n", event);
        break;
    }
}

//gets called when WiFiManager enters configuration mode
//===========================================================================================
void configModeCallback (WiFiManager *myWiFiManager) 
{
  DebugTln(F("Entered config mode\r"));
  DebugTln(WiFi.softAPIP().toString());
  //if you used auto generated SSID, print it
  DebugTln(myWiFiManager->getConfigPortalSSID());

} // configModeCallback()

//===========================================================================================
void startWiFi(const char* hostname, int timeOut) 
{
  WiFiManager manageWiFi;
  uint32_t lTime = millis();
  String thisAP = String(hostname) + "-" + WiFi.macAddress();
  
  Debugln(F("Wifi Starting"));
  digitalWrite(LED, HIGH); //OFF
  WifiBoot = true;
  WiFi.onEvent(onWifiEvent);
  
  manageWiFi.setDebugOutput(false);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  //--- set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  manageWiFi.setAPCallback(configModeCallback);

  //--- sets timeout until configuration portal gets turned off
  //--- useful to make it all retry or go to sleep in seconds
  //manageWiFi.setTimeout(240);  // 4 minuten
  manageWiFi.setTimeout(timeOut);  // in seconden ...
  //--- fetches ssid and pass and tries to connect
  //--- if it does not connect it starts an access point with the specified name
  //--- here  "DSMR-WS-<MAC>"
  //--- and goes into a blocking loop awaiting configuration
  if (!manageWiFi.autoConnect("P1-Dongle")) 
  {
    LogFile("Wifi failed to connect and hit timeout",true);   
    //reset and try again, or maybe put it to deep sleep
    DebugTf(" took [%d] seconds ==> ERROR!\r\n", (millis() - lTime) / 1000);
    P1Reboot();
    return;
  } 
  DebugTf("took [%d] seconds => OK!\n", (millis() - lTime) / 1000);

  httpUpdater.setup(&httpServer);
  httpUpdater.setIndexPage(UpdateHTML);
  
} // startWiFi()

//===========================================================================================
void startTelnet() 
{
  TelnetStream.begin();
  DebugTln(F("Telnet server started"));
  TelnetStream.flush();

} // startTelnet()


//=======================================================================
void startMDNS(const char *Hostname) 
{
  DebugTf("[1] mDNS setup as [%s.local]\r\n", Hostname);
  if (MDNS.begin(Hostname))               // Start the mDNS responder for Hostname.local
  {
    DebugTf("[2] mDNS responder started as [%s.local]\r\n", Hostname);
  } 
  else 
  {
    DebugTln(F("[3] Error setting up MDNS responder!\r\n"));
  }
  MDNS.addService("http", "tcp", 80);
  
} // startMDNS()

/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
