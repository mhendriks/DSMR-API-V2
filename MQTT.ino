/* 
***************************************************************************  
**  Program  : MQTTstuff, part of DSMRloggerAPI
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel / Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
**  RvdB  changed MQTT stuff to FSM 
**  AaW   simplified and restructured the FSM 
*/

// Declare some variables within global scope

  static IPAddress  MQTTbrokerIP;
  static char       MQTTbrokerIPchar[20];

#ifdef USE_MQTT
//  #include <PubSubClient.h>           // MQTT client publish and subscribe functionality
  
//  static PubSubClient MQTTclient(wifiClient);
  int8_t              reconnectAttempts = 0;
  char                lastMQTTtimestamp[15] = "-";
  char                mqttBuff[100];


  enum states_of_MQTT { MQTT_STATE_INIT, MQTT_STATE_TRY_TO_CONNECT, MQTT_STATE_IS_CONNECTED, MQTT_STATE_ERROR };
  enum states_of_MQTT stateMQTT = MQTT_STATE_INIT;

  String            MQTTclientId;

#endif

//===========================================================================================
void connectMQTT() 
{
#ifdef USE_MQTT
  
  if (Verbose2) DebugTf("MQTTclient.connected(%d), mqttIsConnected[%d], stateMQTT [%d]\r\n"
                                              , MQTTclient.connected()
                                              , mqttIsConnected, stateMQTT);

  if (settingMQTTinterval == 0) {
    mqttIsConnected = false;
    return;
  }

  if (!MQTTclient.connected() || stateMQTT != MQTT_STATE_IS_CONNECTED)
  {
    mqttIsConnected = false;
    stateMQTT = MQTT_STATE_INIT;
  }

  mqttIsConnected = connectMQTT_FSM();
  
  if (Verbose1) DebugTf("connected()[%d], mqttIsConnected[%d], stateMQTT [%d]\r\n"
                                              , MQTTclient.connected()
                                              , mqttIsConnected, stateMQTT);

  CHANGE_INTERVAL_MIN(reconnectMQTTtimer, 5);

#endif
}

//===========================================================================================
bool connectMQTT_FSM() 
{
#ifdef USE_MQTT
  
  switch(stateMQTT) 
  {
    case MQTT_STATE_INIT:  
          DebugTln(F("MQTT State: MQTT Initializing"));
          WiFi.hostByName(settingMQTTbroker, MQTTbrokerIP);  // lookup the MQTTbroker convert to IP
          snprintf(MQTTbrokerIPchar, sizeof(MQTTbrokerIPchar), "%d.%d.%d.%d", MQTTbrokerIP[0]
                                                                            , MQTTbrokerIP[1]
                                                                            , MQTTbrokerIP[2]
                                                                            , MQTTbrokerIP[3]);
          if (!isValidIP(MQTTbrokerIP))  
          {
            DebugTf("ERROR: [%s] => is not a valid URL\r\n", settingMQTTbroker);
            settingMQTTinterval = 0;
            DebugTln(F("Next State: MQTT_STATE_ERROR"));
            stateMQTT = MQTT_STATE_ERROR;
            return false;
          }
          
          //MQTTclient.disconnect();
          //DebugTf("disconnect -> MQTT status, rc=%d \r\n", MQTTclient.state());
          DebugTf("[%s] => setServer(%s, %d) \r\n", settingMQTTbroker, MQTTbrokerIPchar, settingMQTTbrokerPort);
          MQTTclient.setServer(MQTTbrokerIPchar, settingMQTTbrokerPort);
          DebugTf("setServer  -> MQTT status, rc=%d \r\n", MQTTclient.state());
          MQTTclientId  = String(settingHostname) + "-" + WiFi.macAddress();
          stateMQTT = MQTT_STATE_TRY_TO_CONNECT;
          DebugTln(F("Next State: MQTT_STATE_TRY_TO_CONNECT"));
          reconnectAttempts = 0;

    case MQTT_STATE_TRY_TO_CONNECT:
          DebugTln(F("MQTT State: MQTT try to connect"));
          DebugTf("MQTT server is [%s], IP[%s]\r\n", settingMQTTbroker, MQTTbrokerIPchar);
      
          DebugTf("Attempting MQTT connection as [%s] .. \r\n", MQTTclientId.c_str());
          reconnectAttempts++;

          //--- If no username, then anonymous connection to broker, otherwise assume username/password.
          if (String(settingMQTTuser).length() == 0) 
          {
            DebugT(F("without a Username/Password "));
            MQTTclient.connect(MQTTclientId.c_str());
          } 
          else 
          {
            DebugTf("with Username [%s] and password ", settingMQTTuser);
            MQTTclient.connect(MQTTclientId.c_str(), settingMQTTuser, settingMQTTpasswd);
          }
          //--- If connection was made succesful, move on to next state...
          if (MQTTclient.connected())
          {
            reconnectAttempts = 0;  
            Debugf(" .. connected -> MQTT status, rc=%d\r\n", MQTTclient.state());
            MQTTclient.loop();
            stateMQTT = MQTT_STATE_IS_CONNECTED;
            return true;
          }
          Debugf(" -> MQTT status, rc=%d \r\n", MQTTclient.state());
      
          //--- After 3 attempts... go wait for a while.
          if (reconnectAttempts >= 3)
          {
            DebugTln(F("3 attempts have failed. Retry wait for next reconnect in 10 minutes\r"));
            stateMQTT = MQTT_STATE_ERROR;  // if the re-connect did not work, then return to wait for reconnect
            DebugTln(F("Next State: MQTT_STATE_ERROR"));
          }   
          break;
          
    case MQTT_STATE_IS_CONNECTED:
          MQTTclient.loop();
          return true;

    case MQTT_STATE_ERROR:
          DebugTln(F("MQTT State: MQTT ERROR, wait for 10 minutes, before trying again"));
          //--- next retry in 10 minutes.
          CHANGE_INTERVAL_MIN(reconnectMQTTtimer, 10);
          break;

    default:
          DebugTln(F("MQTT State: default, this should NEVER happen!"));
          //--- do nothing, this state should not happen
          stateMQTT = MQTT_STATE_INIT;
          CHANGE_INTERVAL_MIN(reconnectMQTTtimer, 10);
          DebugTln(F("Next State: MQTT_STATE_INIT"));
          break;
  }
#endif

  return false;  

} // connectMQTT_FSM()

