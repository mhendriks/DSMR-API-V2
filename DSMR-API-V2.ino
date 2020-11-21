/*
***************************************************************************  
**  Program  : DSMRloggerAPI (restAPI)
*/
#define _FW_VERSION "v2.1.1 (21-11-2020)"
/*
**  Copyright (c) 2020 Willem Aandewiel / Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*      
*      TODO
*      - v1/dev/settings .js file (opslaan settings)
*      - merge writeRingFile ipv per veld
*      - uploadserver
*      
*      Backlog
*      - ringfile : fout bestaande file - oude renamen en nieuwe maken
*      - telegram message bij drempelwaardes
*      - telegram verbruiksrapport einde dag/week/maand
*      - telegram bot gebruiken als UI

  Arduino-IDE settings for DSMR-logger Version 1 (ESP-M3):

    - Board: "Generic ESP8266 Module"
    - Flash mode: "DOUT" | "DIO"    // change only after power-off and on again!
    - Flash size: "1MB (FS: 128KB OAT:~438KB)"  
    - DebugT port: "Disabled"
    - DebugT Level: "None"
    - IwIP Variant: "v2 Lower Memory"
    - Reset Method: "none"   // but will depend on the programmer!
    - Crystal Frequency: "26 MHz" 
    - VTables: "Flash"
    - Flash Frequency: "40MHz"
    - CPU Frequency: "80 MHz of 160MHz"
    - Buildin Led: 2 
    - Upload Speed: "115200"                                                                                                                                                                                                                                                 
    - Erase Flash: "Only Sketch"
    - Port: <select correct port>
*/
/******************** compiler options  ********************************************/
#define USE_REQUEST_PIN           // define if it's a esp8266 with GPIO 12 connected to SM DTR pin
#define USE_AUX                     // define if the aux port should be used
//#define USE_UPDATE_SERVER         // define if there is enough memory and updateServer to be used
//  #define USE_BELGIUM_PROTOCOL      // define if Slimme Meter is a Belgium Smart Meter
//  #define USE_NTP_TIME              // define to generate Timestamp from NTP (Only Winter Time for now)
//  #define HAS_NO_SLIMMEMETER        // define for testing only!
#define USE_MQTT                  // define if you want to use MQTT (configure through webinterface)
//#define USE_MINDERGAS             // define if you want to update mindergas (configure through webinterface)
//  #define USE_SYSLOGGER             // define if you want to use the sysLog library for debugging
//  #define SHOW_PASSWRDS             // well .. show the PSK key and MQTT password, what else?
/******************** don't change anything below this comment **********************/
#define ALL_OPTIONS "[USE_REQUEST_PIN][USE_AUX][USE_MQTT][USE_DUTCH_PROTOCOL]" //change by hand
//possible values [USE_REQUEST_PIN][USE_AUX][USE_MQTT]([USE_DUTCH_PROTOCOL] or [USE_BELGIUM_PROTOCOL])[USE_UPDATE_SERVER][USE_MINDERGAS][USE_SYSLOGGER][USE_NTP_TIME]"

#include "DSMRloggerAPI.h"

struct showValues {
  template<typename Item>
  void apply(Item &i) {
    TelnetStream.print(F("showValues: "));
    if (i.present()) 
    {
      TelnetStream.print(Item::name);
      TelnetStream.print(F(": "));
      TelnetStream.print(i.val());
      TelnetStream.print(Item::unit());
    //} else 
    //{
    //  TelnetStream.print(F("<no value>"));
    }
    TelnetStream.println();
  }
};

#ifdef USE_SYSLOGGER
//===========================================================================================
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
  pinMode(2, INPUT); //optocoopler input
  pinMode(FLASH_BUTTON, INPUT);
#ifdef DTR_ENABLE
  pinMode(DTR_ENABLE, OUTPUT);
