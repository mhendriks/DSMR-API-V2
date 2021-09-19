#ifdef USE_BLYNK 

#include <BlynkSimpleEsp8266.h>

char BlynckAuth[45] = "";

byte readBlynkAuth(){
  int l = 0;
  
  File BlynkFile = LittleFS.open("/BlynkSetup", "r"); // open for appending  
  if (!BlynkFile) {
    DebugTln(F("open BlynkFile file FAILED!!!--> Bailout\r\n"));
    return 0;
  }

  while (BlynkFile.available()) {
    l = BlynkFile.readBytesUntil('\n', BlynckAuth, sizeof(BlynckAuth));
  }
  BlynckAuth[l] = '\0';
 
  BlynkFile.close();
  return l;
}

void handleBlynk(){
  if (strlen(BlynckAuth) > 0) {
    DebugTln(F("Blynk: handle data"));
    Blynk.virtualWrite(V0, DSMRdata.power_delivered*1000); // Huidig verbruik [Watt]
    Blynk.virtualWrite(V1, DSMRdata.energy_delivered_tariff1); // Meterstand t1 = laag [kWh]
    Blynk.virtualWrite(V2, DSMRdata.energy_delivered_tariff2); // Meterstand t2 = hoog [kWh]
    Blynk.virtualWrite(V3, DSMRdata.power_returned*1000); // Huidig teruglevering [Watt]
    Blynk.virtualWrite(V6, DSMRdata.energy_returned_tariff1); // Meterstand 1 = laag teruglevering
    Blynk.virtualWrite(V7, DSMRdata.energy_returned_tariff2); // Meterstand 2 = hoog teruglevering
    Blynk.virtualWrite(V5, gasDelivered); // Gastellerstand [m3]
//    Blynk.virtualWrite(V9, slimmeMeter.raw()); //telegram 
    yield();
//    if (DagSlot < 99) {
//      Blynk.virtualWrite(V10, DSMRdata.gas_delivered - GDT_G ); // gas vandaag
//      Blynk.virtualWrite(V11, DSMRdata.energy_delivered_tariff1 + DSMRdata.energy_delivered_tariff2 - EDT1_G - EDT2_G ); // Verbuik electra vandaag
//      Blynk.virtualWrite(V12, DSMRdata.energy_returned_tariff1 + DSMRdata.energy_returned_tariff2 - ERT1_G - ERT2_G); // Teruglevering electra vandaag
//    }
  } else {
    if (FSmounted && LittleFS.exists("/BlynkSetup")) SetupBlynk(); //check if  auth file is present then setup
  }
}

void SetupBlynk(){
  if (readBlynkAuth() > 0) Blynk.config(BlynckAuth);
  else DebugTln(F("Setup Blynk: SetupBlynk file not available"));
  DebugTf("Blynk auth: %s (%d)\n",BlynckAuth,strlen(BlynckAuth));
}
#endif
