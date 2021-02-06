# **Blynk App**

Deze functie is beschikbaar vanaf release 2.3.1 van de dsmr V2 firmware.

In de Blynk app kunnen diverse gegevens gedeeld worden zodat actuele gegevens overal en altijd zichtbaar zijn. 
Deze gegevens lopen via de Blynk Cloud Service. Er is geen link tussen het soort gegevens en de inhoud omdat het virtuele veldnamen (V1 - V11) zijn maar het is wel goed om te weten dat deze data via dit platform verloopt.

**Welke gegevens kunnen worden bekeken?**  
De onderstaande gegevens kunnen worden bekeken via de Blynk apa.

Virtueel veld | Omschrijving | Eenheid 
------------- |------------- | -------
V0  | Actueel elektriciteit verbruik | [Watt]
V1  | Meterstand t1 = laag | [kWh]
V2  | Meterstand t2 = hoog | [kWh]  
V3  | Actuele teruglevering | [Watt]  
V5  | Gastellerstand | [m3]  
V6  | Meterstand 1 teruglevering = laag teruglevering  | [kWh]  
V7  | Meterstand 2 teruglevering = hoog teruglevering  | [kWh]  
V9  | Ruw telegram  (werkt nog niet optimaal, app buffer is snel vol)
V10 | gas per uur (is nu gelijk aan V5)  | [m3]
V11 | elektriciteit per uur (is nu gelijk aan V0)  | [kWh]

**Blynk app instellen** 

Stap 1: Download de Blynk app uit de app store.

[Blynk voor Android](https://play.google.com/store/apps/details?id=cc.blynk)  
[Blynk voor iOS](https://apps.apple.com/us/app/blynk-iot-for-arduino-esp32/id808760481) 

Stap 2: Instellen van de app

- registreer app en maak eventueel nieuw account aan
- scan qr code
- email authenticatie token


**Blynk instellen in adapter** 
Het enige wat de P1 adapter nodig heeft is het authenticatie token. Dit token kan worden opgegeven door een simpel bestandje te maken met de naam "BlynkSetup". Let op de hoofd en kleine letters. In dit bestandje wordt het token gekopieerd. Via bestandsbeheer op de p1 adapter (web interface) kan dit bestand worden geupload. Na 10 seconden zal de Blynk app aangegeven dat de datafeed actief is.

**Blynk interval dataverversing** 
De snelheid waarmee ververst wordt hangt samen met de snelheid waarmee de datagrammen uit de slimme meter gelezen worden. In de standaard configuratie is dat 10 seconden. In de instellingen kan dit worden aangepast naar elk gewenste snelheid.

