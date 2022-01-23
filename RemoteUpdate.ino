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
  LogFile("Update geslaagd",true);
}

void update_started() {
  LogFile("Firmware update gestart",true);
  httpServer.send(200, "text/html", "Firmware update gestart...");
}

void update_progress(int cur, int total) {
  Debugf("HTTP update process at %d of %d bytes...\r", cur, total);
}

void update_error(int err) {
  Debugf("HTTP update fatal error code %d | %s\n", err, ESPhttpUpdate.getLastErrorString().c_str());
  LogFile("Firmware update ERROR",true);
  httpServer.send(200, "text/html", "Firmware update ERROR: " + err);
}

//---------------
void RemoteUpdate(){
  RemoteUpdate("", true);
}

//---------------
void RemoteUpdate(const char* versie, bool sketch){
/*
 * nodig bij de update:
 * - Flashsize
 * - versienummer + land 
 * voorbeeld aanroep : /remote-update?version=3.0.4
 * voorbeeld : invoer 2.3.7BE -> DMSR-API-V2.3.7BE_<FLASHSIZE>Mb.bin.gz
 * voorbeeld : invoer 2.3.7 -> DMSR-API-V2.3.7_<FLASHSIZE>Mb.bin.gz
 */
 
  int flashSize = (ESP.getFlashChipRealSize() / 1024.0 / 1024.0);
  String otaFile, _versie, path;
  t_httpUpdate_return ret;
  WiFiClient client;

  Debugln(F("\n!!! OTA UPDATE !!!"));
  Debugln(sketch ? "Update type: Sketch" : "Update type: File"); 
  
  if (strlen(versie) == 0) _versie = httpServer.arg(0);
  else _versie = versie;
  if ((httpServer.argName(0) == "version") || strlen(versie) > 0) {
    // Add optional callback notifiers
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);

    otaFile = "DSMR-API-V" + _versie + "_" + flashSize; 
    otaFile+= sketch ? "Mb.bin" : "Mb.spiffs.bin";
    path = String(BaseOTAurl) + otaFile;
    Debugf("OTA versie %s | flashsize %i Mb\n", _versie.c_str(), flashSize);
    Debugln("OTA path: " + path);
        
      //start update proces
      if (sketch) ret = ESPhttpUpdate.update(client,path);
      else ret = ESPhttpUpdate.updateFS(client,path);
      
    } else {
      //onjuist argument in url aanroep
      httpServer.send(200, "text/html", "ERROR: Update Failed - No version argument");
      LogFile("Remote update ERROR: Update Failed - No version argument",false );
    }
  UpdateRequested = false;
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
