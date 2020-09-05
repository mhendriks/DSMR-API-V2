# Setup Tasmota voor de P1 interface
Er zijn een aantal stappen nodig om Tasmota op de p1 adapter te laten draaien.
- Stap 1: Tasmota flashen (of zo geleverd krijgen)
- Stap 2: Tasmota configuratie instellen
- Stap 3: domotica oplossing instellen (bv. OpenHAB, HomeAssistant, ...)

## 1 Tasmota flashen
Dit kan op verschillende manieren. 
Met de Tasmota 8.4.0 versie is het nog niet mogelijk om out of the box de firmware te gebruiken.
Een aanpassing is aangevraagd en zal in een nieuwe versie van tasmota komen te zitten 8.4.1 en hoger.

OF gebruik de aangepaste versie firmware_8_4_0_3_aanpassing.bin in deze folder.

Prik de P1 adapter hierna in de slimme meter en wacht totdat je de tasmota-... adapter in je wifi netwerk ziet.
Klik op dit netwerk en wacht totdat je het instellingenscherm van tasmota ziet om de adapter aan het netwerk te koppelen.
Zoek de P1 module in het netwerk op.

## 2 Tasmota configuratie instellen
In de configuratie van Tasmota geef geef je aan dat je de configuratie wilt terugzetten (Restore).
Gebruik het Config_tasmota_C3C439_1081_8.4.0.3.dmp bestand in deze folder.

## 3 domotica oplossing instellen 
Stel de mqtt host in op de adapter.
Daarna dien je in je domotica oplossing het ontvangen van de berichten in te richten. Hieronder de voorbeelden voor OpenHAB en HomeAssistant.

### OpenHAB

mqtt.things
Heb je al een bridge gedefineerd voeg dan onderstaande toe aan dit bestand

// P1 - slimmemeter Tasmota implementatie
     Thing topic slimmemeter_tas "Slimmemeter" @ "Hal" {
    Channels:
		Type string : p1	"receive data"	[ stateTopic="tele/tasmota_C3C439/RESULT", transformationPattern="JSONPATH:SSerialReceived" ]	
    }
    
in de rules folder maak je een nieuw bestand aan p1-dsmr.rules met onderstaande code
```rule "P2 P"
when
    Item P2 received update
then
	if (P2.state == NULL || P2 == "") return;
    var rawString = P2.state.toString
	//P1_I 		1-0:31.7.0(002*A)
	val rawString3 = transform("REGEX", ".*([0-9]{3}).A.*", rawString)
	if ( rawString3 != NULL && rawString3 !== null && rawString3 != "") { 
		P1_I.postUpdate(rawString3) 
		} 
	//P1_T1		1-0:1.8.1(000013.000*kWh) (Totaal geleverd tarief 1 (nacht))
	 rawString3 = transform("REGEX", ".*1.8.1.([0-9]{6}.[0-9]{3}).kWh.*", rawString)
	if ( rawString3 != NULL && rawString3 !== null && rawString3 != "") { 
		P1_T1.postUpdate(rawString3) 
		} 
	//P1_T2 	1-0:1.8.2(000084.000*kWh) (Totaal verbruik tarief 2 (dag))
	 rawString3 = transform("REGEX", ".*1.8.2.([0-9]{6}.[0-9]{3}).kWh.*", rawString)
	if ( rawString3 != NULL && rawString3 !== null && rawString3 != "") { 
		P1_T2.postUpdate(rawString3) 
		} 
	//P2_GAS 	0-1:24.2.1(191222151005W)(02728.449*m3)
	rawString3 = transform("REGEX", ".*[0-9]{12}.*([0-9]{5}.[0-9]{3}).m3.*", rawString)
	if ( rawString3 != NULL && rawString3 !== null && rawString3 != "") { 
		P1_GAS.postUpdate(rawString3) 
		} 
	// P2_P 		1-0:21.7.0(00.523*kW)
	rawString3 = transform("REGEX", ".*21.7.0.([0-9]{2}.[0-9]{3}).kW.*", rawString)
	if ( rawString3 != NULL && rawString3 !== null && rawString3 != "") { 
		P1_P.postUpdate(rawString3) 
		} 
		//time: 0-0:1.0.0(200716222227S)
	rawString3 = transform("REGEX", ".*1.0.0.([0-9]{12})S.*", rawString)
	if ( rawString3 != NULL && rawString3 !== null && rawString3 != "") { 
		P1_time.postUpdate(rawString3) 
		} 
end

rule "P1 last update"
when
    Item P1_time received update
then
	if (P1_time.state == NULL || P1_time == "") return;
    var rawString = P1_time.state.toString
	val res = String::format("%s-%s-%s %s:%s:%s", rawString.substring(4, 6), rawString.substring(2, 4), rawString.substring(0, 2),rawString.substring(6, 8), rawString.substring(8, 10),rawString.substring(10, 12))
	if ( res != NULL && res !== null && res != "") { 
		P1_tijd.postUpdate(res) 
		} 
end   
``` 

In het .items bestand voeg je de waardes toe die je wilt zien. Hieronder een voorbeeld (dit voorbeeld is voor tasmota + standalone implementatie geschikt).
```
Number P1_P "Actueel vermogen [%s]"	 {channel="mqtt:topic:mosquitto:slimmemeter:P" } 
Number P1_GAS "Gasmeter: [%.1f]"	 {channel="mqtt:topic:mosquitto:slimmemeter:G" }
Number P1_T1 "kWh Teller 1: [%s]"	 {channel="mqtt:topic:mosquitto:slimmemeter:T1" } 
Number P1_T2 "kWh Teller 1: [%s]"	 {channel="mqtt:topic:mosquitto:slimmemeter:T2" } 
Number P1_I "Stroom: [%s]"			 {channel="mqtt:topic:mosquitto:slimmemeter:I" }
Number P1_U "Spanning: [%s]"		 {channel="mqtt:topic:mosquitto:slimmemeter:U" } 
String P1_time "tijd: [%s]" 		 {channel="mqtt:topic:mosquitto:slimmemeter:time" } //dit is de systeemtijd uit de slimmemeter
String P1_tijd "tijd: [%s]"

String P2 "test [%s]"				 {channel="mqtt:topic:mosquitto:slimmemeter_tas:p1" } 
```
In het .sitemap bestand laat je de waardes uit .items terugkomen
```
Frame label="Slimmemeter" {
    Text item=P1_T1 label="kWh Teller Dal: [%.1f kWh]"
    Text item=P1_T2 label="kWh Teller Normaal: [%.1f kWh]"
    Text item=P1_P label="Actuele vermogen: [%.3f kW]"
    Text item=P1_I label="Actuele stroom: [%.0f A]"
    Text item=P1_U label="Actuele Spanning: [%.1f V]"
    Text item=P1_GAS label="Gasmeter: [%.1f m3]"
   	Text item=P1_tijd label="Laatste P1 update: [%s]"
   	Text item=P1_time label="Laatste P1 update: [%s]" //zou je ook weg kunnen laten dit is de ruwe tijd
   	}
```
### HomeAssistant
Work in progress