//===========================================================================================
String trimVal(char *in) 
{
  String Out = in;
  Out.trim();
  return Out;
} // trimVal()


//=======================================================================
struct buildJsonMQTT {
#ifdef USE_MQTT

    char topicId[100];

    template<typename Item>
    void apply(Item &i) {
      if (i.present()) 
      {
        String Name = Item::name;
        //-- for dsmr30 -----------------------------------------------
  #if defined( USE_PRE40_PROTOCOL )
        if (Name.indexOf("gas_delivered2") == 0) Name = "gas_delivered";
  #endif
        String Unit = Item::unit();

        if (settingMQTTtopTopic[strlen(settingMQTTtopTopic)-1] == '/')
              snprintf(topicId, sizeof(topicId), "%s",  settingMQTTtopTopic);
        else  snprintf(topicId, sizeof(topicId), "%s/", settingMQTTtopTopic);
        strConcat(topicId, sizeof(topicId), Name.c_str());
        if (Verbose2) DebugTf("topicId[%s]\r\n", topicId);
        
        if (Unit.length() > 0)
        {
          createMQTTjsonMessage(mqttBuff, Name.c_str(), typecastValue(i.val()), Unit.c_str());
        }
        else
        {
          createMQTTjsonMessage(mqttBuff, Name.c_str(), typecastValue(i.val()));
        }
        
        //snprintf(cMsg, sizeof(cMsg), "%s", jsonString.c_str());
        //DebugTf("topicId[%s] -> [%s]\r\n", topicId, mqttBuff);
        if (!MQTTclient.publish(topicId, mqttBuff) )
        {
          DebugTf("Error publish(%s) [%s] [%d bytes]\r\n", topicId, mqttBuff, (strlen(topicId) + strlen(mqttBuff)));
        }
      }
  }
#endif

};  // struct buildJsonMQTT


//===========================================================================================
void sendMQTTData() 
{
#ifdef USE_MQTT
  String dateTime, topicId, json;

  if (settingMQTTinterval == 0) return;

  if (bailout()) return;


  if (!MQTTclient.connected() || ! mqttIsConnected)
  {
    DebugTf("MQTTclient.connected(%d), mqttIsConnected[%d], stateMQTT [%d]\r\n"
                                              , MQTTclient.connected()
                                              , mqttIsConnected, stateMQTT);
  }
  if (!MQTTclient.connected())  
  {
    if ( DUE( reconnectMQTTtimer) || mqttIsConnected)
    {
      mqttIsConnected = false;
      connectMQTT();
    }
    else
    {
      DebugTf("trying to reconnect in less than %d minutes\r\n", (TIME_LEFT_MIN(reconnectMQTTtimer) +1) );
    }
    if ( !mqttIsConnected ) 
    {
      DebugTln(F("no connection with a MQTT broker .."));
      return;
    }
  }

  DebugTf("Sending data to MQTT server [%s]:[%d]\r\n", settingMQTTbroker, settingMQTTbrokerPort);
  
  DSMRdata.applyEach(buildJsonMQTT());

#endif

} // sendMQTTData()


