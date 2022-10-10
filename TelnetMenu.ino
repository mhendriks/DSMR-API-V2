/* 
***************************************************************************  
**  Program  : TelnetMenu, part of DSMRloggerAPI
**  
**
**  Copyright (c) 2021 Willem Aandewiel / Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

void DisplayLogFile(const char *fname) { 
  if (bailout() || !FSmounted) return; //exit when heapsize is too small
  File file = FS.open(fname, "r"); // open for reading
  if (file)  {
    DebugTln(F("Ringfile output (telnet only): "));
    //read the content and output to serial interface
    while (file.available()) TelnetStream.println(file.readStringUntil('\n'));
    Debugln();    
  } else DebugT(F("LogFile doesn't exist: "));
  file.close();
} //displaylogfile

//--------------------------------

void ResetDataFiles() {
  FS.remove(RingFiles[0].filename);
  FS.remove(RingFiles[1].filename);
  FS.remove(RingFiles[2].filename);
  FS.remove("/P1_log.old");
  FS.remove("/P1.log");
  FS.remove("/Reboot.log");      //pre 3.1.1 
  FS.remove("/Reboot.old");      //pre 3.1.1  
  FS.remove("/DSMRstatus.json"); //pre 3.1.1 
  DebugTln(F("Datafiles are reset"));
}

//--------------------------------

void P1Update(bool sketch){
  char versie[30] = "";

  //clear buffer
  while (TelnetStream.available() > 0) { (char)TelnetStream.read(); yield(); }
  
  Debugln(F("\n/!\\ UPDATE MODULE /!\\"));
  Debugf("Geef %s versie op (bv. 3.1.1): ",sketch?"SKETCH":"FILE");
  TelnetStream.setTimeout(10000);
  TelnetStream.readBytesUntil('\n', versie, sizeof(versie)); 
  TelnetStream.setTimeout(1000);
  
  versie[strlen(versie)-1] = '\0'; //remove enter

  if (strlen(versie)>4) RemoteUpdate(versie,sketch); 
  else Debugln(F("Fout in versie opgave: formaat = x.x.x")); 
}

//--------------------------------
void displayBoardInfo() 
{
  FS.info(fs_info);

  Debugln(F("\r\n==================================================================\r"));
  Debug(F("]\r\n      Firmware Version ["));  Debug( _VERSION );
  Debug(F("]\r\n              Compiled ["));  Debug( __DATE__ "  "  __TIME__ );
  Debug(F("]\r\n              #defines "));   Debug(F(ALL_OPTIONS));
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

  if ((ESP.getFlashChipId() & 0x000000ff) == 0x85) snprintf(cMsg, sizeof(cMsg), "%08X (PUYA)", ESP.getFlashChipId());
  else  snprintf(cMsg, sizeof(cMsg), "%08X", ESP.getFlashChipId());

  Debug(F("]\r\n         Flash Chip ID ["));  Debug( cMsg );
  Debug(F("]\r\n  Flash Chip Size (kB) ["));  Debug( ESP.getFlashChipSize() / 1024 );
  Debug(F("]\r\n   Chip Real Size (kB) ["));  Debug( ESP.getFlashChipRealSize() / 1024 );
  Debug(F("]\r\n          FS Size (kB) ["));  Debug( fs_info.totalBytes / 1024 );

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
      case 'a':     
      case 'A':     {   char c;
                        while (TelnetStream.available() > 0) { 
                        c = (char)TelnetStream.read();
                        switch(c){
                        case 'a': P1StatusAvailable(); break;
                        case 'r': P1StatusRead(); break;
                        case 'w': P1StatusWrite(); break;
                        case 'p': P1StatusPrint(); break;
                        case 'x': ReadEepromBlock();break;
                        case 'z': P1StatusReset(); break;
                        default: Debugln(F("P1 Status info: \na = available\nr = read\nw = write\np = print\nx = EEPROM dump\nz = erase"));
                        } //switch
                        while (TelnetStream.available() > 0) {(char)TelnetStream.read();} //verwijder extra input
                      } //while
                      break; }
      case 'b':     
      case 'B':     displayBoardInfo();
                    break;
      case 'l':
      case 'L':     readSettings(true);
                    break;
      case 'd':
      case 'D':     RingFileTo(RINGDAYS, false);
                    break;
      case 'E':     eraseFile();
                    break;
      case 'f':
      case 'F':     listFS(false);
                    break;      
      case 'g':
      case 'G':     CHANGE_INTERVAL_MS(StatusTimer, 100); //force mqtt static + water publishing + P1Status write
                    break;      
      
      case 'h':
      case 'H':     RingFileTo(RINGHOURS, false);
                    break;
      case 'n':
      case 'N':     DisplayLogFile("P1.log");
                    break;
      case 'm':
      case 'M':     RingFileTo(RINGMONTHS, false);
                    break;
                    
      case 'W':     Debugf("\r\nConnect to AP [%s] and go to ip address shown in the AP-name\r\n", settingHostname);
                    delay(1000);
                    WiFi.disconnect(true);  // deletes credentials !
                    //setupWiFi(true);
                    P1Reboot();
                    break;
      case 'p':
      case 'P':     showRaw = !showRaw;
                    break;
      case 'Q':     ResetDataFiles();
                    P1StatusClear();
                    resetWifi();  
                    break;              
      case 'R':     DebugFlush();
                    P1Reboot();
                    break;
                    
      case 's':
      case 'S':     P1Update(false);
                    break;
                    
      case 'u': 
      case 'U':     P1Update(true);
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
      case 'x':     
      case 'X':     writeRingFiles();
                    break;
      case 'y':     
      case 'Y':     
                    CHANGE_INTERVAL_MS(StatusTimer, 10);
                    break;
      case 'Z':     P1StatusClear();
                    break;
      default:      Debugf("Dongle version %s | mac address %s\n\r",_VERSION, WiFi.macAddress().c_str());
                    Debugln(F("\r\nCommands are:"));
                    Debugln(F("   A - P1 Status info a=available|r=read|w=write|p=print|z=erase\r"));
                    Debugln(F("   B - Board Info\r"));
                    Debugln(F("  *E - erase file from File System\r"));
                    Debugln(F("   F - File info on SPIFFS\r"));
                    Debugln(F("   L - list Settings\r"));
                    Debugln(F("   D - Display Day table from FS\r"));
                    Debugln(F("   H - Display Hour table from FS\r"));
                    Debugln(F("   N - Display LogFile P1.log\r"));
                    Debugln(F("   M - Display Month table from FS\r"));
                    Debugln(F("   P - No Parsing (show RAW data - only 1 Telegram)\r"));
                    Debugln(F("  *W - Force Re-Config WiFi\r"));
                    Debugln(F("  *Q - Factory Reset\r"));
                    Debugln(F("  *R - Reboot\r"));
                    Debugln(F("   S - Update File Remote\r"));
                    Debugln(F("   U - Update Sketch Remote\r"));
                    Debugln(F("   X - Save Ringfiles now\r"));
                    Debugln(F("   Y - force Statusinfo & MQTT statics \r"));
                    Debugln(F("  *Z - Zero counters\r"));
                      Debug(F("   V - Toggle Verbose "));
                    if (Verbose1 & Verbose2)  Debugln(F("Off\r"));
                    else Verbose1 ? Debugln(F("2\r")) : Debugln(F("1\r"));
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
