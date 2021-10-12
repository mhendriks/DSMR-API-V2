/* 
***************************************************************************  
**  Program  : RingFiles, part of DSMRloggerAPI
**  Version  : v3.0.0
**
**  Copyright (c) 2021 Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

void createRingFile(E_ringfiletype ringfiletype) 
{
  if (!FSmounted) return;
  File RingFile = LittleFS.open(RingFiles[ringfiletype].filename, "w"); // open for writing  
  if (!RingFile) {
    DebugT(F("open ring file FAILED!!! --> Bailout\r\n"));Debugln(RingFiles[ringfiletype].filename);
    return;
  }
  //write json to ring file
  DebugT(F("createFile: ")); Debugln(RingFiles[ringfiletype].filename);
    
  //fill file with 0 values
  RingFile.print("{\"actSlot\": 0,\"data\":[\n"); //start the json file 
  for (uint8_t slot=0; slot < RingFiles[ringfiletype].slots; slot++ ) 
  { 
    RingFile.print("{\"date\":\"21000000\",\"values\":[     0.000,     0.000,     0.000,     0.000,     0.000]}"); // one empty record
   if (slot < (RingFiles[ringfiletype].slots - 1) ) RingFile.print(",\n");

  }
  RingFile.print("\n]}"); //terminate the json string
  RingFile.close();
}

uint8_t CalcSlot(E_ringfiletype ringfiletype, const char* Timestamp) 
{
  //slot positie bepalen
  uint32_t  nr=0;
  //time_t    t1 = epoch((char*)actTimestamp, strlen(actTimestamp), false);
  time_t    t1 = epoch(Timestamp, strlen(Timestamp), false);
  if (ringfiletype == RINGMONTHS ) nr = ( (year(t1) -1) * 12) + month(t1);    // eg: year(2023) * 12 = 24276 + month(9) = 202309
  else nr = t1 / RingFiles[ringfiletype].seconds;
  uint8_t slot = nr % RingFiles[ringfiletype].slots;
//  DebugTf("slot: [%d], nr: [%d]\n", slot, nr);

  if (slot < 0 || slot >= RingFiles[ringfiletype].slots)
  {
    DebugT(F("RINGFile: Some serious error! Slot is: ")); Debugln(slot);
//    slot = RingFiles[ringfiletype].slots;
    P1Status.sloterrors++;
    return 99;
  }
  return slot;
}

//===========================================================================================

void createRingFile(const char* filename) 
{
  if (strcmp(filename,"/RINGhours.json")==0) createRingFile(RINGHOURS);
  else if (strcmp(filename,"/RINGdays.json")==0) createRingFile(RINGDAYS);
  else if (strcmp(filename,"/RINGmonths.json")==0) createRingFile(RINGMONTHS);
  }

//===========================================================================================

void RingFileTo(E_ringfiletype ringfiletype, bool toFile) 
{  
  if (bailout() || !FSmounted) return; //exit when heapsize is too small

  if (!LittleFS.exists(RingFiles[ringfiletype].filename))
  {
    DebugT(F("read(): Ringfile doesn't exist: "));Debugln(RingFiles[ringfiletype].filename);
    createRingFile(ringfiletype);
    return;
    }

  File RingFile = LittleFS.open(RingFiles[ringfiletype].filename, "r"); // open for reading

  if (toFile) {
      DebugTln(F("http: json sent .."));
      httpServer.sendHeader("Access-Control-Allow-Origin", "*");
      httpServer.streamFile(RingFile, "application/json"); 
  } else {
      DebugT(F("Ringfile output: "));
      while (RingFile.available()) //read the content and output to serial interface
      { 
        //Serial.write(RingFile.read());
        TelnetStream.write(RingFile.read());
      }
      Debugln();
  }
    
  RingFile.close();
} //RingFileTo

//===========================================================================================
void writeRingFile(E_ringfiletype ringfiletype,const char *JsonRec) 
{
  char key[9] = "";
  byte slot = 0;
  uint8_t actSlot = CalcSlot(ringfiletype, actTimestamp);
  if (actSlot == 99) return;  // stop if error occured
  StaticJsonDocument<145> rec;
  if (!FSmounted) return;
  
  char buffer[DATA_RECLEN];
  if (strlen(JsonRec) > 1) {
    DebugTln(JsonRec);
    DeserializationError error = deserializeJson(rec, JsonRec);
    if (error) {
      DebugT(F("convert:Failed to deserialize RECORD: "));Debugln(error.c_str());
      httpServer.send(500, "application/json", httpServer.arg(0));
      return;
    } 
  }

  //json openen
  DebugT(F("read(): Ring file ")); Debugln(RingFiles[ringfiletype].filename);
  
  File RingFile = LittleFS.open(RingFiles[ringfiletype].filename, "r+"); // open for reading  
  if (!RingFile) {
    DebugT(F("open ring file FAILED!!! --> Bailout\r\n"));
    Debugln(RingFiles[ringfiletype].filename);
    return;
  }

  // add/replace new actslot to json object
  snprintf(buffer, sizeof(buffer), "{\"actSlot\":%2d,\"data\":[\n", actSlot);
  RingFile.print(buffer);
  
  if (strlen(JsonRec) > 1) {
    //write data from onerecord
    strncpy(key, rec["recid"], 8); 
    slot = CalcSlot(ringfiletype, key);
//    DebugTln("slot from rec: "+slot);
//    DebugT(F("update date: "));Debugln(key);

    //create record
    snprintf(buffer, sizeof(buffer), (char*)DATA_FORMAT, key , (float)rec["edt1"], (float)rec["edt2"], (float)rec["ert1"], (float)rec["ert2"], (float)rec["gdt"]);
    httpServer.send(200, "application/json", httpServer.arg(0));
  } else {
    //write actual data
    strncpy(key, actTimestamp, 8);  
    slot = actSlot;
//    DebugTln("actslot: "+actSlot);
//    DebugT(F("update date: "));Debugln(key);
    //create record
    snprintf(buffer, sizeof(buffer), (char*)DATA_FORMAT, key , (float)DSMRdata.energy_delivered_tariff1, (float)DSMRdata.energy_delivered_tariff2, (float)DSMRdata.energy_returned_tariff1, (float)DSMRdata.energy_returned_tariff2, (float)gasDelivered);
  }
  //DebugT("update timeslot: ");Debugln(slot);
  //goto writing starting point  
  uint16_t offset = (slot * DATA_RECLEN) + JSON_HEADER_LEN;
  RingFile.seek(offset, SeekSet);   
 
  //overwrite record in file
  int32_t bytesWritten = RingFile.print(buffer);
  if (bytesWritten != (DATA_RECLEN - 2)) DebugTf("ERROR! slot[%02d]: written [%d] bytes but should have been [%d]\r\n", slot, bytesWritten, DATA_RECLEN);
  if ( slot < (RingFiles[ringfiletype].slots -1)) RingFile.print(",\n");
  else RingFile.print("\n"); // no comma at last record
  
  RingFile.close();
} // writeRingFile()

//===========================================================================================
void writeRingFiles() 
{
  if (!EnableHistory) return; //do nothing
  writeRingFile(RINGHOURS, "");
  writeRingFile(RINGDAYS, "");
  writeRingFile(RINGMONTHS, "");

} // writeRingFiles()

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
