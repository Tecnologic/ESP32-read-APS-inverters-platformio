# ESP32-read-APS-inverters - PlatformIO port for Wemos D1 Mini32 and ESP32-C3 SuperMini

This repository is a PlatformIO port of [patience4711/ESP32-read-APS-inverters](https://github.com/patience4711/ESP32-read-APS-inverters), originally an Arduino IDE multi-tab sketch based on version `ESP32_ECU_v1_4`.

Two build targets are provided in `platformio.ini`:

- `env:wemos_d1_mini32` - classic ESP32 (Xtensa, dual-core)
- `env:esp32-c3-supermini` - ESP32-C3 SuperMini (RISC-V, single-core), intended for the [antenna mod by Peter Neufeld](https://peterneufeld.wordpress.com/2025/03/04/esp32-c3-supermini-antenna-modification/) (soldered quarter-wave wire antenna; a pure RF modification with no GPIO impact, so it works here without additional code changes)

Build:

```bash
pio run -e wemos_d1_mini32
pio run -e esp32-c3-supermini
```

Flash:

```bash
pio run -e wemos_d1_mini32 -t upload
```

Serial monitor at 115200 baud, matching `Serial.begin(115200)` in the code:

```bash
pio device monitor -b 115200
```

## Project Structure

```text
platformio.ini
include/board_pins_wemos_d1_mini32.h    central pin map for the classic ESP32
include/board_pins_esp32c3_supermini.h  central pin map for the ESP32-C3 SuperMini
src/main.ino                            main file (setup()/loop()), formerly ESP32_ECU_v1_4.ino
src/*.ino, src/*.h                      remaining original sketch tabs, renamed to snake_case
```

The file names were renamed from the original Arduino IDE tab names, including `AAA_` prefixes that only served alphabetical tab sorting and CamelCase names such as `ASYSERVER.ino`, to descriptive `snake_case` names. Examples: `AAA_INCLUDES.h` became `includes.h`, `ESP32_ECU_v1_4.ino` became `main.ino`, `ASYSERVER.ino` became `async_server.ino`, and `handeforms.ino` (a typo in the original) became `handle_forms.ino`. The `AAA_` prefixes were ineffective in PlatformIO anyway, because file order is not what matters there; the file containing `setup()`/`loop()` is discovered automatically.

All `.ino` tabs and their associated `.h` headers remain in `src/` unchanged except for the necessary adjustments listed below. PlatformIO automatically detects which `.ino` file contains `setup()`/`loop()` (`main.ino`) and places it first during the build, which matches the Arduino IDE behavior when opening the sketch. The old bundled preview `ESP32-ECU_v0-9.ino` from the repo, which is a separate sketch with its own `setup()`/`loop()`, was intentionally not included because it would cause duplicate symbol definitions.

## Original Project Overview

This project reads out APS microinverters, specifically YC600, DS3, and QS1. It is a continuation of the older [ESP-ECU](https://github.com/patience4711/read-APSystems-YC600-QS1-DS3) project.

The main reason for moving to an ESP32-class device is that the ESP8266 did not provide enough program space for the necessary extensions. The ESP32 also has a second UART, which lets one port talk to the Zigbee module while the other remains available for serial debugging over USB. There is also much more heap available, which makes later expansion easier.

See the [wiki](https://github.com/patience4711/ESP32-read-APS-inverters/wiki) for build instructions, operating details, and background information.

## Status

The system has been tested in practice with YC600, QS1, and DS3 inverters and works reliably.

## Purpose

The system is intended to read APS Systems inverters. The program can pair and poll YC600, QS1, and DS3 inverters, up to 9 units. The read values are displayed on a web page and sent through Mosquitto in JSON format.

## Features

- Easy Wi-Fi setup
- Easy add, delete, and pair inverters
- Automatic polling or on-demand access via MQTT or HTTP
- Data can be requested via HTTP and Mosquitto
- Five different MQTT JSON formats
- Fast asynchronous web server
- Serial and web console for commands and debugging
- Smart timekeeping
- Lots of system information on the web page
- Easy firmware update over the air
- Maximum inverter output can be limited (throttling)

## Hardware

The hardware is simply an ESP32 device and a prepared CC2530 or CC2531 Zigbee module, plus a power supply. The Zigbee module should be flashed with the firmware developed by kadsol: [CC25xx_firmware](https://github.com/Koenkk/zigbee2mqtt/files/10193677/discord-09-12-2022.zip). The firmware is also available [here](https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/blob/main/cc25xx_firmware.zip). More background on the development of this software can be found in [zigbee2mqtt issue 4221](https://github.com/Koenkk/zigbee2mqtt/issues/4221).

For information on how to build and use it, please see the wiki.

## How It Works

APS uses its own Zigbee implementation. The ESP-ECU sends Zigbee commands wirelessly to the inverters and analyzes the responses to extract values. The ESP communicates with the Zigbee module through the alternative serial port over wires.

The ESP-ECU starts a coordinator, which is the entity that can start a Zigbee network. The coordinator binds the inverters and sends poll requests to them. The interesting values are sent via MQTT and displayed on the main page. The ECU sends a message that new data is available, and the web page reacts by requesting the fresh values.

See it in action on [YouTube](https://youtu.be/WKFVQ6d8KhQ).

## Changelog

Version `ESP32-ECU_V1_4`:

- Improved inverter throttling, working for YC600 and DS3
- Added an inverter query command via the console
- Added a Mosquitto input topic based on chip ID
- Throttling is possible via UI, HTTP request, or Mosquitto
- APIs can take a debug argument to show debug information

Version `ESP32-ECU_V1_2`:

- Added options to throttle the inverter, tested on YC600 and DS3

Version `ESP32-ECU_V1_1`:

- Adapted to a modern Arduino IDE (2.3.4) and board definitions (2.0.18 arduino5)
- Changed the SPIFFS save functions
- Changed the Wi-Fi connection portal

Version `ESP32-ECU_V0_9`:

- Fixed a bug in the HTML of the inverters page
- Introduced an improved debugging method

Version `ESP32-ECU_V0_8`:

- Fixed a bug related to the button behavior

Version `ESP32-ECU_V0_7`:

- More efficient browser/server communication (event driven)
- Minimized all web pages and JavaScripts
- Improved menu and browsing on the ECU website

Version `ESP32-ECU_V0_5`:

- More efficient use of memory
- Use of ArduinoJson

Version `ESP32-ECU_V0_4`:

- Removed string operations from the main processes
- Some web pages improved

Version `ESP32-ECU_V0_3b`:

- Some security updates for maintenance from outside the local network
- Fine-tuned the pairing process
- Redesigned the important processes to gain more free heap
- Some cosmetic changes and small bug fixes

Version `ESP32-ECU_V0_1d`:

- Replaced elegantOta with a custom implementation
- Fixed a bug in the pairing process
- Solved system crashes due to string operations

Version `ESP32-ECU_V0_1a`:

- Added a new front page with buttons for inverter details
- Removed the websocket console to reduce web server load
- Added a serial console for commands

## ESP32-C3 SuperMini - Why a Second Pin Header Was Needed

The C3 is not a drop-in replacement for the classic ESP32, so three real differences had to be handled in code, not just in `platformio.ini`:

| Point | Classic ESP32 (D1 Mini32) | ESP32-C3 SuperMini |
|---|---|---|
| UARTs | 3 (UART0/1/2) -> `Serial2` exists | only 2 (UART0/1) -> no `Serial2` in the framework |
| Zigbee UART pins | GPIO16/17 | GPIO16/17 not broken out (used internally for flash) -> GPIO20/21 |
| Onboard LED | GPIO2, active-HIGH | GPIO8, active-LOW |
| Onboard BOOT button | GPIO0 | GPIO9 |
| Zigbee module reset pin | GPIO5 | GPIO10 |
| Extra output | GPIO4 | GPIO3 |

Because the C3 has no `Serial2`, all `Serial2.xxx()` calls in the code were replaced with a board-independent `ZB_SERIAL`:

- on the classic ESP32, `ZB_SERIAL` is just a `#define` alias for `Serial2`
- on the C3, `ZB_SERIAL` is instantiated as its own `HardwareSerial(1)` object on GPIO20/21

The pin header that gets used (`include/board_pins_wemos_d1_mini32.h` or `include/board_pins_esp32c3_supermini.h`) is selected by the build flag `-DBOARD_ESP32C3_SUPERMINI`, which is only set for `env:esp32-c3-supermini` in `platformio.ini`.

**AsyncTCP/RISC-V checked:** the `dvarrel/AsyncTCP` library used here internally calls `xTaskCreateUniversal(..., CONFIG_ASYNC_TCP_RUNNING_CORE)` with `CONFIG_ASYNC_TCP_RUNNING_CORE = -1` (any core), not a hard-coded `xTaskCreatePinnedToCore(..., 1)`, which would otherwise fail on the single-core C3 with `assert failed: Invalid core ID`. The project itself also does not pin tasks. Both libraries (`AsyncTCP` and `ESPAsyncWebSrv`) are therefore safe on a single-core target.

**Flash/erase note for the C3 (native USB):** the SuperMini uses USB CDC instead of an external CH340/CP210x. Usually `pio run -t upload` works without manual boot mode handling; if the board is not reached, hold BOOT (GPIO9) while plugging in or resetting.

## What Was Changed

### 1. Pins, as required

All GPIOs used by the original project (0, 2, 4, 5, 16, 17) are available on the Wemos D1 Mini32 and can be used directly. So the pin numbers did not need to change, unlike on some other form factors. To make the mapping truly board-specific instead of accidentally correct, the pins were centralized out of the inline text (`includes.h`, several hard-coded `4`s) into `include/board_pins_wemos_d1_mini32.h`:

| Function | GPIO | D1 Mini32 silkscreen | Note |
|---|---:|---|---|
| `knop` (config button) | 0 | D3 / BOOT | onboard BOOT button |
| `led_onb` (LED) | 2 | LED_BUILTIN | blue onboard LED |
| `ZB_RESET` | 5 | D5 | reset line for CC2530/CC2531 |
| `RXD2` (UART2 RX) | 16 | RX2 | from the Zigbee module |
| `TXD2` (UART2 TX) | 17 | TX2 | to the Zigbee module |
| `EXTRA_OUT_PIN` | 4 | D4 | extra output, set LOW after boot |

**Important, please check on your own board:** the onboard LED (GPIO2) is active-HIGH on original ESP32 dev boards. Some D1 Mini32 clones wire it active-LOW, like an ESP8266 D1 mini. If the LED appears inverted, set `WEMOS_D1_MINI32_LED_ACTIVE_LOW` to `1` in `board_pins_wemos_d1_mini32.h`.

### 2. Real bug fixed in the original

`handle_data.ino` (formerly `handledata.ino`) contained a typo with a missing closing parenthesis:

```cpp
consoleOut("handleDataRequest the request is";   // original - does not compile
consoleOut("handleDataRequest the request is");  // fixed
```

This is not a porting error; it is present in the original repository (as of the loaded `main` branch) and would have broken the build in the Arduino IDE as well.

### 3. Dead/unclear dependency removed

`#include "PSACrypto.h"` was included, but nowhere used in the code base (no `psa_*` calls at all). Because that file is not shipped in the repository, the include was removed instead of carrying an unused third-party dependency. The same was done for `Async_TCP.h` (a local copy in the repo) - it is never included anywhere, and the actually used `AsyncTCP.h` comes from the library listed below.

## Dependencies

All dependencies were checked and are listed in `platformio.ini` under `lib_deps`.

| Include in code | Library |
|---|---|
| `ESPAsyncWebSrv.h`, `AsyncEventSource.h` | [dvarrel/ESPAsyncWebSrv](https://github.com/dvarrel/ESPAsyncWebSrv) (a fork of ESPAsyncWebServer that intentionally renames the headers to `ESPAsyncWebSrv.h`, exactly what the original code includes) |
| `AsyncTCP.h` | [dvarrel/AsyncTCP](https://github.com/dvarrel/AsyncTCP) (required by ESPAsyncWebSrv) |
| `ArduinoJson.h` (`JsonDocument` API = v7) | [bblanchon/ArduinoJson](https://github.com/bblanchon/ArduinoJson) `^7` |
| `PubSubClient.h` | [knolleary/PubSubClient](https://github.com/knolleary/pubsubclient) `^2.8` |
| `NTPClient.h` | [arduino-libraries/NTPClient](https://github.com/arduino-libraries/NTPClient) `^3.2.1` |
| `TimeLib.h` | [PaulStoffregen/Time](https://github.com/PaulStoffregen/Time) `^1.6.1` |
| `sunMoon.h` | [sfrwmaker/sunMoon](https://github.com/sfrwmaker/sunMoon) (not listed in the PlatformIO registry, so it is included via Git URL) |
| `WiFi.h`, `esp_wifi.h`, `DNSServer.h`, `Update.h`, `SPIFFS.h`, `FS.h`, `EEPROM.h`, `Preferences.h`, `Ticker.h`, `esp_task_wdt.h`, `rtc_wdt.h`, `soc/*.h` | Part of `framework = arduino` (arduino-esp32 core), no extra dependency |

## Verification Notes

The project was checked as far as this sandbox allows:

- Static completeness check: all brackets and blocks in every source file are balanced, including the typo fixed in `handle_data.ino`.
- Every `#include` was resolved to a concrete, existing library with a matching header file.
- Pin availability for the Wemos D1 Mini32 was checked against the board layout.
- The `.ino` merge behavior was verified against PlatformIO's `InoToCPPConverter`: the file with `setup()`/`loop()` is detected automatically and placed first, just like the Arduino IDE.

A real `pio run` compile pass, including downloading the Xtensa toolchain and the arduino-esp32 framework, could not be completed in this sandbox because PlatformIO's package downloader fails here with a TLS certificate error when fetching the large GitHub release assets. That is a network-sandbox issue, not a project issue.

Please run `pio run -e wemos_d1_mini32` locally with normal internet access. If anything still breaks there, I can adjust the porting code accordingly.
