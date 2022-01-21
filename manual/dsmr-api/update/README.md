# **DSMR-API DONGLE UPDATEN**

>Let op: er zijn dongles in omloop (aankoop in 2021) die verwijzen naar htpp://smart-stuff.nl/ota (in de DSMRsettings.json file). Dit dient eerst aangepast te worden naar http://ota.smart-stuff.nl/

**Remote update**<br>
Vanaf de 3.0.4 firmware is een remote Firmware update ook mogelijk. Dit kan op diverse manieren, namelijk
1) via Telnet
2) via de webinterface 
3) via MQTT call

**Telnet update**
U maakt connectie met de dongle via een Telnet client. 
Afhankelijk van uw firmware versie is de handeling iets anders.

Firmware versies 3.0.4 - 3.1.3: dient u achter de U direct de  update versie te typen. Deze kunt u in Github vinden bij releases. Op het moment van schrijven is dat 3.2.2. Bijvoorbeeld `U 3.2.2 <return>`
Firmware versies 3.1.4 - en verder : Eerst dient de U van Update ingetypt te worden gevolgd door de return. Daarna dient het versienummer ingevoegd te worden. Dan wordt de volgorde `U <return> 3.2.2<return>`

Vanaf 3.1.4 versie is ook Filesysteem update mogelijk via telnet. Dit door S <return> versie <return> in te voeren. 
Standaard zal er geupdate worden van de ota.smart-stuff.nl directory. Deze verwijzing kan worden aangepast in de settingsfile op de dongel. 

**Webinterface update**
Via de webinterface kan er bij het wieltje voor Reset & update gekozen worden. Dan wordt een Update scherm zichtbaar.
Via dit scherm kunnen Firmware en de filestructuur vernieuwd worden. Dit doet u om eerst een update versie te downloaden en deze daarna als upload aan te bieden aan de dongle.

Er is ook een mogelijkheid om via de webbrowser zonder upload een update te doen. Dit kan door in de browser balk direct achter de hoofd URL  `/remote-update?version=3.2.2` in te voeren. Bijvoorbeeld: `http://dsmr-api/remote-update?version=3.2.2`

**MQTT update**
Als er een MQTT broker is geïnstalleerd kan de update ook geïnitieerd worden vanuit een andere MQTT client. Dit door in een MQTT client het volgende te posten:

Topic : <DONGLE TOPIC>/update -> in de standaard configuratie is dat: `DSMR-API/update`
Payload: <versie nummer> bijvoorbeeld : `3.2.2`

**Aanpassen ota url**
Indien je een eigen ota server hebt kan je de remote update url in de DSMRsettings.json aanpassen naar wens.
>Let op: er kan alleen verwezen worden naar http servers.