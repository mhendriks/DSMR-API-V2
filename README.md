# Slimmemeter MQTT interface - DSMR-API - Hardware Versie 2
Gebaseerd op versie 1 met een aantal verbeteringen en aanvullingen
- ESP-M2 in plaats van M3
- gebruikt secundaire seriale interface (sneller en geen extra library meer nodig)
- 2 SMD optocouplers; 1 voor de P1 en een extra voor externe input (bijvoorbeeld deurbel)
- iets compacter

## SCHEMA
Gekozen voor iets compactere ESP-M2 (ESP8285) vooral omdat de secundaire seriele interface van de ESP bereikbaar is op de M2 en niet op de M3. 
Op het printje zitten de volgende modules:
- signaal inverter om het P1 signaal te inverteren
- optocoupler voor het aansturen van de P1 (data request)
- optocoupler voor extra input; omdat er vaak een deurbel in de buurt van de slimme meter zit is het idee om de deurbel input te gebruiken
- spanningsregulator voor het naar 3.3Volt brengen van de P1 spanning.
- de ESP-M2 natuurlijk

Alle modules samen zie je hieronder.
![Kicad schema](hardware/v2-kicad-schema.png) 

Omgezet naar een board ziet dit er zo uit:
Bovenkant             |  Onderkant |  Eindresultaat
:-------------------------:|:-------------------------:|:-------------------------:
![hardware bovenkant](hardware/v2-print-boven.png)  |  ![hardware onderkant](hardware/v2-print-onder.png) | volgt nog)

Afmeting van de print is 20 x 37mm

## SOFTWARE
Er is veel software online te vinden. Keuze voor de gebruiker vind ik belangrijk daarom gekozen voor twee scenario's, namelijk:
- Tasmota Out of the box (MQTT)
- Json API + User interface (Willem AandeWiel REST-API oplossing)

### Tasmota
Zelf ben ik een groot fan van de Tasmota software voor de ESP8266 familie. Deze is dan ook als eerste gebruikt om de oplossing werkend te krijgen. Tasmota kan out of the box op de module geflasht worden. Instructie is te vinden in de [Tasmota](Tasmota) folder.

### REST-API DSMR Logger van Willem AandeWiel
Veel dank aan Willem AandeWiel voor zijn oplossing. Deze oplossing is als basis genomen en diverse veranderingen aan doorgevoerd. Oplossing van Willem is gemaakt voor een 4MB esp module en de M3 heeft er maar 1. Meer dan genoeg als alle extra functionaliteit verwijderd wordt en de overige compacter wordt gemaakt.
Aanpassingen zijn:
- omdat de ESP-M3 geen secundaire seriele interface heeft die te gebruiken is is er een software oplossing gebruikt (met dank aan Tasmota); In de basis is de oplossing van Willem te gebruiken met een kleine aanpassing voor deze seriele interface.
- alle statische pagina's komen uit een CDN (esp modules hebben maar een beperkte capaciteit en zijn geen hele goede webservers;)
- alle plaatjes zijn nu iconen geworden, ook van cdn
- files zijn omgezet naar Json zodat dit makkelijk te onderhouden is en compacter wordt
- json API communicatie is ook gewijzigd (compacter en als een burst ipv gesegmenteerd)
- opmaak is zo veel als mogelijk uit de html / js files gehaald en in de css gestopt (overigens nog niet af)