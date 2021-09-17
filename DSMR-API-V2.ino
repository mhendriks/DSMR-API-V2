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
*      - update via site ipv url incl logica voor uitvragen hiervan
*      - aanpassen Telnet plugin ivm crash na x minuten
*      - indien api niet beschikbaar (chck time) dan overige calls ook niet meer doen
*      - frontend instellingen mutabel in webpagina
*      - mqtt broker benaderen via host name http://www.iotsharing.com/2017/06/how-to-get-ip-address-from-mdns-host-name-in-arduino-esp32.html 
*      - herzien navigate
*      - webclient GEEN FOCUS -> stop met uitvragen api
*      - verplaatsen update/reset/restart
*      - nieuwe update scherm
*      - instellingen FrontEnd zichtbaar in webpagina read only
*      - fix sm data gas data indien deze niet aanwezig is
*      √ Remote update via url (alleen http)
*      √ Remote update via Telnet
*      √ Telnetmenu Save Ringfiles
*      - Piekvermogen bijhouden Belgie
*      - enelogic koppeling
*      √ uitzetten van hist data (Ringfiles)
*      - watermeter 
*      
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
#define USE_MQTT                    // default ON : define if you want to use MQTT (configure through webinterface)
#define USE_UPDATE_SERVER           // default ON : define if there is enough memory and updateServer to be used
//#define HAS_NO_SLIMMEMETER        // define for testing only!
//#define SHOW_PASSWRDS             // well .. show the PSK key and MQTT password, what else?
//#define DEBUG_MODE

//----- EXTENSIONS
//#define USE_NTP_TIME              // define to generate Timestamp from NTP (Only Winter Time for now)
//#define USE_SYSLOGGER             // define if you want to use the sysLog library for debugging
//#define USE_PUSHOVER              // define if the pushover app could be used

#define ALL_OPTIONS "[MQTT][UPDATE_SERVER][LITTLEFS]"
#include "DSMRloggerAPI.h"

#ifdef USE_SYSLOGGER
void openSysLog(bool empty)
{
  if (sysLog.begin(500, 100, empty)) DebugTln(F("Succes opening sysLog!"));  // 500 lines use existing sysLog file
  else DebugTln(F("Error opening sysLog!"));

  sysLog.setDebugLvl(1);
  sysLog.setOutput(&TelnetStream);
  sysLog.status();
  sysLog.write("\r\n");
  for (uint8_t q=0;q<3;q++)
  {
    sysLog.write("******************************************************************************************************");
  }
  writeToSysLog(F("Last Reset Reason [%s]"), ESP.getResetReason().c_str());
  writeToSysLog("actTimestamp[%s], nrReboots[%u], Errors[%u]", actTimestamp, nrReboots, slotErrors);
  sysLog.write(" ");

} // openSysLog()
#endif

