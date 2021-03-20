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
*      

  Arduino-IDE settings for DSMR-logger hardware V2&3 (ESP-M2):

    - Board: "Generic ESP8266 Module" //https://arduino.esp8266.com/stable/package_esp8266com_index.json
    - Flash mode: "DOUT"
    - Flash size: "1MB (FS: 64KB OAT:~470KB)" // or 2MB newer boards
    - DebugT port: "Disabled"
    - DebugT Level: "None"
    - IwIP Variant: "v2 Lower Memory"
    - Reset Method: "none"   // but will depend on the programmer!
    - Crystal Frequency: "26 MHz"
    - VTables: "Flash"
    - Flash Frequency: "40MHz"
    - CPU Frequency: "160MHz"
    - Buildin Led: 2        //(n/a)
    - Upload Speed: "115200"                                                                                                                                                                                                                                                 
    - Erase Flash: "Only Sketch"
    - Port: <select correct port>
*/
/******************** compiler options  ********************************************/
#define USE_MQTT                      // define if you want to use MQTT (configure through webinterface)
#define ALL_OPTIONS "[BLYNK][USE_MQTT][USE_DUTCH_PROTOCOL]" //change manual -> possible values [USE_AUX][PUSHOVER][BLYNK][USE_MQTT]([USE_DUTCH_PROTOCOL] or [USE_BELGIUM_PROTOCOL])[USE_UPDATE_SERVER][USE_MINDERGAS][USE_SYSLOGGER][USE_NTP_TIME]"
#define USE_UPDATE_SERVER           // define if there is enough memory and updateServer to be used
//#define USE_BELGIUM_PROTOCOL      // define if Slimme Meter is a Belgium Smart Meter
//#define HAS_NO_SLIMMEMETER        // define for testing only!
//#define SHOW_PASSWRDS             // well .. show the PSK key and MQTT password, what else?
//#define DEBUG_MODE
//#define HIST_CONV

//----- EXTENSIONS
//#define USE_NTP_TIME              // define to generate Timestamp from NTP (Only Winter Time for now)
//#define USE_SYSLOGGER             // define if you want to use the sysLog library for debugging
//#define USE_MINDERGAS             // define if you want to update mindergas (configure through webinterface)
//#define USE_AUX                   // define if the aux port input should be used
#define USE_BLYNK                 // define if the blynk app could be used
//#define USE_PUSHOVER              // define if the pushover app could be used

#include "DSMRloggerAPI.h"

#ifdef USE_SYSLOGGER
void openSysLog(bool empty)
{
  if (sysLog.begin(500, 100, empty))  // 500 lines use existing sysLog file
  {   
    DebugTln(F("Succes opening sysLog!"));
    
  }
  else
  {
    DebugTln(F("Error opening sysLog!"));
    
  }

  sysLog.setDebugLvl(1);
  sysLog.setOutput(&TelnetStream);
  sysLog.status();
  sysLog.write("\r\n");
  for (uint8_t q=0;q<3;q++)
  {
    sysLog.write("******************************************************************************************************");
  }
  writeToSysLog(F("Last Reset Reason [%s]"), ESP.getResetReason().c_str());
  writeToSysLog("actTimestamp[%s], nrReboots[%u], Errors[%u]", actTimestamp
                                                             , nrReboots
                                                             , slotErrors);

  sysLog.write(" ");

} // openSysLog()
#endif

