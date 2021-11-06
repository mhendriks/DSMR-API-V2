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
- Core verion met alleen MQTT en essentiele api functies, Niet meer beschikbaar in de MQTT_CORE versie:
-- RING Files
-- api/v2/sm
-- api/v2/hist
- Aanpassen front-end ivm MQTT_CORE feaure- check length Ringfiles voor en na lezen/schrijven ivm fouten
- frontend instellingen mutabel in webpagina
- Piekvermogen bijhouden Belgie
- Blynk 2.0 implentatie
- Aanpassen front-end ivm MQTT_CORE feaure
- eenmalig doorgeven ipadres om het installeren makkelijk te maken (alleen op verzoek en voor maar 15 minuten)
- issue met de datafiles ... dan zelf herstellend verder gaan
*           
*   
  Arduino-IDE settings for DSMR-logger hardware V3.1 - ESP12S module:

    - Board: "Generic ESP8266 Module" //https://arduino.esp8266.com/stable/package_esp8266com_index.json
    - Flash mode: "QIO"
    - Flash size: "4MB (FS: 1MB OAT:~1019KB)"
    - DebugT port: "Disabled"
    - DebugT Level: "None"
    - IwIP Variant: "v2 Lower Memory"
    - Reset Method: "none"   // but will depend on the programmer!
    - Crystal Frequency: "26 MHz"
    - VTables: "Flash"
    - Flash Frequency: "40MHz"
    - CPU Frequency: "160MHz"
    - Buildin Led: 2
    - Upload Speed: "115200"                                                                                                                                                                                                                                                 
    - Erase Flash: "Only Sketch"
    - Port: <select correct port>
*/
/******************** compiler options  ********************************************/
#define USE_UPDATE_SERVER           // default ON : define if there is enough memory and updateServer to be used
//#define HAS_NO_SLIMMEMETER        // define for testing only!
//#define SHOW_PASSWRDS             // well .. show the PSK key and MQTT password, what else?
//#define DEBUG_MODE

//----- EXTENSIONS
#define USE_WATER_SENSOR
//#define USE_NTP_TIME              // define to generate Timestamp from NTP (Only Winter Time for now)
//#define USE_BLYNK                 // define if the blynk app could be used

#define ALL_OPTIONS "[MQTT][UPDATE_SERVER][LITTLEFS]"
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
  
  Debug("\n\n ----> BOOTING....[" _VERSION "] <-----\n\n");
  DebugTln("The board name is: " ARDUINO_BOARD);

  strcpy(lastReset , ESP.getResetReason().c_str());
  DebugT(F("Last reset reason: ")); Debugln(lastReset);
  
  startTelnet();

//================ Read Persistance Settings ====================
  P1StatusBegin();
  delay(500);
  if (P1StatusAvailable()) P1StatusRead(); //load persistant data

//================ FS ===========================================
  if (LittleFS.begin()) 
  {
    DebugTln(F("File System Mount succesful"));
    FSmounted = true;   
  } else DebugTln(F("/!\\File System Mount failed/!\\"));   // Serious problem with LittleFS 

  
  // set the time to actTimestamp!
  actT = epoch(actTimestamp, strlen(actTimestamp), true);
  P1Status.reboots++;
//  DebugTf("===> actTimestamp[%s]-> nrReboots[%u] - Errors[%u]\r\n\n", actTimestamp, P1Status.reboots, P1Status.sloterrors);                                                                    
  P1StatusWrite();
  P1StatusPrint(); //print latest values
  readSettings(true);
//  writeLastStatus(); //update reboots
  LogFile(""); // write reboot status to file
  
//=============start Networkstuff==================================
  
  startWiFi(settingHostname, 240);  // timeout 4 minuten
  
  Debugln();
  Debug (F("Connected to " )); Debugln (WiFi.SSID());
  Debug (F("IP address: " ));  Debugln (WiFi.localIP());
  Debug (F("IP gateway: " ));  Debugln (WiFi.gatewayIP());
  Debugln();

//-----------------------------------------------------------------

  startMDNS(settingHostname);
 
//=============end Networkstuff======================================

#if defined(USE_NTP_TIME)                                   //USE_NTP
//================ startNTP =========================================
                                                        //USE_NTP
                                                            //USE_NTP
  if (!startNTP())                                          //USE_NTP
  {                                                         //USE_NTP
    DebugTln(F("ERROR!!! No NTP server reached!\r\n\r"));   //USE_NTP
//    writeLastStatus();                                      //USE_NTP
    P1Reboot();
  }                                                         //USE_NTP
                                                    //USE_NTP
  prevNtpHour = hour();                                     //USE_NTP
                                                            //USE_NTP
#endif  //USE_NTP_TIME                                      //USE_NTP
//================ end NTP =========================================

  DebugTf("Last reset reason: [%s]\r", ESP.getResetReason().c_str());

