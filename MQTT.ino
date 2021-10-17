/* 
***************************************************************************  
**  Program  : MQTT, part of DSMRloggerAPI
**  Version  : v2.3.2
**
**  Copyright (c) 2021 Willem Aandewiel / Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
**  RvdB  changed MQTT stuff to FSM 
**  AaW   simplified and restructured the FSM 
*/

// Declare some variables within global scope

  static IPAddress  MQTTbrokerIP;
  static char       MQTTbrokerIPchar[20];

  
  int8_t              reconnectAttempts = 0;
  char                lastMQTTtimestamp[15] = "-";

  enum states_of_MQTT { MQTT_STATE_INIT, MQTT_STATE_TRY_TO_CONNECT, MQTT_STATE_IS_CONNECTED, MQTT_STATE_ERROR };
  enum states_of_MQTT stateMQTT = MQTT_STATE_INIT;

  String            MQTTclientId;

//===========================================================================================
void connectMQTT() 
{
  
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

  CHANGE_INTERVAL_SEC(reconnectMQTTtimer, 5);

}

//===========================================================================================
bool connectMQTT_FSM() 
{
  
  switch(stateMQTT) 
  {
    case MQTT_STATE_INIT:  
          DebugTln(F("MQTT State: MQTT Initializing"));
          LogFile("MQTT Starting");
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
          LogFile("MQTT State: MQTT try to connect");
          DebugTf("MQTT server is [%s], IP[%s]\r\n", settingMQTTbroker, MQTTbrokerIPchar);
      
          DebugTf("Attempting MQTT connection as [%s] .. \r\n", MQTTclientId.c_str());
          reconnectAttempts++;

          //--- If no username, then anonymous connection to broker, otherwise assume username/password.
          sprintf(cMsg,"%sLWT",settingMQTTtopTopic);
          if (String(settingMQTTuser).length() == 0) 
          {
            DebugT(F("without a Username/Password "));
//            MQTTclient.connect(MQTTclientId.c_str());
            MQTTclient.connect(MQTTclientId.c_str(),"","",cMsg,1,true,"Offline");
          } 
          else 
          {
            DebugTf("with Username [%s] and password ", settingMQTTuser);
            MQTTclient.connect(MQTTclientId.c_str(), settingMQTTuser, settingMQTTpasswd,cMsg,1,true,"Offline");
          }
          //--- If connection was made succesful, move on to next state...
          if (MQTTclient.connected())
          {
            reconnectAttempts = 0;  
            Debugf(" .. connected -> MQTT status, rc=%d\r\n", MQTTclient.state());
            MQTTclient.publish(cMsg,"Online", true); //send LWT
            LogFile("MQTT connected");
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
          LogFile("MQTT Connected");
          MQTTclient.loop();
          return true;

    case MQTT_STATE_ERROR:
          DebugTln(F("MQTT State: MQTT ERROR, wait for 10 minutes, before trying again"));
          LogFile("MQTT Error: retry in 5 seconds");
          //--- next retry in 10 minutes.
          CHANGE_INTERVAL_SEC(reconnectMQTTtimer, 5);
          break;

    default:
          DebugTln(F("MQTT State: default, this should NEVER happen!"));
          //--- do nothing, this state should not happen
          stateMQTT = MQTT_STATE_INIT;
          CHANGE_INTERVAL_SEC(reconnectMQTTtimer, 5);
          DebugTln(F("Next State: MQTT_STATE_INIT"));
          break;
  }

  return false;  

} // connectMQTT_FSM()

//=======================================================================
 
struct buildJsonMQTT {
/* twee types
 *  {energy_delivered_tariff1":[{"value":11741.29,"unit":"kWh"}]}"
 *  {equipment_id":[{"value":"4530303435303033383833343439383137"}]}"
 *  
 *  msg = "{\""+Name+"\":[{\"value\":"+value_to_json(i.val())+",\"unit\":"+Unit+"}]}";
 *  msg = "\"{\""+Name+"\":[{\"value\":"+value_to_json(i.val())+"}]}\""
 *  
 */
    String msg;
    
    template<typename Item>
    void apply(Item &i) {
      if (i.present()) 
      {   
        strcpy(cMsg,settingMQTTtopTopic);
        strcat(cMsg, (char*)Item::name);
        if (strlen(Item::unit()) > 0) msg = "{\""+String(Item::name)+"\":[{\"value\":"+value_to_json(i.val())+",\"unit\":\""+Item::unit()+"\"}]}";
        else msg = "{\""+String(Item::name)+"\":[{\"value\":"+value_to_json(i.val())+"}]}";
        if (Verbose2) DebugTln("mqtt bericht: "+msg);
        if (!MQTTclient.publish(cMsg, msg.c_str()) ) DebugTf("Error publish(%s) [%s] [%d bytes]\r\n", cMsg, msg.c_str(), (strlen(cMsg) + msg.length()));
      } // if i.present
  } //apply
  
  template<typename Item>
  Item& value_to_json(Item& i) {
    return i;
  }

  String value_to_json( String i){
    return "\"" + i+ "\"";
  }
  

}; // buildJsonMQTT

//===========================================================================================
void MQTTSentStaticP1Info(){
  /**
   * -- identification 
*      -- p1_version
*      -- equipment_id
*      -- gas_device_type
*      -- gas_equipment_id
*      -- Firmware version : _VERSION
*      -- IP-adres: WiFi.localIP().toString();
   */
  String msg;
  StaticInfoSend = true;
  msg = "{\"identification\":[{\"value\":"+ DSMRdata.identification + "\"}]}";
  strcpy(cMsg,settingMQTTtopTopic);
  strcat(cMsg, "identification");
  if (!MQTTclient.publish(cMsg, msg.c_str()),msg.length(),true ) {
    DebugTf("Error publish(%s) [%s] [%d bytes]\r\n", cMsg, msg.c_str(), (strlen(cMsg) + msg.length()));
    StaticInfoSend = false; //probeer het later nog een keer
  }
  strcpy(cMsg,settingMQTTtopTopic);
  strcat(cMsg, "p1_version");
  msg = "{\"p1_version\":[{\"value\":" + DSMRdata.p1_version + "\"}]}";
  if (!MQTTclient.publish(cMsg, msg.c_str()),msg.length(),true ) {
    DebugTf("Error publish(%s) [%s] [%d bytes]\r\n", cMsg, msg.c_str(), (strlen(cMsg) + msg.length()));
    StaticInfoSend = false; //probeer het later nog een keer
  }
  strcpy(cMsg,settingMQTTtopTopic);
  strcat(cMsg, "equipment_id");
  msg = "{\"equipment_id\":[{\"value\":"+DSMRdata.equipment_id+"\"}]}";
  if (!MQTTclient.publish(cMsg, msg.c_str()),msg.length(),true ) {
    DebugTf("Error publish(%s) [%s] [%d bytes]\r\n", cMsg, msg.c_str(), (strlen(cMsg) + msg.length()));
    StaticInfoSend = false; //probeer het later nog een keer
  }
  strcpy(cMsg,settingMQTTtopTopic);
  strcat(cMsg, "firmware");
  msg = "{\"firmware\":[{\"value\":" + String(_VERSION_ONLY) + "\"}]}";
  if (!MQTTclient.publish(cMsg, msg.c_str()),msg.length(),true ) {
    DebugTf("Error publish(%s) [%s] [%d bytes]\r\n", cMsg, msg.c_str(), (strlen(cMsg) + msg.length()));
    StaticInfoSend = false; //probeer het later nog een keer
  }

  strcpy(cMsg,settingMQTTtopTopic);
  strcat(cMsg, "ip_address");
  msg = "{\"ip_address\":[{\"value\":" + WiFi.localIP().toString() + "\"}]}";
  if (!MQTTclient.publish(cMsg, msg.c_str()),msg.length(),true ) {
    DebugTf("Error publish(%s) [%s] [%d bytes]\r\n", cMsg, msg.c_str(), (strlen(cMsg) + msg.length()));
    StaticInfoSend = false; //probeer het later nog een keer
  }
  
  if (DSMRdata.gas_device_type_present){
    strcpy(cMsg,settingMQTTtopTopic);
    strcat(cMsg, "gas_device_type");
    msg = "{\"gas_device_type\":[{\"value\":" + String(DSMRdata.gas_device_type) + "\"}]}";  
    if (!MQTTclient.publish(cMsg, msg.c_str()),msg.length(),true ) {
      DebugTf("Error publish(%s) [%s] [%d bytes]\r\n", cMsg, msg.c_str(), (strlen(cMsg) + msg.length()));
    StaticInfoSend = false; //probeer het later nog een keer
    }
  }
  
  if (DSMRdata.gas_equipment_id_present){
    strcpy(cMsg,settingMQTTtopTopic);
    strcat(cMsg, "gas_equipment_id");
    msg = "{\"gas_equipment_id\":[{\"value\":"+DSMRdata.gas_equipment_id+"\"}]}";
    if (!MQTTclient.publish(cMsg, msg.c_str()),msg.length(),true ) {
      DebugTf("Error publish(%s) [%s] [%d bytes]\r\n", cMsg, msg.c_str(), (strlen(cMsg) + msg.length()));
      StaticInfoSend = false; //probeer het later nog een keer
    }
  }
}

//===========================================================================================
void MQTTSentStaticDevInfo(){
  String msg;
  strcpy(cMsg,settingMQTTtopTopic);
  strcat(cMsg, "wifi_rssi");
  msg = "{\"wifi_rssi\":[{\"value\":" + String(WiFi.RSSI()) + "\"}]}";
  if (!MQTTclient.publish(cMsg, msg.c_str()),msg.length(),true ) {
      DebugTf("Error publish(%s) [%s] [%d bytes]\r\n", cMsg, msg.c_str(), (strlen(cMsg) + msg.length()));
    }
}

//===========================================================================================
void sendMQTTData() 
{

  if ((settingMQTTinterval == 0) || bailout() ) return;

  //make proper TopTopic
  if (settingMQTTtopTopic[strlen(settingMQTTtopTopic)-1] != '/') snprintf(settingMQTTtopTopic, sizeof(settingMQTTtopTopic), "%s/",  settingMQTTtopTopic);
 
  if (!MQTTclient.connected() || ! mqttIsConnected)
  {
    DebugTf("MQTTclient.connected(%d), mqttIsConnected[%d], stateMQTT [%d]\r\n", MQTTclient.connected(), mqttIsConnected, stateMQTT);
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
      DebugTf("trying to reconnect in less than %d seconds\r\n", (TIME_LEFT_MIN(reconnectMQTTtimer) +1) );
    }
    if ( !mqttIsConnected ) 
    {
      DebugTln(F("no connection with a MQTT broker .."));
      return;
    }
  }

  DebugTf("Sending data to MQTT server [%s]:[%d]\r\n", settingMQTTbroker, settingMQTTbrokerPort);
  if ((telegramCount - telegramErrors) > 2 && !StaticInfoSend){
//    MQTTSentStaticP1Info();
//    MQTTSentStaticDevInfo();
  }
  DSMRdata.applyEach(buildJsonMQTT());

} // sendMQTTData()

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
