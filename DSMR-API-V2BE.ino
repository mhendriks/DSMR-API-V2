/*
***************************************************************************  
**  Program  : DSMRloggerAPI (restAPI)
**
**  Copyright (c) 2021 Willem Aandewiel / Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*      
*      TODO
*      - check length Ringfiles voor en na lezen/schrijven ivm fouten
*      - Core verion met alleen MQTT en essentiele api functies, Niet meer beschikbaar in de MQTT_CORE versie:
*      √-- RING Files
*      √-- api/v2/sm
*      √-- api/v2/hist
*      - Aanpassen front-end ivm MQTT_CORE feaure
*      √ MQTT LWT -> TopTopic/LWT geeft status aan (2.3.12)
*      √ Reboot.log opgenomen in P1.log !! Na update Reboot.log en Reboot.old files verwijderen (2.3.13) 
*      - retained mqtt values voor (2.3.14)
*      -- Identification 
*      -- p1_version
*      -- Equipement_id
*      -- Gas_device_type
*      -- Gas_equipement_id
*      -- Firmware version
*      -- IP-adres
*      - Wifi RSSI als mqtt bericht sturen

  Arduino-IDE settings for DSMR-logger hardware V2&3 (ESP-M2):

    - Board: "Generic ESP8266 Module" //https://arduino.esp8266.com/stable/package_esp8266com_index.json
    - Flash mode: "DOUT"
    - Flash size: "2MB (FS: 64KB OAT:~470KB)"
    - DebugT port: "Disabled"
    - DebugT Level: "None"
    - IwIP Variant: "v2 Lower Memory"
    - Reset Method: "none"   // but will depend on the programmer!
    - Crystal Frequency: "26 MHz"
    - VTables: "Flash"
    - Flash Frequency: "40MHz"
    - CPU Frequency: "160MHz"
    - Upload Speed: "115200"                                                                                                                                                                                                                                                 
    - Erase Flash: "Only Sketch"
    - Port: <select correct port>
*/

/******************** compiler options  ********************************************/
#define USE_UPDATE_SERVER           // default ON : define if there is enough memory and updateServer to be used
#define USE_BELGIUM_PROTOCOL        // define if Slimme Meter is a Belgium Smart Meter
//#define MQTT_CORE
//#define HAS_NO_SLIMMEMETER        // define for testing only!
//#define SHOW_PASSWRDS             // well .. show the PSK key and MQTT password, what else?
//#define DEBUG_MODE
//#define HEAP_LOG

//----- EXTENSIONS
//#define USE_NTP_TIME              // define to generate Timestamp from NTP (Only Winter Time for now)

//change manual -> possible values [USE_MQTT][NL] or [BE][USE_UPDATE_SERVER][USE_NTP_TIME]
#ifdef USE_BELGIUM_PROTOCOL
  #ifdef MQTT_CORE
    #define ALL_OPTIONS "[BE][MQTT_CORE][UPDATE_SERVER]" 
  #else
    #define ALL_OPTIONS "[BE][MQTT][UPDATE_SERVER]" 
  #endif
#else 
#ifdef MQTT_CORE
    #define ALL_OPTIONS "[NL][MQTT_CORE][UPDATE_SERVER]" 
  #else
    #define ALL_OPTIONS "[NL][MQTT][UPDATE_SERVER]" 
  #endif
#endif

#include "DSMRloggerAPI.h"

