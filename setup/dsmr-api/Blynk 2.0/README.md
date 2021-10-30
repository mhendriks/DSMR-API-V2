# **Blynk 2.0 App**

Deze functie is beschikbaar vanaf release 3.1.1 van de dsmr V3 firmware.

In de Blynk app kunnen diverse gegevens gedeeld worden zodat actuele gegevens overal en altijd zichtbaar zijn. 
Deze gegevens lopen via de Blynk Cloud Service. Er is geen link tussen het soort gegevens en de inhoud omdat het virtuele veldnamen (V1 - V11) zijn maar het is wel goed om te weten dat deze data via dit platform verloopt.

**Blynk app instellen** 

Stap 1: Download de Blynk app uit de app store.

[Blynk voor Android](https://play.google.com/store/apps/details?id=cloud.blynk)  
[Blynk voor iOS](https://apps.apple.com/us/app/blynk-iot/id1559317868) 


Stap 2: maak een Blynk Account
Via Blynk Console of app maakt u een Blynk Account. Advies is om dit via [Blynk Console](https://blynk.cloud/dashboard/login) te doen ivm de stappen hierna.
 

Stap 3: Instellen Blynk : Template
Bij het scherm QuickStart klikt u op [CANCEL]

In het menu klikt u op Template en daarna op [New Template]
- Name : P1 Dongel
- Hardware : ESP8266
- Connection Type: Wifi
Klik dan op [Done]

Klik in het overzichtscherm op [Save] rechtsboven

Stap 4: Instellen Blynk : Device
Klik op het vergrootglas (Search)
Klik op [New Device]
Klik op [From Template]
TEMPLATE: P1 Dongle
Klik op [Create]

kopieer de gegevens in het rechtervenster zoals het voorbeeld hieronder:
#define BLYNK_TEMPLATE_ID "TMPLPuW-DEMO"
#define BLYNK_DEVICE_NAME "P1 Dongel"
#define BLYNK_AUTH_TOKEN "2V-AJxupJf6uSGUDcXMjnh_zDEMO"


Stap 5: creëer Datastreams
Klik op de ... (menu) achter P1 Dongle (offline) zoals hieronder en daarna op Edit Dashboard.
Klik dan op [Datastreams]
Klik op [+New Datastreams] -> [VirtualPin]
Vul onderstaande in:
Name: Energie-Geleverd
Alias: Energie
PIN : V0
Data type: Integer
Units: Watts
Min: 0 
Max: 999999
Klik daarna op [Create]

herhaal bovenstaande voor de overige VirtualPins.

Druk dan op [Save] + [update 1 active device]



Stap 6: creëer Dashboard
Klik op de ... (menu) achter P1 Dongle (offline) zoals hieronder
en daarna op Edit Dashboard



**New account**  
Indien u de Blynk app voor eerst gebruikt dan dient u een account aan te maken. 

**Start nieuw project**  
Druk op het scan icoon (rode pijl) in de Blynk app.

<img src="Blynk-New-Project.jpg" width="30%">



**scan qr code**  
Scan onderstaande qr code met de Blynk App.

<img src="blynk-qr.png" width="30%">

**copier/e-mail auth token voor in de P1 adapter**  
Na het scannen van de qr-code wordt het dashboard getoond. Druk op de instellingen zoals dit in onderstaande afbeelding wordt weergegeven.

<img src="blynk-Project-Settings.png" width="30%">

Onderstaande scherm wordt dan getoond. Dit scherm bevat AUITH token. Deze is nodig om op te nemen in de P1-adapter.

<img src="blynk-auth-token.png" width="30%">

Druk op E-mail knop om de code naar u te laten e-mailen.

**zet auth token in bestand "BlynkSetup" (notepad / kladblok app)**  
De P1 adapter heeft het token nodig om de Blynk app van data te kunnen voorzien. Dit token kan worden opgegeven door een simpel bestandje te maken met de naam "BlynkSetup". Let op de hoofd en kleine letters en GEEN bestands extensie. In dit bestandje wordt het token gekopieerd. 

<img src="blynk-upload-p1.png" width="50%">


Via bestandsbeheer (folder icoon rechtsboven) op de p1 adapter (web interface) kan dit bestand worden geupload via bestand kiezen. Na 10 seconden zal de Blynk app aangegeven dat de datafeed actief is. Is na 10 seconden in de Blynk app nog steeds geen data binnen gekomen dan kan er iets mis gegaan zijn met het kopiëren van de AUTH gegevens.

**Blynk interval dataverversing** 
De snelheid waarmee ververst wordt hangt samen met de snelheid waarmee de datagrammen uit de slimme meter gelezen worden. In de standaard configuratie is dat 10 seconden. In de instellingen kan dit worden aangepast naar elk gewenste snelheid.

**Welke gegevens kunnen worden bekeken?**  
De onderstaande gegevens kunnen worden bekeken via de Blynk app.

 veld 	| Omschrijving 							| Eenheid 	| Firmware 
----- 	|------------- 							| ------- 	| -------
V0  	| Actueel elektriciteit verbruik 		| [Watt]	| 2.3.1
V1  	| Meterstand t1 = laag 					| [kWh]		| 2.3.1
V2  	| Meterstand t2 = hoog 					| [kWh]  	| 2.3.1
V3  	| Actuele teruglevering 				| [Watt]  	| 2.3.1
V5  	| Gastellerstand 						| [m3]  	| 2.3.1
V6  	| Meterstand 1  = laag teruglevering  	| [kWh]  	| 2.3.1
V7  	| Meterstand 2  = hoog teruglevering  	| [kWh]  	| 2.3.1
V9  	| Ruw telegram  (werkt nog niet optimaal, app buffer is snel vol)| | 2.3.1
V10 	| Gasverbruik vandaag 					| [m3] 		| 2.3.3
V11 	| Verbruik electra vandaag  			| [kWh] 	| 2.3.3
V12 	| Teruglevering electra vandaag  		| [kWh] 	| 2.3.3
