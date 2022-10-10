 /* 
***************************************************************************  
**  Program  : FSexplorer, part of DSMRloggerAPI
**  Version  : v3.0.0
**
**  Mostly stolen from https://www.arduinoforum.de/User-Fips
**  For more information visit: https://fipsok.de
**  See also https://www.arduinoforum.de/arduino-Thread-SPIFFS-DOWNLOAD-UPLOAD-DELETE-Esp8266-NodeMCU
**
***************************************************************************      
  Copyright (c) 2018 Jens Fleischer. All rights reserved.

  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This file is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*******************************************************************
*/

const PROGMEM char Header[] = "HTTP/1.1 303 OK\r\nLocation:/#FileExplorer\r\nCache-Control: no-cache\r\n";

void checkauth(){
  if( strlen(bAuthUser) && !httpServer.authenticate(bAuthUser, bAuthPW) ) {
    httpServer.sendHeader("Location", String("/login"), true);
    httpServer.send ( 302, "text/plain", "");
  }
}

void auth(){
//    httpServer.send(200, "text/html", UpdateHTML)
    if( strlen(bAuthUser) && !httpServer.authenticate(bAuthUser, bAuthPW) ) {
      return httpServer.requestAuthentication();
      httpServer.sendHeader("Location", String("/"), true);
      httpServer.send ( 302, "text/plain", "");
    }
}

//=====================================================================================
void setupFSexplorer()    // Funktionsaufruf "spiffs();" muss im Setup eingebunden werden
{    
  httpServer.on("/logout", HTTP_GET,  [](){ httpServer.send(401);           });
  httpServer.on("/login", HTTP_GET,   [](){ auth();                         });
  httpServer.on("/api/listfiles",     [](){ checkauth(); listFS(true);      });
  httpServer.on("/FSformat",          [](){ checkauth(); formatFS;          });
  httpServer.on("/upload", HTTP_POST, [](){ checkauth();                    }, handleFileUpload);
  httpServer.on("/ReBoot",            [](){ checkauth(); reBootESP();       });
  httpServer.on("/update",            [](){ checkauth(); updateFirmware();  }); //  < v3.5.0
  httpServer.on("/updates",           [](){ checkauth(); updateFirmware();  }); // >= v3.5.0
  httpServer.on("/remote-update",     [](){ checkauth(); RemoteUpdate();    });
  httpServer.on("/ResetWifi",         [](){ checkauth(); resetWifi();       });
  httpServer.on("/api", HTTP_GET,     [](){ checkauth(); processAPI;        }); // all other api calls are catched in FSexplorer onNotFounD!
  httpServer.onNotFound([]() 
  {
    checkauth();
    if (Verbose2) DebugTf("in 'onNotFound()'!! [%s] => \r\n", String(httpServer.uri()).c_str());
    if (httpServer.uri().indexOf("/api/") == 0) 
    {
      if (Verbose1) DebugTf("next: processAPI(%s)\r\n", String(httpServer.uri()).c_str());
      processAPI();
    }
    else
    {
      DebugTf("next: handleFile(%s)\r\n", String(httpServer.urlDecode(httpServer.uri())).c_str());
//      if((httpServer.uri().indexOf("/RING") == 0) && (!FS.exists(httpServer.uri().c_str()))) createRingFile(httpServer.uri().c_str());
        //oude index.js vraagt om RING
        String filename = httpServer.uri();
#ifndef V2_COMPATIBLE  
        if( httpServer.uri().indexOf("/RING") == 0 ) filename.replace("RING","RNG");
#endif
      //if (!handleFile(httpServer.urlDecode(httpServer.uri())))
      DebugT("Filename: ");Debugln(filename);
      if ( !handleFile(filename.c_str()) )
      {
        httpServer.send(404, "text/plain", F("FileNotFound\r\n"));
      }
    }
  });
  
  httpServer.begin();
  DebugTln( F("HTTP server gestart\r") );
  
} // setupFSexplorer()

