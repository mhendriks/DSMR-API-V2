/*
***************************************************************************  
**  Program  : DSMRloggerAPI.h - definitions for DSMRloggerAPI
**  Version  : v2.1.2
**
**  Copyright (c) 2020 Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/  
// Convert 2.1.1 and older history files to 2.1.2+ version structure (fixed length files)

#ifdef HIST_CONV

void hist_conv(){
  DynamicJsonDocument doc(4500);
  char buffer[DATA_RECLEN];
  char filename[21];
  Debugln(F("CHECK OP NIEUWE DATAFILE FORMAAT"));
  for(byte i = 0;i < 3;i++) {
  //check if old data file type is present
  DebugT(F("Open file: "));Debugln(RingFiles[i].filename);
  File RingFile = SPIFFS.open(RingFiles[i].filename, "r"); // open for writing  
  if (!RingFile) {
    DebugT(F("open ring file FAILED!!! --> Bailout\r\n"));Debugln(RingFiles[i].filename);
    RingFile.close();
    continue;
  }
  if (RingFile.size() != (DATA_RECLEN*RingFiles[i].slots+JSON_HEADER_LEN + DATA_CLOSE - 1)) { //4287 = 0
    DebugT(F("Ander formaat file aangetroffen: "));Debugln(RingFile.size());
    //buffer old file
    DeserializationError error2 = deserializeJson(doc, RingFile);
    if (error2) {
      DebugT(F("Failed to deserialize RINGFILE: "));Debugln(RingFiles[i].filename);
      RingFile.close();
      continue;
    }
    // rename old data file
    RingFile.close();
    sprintf(filename, "%s%s", RingFiles[i].filename, "_old");
//    DebugT(F("Oude file verwijderen: "));Debugln(filename);
    if (SPIFFS.exists(filename)){
      //remove _old if existing 
      if (!SPIFFS.remove(filename)) { DebugT(F("remove file FAILED!!! --> Bailout\r\n")); return;}
    }
    if (!SPIFFS.rename(RingFiles[i].filename, filename)) { DebugT(F("rename file FAILED!!! --> Bailout\r\n")); return;}
    
    // convert to new data structure
    RingFile = SPIFFS.open(RingFiles[i].filename, "w+"); // open for writing  
    if (!RingFile) {
      DebugT(F("open ring file FAILED!!! --> Bailout\r\n"));Debugln(RingFiles[i].filename);
      RingFile.close();
      continue;
    }
    //write first row
    snprintf(buffer, sizeof(buffer), "{\"actSlot\":%2d,\"data\":[\n", (int)doc["actSlot"]);
//    DebugT(F("buffer header: "));Debugln(buffer);
    RingFile.print(buffer);
    
    for (byte slot=0; slot < RingFiles[i].slots; slot++) {
      snprintf(buffer, sizeof(buffer), (char*)DATA_FORMAT, doc["data"][slot]["date"].as<char*>(), doc["data"][slot]["values"][0].as<float>(), doc["data"][slot]["values"][1].as<float>(), doc["data"][slot]["values"][2].as<float>(), doc["data"][slot]["values"][3].as<float>(), doc["data"][slot]["values"][4].as<float>());
//      DebugT(F("buffer record: "));Debugln(buffer);
      RingFile.print(buffer);
      if ( slot < (RingFiles[i].slots -1)) RingFile.print(",\n");
      } //for
    RingFile.print("\n]}"); //terminate the json string
    DebugTln(F("Datafile succesvol geconverteerd"));
    }
  else DebugTln(F("Nieuwe datafile al aanwezig"));
  RingFile.close();
  } //for
}
#endif
