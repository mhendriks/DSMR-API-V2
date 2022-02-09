# NEW

Nu ook native ESPHOME - Home Assistant integratie.
Inclusief hardware serial ondersteuning.

Er zijn hiermee meerdere keuzes mogelijk met deze hardware, namelijk:
1. DSMR-API software: met eigen opslag en web interface. ook via mqtt te koppelen met Home Assistant of andere smart home oplossingen Zie [setup/dsmr-api](setup/dsmr-api/README.md)
2. ESPHome: voor naadloze integratie met Home Assistant. Zie [setup/epshome](setup/esphome/README.md)
3. Tasmota: in ontwikkeling Zie [setup/tasmota](setup/tasmota/README.md)

# Slimmemeter P1 Dongel - DSMR-API - Hardware versie 3.4
Eigenschappen
- hardware design voor ESPhome
- ESP-12S (4MB) als CPU Soc -> veel pull-up weerstanden zijn al aanwezig waardoor het aantal extra componenten laag kan blijven
- gebruikt secundaire seriale hardware interface (UART1)
- P1 signaal inversie via transistor
- 6P6C (RJ12) socket voor gemakkelijke aansluiting
- usb micro connector voor externe voeding. Sommige DSMR 5.x en de DSMR 4.x/3.x/2.x leveren geen of te weinig vermogen
- 2 diodes voor de USB/P1 switch
- Primaire componenten op bovenkant van de pcb zodat deze in oven/hotplate gemaakt kan worden
- nieuwe LDO met een nog lager verlies. Resultaat minder warmteafgifte en hogere betrouwbaarheid
- Jumper om te kiezen tussen P1 of USB voedingsbron
- DTR signaal is altijd hoog
- watermeter sensor logica op basis van spannningsdeler
- SMT model en daarom alle basis logica op bovenzijde

## SCHEMA
Op het printje zitten de volgende modules:
- signaal inverter om het P1 signaal te inverteren (level shifter)
- spanningsregulator voor het naar 3.3Volt brengen van de P1 spanning.
- ESP-12S CPU SoC

Alle modules samen zie je in het onderstaande schema.
![Kicad schema](.github/images/v3.4-kicad-schema.png) 

Omgezet naar een pcb ziet dit er zo uit:

Bovenkant             |  Onderkant 
:-------------------------:|:-------------------------:
<img src=".github/images/v3.4-print-boven.png" width="50%"> |  <img src=".github/images/v3.4-print-onder.png" width="50%"> 

Afmeting van de print is 33 mm * 20 mm

## SOFTWARE
Op basis van de DSMR API software van Willem aandeWiel is eerder al een doorvertaling gemaakt naar specifieke code voor de V2 en V3 hardware. Voor de V3.3 versie zijn er wederom aanpassingen gedaan met name vanwege het ontbreken van het DTR signaal. De code is hiervoor geoptimaliseerd.
Naar behoefte zou ook Tasmota of andere firmware gebruikt kunnen worden, dit is op de V3.3 versie verder niet uitgeprobeerd.

Aanpassing (tot nu toe) ten opzichte van DSMR-API van Willem zijn:
- alle statische pagina's komen uit een CDN (esp modules hebben maar een beperkte capaciteit en zijn geen hele goede webservers;)
- alle plaatjes zijn  iconen geworden en komen van een cdn
- files zijn omgezet naar Json zodat dit makkelijk te onderhouden en compacter is
- json API communicatie is ook gewijzigd (compacter en als een burst ipv gesegmenteerd)
- opmaak is zo veel als mogelijk uit de html / js files gehaald en in de css gestopt
- nieuw dashboard gemaakt
- Front-end settings worden van de dongel gelezen (een eerste aanzet)
- LittleFS in plaats van SPIFFS als bestandssysteem

Instructie is te vinden in de [setup/dsmr-api](setup/dsmr-api/README.md) folder.

# Hardware maken of aanschaffen
Je kan je eigen hardware maken of deze aanschaffen. Wil je deze aanschaffen neem dan een kijkje op </a>&nbsp;<a href="https://smart-stuff.nl" target="_blank">smart-stuff.nl</a>