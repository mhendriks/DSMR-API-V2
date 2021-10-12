#define Eaddr   0
#define Esize  50
//EEPROM lengte = 3*4+2+13 = 27 postities

void P1StatusBegin(){ 
    EEPROM.begin(Esize);
    DebugTln(F("EEPROM succesfully initialised"));
  }
  
  void P1StatusPrint(){
      DebugTf("P1 Status: reboots[%i] | sloterrors [%i] | Timestamp [%s] | water [%i] m3 [%i] liter\n",P1Status.reboots,P1Status.sloterrors,P1Status.timestamp,P1Status.wtr_m3,P1Status.wtr_l);
  }
  
  void P1StatusRead(){
//    uint32_t msec = millis();
    EEPROM.get(Eaddr, P1Status);
    if (strlen(P1Status.timestamp)!=13) strcpy(P1Status.timestamp,"010101010101X"); 
    strcpy(actTimestamp, P1Status.timestamp);
//    DebugT("P1StatusRead duration: "); Debugln(millis() - msec);
    P1StatusPrint();
  }

  void P1StatusWrite(){
    strcpy(P1Status.timestamp, actTimestamp);
//    uint32_t msec = millis();

    EEPROM.put(Eaddr, P1Status);
//    DebugT("P1StatusWrite duration: "); Debugln(millis() - msec);

    if (EEPROM.commit()) DebugTln(F("P1Status successfully writen to EEPROM"));
    else DebugTln(F("ERROR! EEPROM commit failed"));      
  }

  void P1StatusReset(){
    for (int i = 0; i < Esize; i++) EEPROM.write(i, 255);
    if (EEPROM.commit()) DebugTln(F("P1Status successfully writen to EEPROM"));
    else DebugTln(F("ERROR! EEPROM commit failed"));
  }

  bool P1StatusAvailable(){
    int val;
    EEPROM.get(0,val); //check of reboots is a valid value
    DebugT("Status Data Available: ");Debugln(val);
    if (val == -1 ) {
      DebugTln("Persist data NOT available");
      return false;
    }
    else {
      DebugTln("Persist data IS available");
      return true;
    }
  }
  
void ReadEepromBlock(){
  byte value;
  for(byte address = 0; address < Esize; address++){
    value = EEPROM.read(address);
    Serial.print("EEPROM - " + address);
    Serial.print("\t");
    Serial.print(value, DEC);
    Serial.println();
  }
}

    
