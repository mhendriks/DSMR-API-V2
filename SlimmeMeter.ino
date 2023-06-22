/*
***************************************************************************  
**  Program  : SlimmeMeter - part of DSMRloggerAPI
**  Version  : v3.0.0
**
**  Copyright (c) 2021 Willem Aandewiel / Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/  

void PushDSMRValues(){
//BE
DSMRdata.identification = "FLU5=769484_A";
DSMRdata.p1_version_be = "50217";
//DSMRdata.peak_pwr_last_q_value = 0;
//DSMRdata.peak_pwr_last_q_value_unit = "kW";
//DSMRdata.highest_peak_pwr_value = 0.93*1000;
//DSMRdata.highest_peak_pwr_unit = "kW";
//DSMRdata.highest_peak_pwr_13mnd = "(3)(1-0:1.6.0)(1-0:1.6.0)(230101000000W)(221222134500W)(04.963*kW)(230201000000W)(230113004500W)(02.923*kW)(230301000000W)(230219183000W)(02.322*kW)";
DSMRdata.timestamp = "230310180022W";
DSMRdata.equipment_id = "3153414733313035303734323233";
DSMRdata.energy_delivered_tariff1._value = 355812;
DSMRdata.energy_delivered_tariff1::unit() = "kWh";
DSMRdata.energy_delivered_tariff2._value = 475691;
//DSMRdata.energy_delivered_tariff2_unit = "kWh";
DSMRdata.energy_returned_tariff1._value = 1621923;
//DSMRdata.energy_returned_tariff1_unit = "kWh";
DSMRdata.energy_returned_tariff2._value = 515293;
//DSMRdata.energy_returned_tariff2_unit = "kWh";
DSMRdata.electricity_tariff = "0001";
//DSMRdata.power_delivered_value = 0;
//DSMRdata.power_delivered_unit = "kW";
//DSMRdata.power_returned_value = 0.023 *1000;
//DSMRdata.power_returned_unit = "kW";
//DSMRdata.electricity_threshold = 999.9;
//DSMRdata.electricity_threshold_unit = "kW";
//DSMRdata.electricity_switch_position = 1;
//DSMRdata.message_long = "";
//DSMRdata.voltage_l1_value = 236.5*1000;
//DSMRdata.voltage_l1_unit = "V";
//DSMRdata.voltage_l2_value = 237*1000;
//DSMRdata.voltage_l2_unit = "V";
//DSMRdata.voltage_l3_value = 237.2*1000;
//DSMRdata.voltage_l3_unit = "V";
//DSMRdata.current_l1_value = 1000*1.15;
//DSMRdata.current_l1_unit = "A";
//DSMRdata.current_l2_value = 1000*0.37;
//DSMRdata.current_l2_unit = "A";
//DSMRdata.current_l3_value = 0.37 * 1000;
//DSMRdata.current_l3_unit = "A";
//DSMRdata.power_delivered_l1._value = 0.083*1000;
//DSMRdata.power_delivered_l1_unit = "kW";
//DSMRdata.power_delivered_l2._value = 0;
//DSMRdata.power_delivered_l2_unit = "kW";
//DSMRdata.power_delivered_l3._value = 0;
//DSMRdata.power_delivered_l3_unit = "kW";
//DSMRdata.power_returned_l1._value = 0;
//DSMRdata.power_returned_l1_unit = "kW";
//DSMRdata.power_returned_l2._value = (int)0.052*1000;
//DSMRdata.power_returned_l2_unit = "kW";
//DSMRdata.power_returned_l3._value = 0.053*1000;
//DSMRdata.power_returned_l3_unit = "kW";
//DSMRdata.mbus1_device_type = 3;
//DSMRdata.mbus1_equipment_id_ntc = "374D495432323030303531323138";
//DSMRdata.mbus1_valve_position = 1;
//DSMRdata.mbus1_delivered_ntc = "728.47";
//DSMRdata.mbus1_delivered_ntc_unit = "m3";


}

struct showValues {
  template<typename Item>
  void apply(Item &i) {
    TelnetStream.print(F("showValues: "));
    if (i.present()) 
    {
      TelnetStream.print(Item::name);
      TelnetStream.print(F(": "));
      TelnetStream.print(i.val());
      TelnetStream.print(Item::unit());
    //} else 
    //{
    //  TelnetStream.print(F("<no value>"));
    }
    TelnetStream.println();
  }
};

//==================================================================================
void handleSlimmemeter()
{
  //DebugTf("showRaw (%s)\r\n", showRaw ?"true":"false");
  if (slimmeMeter.available()) {
    
    if (LEDenabled) digitalWrite(LED, !digitalRead(LED)); //toggle LED when telegram available

    if (showRaw || JsonRaw) {
      //-- process telegrams in raw mode
      Debugf("Telegram Raw (%d)\n/%s!%x\n", slimmeMeter.raw().length(), slimmeMeter.raw().c_str(), slimmeMeter.GetCRC() ); 
      if (JsonRaw) sendJsonBuffer(String("/" + slimmeMeter.raw() + "!" + String(slimmeMeter.GetCRC(),HEX)).c_str());
      showRaw = false; //only 1 reading
      JsonRaw = false;
    } 
    else processSlimmemeter();
    if (LEDenabled) digitalWrite(LED, !digitalRead(LED)); //toggle LED when telegram available
  } //available
} // handleSlimmemeter()

//==================================================================================
void processSlimmemeter()
{
    telegramCount++;
    
    // Voorbeeld: [21:00:11][   9880/  8960] loop        ( 997): read telegram [28] => [140307210001S]
    Debugln(F("\r\n[Time----][FreeHeap/mBlck][Function----(line):\r"));
    DebugTf("telegramCount=[%d] telegramErrors=[%d] bufferlength=[%d]\r\n", telegramCount, telegramErrors,slimmeMeter.raw().length());
        
    DSMRdata = {};
    String    DSMRerror;
        
    if (slimmeMeter.parse(&DSMRdata, &DSMRerror))   // Parse succesful, print result
    {
      if (DSMRdata.identification_present)
      {
        //--- this is a hack! The identification can have a backslash in it
        //--- that will ruin javascript processing :-(
        for(int i=0; i<DSMRdata.identification.length(); i++)
        {
          if (DSMRdata.identification[i] == '\\') DSMRdata.identification[i] = '=';
          yield();
        }
      }
      PushDSMRValues();

      if (DSMRdata.p1_version_be_present)
      {
        DSMRdata.p1_version = DSMRdata.p1_version_be;
        DSMRdata.p1_version_be_present  = false;
        DSMRdata.p1_version_present     = true;
        DSMR_NL = false;
      }

      modifySmFaseInfo();

      //-- handle mbus delivered values
      gasDelivered = modifyMbusDelivered();
      
      processTelegram();
      if (Verbose2) DSMRdata.applyEach(showValues());
    } 
    else                  // Parser error, print error
    {
      telegramErrors++;
      DebugTf("Parse error\r\n%s\r\n\r\n", DSMRerror.c_str());
      slimmeMeter.loop(); //empty buffer
    }
  
} // handleSlimmeMeter()

//==================================================================================
void processTelegram()
{
  DebugTf("Telegram[%d]=>DSMRdata.timestamp[%s]\r\n", telegramCount, DSMRdata.timestamp.c_str());  
                                                    
  strcpy(newTimestamp, DSMRdata.timestamp.c_str()); 

  newT = epoch(newTimestamp, strlen(newTimestamp), true); // update system time
  actT = epoch(actTimestamp, strlen(actTimestamp), false);
  
  // Skip first 3 telegrams .. just to settle down a bit ;-)
  if ((int32_t)(telegramCount - telegramErrors) < 3) {
    strCopy(actTimestamp, sizeof(actTimestamp), newTimestamp);
    actT = epoch(actTimestamp, strlen(actTimestamp), false);   // update system time
    return;
  }
  
  if (Verbose1) DebugTf("actHour[%02d] -- newHour[%02d]\r\n", hour(actT), hour(newT));
  
  // has the hour changed (or the day or month)  
  if (     (hour(actT) != hour(newT)  ) 
//       ||   (day(actT) != day(newT)   ) 
//       || (month(actT) != month(newT) ) 
    )
  {
    writeRingFiles();
  }
  yield();
  if ( DUE(publishMQTTtimer) ) sendMQTTData();  
  
  strCopy(actTimestamp, sizeof(actTimestamp), newTimestamp); //nu pas updaten na het schrijven van de data anders in tijdslot - 1
  actT = epoch(actTimestamp, strlen(actTimestamp), true);   // update system time
} // processTelegram()

//==================================================================================
void modifySmFaseInfo()
{
  if (!settingSmHasFaseInfo)
  {
        if (DSMRdata.power_delivered_present && !DSMRdata.power_delivered_l1_present)
        {
          DSMRdata.power_delivered_l1 = DSMRdata.power_delivered;
          DSMRdata.power_delivered_l1_present = true;
          DSMRdata.power_delivered_l2_present = true;
          DSMRdata.power_delivered_l3_present = true;
        }
        if (DSMRdata.power_returned_present && !DSMRdata.power_returned_l1_present)
        {
          DSMRdata.power_returned_l1 = DSMRdata.power_returned;
          DSMRdata.power_returned_l1_present = true;
          DSMRdata.power_returned_l2_present = true;
          DSMRdata.power_returned_l3_present = true;
        }
  } // No Fase Info
  
} //  modifySmFaseInfo()


//==================================================================================
float modifyMbusDelivered()
{
  float tmpGasDelivered = 0;
  
  if ( DSMRdata.mbus1_device_type == 3 )  { //gasmeter
    if (DSMRdata.mbus1_delivered_ntc_present) 
        DSMRdata.mbus1_delivered = DSMRdata.mbus1_delivered_ntc;
  else if (DSMRdata.mbus1_delivered_dbl_present) 
        DSMRdata.mbus1_delivered = DSMRdata.mbus1_delivered_dbl;
  DSMRdata.mbus1_delivered_present     = true;
  DSMRdata.mbus1_delivered_ntc_present = false;
  DSMRdata.mbus1_delivered_dbl_present = false;
//  if (settingMbus1Type > 0) DebugTf("mbus1_delivered [%.3f]\r\n", (float)DSMRdata.mbus1_delivered);
//  if ( (settingMbus1Type == 3) && (DSMRdata.mbus1_device_type == 3) )
  
    tmpGasDelivered = (float)(DSMRdata.mbus1_delivered * 1.0);
//    DebugTf("gasDelivered .. [%.3f]\r\n", tmpGasDelivered);
    mbusGas = 1;
  }
  
  if ( DSMRdata.mbus2_device_type == 3 ){ //gasmeter
    if (DSMRdata.mbus2_delivered_ntc_present) DSMRdata.mbus2_delivered = DSMRdata.mbus2_delivered_ntc;
    else if (DSMRdata.mbus2_delivered_dbl_present) DSMRdata.mbus2_delivered = DSMRdata.mbus2_delivered_dbl;
    DSMRdata.mbus2_delivered_present     = true;
    DSMRdata.mbus2_delivered_ntc_present = false;
    DSMRdata.mbus2_delivered_dbl_present = false;
  //  if (settingMbus2Type > 0) DebugTf("mbus2_delivered [%.3f]\r\n", (float)DSMRdata.mbus2_delivered);
  //  if ( (settingMbus2Type == 3) && (DSMRdata.mbus2_device_type == 3) )
      tmpGasDelivered = (float)(DSMRdata.mbus2_delivered * 1.0);
  //    DebugTf("gasDelivered .. [%.3f]\r\n", tmpGasDelivered);
    mbusGas = 2;
  }

  if ( (DSMRdata.mbus3_device_type == 3) ){ //gasmeter
    if (DSMRdata.mbus3_delivered_ntc_present) DSMRdata.mbus3_delivered = DSMRdata.mbus3_delivered_ntc;
    else if (DSMRdata.mbus3_delivered_dbl_present) DSMRdata.mbus3_delivered = DSMRdata.mbus3_delivered_dbl;
    DSMRdata.mbus3_delivered_present     = true;
    DSMRdata.mbus3_delivered_ntc_present = false;
    DSMRdata.mbus3_delivered_dbl_present = false;
  //  if (settingMbus3Type > 0) DebugTf("mbus3_delivered [%.3f]\r\n", (float)DSMRdata.mbus3_delivered);
  //  if ( (settingMbus3Type == 3) && (DSMRdata.mbus3_device_type == 3) )
      tmpGasDelivered = (float)(DSMRdata.mbus3_delivered * 1.0);
  //    DebugTf("gasDelivered .. [%.3f]\r\n", tmpGasDelivered);
    mbusGas = 3;
  }

  if ( (DSMRdata.mbus4_device_type == 3) ){ //gasmeter
    if (DSMRdata.mbus4_delivered_ntc_present) DSMRdata.mbus4_delivered = DSMRdata.mbus4_delivered_ntc;
    else if (DSMRdata.mbus4_delivered_dbl_present) DSMRdata.mbus4_delivered = DSMRdata.mbus4_delivered_dbl;
    DSMRdata.mbus4_delivered_present     = true;
    DSMRdata.mbus4_delivered_ntc_present = false;
    DSMRdata.mbus4_delivered_dbl_present = false;
  //  if (settingMbus4Type > 0) DebugTf("mbus4_delivered [%.3f]\r\n", (float)DSMRdata.mbus4_delivered);
  //  if ( (settingMbus4Type == 3) && (DSMRdata.mbus4_device_type == 3) )
      tmpGasDelivered = (float)(DSMRdata.mbus4_delivered * 1.0);
  //    DebugTf("gasDelivered .. [%.3f]\r\n", tmpGasDelivered);
    mbusGas = 4;
  }

  return tmpGasDelivered;
    
} //  modifyMbusDelivered()


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