void setup() 
{
  Serial.begin(115200, SERIAL_8N1);
  pinMode(FLASH_BUTTON, INPUT);
  pinMode(DTR_ENABLE, OUTPUT);
  //--- setup randomseed the right way
  //--- This is 8266 HWRNG used to seed the Random PRNG
  //--- Read more: https://config9.com/arduino/getting-a-truly-random-number-in-arduino/
  randomSeed(RANDOM_REG32); 
  Debug("\n\n ----> BOOTING....[" _VERSION "] <-----\n\n");
  DebugTln("The board name is: " ARDUINO_BOARD);

  strcpy(lastReset, ESP.getResetReason().c_str());
  DebugT(F("Last reset reason: ")); Debugln(lastReset);

  startTelnet();
  
//================ SPIFFS ===========================================
  if (SPIFFS.begin()) 
  {
    DebugTln(F("SPIFFS Mount succesfull"));
    SPIFFSmounted = true;   
  } else DebugTln(F("!!!SPIFFS Mount failed"));   // Serious problem with SPIFFS 


//------ read status file for last Timestamp --------------------
  
  //==========================================================//
  // writeLastStatus();  // only for firsttime initialization //
  //==========================================================//
  readLastStatus(); // place it in actTimestamp
  // set the time to actTimestamp!
  actT = epoch(actTimestamp, strlen(actTimestamp), true);
  nrReboots++;
  DebugTf("===> actTimestamp[%s]-> nrReboots[%u] - Errors[%u]\r\n\n", actTimestamp, nrReboots, slotErrors);                                                                    
  
  readSettings(true);
  writeLastStatus(); //update reboots
  LogFile(""); // write reboot status to file
  
//=============start Networkstuff==================================
  
  startWiFi(settingHostname, 240);  // timeout 4 minuten
  
  Debugln();
  Debug (F("Connected to " )); Debugln (WiFi.SSID());
  Debug (F("IP address: " ));  Debugln (WiFi.localIP());
  Debug (F("IP gateway: " ));  Debugln (WiFi.gatewayIP());
  Debugln();

  delay(1000);
//-----------------------------------------------------------------

  startMDNS(settingHostname);
 
//=============end Networkstuff======================================

#ifdef USE_NTP_TIME
//================ startNTP =========================================
                                                        //USE_NTP
                                                            //USE_NTP
  if (!startNTP())                                          //USE_NTP
  {                                                         //USE_NTP
    DebugTln(F("ERROR!!! No NTP server reached!\r\n\r"));   //USE_NTP
                                                     //USE_NTP
    P1Reboot();                                          //USE_NTP
  }                                                         //USE_NTP
                                                    //USE_NTP
  prevNtpHour = hour();                                     //USE_NTP
                                                            //USE_NTP
#endif  //USE_NTP_TIME                                      //USE_NTP
//================ end NTP =========================================

  DebugTf("Last reset reason: [%s]\r", ESP.getResetReason().c_str());

//============= now test if SPIFFS is correct populated!============
  if (!DSMRfileExist(settingIndexPage, false) ) spiffsNotPopulated = true;   
    
//=============end SPIFFS =========================================
  
#ifdef USE_NTP_TIME                                                           //USE_NTP
  time_t t = now(); // store the current time in time variable t                    //USE_NTP
  snprintf(cMsg, sizeof(cMsg), "%02d%02d%02d%02d%02d%02dW\0\0", (year(t) - 2000), month(t), day(t), hour(t), minute(t), second(t));
  DebugTf("Time is set to [%s] from NTP\r\n", cMsg);                                //USE_NTP
#endif  // use_dsmr_30

//================ Start MQTT  ======================================

  connectMQTT();

//================ Start HTTP Server ================================

  if (!spiffsNotPopulated) {
    DebugTln(F("SPIFFS correct populated -> normal operation!\r"));
    httpServer.serveStatic("/",                 SPIFFS, settingIndexPage);
    httpServer.serveStatic("/DSMRindex.html",   SPIFFS, settingIndexPage);
    httpServer.serveStatic(_DEFAULT_HOMEPAGE,   SPIFFS, settingIndexPage);
    httpServer.serveStatic("/index",            SPIFFS, settingIndexPage);
    httpServer.serveStatic("/index.html",       SPIFFS, settingIndexPage);
  } else {
    DebugTln(F("Oeps! not all files found on SPIFFS -> present FSexplorer!\r"));
    spiffsNotPopulated = true;
  }  
  setupFSexplorer();
  httpServer.on("/api", HTTP_GET, processAPI); // all other api calls are catched in FSexplorer onNotFounD!
  httpServer.begin();
  DebugTln( F("HTTP server gestart\r") );
  
  delay(1500);
  
//================ Start HTTP Server ================================

  DebugTf("Startup complete! actTimestamp[%s]\r\n", actTimestamp);  

//================ Start Slimme Meter ===============================

#if !defined( HAS_NO_SLIMMEMETER ) && !defined( DEBUG_MODE )
  DebugTln(F("Swapping serial port to Smart Meter, debug output will continue on telnet"));
  Debug(F("\nGebruik 'telnet "));
  Debug (WiFi.localIP());
  Debugln(F("' voor verdere debugging"));
  DebugFlush();
  delay(100);
  Serial.swap();
  delay(100);
  DebugTln(F("Enable slimmeMeter...\n"));
  slimmeMeter.enable(true);
#else
  Debug(F("\n!!! DEBUG MODE AAN !!!\n\n")); 
#endif // is_esp12

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

//========================================================================================

  unsigned int heap_before;

//==[ Do Telegram Processing ]===============================================================
void doTaskTelegram()
{
  if (Verbose1) DebugTln("doTaskTelegram");
  #if defined(HAS_NO_SLIMMEMETER)
    handleTestdata();
  #else
    //-- enable DTR to read a telegram from the Slimme Meter
    if (!Verbose1) {
      slimmeMeter.loop(); //voorkomen dat de buffer nog vol zit met andere data
      slimmeMeter.enable(true); 
    } else {
      heap_before = ESP.getFreeHeap();
      slimmeMeter.enable(true); 
      if (heap_before-ESP.getFreeHeap()!=0) Debugf("%5d | %d | %d | %d -> slimmeMeter.enable\n",telegramCount,heap_before,ESP.getFreeHeap(),heap_before-ESP.getFreeHeap());
  
      heap_before = ESP.getFreeHeap();
      slimmeMeter.loop();
      if (heap_before-ESP.getFreeHeap()!=0) Debugf("%5d | %d | %d | %d -> slimmeMeter.loop_taskTelegram\n",telegramCount,heap_before,ESP.getFreeHeap(),heap_before-ESP.getFreeHeap());
    }
  #endif
}

//===[ Do System tasks ]=============================================================
void doSystemTasks()
{
    if (!Verbose1) {
      slimmeMeter.loop();
      handleSlimmemeter();
      MQTTclient.loop();
      httpServer.handleClient();
      MDNS.update();
      handleKeyInput();
    } else {
      heap_before = ESP.getFreeHeap();
      slimmeMeter.loop();
      if (heap_before-ESP.getFreeHeap()!=0) Debugf("%5d | %d | %d | %d -> slimmeMeter.loop_system\n",telegramCount,heap_before,ESP.getFreeHeap(),heap_before-ESP.getFreeHeap());
      
      heap_before = ESP.getFreeHeap();
      handleSlimmemeter();
      if (heap_before-ESP.getFreeHeap()!=0) Debugf("%5d | %d | %d | %d -> handleSlimmemeter\n",telegramCount,heap_before,ESP.getFreeHeap(),heap_before-ESP.getFreeHeap());
      heap_before = ESP.getFreeHeap();
      MQTTclient.loop();
      if (heap_before-ESP.getFreeHeap()!=0) Debugf("%5d | %d | %d | %d -> MQTTclient.loop\n",telegramCount,heap_before,ESP.getFreeHeap(),heap_before-ESP.getFreeHeap());
      heap_before = ESP.getFreeHeap();
      httpServer.handleClient();
      if (heap_before-ESP.getFreeHeap()!=0) Debugf("%5d | %d | %d | %d -> httpServer.handleClient\n",telegramCount,heap_before,ESP.getFreeHeap(),heap_before-ESP.getFreeHeap());

      heap_before = ESP.getFreeHeap();
      MDNS.update();
      if (heap_before-ESP.getFreeHeap()!=0) Debugf("%5d | %d | %d | %d -> MDNS.update\n",telegramCount,heap_before,ESP.getFreeHeap(),heap_before-ESP.getFreeHeap());

      heap_before = ESP.getFreeHeap();
      handleKeyInput();
      if (heap_before-ESP.getFreeHeap()!=0) Debugf("%5d | %d | %d | %d -> handleKeyInput\n",telegramCount,heap_before,ESP.getFreeHeap(),heap_before-ESP.getFreeHeap());
    }
  yield();

} // doSystemTasks()
 
void loop() 
{  
  //--- verwerk volgend telegram
  if DUE(nextTelegram) doTaskTelegram();

  //--- update upTime counter
  if DUE(updateSeconds) upTimeSeconds++;

  //--- do the tasks that has to be done 
  //--- as often as possible
  doSystemTasks();
  
  //--- update statusfile + ringfiles
#ifndef MQTT_CORE
  if (DUE(antiWearRing)) writeRingFiles(); //eens per 25min + elk uur overgang in processtelegram
#endif

  if (DUE(antiWearStatus)) writeLastStatus(); //eens per 15min
  
  //--- if connection lost, try to reconnect to WiFi
  if ( DUE(reconnectWiFi) && (WiFi.status() != WL_CONNECTED) )
  {
    LogFile("Wifi connection lost");  
//    writeToSysLog("Restart wifi with [%s]...", settingHostname);
    startWiFi(settingHostname, 10);
    if (WiFi.status() != WL_CONNECTED) {
//          writeToSysLog("%s", "Wifi still not connected!");
          LogFile("Wifi connection still lost");  
    } 
//    else {
//          snprintf(cMsg, sizeof(cMsg), "IP:[%s], Gateway:[%s]", WiFi.localIP().toString().c_str()
//                                                              , WiFi.gatewayIP().toString().c_str());
//          writeToSysLog("%s", cMsg);
//    }
  }

//--- if NTP set, see if it needs synchronizing
#if defined(USE_NTP_TIME)                                           //USE_NTP
  if DUE(synchrNTP)                                                 //USE_NTP
  {
    if (timeStatus() == timeNeedsSync || prevNtpHour != hour())     //USE_NTP
    {
      prevNtpHour = hour();                                         //USE_NTP
      setSyncProvider(getNtpTime);                                  //USE_NTP
      setSyncInterval(600);                                         //USE_NTP
    }
  }
#endif                                                              //USE_NTP

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
