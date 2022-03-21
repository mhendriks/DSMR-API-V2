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
- Piekvermogen bijhouden Belgie
- eenmalig doorgeven ipadres om het installeren makkelijk te maken (alleen op verzoek en voor maar 15 minuten)
- Opties in de config (bv water/blynk/NTP) obv deze config juiste updates ophalen
- Aanpassen front-end ivm no-history feaure

!!! FIXES
- telnet update via windows ... invoeren lukt niet


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
//#define HAS_NO_SLIMMEMETER        // define for testing only!
//#define SHOW_PASSWRDS             // well .. show the PSK key and MQTT password, what else?
#define DEBUG_MODE
//#define V2_COMPATIBLE              // Spiffs version firmware 2.x compatible ESP-M(2/3) HARDWARE

//----- EXTENSIONS
//#define USE_WATER_SENSOR 1
#define USE_APP        4           // define if the Arduino IOT app could be used
#define HA_DISCOVER

#ifdef USE_WATER_SENSOR
  #define ALL_OPTIONS "[MQTT][UPDATE_SERVER][LITTLEFS][HA DISCOVERY][WATER]"
#else if
  #define ALL_OPTIONS "[MQTT][UPDATE_SERVER][LITTLEFS][HA DISCOVERY]"
#endif

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
  } else DebugTln(F("/!\\File System Mount failed/!\\"));   // Serious problem with FS 

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

#ifdef USE_APP
  APPsetup();
#endif

#ifdef USE_WATER_SENSOR  
  setupWater();
#endif //USE_WATER_SENSOR

#ifndef V2_COMPATIBLE  
  ConvRing3_2_0(); //only new modules
#else
  FS.remove("/Reboot.log");      //pre 3.1.1 
  FS.remove("/Reboot.old");      //pre 3.1.1  
  FS.remove("/DSMRstatus.json"); //pre 3.1.1 
#endif

  CheckRingExists();

#if !defined( HAS_NO_SLIMMEMETER ) && !defined( DEBUG_MODE )
  DebugTln(F("Swapping serial port to Smart Meter, debug output will continue on telnet"));
  Debug(F("\nGebruik 'telnet "));  Debug (WiFi.localIP());  Debugln(F("' voor verdere debugging"));
  DebugFlush();
  delay(200);
  Serial.swap();
  delay(200);
  DebugTln(F("Enable slimmeMeter...\n"));
  slimmeMeter.enable(true);
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

//==[ Do Telegram Processing ]===============================================================
void doTaskTelegram()
{
  if (Verbose1) DebugTln("doTaskTelegram");
  #if defined(HAS_NO_SLIMMEMETER)
    handleTestdata();
  #else
    slimmeMeter.loop(); //voorkomen dat de buffer nog vol zit met andere data
    //-- enable DTR to read a telegram from the Slimme Meter
    slimmeMeter.enable(true); 
  #endif
}

//===[ Do System tasks ]=============================================================
void doSystemTasks()
{
  #ifndef HAS_NO_SLIMMEMETER
    slimmeMeter.loop();
    handleSlimmemeter();
  #endif
  MQTTclient.loop();
  httpServer.handleClient();
  MDNS.update();
  handleKeyInput();
  yield();
} // doSystemTasks()

void loop () 
{  
  //--- verwerk volgend telegram
  if DUE(nextTelegram) doTaskTelegram();

  //--- update upTime counter
  if DUE(updateSeconds) upTimeSeconds++;
  
  //--- do the tasks that has to be done as often as possible
  doSystemTasks(); 

  //--- update statusfile + ringfiles
  if ( DUE(antiWearRing) || RingCylce ) writeRingFiles(); //eens per 25min + elk uur overgang

  if (DUE(StatusTimer)) { //eens per 10min
    P1StatusWrite();
    MQTTSentStaticInfo();
    #ifdef USE_WATER_SENSOR  
      sendMQTTWater();
    #endif
    CHANGE_INTERVAL_MIN(StatusTimer, 10);
  }

  if (UpdateRequested) RemoteUpdate(UpdateVersion, true);

#ifdef USE_WATER_SENSOR    
  if ( WtrTimeBetween )  {
    DebugTf("Wtr delta readings: %d | debounces: %d | waterstand: %i.%i\n",WtrTimeBetween,debounces, P1Status.wtr_m3, P1Status.wtr_l);
    WtrTimeBetween = 0;
  }
#endif

#ifdef USE_APP
  if DUE(APPtimer) APPUpdate();
#endif

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
