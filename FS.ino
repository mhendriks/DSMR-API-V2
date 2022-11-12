/* 
***************************************************************************  
**  Program  : FSstuff, part of DSMRloggerAPI
**  Version  : v3.0.0
**
**  Copyright (c) 2021 Willem Aandewiel / Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
//===========================================================================================
int32_t freeSpace() 
{
   int32_t space;
  
  FS.info(fs_info);
  space = (int32_t)(fs_info.totalBytes - fs_info.usedBytes);
  return space;
  
} // freeSpace()

//===========================================================================================
void listFS(bool api) 
{
   typedef struct _fileMeta {
    char    Name[20];     
    int32_t Size;
  } fileMeta;

  _fileMeta dirMap[30];
  int fileNr = 0;
  
  Dir dir = FS.openDir("/");         // List files on FS
  while (dir.next())  
  {
    dirMap[fileNr].Name[0] = '\0';
    
#ifdef V2_COMPATIBLE
    strncat(dirMap[fileNr].Name, dir.fileName().substring(1).c_str(), 19); // remove leading '/' for SPIFFS
#else
    strcpy( dirMap[fileNr].Name, dir.fileName().c_str() ); //littlefs
#endif

    dirMap[fileNr].Size = dir.fileSize();
    fileNr++;
  }

  // -- bubble sort dirMap op .Name--
  for (int8_t y = 0; y < fileNr; y++) {
    yield();
    for (int8_t x = y + 1; x < fileNr; x++)  {
      //DebugTf("y[%d], x[%d] => seq[x][%s] ", y, x, dirMap[x].Name);
      if (compare(String(dirMap[x].Name), String(dirMap[y].Name)))  
      {
        fileMeta temp = dirMap[y];
        dirMap[y]     = dirMap[x];
        dirMap[x]     = temp;
      } /* end if */
      //Debugln();
    } /* end for */
  } /* end for */

  DebugTln(F("\r\n"));
  for(int f=0; f<fileNr; f++)
  {
    if (api) DebugTln(dirMap[f].Name);
    else Debugf("%-25s %6d bytes \r\n", dirMap[f].Name, dirMap[f].Size);

    yield();
  }

  if (api) {
    String temp = "[";
    for (int f=0; f < fileNr; f++)  
    {
      if (temp != "[") temp += ",";
      temp += R"({"name":")" + String(dirMap[f].Name) + R"(","size":")" + formatBytes(dirMap[f].Size) + R"("})";
    }
    FS.info(fs_info);
    temp += R"(,{"usedBytes":")" + formatBytes(fs_info.usedBytes * 1.05) + R"(",)" +       // Berechnet den verwendeten Speicherplatz + 5% Sicherheitsaufschlag
            R"("totalBytes":")" + formatBytes(fs_info.totalBytes) + R"(","freeBytes":")" + // Zeigt die Größe des Speichers
            (fs_info.totalBytes - (fs_info.usedBytes * 1.05)) + R"("}])";               // Berechnet den freien Speicherplatz + 5% Sicherheitsaufschlag
    
    httpServer.setContentLength(temp.length());
    httpServer.send(200, "application/json", temp);
  } else {
    
    FS.info(fs_info);
  
    Debugln(F("\r"));
    if (freeSpace() < (10 * fs_info.blockSize))
          Debugf("Available FS space [%6d]kB (LOW ON SPACE!!!)\r\n", (freeSpace() / 1024));
    else  Debugf("Available FS space [%6d]kB\r\n", (freeSpace() / 1024));
    Debugf("           FS Size [%6d]kB\r\n", (fs_info.totalBytes / 1024));
    Debugf("     FS block Size [%6d]bytes\r\n", fs_info.blockSize);
    Debugf("      FS page Size [%6d]bytes\r\n", fs_info.pageSize);
    Debugf(" FS max.Open Files [%6d]\r\n\r\n", fs_info.maxOpenFiles);
  }

} // listFS()


//===========================================================================================
void eraseFile() 
{
  char eName[30] = "";

  //--- erase buffer
  while (TelnetStream.available() > 0) 
  {
    yield();
    (char)TelnetStream.read();
  }

  Debug("Enter filename to erase: ");
  TelnetStream.setTimeout(10000);
  TelnetStream.readBytesUntil('\n', eName, sizeof(eName)); 
  TelnetStream.setTimeout(1000);

  //--- remove control chars like \r and \n ----
  //--- and shift all char's one to the right --
  for(int i=strlen(eName); i>0; i--) 
  {
    eName[i] = eName[i-1];
    if (eName[i] < ' ') eName[i] = '\0';
  }
  //--- add leading slash on position 0
  eName[0] = '/';

  if (FS.exists(eName))
  {
    Debugf("\r\nErasing [%s] from FS\r\n\n", eName);
    FS.remove(eName);
  }
  else
  {
    Debugf("\r\nfile [%s] not found..\r\n\n", eName);
  }
  //--- empty buffer ---
  while (TelnetStream.available() > 0) 
  {
    yield();
    (char)TelnetStream.read();
  }

} // eraseFile()


//===========================================================================================
bool DSMRfileExist(const char* fileName, bool doDisplay) 
{
  char fName[30] = "";
  if (fileName[0] != '/')
  {
    strConcat(fName, 5, "/");
  }
  strConcat(fName, 29, fileName);
  
  DebugTf("check if [%s] exists .. ", fName);
 

  if (!FS.exists(fName) )
  {
    if (doDisplay)
    {
      Debugln(F("NO! Error!!"));
      return false;
    }
    else
    {
      Debugln(F("NO! "));      
      return false;
    }
  } 
  else 
  {
    Debugln(F("Yes! OK!"));
    
  }
  return true;
  
} //  DSMRfileExist()


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
