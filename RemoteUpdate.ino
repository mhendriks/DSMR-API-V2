/* 
***************************************************************************  
**  Program  : processTelegram, part of DSMRloggerAPI
**  Version  : v3.0.0
**
**  Copyright (c) 2021 Willem Aandewiel / Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
#include <ESP8266httpUpdate.h>

void update_finished() {
  LogFile("Firmware update geslaagd");
}

void update_started() {
  LogFile("Firmware update gestart");
}

void update_progress(int cur, int total) {
  DebugTf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  DebugTf("CALLBACK:  HTTP update fatal error code %d\n", err);
  LogFile("Firmware update ERROR");  
}

//---------------
void RemoteUpdate(){
/*
 * nodig bij de update:
 * - Flashsize
 * - versienummer + land 
 * aanroep voorbeeld: /remote-update?version=3.0.4
 * voorbeeld : invoer 2.3.7BE -> DMSR-API-V2.3.7BE_<FLASHSIZE>Mb.bin.gz
 * voorbeeld : invoer 2.3.7 -> DMSR-API-V2.3.7_<FLASHSIZE>Mb.bin.gz
 *  strcpy(BaseOTAurl,"http://192.168.2.250/ota/");
    strcpy(BaseOTAurl,"http://raw.githubusercontent.com/mhendriks/DSMR-API-V2/master/ota/");
    https://raw.githubusercontent.com/mhendriks/DSMR-API-V2/master/ota//DSMR-API-V3.0.4_4Mb.bin.gz
    fingerprint sha-1: 70 94 DE DD E6 C4 69 48 3A 92 70 A1 48 56 78 2D 18 64 E0 B7
 */
  int flashSize = (ESP.getFlashChipRealSize() / 1024.0 / 1024.0);
  String versie  = httpServer.arg(0);
  String otaFile = "DSMR-API-V" + versie + "_" + flashSize + "Mb.bin.gz";
  if (httpServer.argName(0) == "version") {
    DebugTln("RemoteUpdate: versie " + versie + " | " + "flashsize " + flashSize + " Mb");
    DebugTln("Remote update url: " + (String)BaseOTAurl + otaFile  );
    LogFile("Remote update url: " + (String)BaseOTAurl + otaFile  );
    httpServer.send(200, "text/html", "Update request with filename : " + otaFile);
    
     // Add optional callback notifiers
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);
    
    //check of de file bestaat
    HTTPClient httpClient;
    if (strlen(otaFingerprint) == 0) httpClient.begin( BaseOTAurl + otaFile );
    else httpClient.begin( BaseOTAurl + otaFile, otaFingerprint );
    int httpCode = httpClient.GET();
    if( httpCode == 200 ) { 
      //start update proces
      DebugTln("OTA file found --> start update proces");
      if (strlen(otaFingerprint) == 0) t_httpUpdate_return ret = ESPhttpUpdate.update(BaseOTAurl + otaFile );
      else t_httpUpdate_return ret = ESPhttpUpdate.update(BaseOTAurl + otaFile, "", otaFingerprint );
    } else {
      DebugTln("Remote update ERROR: OTA file missing");
      LogFile("Remote update ERROR: OTA file missing");      
      }
    } else {
      //onjuist argument in url aanroep
      httpServer.send(200, "text/html", "ERROR: Update Failed - No version argument");
      LogFile("Remote update ERROR: Update Failed - No version argument" );
    }
  
} //RemoteUpdate



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
