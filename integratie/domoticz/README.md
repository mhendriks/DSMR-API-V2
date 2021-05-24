# Domoticz integratie
Om een koppeling te maken met Domoticz dienen de volgende stappen doorlopen te worden
1) ga naar je 'Domoticz/scripts/lua folder' en kopier het ...dsmr-logger.lua script hier in
2) pas daarin local DSMR_IP = "dsmr-api.local" indien de hostname van de p1 dongel gewijzgd is
3) pas de link naar de json.lue aan in regel 70 (json = ...) op basis van de plek waar deze staat. Normaal gesproken staat deze ook in de 'Domoticz/scripts/lua folder'. 
4) in de lua staat nu een minimale set aan informatie aan. Deze kan worden uitgebreid door "--" weg te halen voor het item wat getoond dient te worden
5) herlaad de Domoticz configuratie na aanpassen

**Veel plezier met de integratie!**