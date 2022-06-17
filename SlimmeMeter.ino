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
      Debugf("Telegram Raw (%d)\n%s\n" , slimmeMeter.raw().length(),slimmeMeter.raw().c_str()); 
      if (JsonRaw) sendJsonBuffer(slimmeMeter.raw().c_str());
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
//  if (slimmeMeter.available()) 
//  {

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
      //--- set DTR to get a new telegram as soon as possible
        slimmeMeter.disable(); //empty buffer
        slimmeMeter.clear(); //empty buffer
//        slimmeMeter.enable(true);
//        slimmeMeter.loop();
        
    }
  
} // handleSlimmeMeter()

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
