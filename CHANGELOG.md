# Changelog

Alle nennenswerten Änderungen an diesem Projekt werden in dieser Datei dokumentiert.
Das Format orientiert sich an [Keep a Changelog](https://keepachangelog.com/de/1.0.0/),
die Versionierung an [SemVer](https://semver.org/lang/de/).

## [10.1.0] - 2026-08-10

Erstes offizielles, getaggtes Release dieses Forks (`Oxi75/homee-api-esp32`, ausgehend von
`DanielKnoop/homee-api-esp32` v0.2.7). 

**Zur Versionsnummer:** Der Sprung auf `10.1.0` (statt Fortführung der ursprünglichen
0.x-Zählung) dient ausschließlich dazu, ausreichend Abstand zur Versionsnummer des
Original-Projekts zu schaffen und Verwechslungen zu vermeiden — Daniel Knoops Original
läuft unabhängig weiter und trägt zeitweise dieselben 0.x-Nummern. Die Zahl `10` ist
**kein** Hinweis auf Reife, Stabilität oder Funktionsumfang dieser Library.

### Added
- `src/virtualHomee/homee_defines.h`: `CANodeProfile...`- und `CAAttributeType...`-Enums,
  1:1 aus dem offiziellen [homee-api](https://github.com/stfnhmplr/homee-api) JS-SDK
  (`enums.js`, Stand v2.40.1) nach C++ konvertiert.
- `src/virtualHomee/homee_icons.h`: `NodeIcon...`-Konstanten für `node::setImage()`,
  aus Beobachtungen der homee-App zusammengetragen (inoffiziell, unvollständig).
- Setter/Getter für Node-Notiz (`node::setNote`/`getNote`) und Node-Status (`node::setState`/`getState`).
- `virtualHomee::removeNodeById()` zum gezielten Entfernen eines Nodes.

### Changed
- Migration von ArduinoJson v6 auf v7 (`^7.3.1`).
- Migration von ESPAsyncWebServer/AsyncTCP (me-no-dev) auf den aktiv gepflegten
  [ESP32Async](https://github.com/ESP32Async)-Fork (`ESPAsyncWebServer ^3.7.2`,
  `AsyncTCP ^3.3.6`, `ESPAsyncTCP ^3.1.5`).
- Vereinheitlichte Versionsnummer in `library.json` und `library.properties`
  (vorher inkonsistent: `0.3.0` vs. `0.3.1 Oxi`), jetzt bewusst `10.1.0`
  (siehe Hinweis zur Versionsnummer oben).
- Beispielcode (`src/main.cpp`) und README nutzen jetzt die neuen `CANodeProfile...`/
  `CAAttributeType...`-Enums statt unkommentierter Zahlen-Literale.
- README: Abhängigkeits-Versionen an `library.json` angeglichen.

### Fixed
- Diverse Stabilitätsverbesserungen und kleinere Bugfixes seit dem Fork
  (u. a. Umgang mit sich überlappenden Attribut-IDs, WebSocket-Server
  Start/Stop-Verhalten).

### Known Issues
- Die Migration auf ArduinoJson v7 und den ESP32Async-Fork von
  ESPAsyncWebServer/AsyncTCP wurde bisher ausschließlich auf ESP32 getestet.
  Die ESP8266-Unterstützung ist seitdem ungetestet und möglicherweise nicht
  funktionsfähig.

## Vor 10.1.0

Vor diesem Release wurde nie ein Git-Tag oder GitHub-Release erstellt; der Stand der
Library ergab sich ausschließlich aus dem `master`-Branch. Die vollständige Historie ist
über `git log` einsehbar.
