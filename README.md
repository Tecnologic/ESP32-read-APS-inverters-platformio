# ESP32-read-APS-inverters – PlatformIO-Port für Wemos D1 Mini32 & ESP32-C3 SuperMini

Portierung von [patience4711/ESP32-read-APS-inverters](https://github.com/patience4711/ESP32-read-APS-inverters)
(Arduino-IDE Multi-Tab-Sketch, Version `ESP32_ECU_v1_4`) nach PlatformIO.
Zwei Zielboards in einer `platformio.ini`:

- `env:wemos_d1_mini32` – klassischer ESP32 (Xtensa, Dual-Core)
- `env:esp32-c3-supermini` – ESP32-C3 SuperMini (RISC-V, Single-Core), gedacht
  für den [Antennen-Mod von Peter Neufeld](https://peterneufeld.wordpress.com/2025/03/04/esp32-c3-supermini-antenna-modification/)
  (gelötete λ/4-Drahtantenne – reine HF-Modifikation, keine GPIO-Auswirkung,
  daher hier ohne weitere Codeänderung nutzbar)

Bauen:
```bash
pio run -e wemos_d1_mini32       # klassischer ESP32
pio run -e esp32-c3-supermini    # C3 SuperMini
```


## Projektstruktur

```
platformio.ini
include/board_pins_wemos_d1_mini32.h    <- zentrale Pin-Belegung klassischer ESP32
include/board_pins_esp32c3_supermini.h  <- zentrale Pin-Belegung ESP32-C3 SuperMini
src/main.ino                            <- Hauptdatei (setup()/loop()), vormals ESP32_ECU_v1_4.ino
src/*.ino, src/*.h                      <- restliche Original-"Tabs" des Sketches, in snake_case umbenannt
```

Alle Dateinamen wurden von den ursprünglichen Arduino-IDE-Tab-Namen (u. a. mit
`AAA_`-Präfixen, die nur der alphabetischen Tab-Sortierung dienten, oder
CamelCase wie `ASYSERVER.ino`) auf sprechende `snake_case`-Namen umgestellt,
z. B. `AAA_INCLUDES.h` → `includes.h`, `ESP32_ECU_v1_4.ino` → `main.ino`,
`ASYSERVER.ino` → `async_server.ino`, `handeforms.ino` (Tippfehler im
Original) → `handle_forms.ino`. Die `AAA_`-Präfixe waren in PlatformIO ohnehin
wirkungslos, da hier nicht die Dateireihenfolge, sondern die automatische
Erkennung der Datei mit `setup()`/`loop()` entscheidet (siehe unten).

Alle `.ino`-Tabs und die zugehörigen `.h`-Header liegen unverändert (bis auf
die unten aufgeführten, notwendigen Anpassungen) in `src/`. PlatformIO
erkennt automatisch, welche `.ino`-Datei `setup()`/`loop()` enthält
(`main.ino`) und stellt sie beim Zusammenbau an den Anfang – exakt
das Verhalten, das die Arduino-IDE beim Öffnen des Sketches auch zeigen
würde. Die alte, im Repo mitgelieferte Vorversion `ESP32-ECU_v0-9.ino`
(eigenständiger Sketch mit eigenem `setup()/loop()`) wurde **nicht**
übernommen, da sie sonst zu doppelt definierten Symbolen führen würde.

## ESP32-C3 SuperMini – warum ein zweiter Pin-Header nötig war

Der C3 ist kein Drop-in-Ersatz für den klassischen ESP32, drei echte
Unterschiede mussten im Code (nicht nur in der `platformio.ini`) berücksichtigt
werden:

| Punkt | Klassischer ESP32 (D1 Mini32) | ESP32-C3 SuperMini |
|---|---|---|
| UARTs | 3 (UART0/1/2) → `Serial2` existiert | nur 2 (UART0/1) → **kein** `Serial2` im Framework |
| Zigbee-UART-Pins | GPIO16/17 | GPIO16/17 nicht herausgeführt (intern für Flash) → **GPIO20/21** |
| Onboard-LED | GPIO2, active-HIGH | **GPIO8**, active-LOW |
| Onboard-BOOT-Taster | GPIO0 | **GPIO9** |
| Reset-Pin Zigbee-Modul | GPIO5 | **GPIO10** |
| Extra-Ausgang | GPIO4 | **GPIO3** |

Da es kein `Serial2` auf dem C3 gibt, habe ich alle `Serial2.xxx()`-Aufrufe im
Code (in `main.ino`, `helpers.ino`, `zigbee_helpers.ino`, `test.ino`)
durch ein board-unabhängiges `ZB_SERIAL` ersetzt:
- auf dem klassischen ESP32 ist `ZB_SERIAL` nur ein `#define` auf `Serial2`
- auf dem C3 wird `ZB_SERIAL` als eigenes `HardwareSerial(1)`-Objekt auf
  GPIO20/21 instanziiert

Welcher Pin-Header (`include/board_pins_wemos_d1_mini32.h` bzw.
`include/board_pins_esp32c3_supermini.h`) verwendet wird, steuert das Build-Flag
`-DBOARD_ESP32C3_SUPERMINI`, das in `platformio.ini` nur für
`env:esp32-c3-supermini` gesetzt ist.

**AsyncTCP/RISC-V geprüft:** Die verwendete `dvarrel/AsyncTCP`-Bibliothek nutzt
intern `xTaskCreateUniversal(..., CONFIG_ASYNC_TCP_RUNNING_CORE)` mit
`CONFIG_ASYNC_TCP_RUNNING_CORE = -1` ("beliebiger Core") – **kein**
hartkodiertes `xTaskCreatePinnedToCore(..., 1)`, was auf dem Single-Core-C3
sonst zuverlässig mit `assert failed: Invalid core ID` abstürzen würde. Das
Projekt selbst pinnt ebenfalls keine Tasks. Beide Bibliotheken (`AsyncTCP`,
`ESPAsyncWebSrv`) laufen also single-core-sicher.

**Flash-Erase-Hinweis für den C3 (native USB):** Der SuperMini nutzt USB-CDC
statt eines externen CH340/CP210x. Meist reicht `pio run -t upload` ohne
manuelles Bootmode-Gefummel; falls das Board nicht angesprochen wird, BOOT
(GPIO9) beim Einstecken/Reset gedrückt halten.

## Was wurde angepasst

### 1. Pins (wie gefordert)
Alle vom Original genutzten GPIOs (0, 2, 4, 5, 16, 17) sind auf dem
Wemos D1 Mini32 vorhanden und nutzbar – anders als z. B. bei manchen anderen
Formfaktoren mussten die Pin-*Nummern* also nicht geändert werden. Damit die
Belegung aber wirklich board-spezifisch und nicht zufällig richtig ist, habe
ich sie aus dem Fließtext (`includes.h`, mehrere hartkodierte `4`en) in
`include/board_pins_wemos_d1_mini32.h` zentralisiert:

| Funktion              | GPIO | D1-Mini32-Silkscreen | Hinweis |
|------------------------|------|----------------------|---------|
| `knop` (Config-Taster)| 0    | D3 / BOOT             | On-Board-BOOT-Taster |
| `led_onb` (LED)        | 2    | LED_BUILTIN            | blaue On-Board-LED |
| `ZB_RESET`             | 5    | D5                     | Reset-Leitung CC2530/CC2531 |
| `RXD2` (UART2 RX)      | 16   | RX2                    | vom Zigbee-Modul |
| `TXD2` (UART2 TX)      | 17   | TX2                    | zum Zigbee-Modul |
| `EXTRA_OUT_PIN`        | 4    | D4                     | zusätzlicher Ausgang, wird nach dem Boot LOW gesetzt |

**Wichtig, bitte am eigenen Board prüfen:** Die On-Board-LED (GPIO2) ist bei
den originalen ESP32-Dev-Boards active-HIGH. Manche D1-Mini32-Klone verbauen
sie active-LOW (wie beim ESP8266-D1-mini). Falls die LED invertiert
leuchtet, in `board_pins_wemos_d1_mini32.h` einfach
`WEMOS_D1_MINI32_LED_ACTIVE_LOW` auf `1` setzen.

### 2. Echter Bug im Original behoben
`handle_data.ino` (ehemals `handledata.ino`) enthielt einen Tippfehler mit fehlender schließender
Klammer:
```cpp
consoleOut("handleDataRequest the request is";   // Original – kompiliert nicht
consoleOut("handleDataRequest the request is");  // korrigiert
```
Das ist kein Portierungsfehler, sondern steckt so im Original-Repository
(Stand des geladenen `main`-Branches) und hätte den Build auch in der
Arduino-IDE zum Absturz gebracht.

### 3. Tote/unklare Abhängigkeit entfernt
`#include "PSACrypto.h"` wurde eingebunden, aber im gesamten Code nirgends
verwendet (kein einziger `psa_*`-Aufruf). Da die Datei im Repo auch gar nicht
mitgeliefert wird, habe ich den Include entfernt statt eine ungenutzte
Fremdbibliothek als Abhängigkeit mitzuschleppen. Ebenso `Async_TCP.h`
(lokale Kopie im Repo) – sie wird nirgends `#include`t, die tatsächlich
verwendete `AsyncTCP.h` kommt aus der unten genannten Bibliothek.

## Abhängigkeiten (alle geprüft, `platformio.ini` → `lib_deps`)

| Include im Code                              | Bibliothek                                                        |
|-----------------------------------------------|---------------------------------------------------------------------|
| `ESPAsyncWebSrv.h`, `AsyncEventSource.h`       | [dvarrel/ESPAsyncWebSrv](https://github.com/dvarrel/ESPAsyncWebSrv) (Fork von ESPAsyncWebServer, der die Header bewusst in `ESPAsyncWebSrv.h` umbenennt – genau das, was der Originalcode inkludiert) |
| `AsyncTCP.h`                                   | [dvarrel/AsyncTCP](https://github.com/dvarrel/AsyncTCP) (von ESPAsyncWebSrv als Abhängigkeit vorausgesetzt) |
| `ArduinoJson.h` (`JsonDocument`-API = v7)      | [bblanchon/ArduinoJson](https://github.com/bblanchon/ArduinoJson) `^7` |
| `PubSubClient.h`                               | [knolleary/PubSubClient](https://github.com/knolleary/pubsubclient) `^2.8` |
| `NTPClient.h`                                  | [arduino-libraries/NTPClient](https://github.com/arduino-libraries/NTPClient) `^3.2.1` |
| `TimeLib.h`                                    | [PaulStoffregen/Time](https://github.com/PaulStoffregen/Time) `^1.6.1` |
| `sunMoon.h`                                    | [sfrwmaker/sunMoon](https://github.com/sfrwmaker/sunMoon) (nicht in der PlatformIO-Registry gelistet, daher per Git-URL eingebunden) |
| `WiFi.h`, `esp_wifi.h`, `DNSServer.h`, `Update.h`, `SPIFFS.h`, `FS.h`, `EEPROM.h`, `Preferences.h`, `Ticker.h`, `esp_task_wdt.h`, `rtc_wdt.h`, `soc/*.h` | Teil des `framework = arduino` (arduino-esp32 Core), keine zusätzliche Abhängigkeit |

## Bauen

```bash
pio run -e wemos_d1_mini32
```

Flashen:

```bash
pio run -e wemos_d1_mini32 -t upload
```

Serial Monitor (115200 Baud, wie im Code per `Serial.begin(115200)`):

```bash
pio device monitor -b 115200
```

## Wichtiger Hinweis zur Build-Verifikation

Ich habe das Projekt so weit wie in dieser Sandbox möglich geprüft:

- **Statische Vollständigkeitsprüfung**: alle Klammern/Blöcke in jeder
  Quelldatei sind ausbalanciert (Skript-Check über alle `.ino`/`.h`-Dateien),
  dabei wurde der oben genannte reale Tippfehler in `handle_data.ino`
  gefunden und behoben.
- **Alle `#include`s einzeln aufgelöst** und einer konkreten, tatsächlich
  existierenden Bibliothek (mit passender Header-Datei) zugeordnet – siehe
  Tabelle oben. Keine der Abhängigkeiten ist "geraten".
- **Pin-Verfügbarkeit** für den Wemos D1 Mini32 anhand des Boards
  gegengeprüft (alle genutzten GPIOs sind dort herausgeführt).
- **Merge-Verhalten der `.ino`-Tabs** über PlatformIOs eigenen
  `InoToCPPConverter` nachvollzogen: er erkennt automatisch die Datei mit
  `setup()`/`loop()` und stellt sie voran, exakt wie die Arduino-IDE.

Ein **tatsächlicher `pio run`-Compile-Lauf** (Xtensa-Toolchain +
arduino-esp32-Framework herunterladen und den Code damit übersetzen) lässt
sich in dieser Sandbox aber nicht abschließen: PlatformIOs Paket-Downloader
bricht beim Herunterladen der (mehrere hundert MB großen) Toolchain-/
Framework-Archive von GitHub-Releases mit einem TLS-Zertifikatsfehler ab,
der spezifisch an der Netzwerk-Sandbox dieser Umgebung liegt (git/`curl`/
`raw.githubusercontent.com`-Zugriffe funktionieren einwandfrei, der
PlatformIO-Downloader bei großen Release-Assets nicht zuverlässig). Das ist
kein Problem des Projekts selbst.

**Bitte einmal `pio run -e wemos_d1_mini32` bei dir lokal (normales
Internet, kein Sandbox-Proxy) laufen lassen** – dort sollte der Download
klappen. Falls dabei doch noch etwas auffällt (z. B. eine neue
Bibliotheksversion mit brechenden API-Änderungen), sag Bescheid, dann fixe
ich das gezielt nach.
