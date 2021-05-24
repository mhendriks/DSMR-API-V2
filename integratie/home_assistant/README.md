# Home Assistant integratie
HA maakt gebruik van de MQTT koppeling om de data vanuit de P1 dongel te ontvangen.
Om een koppeling te maken met Domoticz dienen de volgende stappen doorlopen te worden.
1) neem onderstaande code op in de `configuratie.yaml` file (username en wachtwoord dienen alleen ingevuld te worden indien deze ook gebruikt worden. anders verwijderen)

```group:  !include_dir_merge_named groups/
script: !include scripts.yaml
sensor: !include_dir_merge_list sensors/
switch: !include_dir_merge_list switches/
light:  !include_dir_merge_list lights/
camera: !include_dir_merge_list cameras/

mqtt:
  broker: <ip-adres-MQTT-broker>
  port: 1883
  client_id: HassIO
  keepalive: 60
  username: <user name>
  password: <password>
  ```
2) pass broker adres aan aan het ipadres van de MQTT broker
3) voeg de folders + yaml bestanden toe aan de folder waarin ook de configuration.yaml staat
4) configureer gebruikersinterface 

**Veel plezier met de integratie!**