//===========================================================================================
void setup() 
{
  Serial.begin(115200, SERIAL_8N1);
  pinMode(AUX_IN, INPUT); //optocoopler input
  pinMode(FLASH_BUTTON, INPUT);
  pinMode(DTR_ENABLE, OUTPUT);
  //--- setup randomseed the right way
  //--- This is 8266 HWRNG used to seed the Random PRNG
  //--- Read more: https://config9.com/arduino/getting-a-truly-random-number-in-arduino/
  randomSeed(RANDOM_REG32); 
  Debug("\n\n ----> BOOTING....[" _VERSION "] <-----\n\n");
  DebugTln("The board name is: " ARDUINO_BOARD);

  lastReset = ESP.getResetReason();
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
  Rebootlog(); // write reboot status to file
  
//=============start Networkstuff==================================
  
  startWiFi(settingHostname, 240);  // timeout 4 minuten
  
  Debugln();
  Debug (F("Connected to " )); Debugln (WiFi.SSID());
  Debug (F("IP address: " ));  Debugln (WiFi.localIP());
  Debug (F("IP gateway: " ));  Debugln (WiFi.gatewayIP());
  Debugln();

  delay(1000);
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

//============= now test if SPIFFS is correct populated!============
  if (!DSMRfileExist(settingIndexPage, false) )
  {
    spiffsNotPopulated = true;   
  }
    
//=============end SPIFFS =========================================
#ifdef USE_SYSLOGGER
  if (spiffsNotPopulated)
  {
    sysLog.write(F("SPIFFS is not correct populated (files are missing)"));
  }
#endif
  
#if defined(USE_NTP_TIME)                                                           //USE_NTP
  time_t t = now(); // store the current time in time variable t                    //USE_NTP
  snprintf(cMsg, sizeof(cMsg), "%02d%02d%02d%02d%02d%02dW\0\0"                      //USE_NTP
                                               , (year(t) - 2000), month(t), day(t) //USE_NTP
                                               , hour(t), minute(t), second(t));    //USE_NTP
  pTimestamp = cMsg;                                                                //USE_NTP
  DebugTf("Time is set to [%s] from NTP\r\n", cMsg);                                //USE_NTP
#endif  // use_dsmr_30


//================ Start MQTT  ======================================

#ifdef USE_MQTT                                                 //USE_MQTT
  connectMQTT();                                                //USE_MQTT
#endif                                                          //USE_MQTT

//================ End of Start MQTT  ===============================
//================ Start HTTP Server ================================

  if (!spiffsNotPopulated) {
    DebugTln(F("SPIFFS correct populated -> normal operation!\r"));
    httpServer.serveStatic("/",                 SPIFFS, settingIndexPage);
    httpServer.serveStatic("/DSMRindex.html",   SPIFFS, settingIndexPage);
    httpServer.serveStatic("/DSMRindexEDGE.html",SPIFFS, settingIndexPage);
    httpServer.serveStatic("/index",            SPIFFS, settingIndexPage);
    httpServer.serveStatic("/index.html",       SPIFFS, settingIndexPage);
  } else {
    DebugTln(F("Oeps! not all files found on SPIFFS -> present FSexplorer!\r"));
    spiffsNotPopulated = true;
  }
  
  //httpServer.serveStatic("/FSexplorer",      SPIFFS, "/FSexplorer.html"); //for version 2.0.0 firmware
  
  setupFSexplorer();
  httpServer.on("/api", HTTP_GET, processAPI); // all other api calls are catched in FSexplorer onNotFounD!
  httpServer.begin();
  DebugTln( F("HTTP server gestart\r") );
  
  delay(1500);
  
//================ Start HTTP Server ================================
  
#ifdef USE_MINDERGAS
    handleMindergas();
#endif

  DebugTf("Startup complete! actTimestamp[%s]\r\n", actTimestamp);  
  writeToSysLog("Startup complete! actTimestamp[%s]", actTimestamp);  

//================ End of Slimmer Meter ============================
//================ The final part of the Setup =====================

  
//================ Start Slimme Meter ===============================
#ifdef USE_AUX
  DebugTln(F("Enable Aux interrupt..."));
  attachInterrupt(digitalPinToInterrupt(2), isrAux, CHANGE);       // interrupt program when signal to pin 2 detected call ISR function when happens
#endif

#ifdef USE_BLYNK
  DebugTln(F("Enable Blynk..."));
  SetupBlynk();
#endif

#ifdef HIST_CONV
  hist_conv(); 
#endif


#if !defined( HAS_NO_SLIMMEMETER ) && !defined( DEBUG_MODE )
  DebugTf("Swapping serial port to Smart Meter, debug output will continue on telnet\r\n");
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

//==[ Do Telegram Processing ]===============================================================
void doTaskTelegram()
{
  if (Verbose1) DebugTln("doTaskTelegram");
  #if defined(HAS_NO_SLIMMEMETER)
    handleTestdata();
  #else
    //-- enable DTR to read a telegram from the Slimme Meter
    slimmeMeter.enable(true); 
    slimmeMeter.loop();
    handleSlimmemeter();
  #endif
  if (WiFi.status() != WL_CONNECTED)
  {
    //for(int b=0; b<10; b++) { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); delay(75);}
    delay(750);
  }
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

//===[ Do System tasks ]=============================================================
void doSystemTasks()
{
  #ifndef HAS_NO_SLIMMEMETER
    slimmeMeter.loop();
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
  //--- do the tasks that has to be done 
  //--- as often as possible
  doSystemTasks();

  loopCount++;

  //--- verwerk volgend telegram
  if DUE(nextTelegram)
  {
    doTaskTelegram();
    #ifdef USE_BLYNK
      UpdateDayStats();
      Blynk.run();
      handleBlynk();
    #endif
  }

  //--- update upTime counter
  if DUE(updateSeconds)
  {
    upTimeSeconds++;
  }

  

//--- if mindergas then check
#ifdef USE_MINDERGAS
  if ( DUE(minderGasTimer) )
  {
    handleMindergas();
  }
#endif

  //--- if connection lost, try to reconnect to WiFi
  if ( DUE(reconnectWiFi) && (WiFi.status() != WL_CONNECTED) )
  {
    writeToSysLog("Restart wifi with [%s]...", settingHostname);
    startWiFi(settingHostname, 10);
    if (WiFi.status() != WL_CONNECTED)
          writeToSysLog("%s", "Wifi still not connected!");
    else {
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
  
#ifdef USE_AUX
  if DUE(AuxTimer) handleAux(); //manage Aux interupt
#endif



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
