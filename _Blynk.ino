#ifdef USE_BLYNK 

char auth[45] = "";

byte readBlynkAuth(){
  int l = 0;
  if (!SPIFFSmounted) return 0;
  
  File BlynkFile = SPIFFS.open("/BlynkSetup", "r"); // open for appending  
  if (!BlynkFile) {
    DebugTln(F("open BlynkFile file FAILED!!!--> Bailout\r\n"));
    return 0;
  }

  while (BlynkFile.available()) {
    l = BlynkFile.readBytesUntil('\n', auth, sizeof(auth));
  }
  auth[l] = '\0';
 
  BlynkFile.close();
  return l;
}

void handleBlynk(){
  if (strlen(auth) > 0) {
    DebugTln(F("handleBlynk"));
    Blynk.virtualWrite(V0, DSMRdata.power_delivered*1000); // Huidig verbruik [Watt]
    Blynk.virtualWrite(V1, DSMRdata.energy_delivered_tariff1); // Meterstand t1 = laag [kWh]
    Blynk.virtualWrite(V2, DSMRdata.energy_delivered_tariff2); // Meterstand t2 = hoog [kWh]
    Blynk.virtualWrite(V3, DSMRdata.power_returned*1000); // Huidig teruglevering [Watt]
    Blynk.virtualWrite(V6, DSMRdata.energy_returned_tariff1); // Meterstand 1 = laag teruglevering
    Blynk.virtualWrite(V7, DSMRdata.energy_returned_tariff2); // Meterstand 2 = hoog teruglevering
    Blynk.virtualWrite(V5, DSMRdata.gas_delivered); // Gastellerstand [m3]
    Blynk.virtualWrite(V9, slimmeMeter.raw()); //telegram 
    yield();
    if (DagSlot < 99) {
      Blynk.virtualWrite(V10, DSMRdata.gas_delivered - GDT_G ); // gas vandaag
      Blynk.virtualWrite(V11, DSMRdata.energy_delivered_tariff1 + DSMRdata.energy_delivered_tariff2 - EDT1_G - EDT2_G ); // Verbuik electra vandaag
      Blynk.virtualWrite(V12, DSMRdata.energy_returned_tariff1 + DSMRdata.energy_returned_tariff2 - ERT1_G - ERT2_G); // Teruglevering electra vandaag
//      DebugT(F("Blynk V10: "));Debugln(DSMRdata.gas_delivered - GDT_G);
//      DebugT(F("Blynk V11: "));Debugln(DSMRdata.energy_delivered_tariff1 + DSMRdata.energy_delivered_tariff2 - EDT1_G - EDT2_G);
//      DebugT(F("Blynk V12: "));Debugln(DSMRdata.energy_returned_tariff1 + DSMRdata.energy_returned_tariff2 - ERT1_G - ERT2_G);
    }
  } else {
    if (SPIFFS.exists("/BlynkSetup")) SetupBlynk(); //check if  auth file is present then setup
  }
}

void SetupBlynk(){
  if (readBlynkAuth() != 0) Blynk.config(auth);
  else DebugTln(F("Setup Blynk: auth. not available"));
  DebugTf("Blynk auth: %s (%d)\n",auth,strlen(auth));
  delay(500);
  handleBlynk(); //firsttime
}
#endif
