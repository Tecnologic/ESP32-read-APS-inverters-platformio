#ifndef BOARD_PINS_ESP32C3_SUPERMINI_H
#define BOARD_PINS_ESP32C3_SUPERMINI_H
/*
 * Pin mapping for the ESP32-C3 SuperMini (ESP32-C3FN4, RISC-V, 13 exposed
 * GPIOs: 0-10, 20, 21).
 *
 * This chip is NOT a drop-in replacement for the classic ESP32:
 *  - only 2 hardware UARTs (UART0 + UART1) -> no "Serial2" object exists in
 *    the framework at all. We therefore instantiate our own HardwareSerial
 *    on UART1 and route the whole project's zigbee-UART code through the
 *    portable "ZB_SERIAL" name (see below) instead of the hard-coded
 *    "Serial2" the upstream project used.
 *  - GPIO16/17 (used by the classic-ESP32 port) are not broken out on this
 *    board (used internally for the embedded flash) -> moved to GPIO20/21.
 *  - the on-board LED sits on GPIO8 (active-LOW) here, not GPIO2.
 *  - the on-board BOOT button sits on GPIO9 here, not GPIO0.
 *
 * Antenna: this pin file assumes the SMD-antenna -> soldered λ/4-wire mod
 * from https://peterneufeld.wordpress.com/2025/03/04/esp32-c3-supermini-antenna-modification/
 * A pure hardware/RF modification - it does not touch any GPIO and needs no
 * changes here.
 */

// button used to open the config portal / factory-reset
// (on-board BOOT button; GPIO9 is a strapping pin, but - exactly like GPIO0
// on the classic ESP32 - that only matters while RESET is asserted, not
// during normal operation, so re-using it as INPUT_PULLUP + interrupt is
// safe here too)
#define knop              9
// on-board LED
#define led_onb           8
// reset line to the CC2530/CC2531 zigbee module
#define ZB_RESET          10
// extra output pin (kept from the original project, purpose: see setup())
#define EXTRA_OUT_PIN      3

// UART1 to the zigbee module (GPIO16/17 from the classic-ESP32 port are not
// available on this board -> re-mapped to GPIO20/21, both free/unused here)
#define RXD2              20
#define TXD2              21

// The ESP32-C3 only has UART0 and UART1 - there is no "Serial2". We create
// our own HardwareSerial instance on UART1 and give it a board-independent
// name; all "Serial2.xxx()" calls in the original code were replaced with
// "ZB_SERIAL.xxx()" for exactly this reason (see board_pins_wemos_d1_mini32.h
// for the classic-ESP32 side, where ZB_SERIAL is just an alias for Serial2).
HardwareSerial ZB_SERIAL(1);

/*
 * LED polarity - the on-board blue LED on the C3 SuperMini is active-LOW.
 */
#define LED_AAN LOW
#define LED_UIT HIGH

#endif // BOARD_PINS_ESP32C3_SUPERMINI_H
