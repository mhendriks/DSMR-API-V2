/* 
***************************************************************************  
**  Program  : menuStuff, part of DSMRloggerAPI
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel / Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/


//===========================================================================================
void displayHoursHist(bool Telnet=true) 
{
    //readAllSlots(HOURS, HOURS_FILE, actTimestamp, false, "");
    RingFileTo(RINGHOURS, false);
} // displayHoursHist()


//===========================================================================================
void displayDaysHist(bool Telnet=true) 
{
    //readAllSlots(DAYS, DAYS_FILE, actTimestamp, false, "");
    RingFileTo(RINGDAYS, false);
} // displayDaysHist()


//===========================================================================================
void displayMonthsHist(bool Telnet=true) 
{
    //readAllSlots(MONTHS, MONTHS_FILE, actTimestamp, false, "");
    RingFileTo(RINGMONTHS, false);
} // displayMonthsHist()


//===========================================================================================
void displayBoardInfo() 
{
  Debugln(F("\r\n==================================================================\r"));
  Debug(F(" \r\n            (c)2019 by [Willem Aandewiel / Martijn Hendriks"));
  Debug(F("]\r\n      Firmware Version ["));  Debug( _FW_VERSION );
  Debug(F("]\r\n              Compiled ["));  Debug( __DATE__ ); 
                                               Debug( "  " );
                                               Debug( __TIME__ );
#ifdef USE_PRE40_PROTOCOL
  Debug(F("]\r\n         compiled with [dsmr30.h] [USE_PRE40_PROTOCOL"));
#else
  Debug(F("]\r\n         compiled with [dsmr.h"));
#endif
  Debug(F("]\r\n              #defines "));
#ifdef USE_REQUEST_PIN
  Debug(F("[USE_REQUEST_PIN]"));
#endif
#ifdef USE_UPDATE_SERVER
  Debug(F("[USE_UPDATE_SERVER]"));
#endif
#ifdef USE_MQTT
  Debug(F("[USE_MQTT]"));
#endif
#ifdef USE_MINDERGAS
  Debug(F("[USE_MINDERGAS]"));
#endif
#ifdef USE_SYSLOGGER
  Debug(F("[USE_SYSLOGGER]"));
#endif
#ifdef USE_NTP_TIME
  Debug(F("[USE_NTP_TIME]"));
#endif
#ifdef USE_BELGIUM_PROTOCOL
  Debug(F("[USE_BELGIUM_PROTOCOL]"));
#endif
#ifdef SHOW_PASSWRDS
  Debug(F("[SHOW_PASSWRDS]"));
#endif

  Debug(F(" \r\n   Telegrams Processed ["));  Debug( telegramCount );
  Debug(F("]\r\n           With Errors ["));  Debug( telegramErrors );
  Debug(F("]\r\n              FreeHeap ["));  Debug( ESP.getFreeHeap() );
  Debug(F("]\r\n             max.Block ["));  Debug( ESP.getMaxFreeBlockSize() );
  Debug(F("]\r\n               Chip ID ["));  Debug( ESP.getChipId(), HEX );
  Debug(F("]\r\n          Core Version ["));  Debug( ESP.getCoreVersion() );
  Debug(F("]\r\n           SDK Version ["));  Debug( ESP.getSdkVersion() );
  Debug(F("]\r\n        CPU Freq (MHz) ["));  Debug( ESP.getCpuFreqMHz() );
  Debug(F("]\r\n      Sketch Size (kB) ["));  Debug( ESP.getSketchSize() / 1024.0 );
  Debug(F("]\r\nFree Sketch Space (kB) ["));  Debug( ESP.getFreeSketchSpace() / 1024.0 );

  if ((ESP.getFlashChipId() & 0x000000ff) == 0x85) 
        snprintf(cMsg, sizeof(cMsg), "%08X (PUYA)", ESP.getFlashChipId());
  else  snprintf(cMsg, sizeof(cMsg), "%08X", ESP.getFlashChipId());

  SPIFFS.info(SPIFFSinfo);

  Debug(F("]\r\n         Flash Chip ID ["));  Debug( cMsg );
  Debug(F("]\r\n  Flash Chip Size (kB) ["));  Debug( ESP.getFlashChipSize() / 1024 );
  Debug(F("]\r\n   Chip Real Size (kB) ["));  Debug( ESP.getFlashChipRealSize() / 1024 );
  Debug(F("]\r\n      SPIFFS Size (kB) ["));  Debug( SPIFFSinfo.totalBytes / 1024 );

  Debug(F("]\r\n      Flash Chip Speed ["));  Debug( ESP.getFlashChipSpeed() / 1000 / 1000 );
  FlashMode_t ideMode = ESP.getFlashChipMode();
  Debug(F("]\r\n       Flash Chip Mode ["));  Debug( flashMode[ideMode] );

  Debugln(F("]\r"));

  Debugln(F("==================================================================\r"));
  Debug(F(" \r\n            Board type ["));
#ifdef ARDUINO_ESP8266_NODEMCU
    Debug(F("ESP8266_NODEMCU"));
#endif
#ifdef ARDUINO_ESP8266_GENERIC
    Debug(F("ESP8266_GENERIC"));
#endif
#ifdef ESP8266_ESP01
    Debug(F("ESP8266_ESP01"));
#endif
#ifdef ESP8266_ESP12
    Debug(F("ESP8266_ESP12"));
#endif
  Debug(F("]\r\n                  SSID ["));  Debug( WiFi.SSID() );
#ifdef SHOW_PASSWRDS
  Debug(F("]\r\n               PSK key ["));  Debug( WiFi.psk() );
#else
  Debug(F("]\r\n               PSK key [**********"));
#endif
  Debug(F("]\r\n            IP Address ["));  Debug( WiFi.localIP().toString() );
  Debug(F("]\r\n              Hostname ["));  Debug( settingHostname );
  Debug(F("]\r\n     Last reset reason ["));  Debug( ESP.getResetReason() );
  Debug(F("]\r\n                upTime ["));  Debug( upTime() );
  Debugln(F("]\r"));

#ifdef USE_MQTT
  Debugln(F("==================================================================\r"));
  Debug(F(" \r\n           MQTT broker ["));  Debug( settingMQTTbroker );
  Debug(F("]\r\n             MQTT User ["));  Debug( settingMQTTuser );
  #ifdef SHOW_PASSWRDS
    Debug(F("]\r\n         MQTT PassWord ["));  Debug( settingMQTTpasswd );
  #else
    Debug(F("]\r\n         MQTT PassWord [**********"));
  #endif
  Debug(F("]\r\n             Top Topic ["));  Debug(settingMQTTtopTopic );
  Debug(F("]\r\n       Update Interval ["));  Debug(settingMQTTinterval);
  Debugln(F("]\r"));
  Debugln(F("==================================================================\r\n\r"));
#endif

} // displayBoardInfo()



//===========================================================================================
void handleKeyInput() 
{
  char    inChar;

  while (TelnetStream.available() > 0) 
  {
    yield();
    inChar = (char)TelnetStream.read();
    
    switch(inChar) {
      case 'b':
      case 'B':     displayBoardInfo();
                    break;
      case 'l':
      case 'L':     readSettings(true);
                    break;
      case 'd':
      case 'D':     displayDaysHist(true);
                    break;
      case 'E':     eraseFile();
                    break;
#if defined(HAS_NO_SLIMMEMETER)
      case 'F':     forceBuildRingFiles = true;
                    runMode = SInit;
                    break;
#endif
      case 'h':
      case 'H':     displayHoursHist(true);
                    break;
      case 'm':
      case 'M':     displayMonthsHist(true);
                    break;
                    
      case 'W':     Debugf("\r\nConnect to AP [%s] and go to ip address shown in the AP-name\r\n", settingHostname);
                    delay(1000);
                    WiFi.disconnect(true);  // deletes credentials !
                    //setupWiFi(true);
                    delay(2000);
                    ESP.reset();
                    delay(2000);
                    break;
      case 'i':
      case 'I':     for(int I=0; I<10; I++) 
                    {
                      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
                      delay(1000);
                    }
                    break;
#ifdef USE_MINDERGAS
      case 't':
      case 'T':     forceMindergasUpdate();  //skip waiting for (midnight||countdown) 
                    break;
#endif
      case 'p':
      case 'P':     showRaw = !showRaw;
                 #ifdef USE_REQUEST_PIN
                    if (showRaw)  digitalWrite(DTR_ENABLE, HIGH);
                    else          digitalWrite(DTR_ENABLE, LOW);
                 #endif
                    showRawCount = 0;
                    break;
      case 'R':     DebugT(F("Reboot in 3 seconds ... \r\n"));
                    DebugFlush();
                    delay(3000);
                    DebugTln(F("now Rebooting. \r"));
                    DebugFlush();
                    ESP.reset();
                    break;
      case 's':
      case 'S':     listSPIFFS();
                    break;
      case 'v':
      case 'V':     if (Verbose2) 
                    {
                      Debugln(F("Verbose is OFF\r"));
                      Verbose1 = false;
                      Verbose2 = false;
                    } 
                    else if (Verbose1) 
                    {
                      Debugln(F("Verbose Level 2 is ON\r"));
                      Verbose2 = true;
                    } 
                    else 
                    {
                      Debugln(F("Verbose Level 1 is ON\r"));
                      Verbose1 = true;
                      Verbose2 = false;
                    }
                    break;
#ifdef USE_SYSLOGGER
      case 'q':
      case 'Q':     sysLog.setDebugLvl(0);
                    sysLog.dumpLogFile();
                    sysLog.setDebugLvl(1);
                    break;
#endif
      case 'Z':     slotErrors      = 0;
                    nrReboots       = 0;
                    telegramCount   = 0;
                    telegramErrors  = 0;
                    writeLastStatus();
                    #ifdef USE_SYSLOGGER
                      sysLog = {};
                      openSysLog(true);
                    #endif
                    break;
      default:      Debugln(F("\r\nCommands are:\r\n"));
                    Debugln(F("   B - Board Info\r"));
                    Debugln(F("  *E - erase file from SPIFFS\r"));
                    Debugln(F("   L - list Settings\r"));
                    Debugln(F("   D - Display Day table from SPIFFS\r"));
                    Debugln(F("   H - Display Hour table from SPIFFS\r"));
                    Debugln(F("   M - Display Month table from SPIFFS\r"));
                  #if defined(HAS_NO_SLIMMEMETER)
                    Debugln(F("  *F - Force build RING files\r"));
                  #endif
                    Debugf ("   I - Identify by blinking LED on GPIO[%02d]\r\n", LED_BUILTIN);
                    if (showRaw) 
                    {
                      Debugln(F("   P - Start Parsing again\r"));
                    } 
                    else 
                    {
                      Debugln(F("   P - No Parsing (show RAW data from Smart Meter)\r"));
                      showRawCount = 0;
                    }
                    Debugln(F("  *W - Force Re-Config WiFi\r"));
#ifdef USE_SYSLOGGER
                    Debugln(F("   Q - dump sysLog file\r"));
#endif
                    Debugln(F("  *R - Reboot\r"));
                    Debugln(F("   S - File info on SPIFFS\r"));
                    Debugln(F("  *U - Update SPIFFS (save Data-files)\r"));
                    Debugln(F("  *Z - Zero counters\r\n"));
                    if (Verbose1 & Verbose2)  Debugln(F("   V - Toggle Verbose Off\r"));
                    else if (Verbose1)        Debugln(F("   V - Toggle Verbose 2\r"));
                    else                      Debugln(F("   V - Toggle Verbose 1\r"));
                    #ifdef USE_MINDERGAS
                    Debugln(F("   T - Force update mindergas.nl\r"));
                    #endif

    } // switch()
    while (TelnetStream.available() > 0) 
    {
       yield();
       (char)TelnetStream.read();
    }
  }
  
} // handleKeyInput()


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
