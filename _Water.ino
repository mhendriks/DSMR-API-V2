#ifdef USE_WATER_SENSOR

void sendMQTTWater(){
  if (!WtrMtr) return;
  String msg = "{\"water\":[{\"value\":" + String(P1Status.wtr_m3) + ",\"unit\":\"m3\"}]}";
  sprintf(cMsg,"%swater",settingMQTTtopTopic);
  if ( !MQTTclient.publish(cMsg, msg.c_str(),true) ) DebugTf("Error publish(%s) [%s] [%d bytes]\r\n", cMsg, msg.c_str(), (strlen(cMsg) + msg.length()));
}

void IRAM_ATTR iWater() {
    if (DUE(WaterTimer)) { //negeer te korte foutieve meting (1500 ltr/uur / 3600 = 0,4l/sec kan daarom niet zo zijn dat binnen de 1/0,4 = 2,4§s er nog een puls komt) = debounce timer
      P1Status.wtr_l += WtrFactor;
      if (WtrPrevReading) DebugTf("Watermeter time between readings: %d",now() - WtrPrevReading);
      WtrPrevReading = now();
      if (P1Status.wtr_l >= 1000) {
        P1Status.wtr_m3++;
        P1Status.wtr_l = 0;
        CHANGE_INTERVAL_MS(StatusTimer, 100); //schrijf status weg bij elke m3
      }
    } else {
      DebugTln(F("Water Debouncetimer"));
      debounce[debounces] = now();
      debounces++;
      debounces%=25;
    }
}

void setupWater() {
  pinMode(PIN_WATER_SENSOR, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_WATER_SENSOR), iWater, RISING);
  DebugTln(F("WaterSensor setup completed"));
}

#endif