#endif
  //--- setup randomseed the right way
  //--- This is 8266 HWRNG used to seed the Random PRNG
  //--- Read more: https://config9.com/arduino/getting-a-truly-random-number-in-arduino/
  randomSeed(RANDOM_REG32); 
  snprintf(settingHostname, sizeof(settingHostname), "%s", _DEFAULT_HOSTNAME);
  Serial.printf("\n\nBooting....[%s]\r\n\r\n", String(_FW_VERSION).c_str());
  delay(500);
  lastReset     = ESP.getResetReason();

  startTelnet();
  Serial.println("The board name is: " ARDUINO_BOARD);
  
//================ SPIFFS ===========================================
  if (SPIFFS.begin()) 
  {
    DebugTln(F("SPIFFS Mount succesfull\r"));
    SPIFFSmounted = true;
      
  } else { 
    DebugTln(F("SPIFFS Mount failed\r"));   // Serious problem with SPIFFS 
    SPIFFSmounted = false;
    
  }

//------ read status file for last Timestamp --------------------
  
  //==========================================================//
  // writeLastStatus();  // only for firsttime initialization //
  //==========================================================//
  readLastStatus(); // place it in actTimestamp
  // set the time to actTimestamp!
  actT = epoch(actTimestamp, strlen(actTimestamp), true);
  DebugTf("===> actTimestamp[%s]-> nrReboots[%u] - Errors[%u]\r\n\n", actTimestamp
                                                                    , nrReboots++
                                                                    , slotErrors);                                                                    
  readSettings(true);
  
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

  Serial.print(F("\nGebruik 'telnet "));
  Serial.print (WiFi.localIP());
  Serial.println(F("' voor verdere debugging\r\n"));

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
    httpServer.serveStatic("/",                 SPIFFS, "/DSMRindexEDGE.html");
    httpServer.serveStatic("/DSMRindex.html",   SPIFFS, "/DSMRindexEDGE.html");
    httpServer.serveStatic("/index",            SPIFFS, "/DSMRindexEDGE.html");
    httpServer.serveStatic("/index.html",       SPIFFS, "/DSMRindexEDGE.html");
  } else {
    DebugTln(F("Oeps! not all files found on SPIFFS -> present FSexplorer!\r"));
    spiffsNotPopulated = true;
  }
  
  httpServer.serveStatic("/FSexplorer",      SPIFFS, "/FSexplorer.html"); //for version 2.0.0 firmware
  
  setupFSexplorer();
  httpServer.on("/api", HTTP_GET, processAPI); // all other api calls are catched in FSexplorer onNotFounD!
  httpServer.begin();
  DebugTln( F("HTTP server gestart\r") );
  
  delay(2000);
  
//================ Start HTTP Server ================================
  
#ifdef USE_MINDERGAS
    handleMindergas();
#endif

  DebugTf("Startup complete! actTimestamp[%s]\r\n", actTimestamp);  
  writeToSysLog("Startup complete! actTimestamp[%s]", actTimestamp);  

//================ End of Slimmer Meter ============================
//================ The final part of the Setup =====================

  snprintf(cMsg, sizeof(cMsg), "Last reset reason: [%s]\r", ESP.getResetReason().c_str());
  DebugTln(cMsg);

//================ Start Slimme Meter ===============================
  DebugTln(F("Enable Aux interrupt..\r"));
#ifdef USE_AUX
  //enable AUX input 
  pinMode(2, INPUT);                   // set pin 2 as input
  attachInterrupt(digitalPinToInterrupt(2), isrAux, CHANGE);       // interrupt program when signal to pin 2 detected call ISR function when happens
#endif

  DebugTln(F("Enable slimmeMeter..\r"));

#if defined( USE_REQUEST_PIN ) && !defined( HAS_NO_SLIMMEMETER )
    DebugTf("Swapping serial port to Smart Meter, debug output will continue on telnet\r\n");
    DebugFlush();
    Serial.swap();
#endif // is_esp12
  delay(100);
  slimmeMeter.enable(true);
  Rebootlog(); // write reboot status to file
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
