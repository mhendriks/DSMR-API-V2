# Slimmemeter P1 Dongel - DSMR-API - Hardware versie 3.1
Eigenschappen
- ESP-12S (4MB) als CPU Soc -> veel pullup weerstanden zijn al aanwezig waardoor het aantal extra componenten laag kan blijven
- gebruikt secundaire seriale interface (UART1)
- P1 inversie via transistor, 1 weerstand minder werkt ook prima; Software serial is niet betrouwbaar genoeg
- RJ11 socket voor gemakkelijke aansluiting
- usb micro connector voor externe voeding (DSMR 4.x en sommige 5.x meters)
- Power MUX om automatisch van voedingsbron te wisselen. USB is primair.
- Primaire functie op een kant van de pcb zodat deze in oven/hotplate gemaakt kan worden
- Secundaire componenten (MUX/usb) zitten op de onderkant
- 6 pin aansluiting aan de zijkant voor eenvoudige flashing
- nieuwe spanningsregelaar die een nog lagere spanningsval, meer stroom aan kan en al met al minder warmte af geeft
- GEEN optocoupler meer ... DTR signaal is altijd hoog. Werkt ook prima ... echter met DTR komen er minder telegram errors voor. In de praktijk ca 0,1% errors is acceptabel

## SCHEMA
Op het printje zitten de volgende modules:
- signaal inverter om het P1 signaal te inverteren (level shifter)
- spanningsregulator voor het naar 3.3Volt brengen van de P1 spanning.
- ESP-12S CPU SoC
- Power Mux

Alle modules samen zie je in het onderstaande schema.
![Kicad schema](hardware/v3.1-kicad-schema.png) 


Omgezet naar een board ziet dit er zo uit:
Bovenkant             |  Onderkant |  Eindresultaat
:-------------------------:|:-------------------------:|:-------------------------:
![hardware bovenkant](hardware/v3.1-print-boven.png)  |  ![hardware onderkant](hardware/v3.1-print-onder.png) | ![hardware onderkant](hardware/v3.1-eindresultaat.png)

Afmeting van de print is ..........

## SOFTWARE
Op basis van de DSMR API software van Willem aandeWiel is eerder al een doorvertaling gemaakt naar specifieke code voor deze hardware. Als je wilt zou ook Tasmota of andere firmware gebruikt kunnen worden.
- DSMR API firmware : Json API, MQTT + web user interface (op basis van de Willem AandeWiel oplossing) [setup/dsmr-api](setup/dsmr-api/README.md)

Aanpassing ten opzichte van DSMR-API van Willem zijn:
- alle statische pagina's komen uit een CDN (esp modules hebben maar een beperkte capaciteit en zijn geen hele goede webservers;)
- alle plaatjes zijn nu iconen geworden, ook van cdn
- files zijn omgezet naar Json zodat dit makkelijk te onderhouden is en compacter wordt
- json API communicatie is ook gewijzigd (compacter en als een burst ipv gesegmenteerd)
- opmaak is zo veel als mogelijk uit de html / js files gehaald en in de css gestopt
- voor de extra input (deurbel) is functionele uitbreiding nodig (work in progress)
- nieuw dashboard gemaakt
- 
Instructie is te vinden in de [setup/dsmr-api](setup/dsmr-api/README.md) folder.

# Hardware maken of aanschaffen
Je kan je eigenhardware maken of deze aanschaffen. Wil je deze aanschaffen dan kan je mij altijd een DM sturen. De oplossing is ook Plug-and-play te koop inclusief firmware.