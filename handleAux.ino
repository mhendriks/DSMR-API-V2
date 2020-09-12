/*
***************************************************************************  
**  Program  : DSMRloggerAPI.h - definitions for DSMRloggerAPI
**  Version  : v2.1.0
**
**  Copyright (c) 2020 Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/  

#ifdef USE_AUX
ICACHE_RAM_ATTR void isrAux() //isr routine 
{ 
  isrAux_cnt++;
}

void handleAux()
{
  if (isrAux_cnt > 2) // don't trigger on single pulse
  {
    char topicId[30];
    
    isrAux_cnt = 0;
    if (settingMQTTtopTopic[strlen(settingMQTTtopTopic)-1] == '/')
              snprintf(topicId, sizeof(topicId), "%s",  settingMQTTtopTopic);
    else  snprintf(topicId, sizeof(topicId), "%s/", settingMQTTtopTopic);
    strConcat(topicId, sizeof(topicId), "Aux");
        
    if (!MQTTclient.publish(topicId, "Signal") ) DebugTf("Error publish(%s) \r\n", topicId );
    else DebugT(F("Aux signal send\r\n"));    
    CHANGE_INTERVAL_MS(AuxTimer,  3000); //longer waiting time to prevent pushing out a lot of events
  }  else CHANGE_INTERVAL_MS(AuxTimer,  500); //normal waiting time
  RESTART_TIMER(AuxTimer);
}

#endif


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