//============= now test if LittleFS is correct populated!============
  if (!DSMRfileExist(settingIndexPage, false) )  FSNotPopulated = true;   
    
//=============end FS =========================================
  
#if defined(USE_NTP_TIME)                                                           //USE_NTP
  time_t t = now(); // store the current time in time variable t                    //USE_NTP
  snprintf(cMsg, sizeof(cMsg), "%02d%02d%02d%02d%02d%02dW\0\0"                      //USE_NTP
                                               , (year(t) - 2000), month(t), day(t) //USE_NTP
                                               , hour(t), minute(t), second(t));    //USE_NTP
//  pTimestamp = cMsg;                                                              //USE_NTP
  DebugTf("Time is set to [%s] from NTP\r\n", cMsg);                                //USE_NTP
#endif  // use_dsmr_30


//================ Start MQTT  ======================================

  if ( (strlen(settingMQTTbroker) > 3) && settingMQTTinterval) connectMQTT();

//================ End of Start MQTT  ===============================
//================ Start HTTP Server ================================

  if (!FSNotPopulated) {
    DebugTln(F("File System correct populated -> normal operation!\r"));
    httpServer.serveStatic("/",                 LittleFS, settingIndexPage);
    httpServer.serveStatic("/DSMRindex.html",   LittleFS, settingIndexPage);
    httpServer.serveStatic(_DEFAULT_HOMEPAGE,   LittleFS, settingIndexPage);
    httpServer.serveStatic("/index",            LittleFS, settingIndexPage);
    httpServer.serveStatic("/index.html",       LittleFS, settingIndexPage);
  } else {
    DebugTln(F("Oeps! not all files found on LittleFS -> present FSexplorer!\r"));
    FSNotPopulated = true;
  }
    
  setupFSexplorer();
 
  delay(1000);
  
//================ Start HTTP Server ================================
  
  DebugTf("Startup complete! actTimestamp[%s]\r\n", actTimestamp);  

//================ End of Slimmer Meter ============================
//================ The final part of the Setup =====================

  
//================ Start Slimme Meter ===============================

#ifdef USE_BLYNK
  SetupBlynk();
#endif

#ifdef USE_WATER_SENSOR  
  setupWater();
#endif //USE_WATER_SENSOR

#if !defined( HAS_NO_SLIMMEMETER ) && !defined( DEBUG_MODE )
  DebugTf("Swapping serial port to Smart Meter, debug output will continue on telnet\r\n");
  Debug(F("\nGebruik 'telnet "));
  Debug (WiFi.localIP());
  Debugln(F("' voor verdere debugging"));
  DebugFlush();
  delay(200);
  Serial.swap();
  delay(200);
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
  if DUE(nextTelegram) {
    doTaskTelegram();
    #ifdef USE_WATER_SENSOR  
      if (Verbose1) DebugTf("Watermeter : %d m3 %u Liters\n", P1Status.wtr_m3, P1Status.wtr_l);
    #endif
  }

  //--- update upTime counter
  if DUE(updateSeconds) upTimeSeconds++;
  
  //--- do the tasks that has to be done as often as possible
  doSystemTasks(); 

  //--- update statusfile + ringfiles
  if ( DUE(antiWearRing) || RingCylce ) writeRingFiles(); //eens per 25min + elk uur overgang

  if (DUE(StatusTimer)) { //eens per 15min of indien extra m3
    P1StatusWrite();
    CHANGE_INTERVAL_MIN(StatusTimer, 15);
    MQTTSentStaticP1Info();
  }

  if (UpdateRequested) RemoteUpdate(UpdateVersion);
  
#ifdef USE_BLYNK
  if (LittleFS.exists(_BLYNK_FILE)){
    slimmeMeter.loop(); //ivm evt verliezen data
    yield();
    Blynk.run();
    if (DUE(RefreshBlynk) && Blynk.connected()) handleBlynk(); //eens per 5sec
  }
#endif

#ifdef USE_WATER_SENSOR  
  if (DUE(WaterTimer)) {
    P1StatusWrite();
    sendMQTTWater();
    CHANGE_INTERVAL_MIN(WaterTimer, 30);
  }
#endif //USE_WATER_SENSOR

  
  //--- if connection lost, try to reconnect to WiFi
  if ( DUE(reconnectWiFi) && (WiFi.status() != WL_CONNECTED) )
  {
    sprintf(cMsg,"Wifi connection lost | rssi: %d",WiFi.RSSI());
    LogFile(cMsg);  
    startWiFi(settingHostname, 10);
    if (WiFi.status() != WL_CONNECTED){
          LogFile("Wifi connection still lost");  
    } else snprintf(cMsg, sizeof(cMsg), "IP:[%s], Gateway:[%s]", WiFi.localIP().toString().c_str(), WiFi.gatewayIP().toString().c_str());
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
