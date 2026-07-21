#ifndef BOARD_PINS_WEMOS_D1_MINI32_H
#define BOARD_PINS_WEMOS_D1_MINI32_H
/*
 * Pin mapping for the Wemos/Lolin D1 Mini32 (ESP32-WROOM-32, D1-mini form
 * factor).
 *
 * The original project targeted a generic "ESP32Dev WROOM32" board. All pins
 * it uses (GPIO0, GPIO2, GPIO4, GPIO5, GPIO16, GPIO17) are broken out on the
 * D1 Mini32 as well, so the numeric values are unchanged - but they are now
 * centralised here (instead of being scattered/hard-coded in AAA_INCLUDES.h)
 * so the board can be re-targeted by editing a single file.
 *
 * D1 Mini32 silkscreen reference:
 *   D3 / BOOT  -> GPIO0   (also the on-board BOOT button -> used as "knop")
 *   LED_BUILTIN-> GPIO2   (on-board blue LED)
 *   D5         -> GPIO5   (also VSPI-CS, free to use as ZB_RESET)
 *   D2 / TX2   -> GPIO17  (UART2 TX -> to the CC2530/CC2531 module RX)
 *   D2 / RX2   -> GPIO16  (UART2 RX -> from the CC2530/CC2531 module TX)
 *   D4         -> GPIO4   (used as an extra output, driven LOW after boot)
 */

// button used to open the config portal / factory-reset (on-board BOOT button)
#define knop              0
// on-board LED
#define led_onb           2
// reset line to the CC2530/CC2531 zigbee module
#define ZB_RESET          5
// extra output pin (kept from the original project, purpose: see setup())
#define EXTRA_OUT_PIN     4

// UART2 to the zigbee module
#define RXD2              16
#define TXD2              17
// the classic ESP32 has a real UART2/Serial2 - just alias it so the rest of
// the code can use the board-independent name "ZB_SERIAL"
#define ZB_SERIAL Serial2

/*
 * LED polarity.
 * The original ESP32Dev boards this project targeted use positive logic
 * (HIGH = on). Several D1 Mini32 clones wire the blue LED the other way
 * round (active-LOW, same as the classic ESP8266 D1 mini). If the on-board
 * LED behaves inverted (on when it should be off) on your particular board,
 * flip this single define.
 */
#define WEMOS_D1_MINI32_LED_ACTIVE_LOW 0

#if WEMOS_D1_MINI32_LED_ACTIVE_LOW
  #define LED_AAN LOW
  #define LED_UIT HIGH
#else
  #define LED_AAN HIGH
  #define LED_UIT LOW
#endif

#endif // BOARD_PINS_WEMOS_D1_MINI32_H
