# **Frontend / webinterface**
De frontend kan op diverse manier worden aangepast na de behoefte en configuratie van de slimme meter.

##Wat kan is er aan te passen
Onderstaande waardes zijn aan te passen.
1. ShowVoltage: weergeven van de spanningsmeter op dashboard [true = geen teruglevering = aan,  teruglevering = uit, false = altijd uit]
2. Injection: weergeven van teruglevering [false = bepaald door de software, true - altijd aan]
3. Phases: aantal fases van je systeem [0 = bepaald door software, 1 = 1 fase, 2 = 2 fases etc]
4. Fuse: zekeringswaarde [ 1 fase systeem heeft meestal 35A, 3 fase meestal 25A]
5. GasAvailable: is er een gasmeter aangesloten [false = bepaald door software, true = altijd aan]
6. HideInitial: **is in ontwikkeling** : verbergen van de eerste waarde zodra het systeem voor het eerst gebruikt word

##Hoe kan je dit doen?
- Ga naar bestandsbeheer (folder icoon rechtsboven in menu)
- download Frontend.json
- bewerk het bestand met een notepad of andere simpele editor
- upload het bestand via bestandsbeheer

Hieronder de standaard gegevens van dit bestand

`{"ShowVoltage":true,"Injection":false,"Phases":0,"Fuse":25,"cdn":true,"GasAvailable":false,"HideInitial":false}`