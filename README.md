# homee-api
Dieses Projekt ist eine Möglichkeit, das Smarthome-System homee mit einem ESP32 Controller zu verbinden.
Hierdurch ist es möglich, Sensoren und Aktoren auf Basis eines ESP32 und ESP8266 zu erstellen.

Ein Beispiel ist hier verfügbar: https://github.com/DanielKnoop/homee-co2-sensor

Die API gliedert sich im Kern in drei Teile.
Das API Objekt selbst, Nodes und Attributes.

Zunächst in es notwendig, die homee-Api zu erstellen (den virtualHomee).

```cpp
#include "virtualHomee.hpp"
#include "virtualHomee/homee_defines.h"
virtualHomee vhih;
```

Diesem virtualHomee müssen Nodes hinzugefügt werden (Vorzugsweise im Bereich Setup).

```cpp
node* n1 = new node(10, CANodeProfileTemperatureAndHumiditySensor, "Luftsensor");
vhih.addNode(n1);
```
Der Konstruktor vom Node nimmt dabei folgende Attribute entgegen: NodeId, Profile, Name

Diesem Node müssen noch Attribute zugeordnet werden. 

```cpp
    na1 = n1->AddAttributes(new nodeAttributes(CAAttributeTypeTemperature));
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

## Node-Profile, Attribut-Typen und Icons

Für die `profile`- und `type`-Parameter von `node` und `nodeAttributes` sowie für `node::setImage()` müssen normalerweise die von homee definierten numerischen IDs bzw. Icon-Strings von Hand nachgeschlagen werden. Die Library liefert dafür zwei Header mit fertigen Konstanten:

* `src/virtualHomee/homee_defines.h` — Enums `CANodeProfile...` (Node-Profile) und `CAAttributeType...` (Attribut-Typen), 1:1 aus dem offiziellen [homee-api](https://github.com/stfnhmplr/homee-api) JS-SDK übernommen.
* `src/virtualHomee/homee_icons.h` — `NodeIcon...`-Konstanten für den `image`-Parameter von `node::setImage()`, inoffiziell aus Beobachtungen der homee-App zusammengetragen.

```cpp
#include "virtualHomee/homee_defines.h"
#include "virtualHomee/homee_icons.h"

node* n1 = new node(10, CANodeProfileTemperatureAndHumiditySensor, "Luftsensor");
n1->setImage(NodeIconTemperature);
```

Details zu Herkunft und Lizenz stehen als Kommentar am Anfang der jeweiligen Datei.

## Hinweise

Das Projekt ist angelehnt an die node-red Implementierung [node-red-contrib-homee](https://github.com/stfnhmplr/node-red-contrib-homee)
von stfnhmplr. Vielen Dank, auch an alle anderen Vorreiter aus dem [homee-Forum](https://community.hom.ee).

Ich stehe in keiner Verbindung zur homee GmbH oder zu codeatelier GmbH
