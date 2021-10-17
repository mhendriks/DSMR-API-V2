 /* 
***************************************************************************  
**  Program  : FSexplorer, part of DSMRloggerAPI
**  Version  : v2.3.1
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

//=====================================================================================
void setupFSexplorer()    // Funktionsaufruf "spiffs();" muss im Setup eingebunden werden
{    
//  SPIFFS.begin(); //double
  httpServer.on("/api/listfiles", APIlistFiles);
  httpServer.on("/SPIFFSformat", formatSpiffs);
  httpServer.on("/upload", HTTP_POST, []() {}, handleFileUpload);
  httpServer.on("/ReBoot", reBootESP);
  httpServer.on("/update", updateFirmware);
  httpServer.on("/remote-update", [](){RemoteUpdate("");});
  httpServer.on("/ResetWifi", resetWifi);
  httpServer.onNotFound([]() 
  {
    if (Verbose2) DebugTf("in 'onNotFound()'!! [%s] => \r\n", String(httpServer.uri()).c_str());
    if (httpServer.uri().indexOf("/api/") == 0) 
    {
      if (Verbose1) DebugTf("next: processAPI(%s)\r\n", String(httpServer.uri()).c_str());
      processAPI();
    }
    else
    {
      DebugTf("next: handleFile(%s)\r\n"
                      , String(httpServer.urlDecode(httpServer.uri())).c_str());
      if (!handleFile(httpServer.urlDecode(httpServer.uri())))
      {
        httpServer.send(404, "text/plain", F("FileNotFound\r\n"));
      }
    }
  });
  
} // setupFSexplorer()

//=====================================================================================
void APIlistFiles()             // Senden aller Daten an den Client
{   
  FSInfo SPIFFSinfo;
  typedef struct _fileMeta {
    char    Name[30];     
    int32_t Size;
  } fileMeta;

  _fileMeta dirMap[30];
  int fileNr = 0;
  
  Dir dir = SPIFFS.openDir("/");         // List files on SPIFFS
  while (dir.next())  
  {
    dirMap[fileNr].Name[0] = '\0';
    strncat(dirMap[fileNr].Name, dir.fileName().substring(1).c_str(), 29); // remove leading '/'
    dirMap[fileNr].Size = dir.fileSize();
    fileNr++;
  }

  // -- bubble sort dirMap op .Name--
  for (int8_t y = 0; y < fileNr; y++) {
    yield();
    for (int8_t x = y + 1; x < fileNr; x++)  {
      //DebugTf("y[%d], x[%d] => seq[y][%s] / seq[x][%s] ", y, x, dirMap[y].Name, dirMap[x].Name);
      if (compare(String(dirMap[x].Name), String(dirMap[y].Name)))  
      {
        //Debug(" !switch!");
        fileMeta temp = dirMap[y];
        dirMap[y] = dirMap[x];
        dirMap[x] = temp;
      } /* end if */
      //Debugln();
    } /* end for */
  } /* end for */

  for (int8_t x = 0; x < fileNr; x++)  
  {
    DebugTln(dirMap[x].Name);
  }

  String temp = "[";
  for (int f=0; f < fileNr; f++)  
  {
    if (temp != "[") temp += ",";
    temp += R"({"name":")" + String(dirMap[f].Name) + R"(","size":")" + formatBytes(dirMap[f].Size) + R"("})";
  }
  SPIFFS.info(SPIFFSinfo);
  temp += R"(,{"usedBytes":")" + formatBytes(SPIFFSinfo.usedBytes * 1.05) + R"(",)" +       // Berechnet den verwendeten Speicherplatz + 5% Sicherheitsaufschlag
          R"("totalBytes":")" + formatBytes(SPIFFSinfo.totalBytes) + R"(","freeBytes":")" + // Zeigt die Größe des Speichers
          (SPIFFSinfo.totalBytes - (SPIFFSinfo.usedBytes * 1.05)) + R"("}])";               // Berechnet den freien Speicherplatz + 5% Sicherheitsaufschlag
  httpServer.send(200, "application/json", temp);
  
} // APIlistFiles()

//=====================================================================================
bool handleFile(String&& path) 
{
  if (httpServer.hasArg("delete")) 
  {
    DebugTf("Delete -> [%s]\n\r",  httpServer.arg("delete").c_str());
    SPIFFS.remove(httpServer.arg("delete"));    // Datei löschen
    httpServer.sendContent(Header);
    return true;
  }
  //if (!SPIFFS.exists("/FSexplorer.html")) httpServer.send(200, "text/html", Helper); //Upload the FSexplorer.html
  if (path.endsWith("/")) path += "index.html";
  return SPIFFS.exists(path) ? ({File f = SPIFFS.open(path, "r"); httpServer.streamFile(f, contentType(path)); f.close(); true;}) : false;

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
    fsUploadFile = SPIFFS.open("/" + httpServer.urlDecode(upload.filename), "w");
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
  }
  
} // handleFileUpload() 

//=====================================================================================
void formatSpiffs() 
{       //Formatiert den Speicher
  if (!SPIFFS.exists("/!format")) return;
  DebugTln(F("Format SPIFFS"));
  SPIFFS.format();
  httpServer.sendContent(Header);
  
} // formatSpiffs()

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
bool freeSpace(uint16_t const& printsize) 
{    
  FSInfo SPIFFSinfo;
  SPIFFS.info(SPIFFSinfo);
  Debugln(formatBytes(SPIFFSinfo.totalBytes - (SPIFFSinfo.usedBytes * 1.05)) + " in SPIFF free");
  return (SPIFFSinfo.totalBytes - (SPIFFSinfo.usedBytes * 1.05) > printsize) ? true : false;
  
} // freeSpace()

//=====================================================================================
void updateFirmware()
{
#ifdef USE_UPDATE_SERVER
  DebugTln(F("Redirect to updateIndex .."));
  doRedirect("Update", 1, "/updateIndex", false,false);
#else
  doRedirect("NoUpdateServer", 10, "/", false,false);
#endif
      
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
