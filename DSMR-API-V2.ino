/*
***************************************************************************  
**  Program  : DSMRloggerAPI (restAPI)
**
**  Copyright (c) 2021 Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*      
TODO
- telnet update via windows ... invoeren lukt niet
- Jos: zou je de dag totalen zoals op het dashboard ook via MQTT kunnen exporteren? Gas_dag, Water_dag, Afname_dag, Teruglevering_dag, Afname-Terug_dag
- data voor dag tabel eens per dag
- data voor maand tabel eens per maand

Wip


Arduino-IDE settings for DSMR-logger hardware ESP12S module:

    - Board: "Generic ESP8266 Module" //https://arduino.esp8266.com/stable/package_esp8266com_index.json
    - Flash mode: "QIO"
    - Flash size: "4MB (FS: 1MB OAT:~1019KB)"
    - DebugT port: "Disabled"
    - DebugT Level: "None"
    - IwIP Variant: "v2 Lower Memory"
    - Reset Method: "none"   // but will depend on the programmer!
    - Crystal Frequency: "26 MHz"
    - VTables: "Flash"
    - Flash Frequency: "40MHz"`
    - CPU Frequency: "160MHz"
    - Buildin Led: 2
    - Upload Speed: "460800"                                                                                                                                                                                                                                                 
    - Erase Flash: "Only Sketch"
    - Port: <select correct port>
*/
/******************** compiler options  ********************************************/
//#define SHOW_PASSWRDS             // well .. show the PSK key and MQTT password, what else?
//#define DEBUG_MODE
//#define V2_COMPATIBLE             // Spiffs version firmware 2.x compatible ESP-M(2/3) HARDWARE
//#define NO_HIST

//----- EXTENSIONS
#define HA_DISCOVER
#define ALL_OPTIONS "[CORE]"

#include "DSMRloggerAPI.h"

//===========================================================================================
void setup() 
{
  Serial.begin(115200, SERIAL_8N1);
  pinMode(DTR_ENABLE, OUTPUT);
  pinMode(LED, OUTPUT); //LED ESP12S
  // sign of life
  digitalWrite(LED, LOW); //ON
  delay(1500);
  digitalWrite(LED, HIGH); //OFF
  
  //--- setup randomseed the right way
  //--- This is 8266 HWRNG used to seed the Random PRNG
  //--- Read more: https://config9.com/arduino/getting-a-truly-random-number-in-arduino/
  randomSeed(RANDOM_REG32); 
  
  Debugln(F("\n\n ----> BOOTING....[" _VERSION "] <-----\n"));
  DebugTln(F("The board name is: " ARDUINO_BOARD));

  strcpy(lastReset , ESP.getResetReason().c_str());
  DebugT(F("Last reset reason: ")); Debugln(lastReset);
  
  startTelnet();

//================ Read Persistance Settings ====================
  P1StatusBegin();
  if (P1StatusAvailable()) P1StatusRead(); //load persistant data

//================ FS ===========================================
  if (FS.begin()) 
  {
    DebugTln(F("File System Mount succesful"));
    FSmounted = true;   
  } else {
    DebugTln(F("/!\\File System Mount failed/!\\"));   // Serious problem with FS 

  }

  // set the time to actTimestamp!
  actT = epoch(actTimestamp, strlen(actTimestamp), true);
  P1Status.reboots++;
//  DebugTf("===> actTimestamp[%s]-> nrReboots[%u] - Errors[%u]\r\n\n", actTimestamp, P1Status.reboots, P1Status.sloterrors);                                                                    
  P1StatusWrite();
  P1StatusPrint(); //print latest values
  readSettings(true);
  LogFile("",false); // write reboot status to file
  
//=============start Networkstuff==================================
  
  startWiFi(settingHostname, 240);  // timeout 4 minuten
  startMDNS(settingHostname);

//================ Start MQTT  ======================================

  if ( (strlen(settingMQTTbroker) > 3) && settingMQTTinterval) connectMQTT();

//================ Start HTTP Server ================================

  if (!DSMRfileExist(settingIndexPage, false) ) CreateIndexHtml();

  if (DSMRfileExist(settingIndexPage, false) ) {
    DebugTln(F("File System correct populated -> normal operation!\r"));
    httpServer.serveStatic("/",                 FS, settingIndexPage);
    httpServer.serveStatic("/DSMRindex.html",   FS, settingIndexPage);
    httpServer.serveStatic(_DEFAULT_HOMEPAGE,   FS, settingIndexPage);
  } else DebugTln(F("Oeps! not all files found on File System -> present FSexplorer!\r"));

  setupFSexplorer();
//  delay(500);  
//  DebugTf("Startup complete! actTimestamp[%s]\r\n", actTimestamp);  

//================ Start Slimme Meter ===============================

#ifndef V2_COMPATIBLE  
  ConvRing3_2_0(); //only new modules
#else
  FS.remove("/Reboot.log");      //pre 3.1.1 
  FS.remove("/Reboot.old");      //pre 3.1.1  
  FS.remove("/DSMRstatus.json"); //pre 3.1.1 
#endif

#ifdef NO_HIST
  if ( EnableHistory ) {
    EnableHistory = false;
    FS.remove(RingFiles[0].filename);
    FS.remove(RingFiles[1].filename);
    FS.remove(RingFiles[2].filename);
    FS.remove("/P1_log.old");
    FS.remove("/Reboot.old");      //pre 3.1.1    
    
    writeSettings(); //write to settings file
  }
#endif

 if (EnableHistory) CheckRingExists();

#ifndef DEBUG_MODE 
  DebugTln(F("Swapping serial port to Smart Meter, debug output will continue on telnet"));
  Debug(F("\nGebruik 'telnet "));  Debug (WiFi.localIP());  Debugln(F("' voor verdere debugging"));
  DebugFlush();
  delay(200);
  Serial.swap();
  delay(200);
  DebugTln(F("Enable slimmeMeter...\n"));
  slimmeMeter.enable(false);
#else
  Debug(F("\n!!! DEBUG MODE AAN !!!\n\n"));
#endif

//check old ota url and update with new --- only once at startup
  if( strcmp(BaseOTAurl,"http://smart-stuff.nl/ota/") == 0 ) {
    Debugln(F("Old OTA url available"));
    strcpy(BaseOTAurl, "http://ota.smart-stuff.nl/");
    writeSettings();
  }

} // setup()

//===[ no-blocking delay with running background tasks in ms ]============================
DECLARE_TIMER_MS(timer_delay_ms, 1);
void delayms(unsigned long delay_ms)
{
  CHANGE_INTERVAL_MS(timer_delay_ms, delay_ms);
  RESTART_TIMER(timer_delay_ms);
  while (!DUE(timer_delay_ms))
  {
    doSystemTasks();
  }   
} // delayms()


//===[ Do System tasks ]=============================================================
void doSystemTasks()
{
  slimmeMeter.loop();
  handleSlimmemeter();
  MQTTclient.loop();
  httpServer.handleClient();
  MDNS.update();
  handleKeyInput();
  yield();
} // doSystemTasks()

void loop () 
{  

  //--- update upTime counter
  if DUE(updateSeconds) upTimeSeconds++;
  
  //--- do the tasks that has to be done as often as possible
  doSystemTasks(); 

  if ( DUE(StatusTimer) && (telegramCount > 2) ) { 
    P1StatusWrite();
    MQTTSentStaticInfo();
    CHANGE_INTERVAL_MIN(StatusTimer, 23);
  }

  if (UpdateRequested) RemoteUpdate(UpdateVersion, true);

  yield();

} // loop()


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