//===========================================================================================
void setup() 
{
  Serial.begin(115200, SERIAL_8N1);
  pinMode(DTR_ENABLE, OUTPUT);
  pinMode(LED, OUTPUT); //LED ESP12S
  
  // sign off life
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
  
//================ FS ===========================================
  if (LittleFS.begin()) 
  {
    DebugTln(F("File System Mount succesful"));
    FSmounted = true;   
  } else DebugTln(F("!!!File System Mount failed!!!"));   // Serious problem with LittleFS 


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
  Rebootlog(); // write reboot status to file
  
//=============start Networkstuff==================================
  
  startWiFi(settingHostname, 240);  // timeout 4 minuten
  
  Debugln();
  Debug (F("Connected to " )); Debugln (WiFi.SSID());
  Debug (F("IP address: " ));  Debugln (WiFi.localIP());
  Debug (F("IP gateway: " ));  Debugln (WiFi.gatewayIP());
  Debugln();

  //for(int b=0; b<3; b++) { digitalWrite(LED, !digitalRead(LED)); delay(300);} //some delay
//-----------------------------------------------------------------
#ifdef USE_SYSLOGGER
  openSysLog(false);
  snprintf(cMsg, sizeof(cMsg), "SSID:[%s],  IP:[%s], Gateway:[%s]", WiFi.SSID().c_str()
                                                                  , WiFi.localIP().toString().c_str()
                                                                  , WiFi.gatewayIP().toString().c_str());
  writeToSysLog("%s", cMsg);
#endif

  startMDNS(settingHostname);
 
//=============end Networkstuff======================================

#if defined(USE_NTP_TIME)                                   //USE_NTP
//================ startNTP =========================================
                                                        //USE_NTP
                                                            //USE_NTP
  if (!startNTP())                                          //USE_NTP
  {                                                         //USE_NTP
    DebugTln(F("ERROR!!! No NTP server reached!\r\n\r"));   //USE_NTP
                                                     //USE_NTP
    delay(1500);                                            //USE_NTP
    ESP.restart();                                          //USE_NTP
    delay(2000);                                            //USE_NTP
  }                                                         //USE_NTP
                                                    //USE_NTP
  prevNtpHour = hour();                                     //USE_NTP
                                                            //USE_NTP
#endif  //USE_NTP_TIME                                      //USE_NTP
//================ end NTP =========================================

  snprintf(cMsg, sizeof(cMsg), "Last reset reason: [%s]\r", ESP.getResetReason().c_str());
  DebugTln(cMsg);

//============= now test if LittleFS is correct populated!============
  if (!DSMRfileExist(settingIndexPage, false) )
  {
    FSNotPopulated = true;   
  }
    
//=============end FS =========================================
#ifdef USE_SYSLOGGER
  if (FSNotPopulated)
  {
    sysLog.write(F("File System is not correct populated (files are missing)"));
  }
#endif
  
#if defined(USE_NTP_TIME)                                                           //USE_NTP
  time_t t = now(); // store the current time in time variable t                    //USE_NTP
  snprintf(cMsg, sizeof(cMsg), "%02d%02d%02d%02d%02d%02dW\0\0"                      //USE_NTP
                                               , (year(t) - 2000), month(t), day(t) //USE_NTP
                                               , hour(t), minute(t), second(t));    //USE_NTP
//  pTimestamp = cMsg;                                                              //USE_NTP
  DebugTf("Time is set to [%s] from NTP\r\n", cMsg);                                //USE_NTP
#endif  // use_dsmr_30


//================ Start MQTT  ======================================

#ifdef USE_MQTT                                                 //USE_MQTT
  //connectMQTT();
  if ( (strlen(settingMQTTbroker) > 3) && settingMQTTinterval) connectMQTT();
#endif                                                          //USE_MQTT

//================ End of Start MQTT  ===============================
//================ Start HTTP Server ================================

  if (!FSNotPopulated) {
    DebugTln(F("File System correct populated -> normal operation!\r"));
    httpServer.serveStatic("/",                 LittleFS, settingIndexPage);
    httpServer.serveStatic("/DSMRindex.html",   LittleFS, settingIndexPage);
    httpServer.serveStatic("/DSMRindexEDGE.html",LittleFS, settingIndexPage);
    httpServer.serveStatic("/index",            LittleFS, settingIndexPage);
    httpServer.serveStatic("/index.html",       LittleFS, settingIndexPage);
  } else {
    DebugTln(F("Oeps! not all files found on LittleFS -> present FSexplorer!\r"));
    FSNotPopulated = true;
  }
    
  setupFSexplorer();
 
  delay(1500);
  
//================ Start HTTP Server ================================
  
  DebugTf("Startup complete! actTimestamp[%s]\r\n", actTimestamp);  
  writeToSysLog("Startup complete! actTimestamp[%s]", actTimestamp);  

//================ End of Slimmer Meter ============================
//================ The final part of the Setup =====================

  
//================ Start Slimme Meter ===============================

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
    //-- enable DTR to read a telegram from the Slimme Meter
    slimmeMeter.enable(true); 
  #endif
//  if (WiFi.status() != WL_CONNECTED)
//  {
//    for(int b=0; b<10; b++) { digitalWrite(LED, !digitalRead(LED)); delay(100);}
//  }
}

//===[ Do System tasks ]=============================================================
void doSystemTasks()
{
  #ifndef HAS_NO_SLIMMEMETER
    slimmeMeter.loop();
    handleSlimmemeter();
  #endif
  #ifdef USE_MQTT
    MQTTclient.loop();
  #endif
  httpServer.handleClient();
  MDNS.update();
  handleKeyInput();
  yield();

} // doSystemTasks()

  
void loop () 
{  
  doSystemTasks(); //--- do the tasks that has to be done as often as possible

  //--- update upTime counter
  if DUE(updateSeconds) upTimeSeconds++;
  
  //--- verwerk volgend telegram
  if DUE(nextTelegram) doTaskTelegram();

  //--- update statusfile + ringfiles
  if (DUE(antiWearTimer))
  {
    writeRingFiles();
    writeLastStatus();
  }

  //--- if connection lost, try to reconnect to WiFi
  if ( DUE(reconnectWiFi) && (WiFi.status() != WL_CONNECTED) )
  {
    LogFile("Wifi connection lost");  
    writeToSysLog("Restart wifi with [%s]...", settingHostname);
    startWiFi(settingHostname, 10);
    if (WiFi.status() != WL_CONNECTED){
          writeToSysLog("%s", "Wifi still not connected!");
          LogFile("Wifi connection still lost");  
    } else {
          snprintf(cMsg, sizeof(cMsg), "IP:[%s], Gateway:[%s]", WiFi.localIP().toString().c_str()
                                                              , WiFi.gatewayIP().toString().c_str());
          writeToSysLog("%s", cMsg);
    }
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
