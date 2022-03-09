#ifdef USE_WATER_SENSOR

void sendMQTTWater(){
  if (!WtrMtr) return;
  sprintf(cMsg,"%d.%3.3d",P1Status.wtr_m3,P1Status.wtr_l);
  MQTTSend("water",cMsg);
}
void IRAM_ATTR iWater() {
    WtrTimeBetween = now() - WtrPrevReading;
    if (DUE(WaterTimer)) { //negeer te korte foutieve meting (1500 ltr/uur / 3600 = 0,4l/sec kan daarom niet zo zijn dat binnen de 1/0,4 = 2,4s er nog een puls komt) = debounce timer
      P1Status.wtr_l += WtrFactor;
      WtrPrevReading = now();
      if (P1Status.wtr_l >= 1000) {
        P1Status.wtr_m3++;
        P1Status.wtr_l = 0;
        CHANGE_INTERVAL_MS(StatusTimer, 100); //schrijf status weg bij elke m3
      }
    } else debounces++;
}

void setupWater() {
  pinMode(PIN_WATER_SENSOR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_WATER_SENSOR), iWater, FALLING);
  DebugTln(F("WaterSensor setup completed"));
}

#endif
