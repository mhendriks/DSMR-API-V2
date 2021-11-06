#ifdef USE_WATER_SENSOR

void sendMQTTWater(){
  String msg = "{\"water\":[{\"value\":" + String(P1Status.wtr_m3) + ",\"unit\":\"m3\"}]}";
  sprintf(cMsg,"%s%s",settingMQTTtopTopic,"water");
  if (!MQTTclient.publish(cMsg, msg.c_str()),true ) DebugTf("Error publish(%s) [%s] [%d bytes]\r\n", cMsg, msg.c_str(), (strlen(cMsg) + msg.length()));
}

void IRAM_ATTR iWater() {
    P1Status.wtr_l++;
    if (P1Status.wtr_l == 1000) {
      P1Status.wtr_m3++;
      P1Status.wtr_l = 0;
      CHANGE_INTERVAL_MS(StatusTimer, 100); //update status file snel bij elke m3 - hoofd loop ivm Interupt interval
    }
}

void setupWater() {
  pinMode(PIN_WATER_SENSOR, INPUT);
  attachInterrupt(PIN_WATER_SENSOR, iWater, RISING);
  DebugTln(F("WaterSensor setup completed"));
}

#endif
