#ifdef USE_BLYNK 

char BlynkAuth[33] = ""; //"-GuFj2BUe63ny_XYMCLKo7j2gKCxcGuA"; //"Ch9aV0mh7NYXE3CZj5VHXrzYuECS9rYF";

//BlynkTimer Btimer;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void handleBlynk()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
//  Blynk.virtualWrite(V0, rand() % 10000);
//  Blynk.virtualWrite(V1, rand() % 10000);
//  Blynk.virtualWrite(V2, rand() % 10000);
//    DebugTln("handleBlynk");
    Blynk.virtualWrite(V0, DSMRdata.power_delivered*1000); // Huidig verbruik [Watt]
    Blynk.virtualWrite(V1, DSMRdata.energy_delivered_tariff1); // Meterstand t1 = laag [kWh]
    Blynk.virtualWrite(V2, DSMRdata.energy_delivered_tariff2); // Meterstand t2 = hoog [kWh]
    Blynk.virtualWrite(V3, DSMRdata.power_returned*1000); // Huidig teruglevering [Watt]
    Blynk.virtualWrite(V6, DSMRdata.energy_returned_tariff1); // Meterstand 1 = laag teruglevering
    Blynk.virtualWrite(V7, DSMRdata.energy_returned_tariff2); // Meterstand 2 = hoog teruglevering
    Blynk.virtualWrite(V5, gasDelivered); // Gastellerstand [m3]
//    Blynk.virtualWrite(V9, slimmeMeter.raw()); //telegram 
    yield();
}


byte readBlynkAuth(){
  int l = 0;
  
  File BlynkFile = LittleFS.open(_BLYNK_FILE, "r"); // open for appending  
  if (!BlynkFile) {
    DebugTln(F("open BlynkFile file FAILED!!!--> Bailout\r\n"));
    return 0;
  }

  while (BlynkFile.available()) { l = BlynkFile.readBytesUntil('\n', BlynkAuth, sizeof(BlynkAuth)); }
  BlynkAuth[l] = '\0';
// DebugT("BlynkAuth: "); Debugln(BlynkAuth);
  BlynkFile.close();
  return l;
}
/**
void SetupBlynk(){
  if (readBlynkAuth() > 0) Blynk.config(BlynckAuth);
  else DebugTln(F("Setup Blynk: SetupBlynk file not available"));
  DebugTf("Blynk auth: %s (%d)\n",BlynckAuth,strlen(BlynckAuth));
}
**/

void SetupBlynk()
{
 if (readBlynkAuth() > 0) {
    Blynk.config(BlynkAuth);
    DebugTln(F("Blynk setup done."));
  }else {
    return;
  }
 
  // Setup a function to be called every 5 seconds
//  Btimer.setInterval(5000L, myTimerEvent);
}



#endif