//=====================================================================================
bool handleFile(String&& path) 
{
  if (httpServer.hasArg("delete")) 
  {
    DebugTf("Delete -> [%s]\n\r",  httpServer.arg("delete").c_str());
    FS.remove(httpServer.arg("delete"));    // Datei löschen
    httpServer.sendContent(Header);
    return true;
  }
  if (path.endsWith("/")) path += "index.html";
  return FS.exists(path) ? ({File f = FS.open(path, "r"); httpServer.streamFile(f, contentType(path)); f.close(); true;}) : false;

} // handleFile()

//=====================================================================================
void handleFileUpload() 
{
  static File fsUploadFile;
  HTTPUpload& upload = httpServer.upload();
  if (upload.status == UPLOAD_FILE_START) 
  {
    if (upload.filename.length() > 30) 
    {
      upload.filename = upload.filename.substring(upload.filename.length() - 30, upload.filename.length());  // Dateinamen auf 30 Zeichen kürzen
    }
    Debugln("FileUpload Name: " + upload.filename);
    fsUploadFile = FS.open("/" + httpServer.urlDecode(upload.filename), "w");
  } 
  else if (upload.status == UPLOAD_FILE_WRITE) 
  {
    Debugln("FileUpload Data: " + (String)upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } 
  else if (upload.status == UPLOAD_FILE_END) 
  {
    if (fsUploadFile)
      fsUploadFile.close();
    Debugln("FileUpload Size: " + (String)upload.totalSize);
    httpServer.sendContent(Header);
    if (upload.filename == "DSMRsettings.json") readSettings(false);
    
  }
  
} // handleFileUpload() 

//=====================================================================================
void formatFS() 
{       //Formatiert den Speicher
  if (!FS.exists("/!format")) return;
  DebugTln(F("Format FS"));
  FS.format();
  httpServer.sendContent(Header);
  
} // formatFS()
//=====================================================================================
const String formatBytes(size_t const& bytes) 
{ 
  return (bytes < 1024) ? String(bytes) + " Byte" : (bytes < (1024 * 1024)) ? String(bytes / 1024.0) + " KB" : String(bytes / 1024.0 / 1024.0) + " MB";

} //formatBytes()

//=====================================================================================
const String &contentType(String& filename) 
{       
  if (filename.endsWith(".htm") || filename.endsWith(".html")) filename = "text/html";
  else if (filename.endsWith(".css")) filename = "text/css";
  else if (filename.endsWith(".js")) filename = F("application/javascript");
  else if (filename.endsWith(".json")) filename = F("application/json");
  else if (filename.endsWith(".png")) filename = F("image/png");
  else if (filename.endsWith(".gif")) filename = F("image/gif");
  else if (filename.endsWith(".jpg")) filename = F("image/jpeg");
  else if (filename.endsWith(".ico")) filename = F("image/x-icon");
  else if (filename.endsWith(".xml")) filename = F("text/xml");
  else if (filename.endsWith(".pdf")) filename = F("application/x-pdf");
  else if (filename.endsWith(".zip")) filename = F("application/x-zip");
  else if (filename.endsWith(".gz")) filename = F("application/x-gzip");
  else filename = "text/plain";
  return filename;
  
} // &contentType()

//=====================================================================================
void updateFirmware()
{
  DebugTln(F("Redirect to updateIndex .."));
  doRedirect("Update", 1, "/updateIndex", false,false);
      
} // updateFirmware()

//=====================================================================================
void resetWifi()
{
  DebugTln(F("ResetWifi .."));
  doRedirect("RebootResetWifi", 45, "/", true, true);
}
//=====================================================================================
void reBootESP()
{
  DebugTln(F("Redirect and ReBoot .."));
  doRedirect("RebootOnly", 45, "/", true,false);

} // reBootESP()

//=====================================================================================
void doRedirect(String msg, int wait, const char* URL, bool reboot, bool resetWifi)
{ 
  DebugTln(msg);
  httpServer.sendHeader("Location", "/#Redirect?msg="+msg, true);
  httpServer.send ( 303, "text/plain", "");

  if (reboot) 
  {
    for( uint8_t i = 0; i < 100;i++) { 
      //handle redirect requests first before rebooting so client processing can take place
      delay(50); 
      httpServer.handleClient();
    }
    if (resetWifi) 
   {   WiFiManager manageWiFi;
       manageWiFi.resetSettings();
   }
    P1Reboot();
  }
  
} // doRedirect()
