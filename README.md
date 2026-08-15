# homee-api
Dieses Projekt bietet die Möglichkeit, das Smarthome-System homee mit einem ESP32 / ESP8266 Controller zu verbinden.
Es basiert auf dem ursprünglichen Projekt von Daniel Knoop in der Version 0.2.7 (https://github.com/DanielKnoop/homee-api-esp32/tree/c4b5c123364566e4ccfca667581b4595b9305d7e) und wurde in einigen Punkten aktualisiert (neue Libraries) und leicht erweitert. Dazu kam u.a. Claude Code zum Einsatz.

> ⚠️ **Hinweis:** Die ESP8266-Version ist in der aktuellen Fassung **NICHT getestet**!

> ℹ️ **Zur Versionsnummer:** Dieser Fork springt bewusst auf `10.1.0`, statt die ursprüngliche 0.x-Zählung fortzuführen. Der große Sprung dient ausschließlich dazu, ausreichend Abstand zur Versionsnummer des Original-Projekts zu schaffen und Verwechslungen zu vermeiden (Daniel Knoops Original läuft unabhängig davon weiter und trägt zeitweise dieselben 0.x-Nummern). Die Zahl **10** ist **kein** Hinweis auf Reife, Stabilität oder Funktionsumfang dieser Library.

Mit Hilfe der Lib ist es daher möglich selbst entwickelte Sensoren und Sensorsysteme an homee anzubinden.
Verschiedene generische Beispiele finden sich in den Branches dieses Projektes: https://github.com/Oxi75/Vhih_ESP32_examples

Außerdem wurden z.B. folgende Projekte damit umgesetzt:
Fußbodenheizungssteuerung: https://github.com/Oxi75/floorHeatingController
EV1527 basierender Fenstersensor: https://github.com/Oxi75/WindowSensor_EV1527
Erweiterung eine Velux-Fernbedienung: https://github.com/Oxi75/VELUX_RemoteCtrl


Die API gliedert sich im Kern in drei Teile.
Das API Objekt selbst, Nodes und Attributes.

Zunächst in es notwendig, die homee-Api zu erstellen (den virtualHomee).

```cpp
#include "virtualHomee.hpp"
#include "virtualHomee/homee_defines.h"  // optional: Enums für Profile/Attribut-Typen
#include "virtualHomee/homee_icons.h"    // optional: Konstanten für node::setImage()
virtualHomee vhih;
```

Diesem virtualHomee müssen Nodes hinzugefügt werden (Vorzugsweise im Bereich Setup).

```cpp
node* n1 = new node(10, CANodeProfileTemperatureAndHumiditySensor, "Luftsensor"); // optional, entspricht der Zahl 3001
n1->setImage(NodeIconTemperature); // optional, entspricht dem String "nodeicon_temperature"
vhih.addNode(n1);
```
Der Konstruktor vom Node nimmt dabei folgende Attribute entgegen: NodeId, Profile, Name

Diesem Node müssen noch Attribute zugeordnet werden. 

```cpp
    na1 = n1->AddAttributes(new nodeAttributes(CAAttributeTypeTemperature)); // optional, entspricht der Zahl 5
    na1->setUnit("°C");
    na1->setMinimumValue(-20);
    na1->setMaximumValue(60);
```

Die maximale Anzahl an Nodes und Attributen ist maßgeblich vom Arbeitsspeicher des Boards abhängig. Jedes Node und jedes Attribut belegt knapp 500 Byte im Arbeitsspeicher. Es müssen mindestens 2 kB für dynamischen Speicher wie Sendebuffer etc. verfügbar bleiben.

Im loop bekommen die Attribute dann ihre Werte. Es empfiehlt sich daher, die Nodes im globalen Bereich zu speichern.

```cpp
nodeAttributes* na1;
```

```cpp
unsigned long previousMillis = 0;
const long interval = 60000; 

void loop()
{
  //Wir dürfen kein delay oder so verwenden, da hierdurch die Kommunikation mit dem homee gestört werden kann.
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    na1->setCurrentValue(random(-20, 60)); //schreibt einen zufälligen Wert zwischen -20 und +60
    vhih.updateAttribute(na1);
  }
}
```

Es ist auch möglich, Werte vom homee zu empfangen. Hierzu kann dem Attribute eine Callback-Funktion mitgegeben werden, welche gerufen wird, wenn der homee einen neuen Wert an das Attribut sendet.

```cpp
void setup()
{
  node* n2 = new node(20, CANodeProfileOnOffPlug, "Schalter");
  schalterAttribute = n2->AddAttributes(new nodeAttributes(CAAttributeTypeOnOff));
  schalterAttribute->setEditable(1);
  schalterAttribute->setMinimumValue(0);
  schalterAttribute->setMaximumValue(1);
  schalterAttribute->setCallback([](nodeAttributes* a)
  {
    a->setCurrentValue(a->getTargetValue());
    vhih.updateAttribute(a);
  });
}
```

Wenn der vhih fertig eingerichtet ist muss er noch mit 
```cpp
vhih.start();
```
gestartet werden.

Um den Sensor zum homee hinzuzufügen sucht man in der homee-app unter Geräte hinzufügen nach "homee in homee" und wählt dort "mit homee verbinden" aus. Hier gibt man die IP des ESP ein sowie irgendein Benutzernamen und Kennwort. Die Zugangsdaten werden vom vhih nicht geprüft, daher sind diese nicht relevant.

## Abhängigkeiten
Die Library ist für ESP8266 und ESP32 Boards ausgelegt.

### ESP8266
Folgende Abhängigkeiten benötigt der ESP8266
* ArduinoJson ^7.3.1 (bblanchon)
* ESPAsyncWebServer ^3.7.2 (ESP32Async)
    * ESPAsyncTCP ^3.1.5 (ESP32Async)
* ESPAsyncUDP (https://github.com/DanielKnoop/ESPAsyncUDP)

### ESP32
Folgende Abhängigkeiten benötigt der ESP32
* ArduinoJson ^7.3.1 (bblanchon, über die Paketverwaltung)
* ESPAsyncWebServer ^3.7.2 (ESP32Async)
    * AsyncTCP ^3.3.6 (ESP32Async)


## Hinweise

Das Projekt ist angelehnt an die node-red Implementierung [node-red-contrib-homee](https://github.com/stfnhmplr/node-red-contrib-homee)
von stfnhmplr. Vielen Dank, auch an alle anderen Vorreiter aus dem [homee-Forum](https://community.hom.ee).

Ich stehe in keiner Verbindung zur homee GmbH oder zu codeatelier GmbH