//=========================================================================
// function to build MQTT Json string ** max message size is 128 bytes!! **
//=========================================================================
void createMQTTjsonMessage(char *mqttBuff, const char *cName, const char *cValue, const char *cUnit)
{
  if (strlen(cUnit) == 0)
  {
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": \"%s\"}]}"
                                      , cName, cValue);
  }
  else
  {
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": \"%s\", \"unit\": \"%s\"}]}"
                                      , cName, cValue, cUnit);
  }

} // createMQTTjsonMessage(*char, *char, *char)

//---------------------------------------------------------------
void createMQTTjsonMessage(char *mqttBuff, const char *cName, const char *cValue)
{
  char noUnit[] = {'\0'};

  createMQTTjsonMessage(mqttBuff, cName, cValue, noUnit);
  
} // createMQTTjsonMessage(*char, *char)


//=======================================================================
void createMQTTjsonMessage(char *mqttBuff, const char *cName, String sValue, const char *cUnit)
{
  uint16_t hdrSize = (strlen(cName) * 2) +strlen(settingMQTTtopTopic) + 30;
  
  if (strlen(cUnit) == 0)
  {
    //DebugTf("sValue.lenght()[%d], strlen(%s)[%d]\r\n", sValue.length(), cName, strlen(cName));
    if ((hdrSize + sValue.length()) >= 128)
    {
      String tmp = sValue.substring(0, (128 - hdrSize));
      snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": \"%s\"}]}"
                         , cName, tmp.c_str());
    }
    else
    {
      snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": \"%s\"}]}"
                                      , cName, sValue.c_str());
    }
  }
  else
  {
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": \"%s\", \"unit\": \"%s\"}]}"
                                      , cName, sValue.c_str(), cUnit);
  }

} // createMQTTjsonMessage(*char, String, *char)

//---------------------------------------------------------------
void createMQTTjsonMessage(char *mqttBuff, const char *cName, String sValue)
{
  char noUnit[] = {'\0'};

  createMQTTjsonMessage(mqttBuff, cName, sValue, noUnit);
  
} // createMQTTjsonMessage(*char, String)


//=======================================================================
void createMQTTjsonMessage(char *mqttBuff, const char *cName, int32_t iValue, const char *cUnit)
{
  if (strlen(cUnit) == 0)
  {
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": %d}]}"
                                      , cName, iValue);
  }
  else
  {
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": %d, \"unit\": \"%s\"}]}"
                                      , cName, iValue, cUnit);
  }

} // createMQTTjsonMessage(*char, int, *char)

//---------------------------------------------------------------
void createMQTTjsonMessage(char *mqttBuff, const char *cName, int32_t iValue)
{
  char noUnit[] = {'\0'};

  createMQTTjsonMessage(mqttBuff, cName, iValue, noUnit);
  
} // createMQTTjsonMessage(char *mqttBuff, *char, int)


//=======================================================================
void createMQTTjsonMessage(char *mqttBuff, const char *cName, uint32_t uValue, const char *cUnit)
{
  if (strlen(cUnit) == 0)
  {
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": %u}]}"
                                      , cName, uValue);
  }
  else
  {
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": %u, \"unit\": \"%s\"}]}"
                                      , cName, uValue, cUnit);
  }

} // createMQTTjsonMessage(*char, uint, *char)

//---------------------------------------------------------------
void createMQTTjsonMessage(char *mqttBuff, const char *cName, uint32_t uValue)
{
  char noUnit[] = {'\0'};

  createMQTTjsonMessage(mqttBuff, cName, uValue, noUnit);
  
} // createMQTTjsonMessage(char *mqttBuff, *char, uint)


//=======================================================================
void createMQTTjsonMessage(char *mqttBuff, const char *cName, float fValue, const char *cUnit)
{
  if (strlen(cUnit) == 0)
  {
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": %.3f}]}"
                                      , cName, fValue);
  }
  else
  {
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": %.3f, \"unit\": \"%s\"}]}"
                                      , cName, fValue, cUnit);
  }

} // createMQTTjsonMessage(*char, float, *char)

//---------------------------------------------------------------
void createMQTTjsonMessage(char *mqttBuff, const char *cName, float fValue)
{
  char noUnit[] = {'\0'};

  createMQTTjsonMessage(mqttBuff, cName, fValue, noUnit);
  
} // createMQTTjsonMessage(char *mqttBuff, *char, float)

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
****************************************************************************
*/
